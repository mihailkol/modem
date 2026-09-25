#ifdef DEVICE_MASTERGAS
#include "MasterGasDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif

MasterGasConfig masterGasCfg;
MasterGasState  masterGasState;

uint32_t MasterGasDevice::_lastPub = 0;

// ── Тестовый импульс remoteEn (диагностика) ──────────────────────────────────
// Только дёргает remoteEn в LOW и обратно — НЕ включает override целиком,
// не пытается отвечать котлу (_sendReply не вызывается). Нужен только чтобы
// проверить, реагирует ли сигнал на GPIO39/остальных щупах на отключение
// штатного пульта от шины.
#define MASTERGAS_TEST_PULSE_MS 3000
static bool     _testPulseActive   = false;
static uint32_t _testPulseStartMs  = 0;

// UART1 — только приём, общая шина котёл<->пульт (boilerRead)
static HardwareSerial _boilerSerial(1);
// UART2 — приём линии пульта (remoteRead) + передача ответа на шину (boilerAnswer)
static HardwareSerial _remoteSerial(2);

// ── Диагностика: счётчик фронтов прямо на ноге, в обход UART framing ────────
// Считает ЛЮБОЙ перепад на пине независимо от бода/инверсии/чётности — если
// линия физически не доходит до GPIO, счётчик останется на нуле даже когда
// UART уже не сможет собрать ни одного валидного байта.
static portMUX_TYPE _edgeMux = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t _boilerEdges       = 0;
static volatile uint32_t _remoteEdges       = 0;
static volatile uint32_t _lastBoilerEdgeMs  = 0;
static volatile uint32_t _lastRemoteEdgeMs  = 0;

static void IRAM_ATTR _onBoilerEdge() {
    portENTER_CRITICAL_ISR(&_edgeMux);
    _boilerEdges++;
    _lastBoilerEdgeMs = millis();
    portEXIT_CRITICAL_ISR(&_edgeMux);
}
static void IRAM_ATTR _onRemoteEdge() {
    portENTER_CRITICAL_ISR(&_edgeMux);
    _remoteEdges++;
    _lastRemoteEdgeMs = millis();
    portEXIT_CRITICAL_ISR(&_edgeMux);
}

// ── Временная диагностика "не перепутаны ли ноги" ────────────────────────────
// Счётчики на соседних/используемых пинах (14, 15 — свободны; 33=remoteEn,
// 32=boilerAnswer — уже наши выходы, но повесить interrupt поверх можно не
// трогая их pinMode/UART-роль). Если реальный сигнал случайно заведён не туда,
// фронты вылезут здесь вместо boilerRead/remoteRead.
#define MASTERGAS_PROBE_PIN_A 14
#define MASTERGAS_PROBE_PIN_B 15
// GPIO39-щуп (был отдельным "подозреваемым" пином) убран — GPIO39 теперь сам
// remoteRead на постоянной основе (см. MasterGasTypes.h), его edge-счётчик
// уже есть штатно (_onRemoteEdge). Выяснили заодно: тот огромный всплеск на
// нём при активном override был наводкой от переключения remoteEn, а не
// честными данными — так что для настоящего remoteRead ещё предстоит
// развести отдельный провод с TX пульта, когда дойдут руки.

static volatile uint32_t _probe14Edges = 0, _probe15Edges = 0;
static volatile uint32_t _en33Edges = 0,    _answer32Edges = 0;
static volatile uint32_t _lastProbe14EdgeMs = 0, _lastProbe15EdgeMs = 0;
static volatile uint32_t _lastEn33EdgeMs = 0,    _lastAnswer32EdgeMs = 0;

static void IRAM_ATTR _onProbe14Edge() {
    portENTER_CRITICAL_ISR(&_edgeMux);
    _probe14Edges++; _lastProbe14EdgeMs = millis();
    portEXIT_CRITICAL_ISR(&_edgeMux);
}
static void IRAM_ATTR _onProbe15Edge() {
    portENTER_CRITICAL_ISR(&_edgeMux);
    _probe15Edges++; _lastProbe15EdgeMs = millis();
    portEXIT_CRITICAL_ISR(&_edgeMux);
}
static void IRAM_ATTR _onEn33Edge() {
    portENTER_CRITICAL_ISR(&_edgeMux);
    _en33Edges++; _lastEn33EdgeMs = millis();
    portEXIT_CRITICAL_ISR(&_edgeMux);
}
static void IRAM_ATTR _onAnswer32Edge() {
    portENTER_CRITICAL_ISR(&_edgeMux);
    _answer32Edges++; _lastAnswer32EdgeMs = millis();
    portEXIT_CRITICAL_ISR(&_edgeMux);
}

// ── Буфер забракованных по CRC пакетов ──────────────────────────────────────
// Стабильные ~50% CRC-ошибок именно при работающей горелке (не единицы
// процентов, как обычный шум) наводят на мысль о втором типе пакета с тем же
// заголовком 0xF9/0xFA, который мы не понимаем, а не о случайной порче битов.
// Раньше не было видно, что именно внутри забракованных пакетов — теперь
// сохраняем последние N целиком, вместе с тем, какую checksum мы ожидали.
#define MASTERGAS_CRCFAIL_BUF_LEN 20

struct MasterGasCrcFail {
    uint32_t ts;
    uint8_t  packet[7];
    uint8_t  expectedCrc;
    bool     burnerOn;
};

static MasterGasCrcFail _crcFailBuf[MASTERGAS_CRCFAIL_BUF_LEN];
static uint8_t _crcFailHead  = 0;
static uint8_t _crcFailCount = 0;

// ── Счётчики OK/CRC-fail по ТИПУ пакета (крест с горелкой) ──────────────────
// Общие pkt_ok/pkt_crc_err мешают в одну кучу статус котла (0xF9/0x00),
// ответ пульта (0xF9/0x80) и extended data (0xFA) — а по дампам забракованных
// пакетов похоже, что бракуются конкретно ответы пульта, а не всё подряд.
// Тут — раздельно, чтобы увидеть точный процент по каждому типу.
enum MasterGasPktType { PKT_BOILER_STATUS = 0, PKT_REMOTE_REPLY = 1, PKT_EXTENDED = 2, PKT_OTHER = 3, PKT_TYPE_COUNT = 4 };
static const char* PKT_TYPE_NAMES[PKT_TYPE_COUNT] = {
    "boiler status (F9/00)", "remote reply (F9/80)", "extended (FA)", "other"
};

static uint32_t _typeOk[PKT_TYPE_COUNT][2]     = {};   // [тип][горелка: 0=выкл,1=вкл]
static uint32_t _typeCrcErr[PKT_TYPE_COUNT][2] = {};

static MasterGasPktType _classifyPkt(const uint8_t* buf) {
    if (buf[0] == 0xFA) return PKT_EXTENDED;
    if (buf[0] == 0xF9 && buf[1] == 0x00) return PKT_BOILER_STATUS;
    if (buf[0] == 0xF9 && buf[1] == 0x80) return PKT_REMOTE_REPLY;
    return PKT_OTHER;
}

// ── Анализ "неизвестных" байт протокола ─────────────────────────────────────
// Пассивно (без всякого риска для котла) отслеживаем набор увиденных значений
// для байт-позиций, которые сейчас нигде не интерпретируются. Первые сутки —
// обучение (просто копим битовую маску всех значений), дальше — любое новое
// значение вне маски считаем аномалией и кладём в кольцевой буфер (пакет
// целиком + позиция + время) для разбора раз в несколько дней.
//
// Слоты:
//  0: raw_status[0]  — buf[2] пакета котла (0xF9/0x00), расшифрован только бит6=горелка
//  1: raw_status[3]  — buf[5] пакета котла, полностью неизвестен
//  2: remote buf[2]  — расшифрован только бит0=питание пульта
//  3: remote buf[4]  — полностью неизвестен
//  4..27: raw_ext[0..23] — 0xFA, полностью неизвестен
#define MASTERGAS_LEARN_MS        (24UL * 3600UL * 1000UL)   // сутки на обучение
#define MASTERGAS_ANOMALY_BUF_LEN 40
#define MASTERGAS_UNKNOWN_COUNT   (4 + 24)

struct MasterGasAnomaly {
    uint32_t ts;
    uint8_t  slot;
    uint8_t  value;
    uint8_t  packet[7];
};

static uint8_t  _unknownMask[MASTERGAS_UNKNOWN_COUNT][32] = {};  // 256 бит/слот
static bool     _learningDone     = false;
static uint32_t _learningStartMs  = 0;

static MasterGasAnomaly _anomalyBuf[MASTERGAS_ANOMALY_BUF_LEN];
static uint8_t  _anomalyHead  = 0;
static uint8_t  _anomalyCount = 0;

// Сырые байты ответа пульта, которых сейчас нигде не хранится (buf[2] целиком, buf[4])
static uint8_t _remoteRaw2 = 0, _remoteRaw4 = 0;

static inline bool _maskGet(uint8_t slot, uint8_t val) {
    return (_unknownMask[slot][val >> 3] >> (val & 7)) & 1;
}
static inline void _maskSet(uint8_t slot, uint8_t val) {
    _unknownMask[slot][val >> 3] |= (uint8_t)(1 << (val & 7));
}

static void _trackUnknown(uint8_t slot, uint8_t value, const uint8_t* packet) {
    if (!_learningDone) {
        if (millis() - _learningStartMs >= MASTERGAS_LEARN_MS) {
            _learningDone = true;
            Serial.println("[MASTERGAS] Anomaly baseline learning complete");
        } else {
            _maskSet(slot, value);
            return;
        }
    }
    if (_maskGet(slot, value)) return;   // уже видели такое значение — не аномалия

    _maskSet(slot, value);   // чтобы то же самое значение не спамило повторно
    MasterGasAnomaly& a = _anomalyBuf[_anomalyHead];
    a.ts    = millis();
    a.slot  = slot;
    a.value = value;
    memcpy(a.packet, packet, 7);
    _anomalyHead = (_anomalyHead + 1) % MASTERGAS_ANOMALY_BUF_LEN;
    if (_anomalyCount < MASTERGAS_ANOMALY_BUF_LEN) _anomalyCount++;
    Serial.printf("[MASTERGAS] Anomaly: slot=%u value=%02X\n", slot, value);
}

// ── HTML вкладки ─────────────────────────────────────────────────────────────
static const char MASTERGAS_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>🔥 Котёл</h3>
  <div id="mg_boiler" style="font-size:13px;line-height:1.8">загрузка...</div>
</div>

<div class="settings-group">
  <h3>🎛 Пульт</h3>
  <div id="mg_remote" style="font-size:13px;line-height:1.8">загрузка...</div>
</div>

<div class="settings-group">
  <h3>📊 Ошибки: горелка вкл vs выкл</h3>
  <div id="mg_burner_stats" style="font-size:13px;line-height:1.9">...</div>
  <button onclick="mgResetDiag()" class="btn-secondary" style="margin-top:8px">🔄 Сбросить счётчики</button>
  <div class="info-row">
    Одни и те же счётчики пакетов boilerRead, но в разрезе состояния горелки
    на момент приёма — чтобы подтвердить/опровергнуть на цифрах, что горелка
    шумит на шину, без ручного сравнения скриншотов.
  </div>
  <h4 style="margin:12px 0 4px">По типу пакета</h4>
  <div id="mg_type_stats" style="font-size:12px;line-height:1.8">...</div>
</div>

<div class="settings-group">
  <h3>⚙️ Управление</h3>
  <label class="toggle-row">
    <span>Passthrough (пульт отвечает сам)</span>
    <input type="checkbox" id="mg_passthrough" onchange="mgToggle()">
  </label>
  <div id="mg_override_fields" style="display:none">
    <label class="toggle-row">
      <span>Питание (override)</span>
      <input type="checkbox" id="mg_o_power">
    </label>
    <label>Уставка температуры, °C</label>
    <input type="number" id="mg_o_temp" min="30" max="85">
  </div>
  <button onclick="mgSave()" style="margin-top:8px">💾 Сохранить</button>
  <div class="info-row">
    В override штатный пульт отключается от шины (remoteEn), и ESP отвечает
    котлу сам заданными power/temp. 0xFA (расширенные данные) в этом режиме
    остаются неотвеченными — see README OpenMasterGas.
  </div>
</div>

<div class="settings-group">
  <h3>📶 Сигнал на ногах (сырой, без UART)</h3>
  <div id="mg_edges" style="font-size:14px;line-height:2">...</div>
  <div class="info-row">
    Считает любые перепады на GPIO напрямую, в обход UART — если счётчик
    растёт, сигнал физически доходит до пина (дальше уже вопрос бода/
    инверсии/пайки). Если стоит на нуле при работающем котле — сигнал не
    доходит до этой ноги вообще.
  </div>
  <h4 style="margin:12px 0 4px">🔍 Щупы "не перепутаны ли ноги" (временно)</h4>
  <div id="mg_probe_edges" style="font-size:13px;line-height:1.9">...</div>
  <div class="info-row">
    14/15 свободны, 33/32 — наши же remoteEn/boilerAnswer. Если реальный
    сигнал по ошибке заведён не туда — фронты вылезут тут, а не на
    boilerRead/remoteRead выше.
  </div>
  <h4 style="margin:12px 0 4px">🧪 Тест remoteEn</h4>
  <div id="mg_test_pulse_status" class="info-row">не запущен</div>
  <button onclick="mgTestPulse()" class="btn-secondary">⏱ Дёрнуть remoteEn на 3с</button>
  <div class="info-row">
    Отключает штатный пульт от шины на 3 секунды (только remoteEn — БЕЗ
    попытки ответить котлу подменным пакетом), чтобы проверить, появляется
    ли что-то на GPIO39/других щупах именно в момент отключения.
  </div>
</div>

<div class="settings-group">
  <h3>📋 Диагностика</h3>
  <div id="mg_diag" style="font-size:12px;color:var(--muted);line-height:1.7">...</div>
  <div style="margin-top:8px">
    <div class="info-row">raw status (0xF9/0x00):</div>
    <pre id="mg_raw_status" style="font-size:11px;color:var(--muted);background:var(--bg);
      padding:6px;border-radius:6px;border:1px solid var(--border)">—</pre>
    <div class="info-row">raw extended (0xFA):</div>
    <pre id="mg_raw_ext" style="font-size:11px;color:var(--muted);background:var(--bg);
      padding:6px;border-radius:6px;border:1px solid var(--border);word-break:break-all">—</pre>
    <div class="info-row">raw ответа пульта (buf[2]+buf[4]):</div>
    <pre id="mg_raw_remote" style="font-size:11px;color:var(--muted);background:var(--bg);
      padding:6px;border-radius:6px;border:1px solid var(--border)">—</pre>
  </div>
</div>

<div class="settings-group">
  <h3>🕵️ Анализ неизвестных байт протокола</h3>
  <div id="mg_anomaly_status" style="font-size:13px;margin-bottom:8px">...</div>
  <div id="mg_anomaly_list" style="font-size:11px;color:var(--muted);line-height:1.6;
    max-height:260px;overflow-y:auto;background:var(--bg);padding:6px;
    border-radius:6px;border:1px solid var(--border)">—</div>
  <button onclick="mgResetAnomalies()" class="btn-secondary" style="margin-top:8px">
    🔄 Сбросить baseline и начать обучение заново
  </button>
  <div class="info-row">
    Первые сутки после сброса — обучение (копим все увиденные значения байт,
    которые сейчас нигде не расшифрованы). Дальше любое новое значение —
    аномалия, попадает в список ниже с полным пакетом для разбора.
  </div>
</div>

<div class="settings-group">
  <h3>❌ Забракованные по CRC пакеты (последние 20)</h3>
  <div id="mg_crcfail_list" style="font-size:11px;color:var(--muted);line-height:1.6;
    max-height:260px;overflow-y:auto;background:var(--bg);padding:6px;
    border-radius:6px;border:1px solid var(--border)">—</div>
  <div class="info-row">
    Пакет целиком + какую checksum мы ожидали в последнем байте (буквально
    седьмой байт пакета — то, что реально пришло, не подошло под неё).
    Полезно смотреть, когда % CRC-ошибок подозрительно высокий/стабильный —
    мусор это или второй, нераспознанный тип пакета.
  </div>
</div>

<script>
let _mgCfg = {};
let _mgFormInited = false;

function mgToggle() {
  document.getElementById('mg_override_fields').style.display =
    document.getElementById('mg_passthrough').checked ? 'none' : 'block';
}

function mgAge(ms) {
  if (ms < 0) return 'нет данных';
  return (ms / 1000).toFixed(0) + ' с назад';
}

async function loadMgStatus() {
  try {
    const r = await fetch('/api/mastergas/status');
    const d = await r.json();
    _mgCfg = d;

    document.getElementById('mg_boiler').innerHTML =
      `<b>Горелка:</b> ${d.burner ? '🔥 работает' : '⚪ выкл'}<br>
       <b>T теплоносителя:</b> ${d.coolant_t}°C<br>
       <b>T ГВС:</b> ${d.water_t}°C`;

    document.getElementById('mg_remote').innerHTML =
      `<b>Питание:</b> ${d.r_power ? '✅ вкл' : '⛔ выкл'}<br>
       <b>Уставка:</b> ${d.r_target_t}°C<br>
       <b>T воздуха (датчик пульта):</b> ${d.r_air_t}°C`;

    // Поля формы синхронизируем с сервером ТОЛЬКО один раз при первой
    // загрузке — иначе периодический опрос (раз в 2с) затирает то, что
    // пользователь только что поменял в форме, но ещё не сохранил (галка
    // открывает override-поля → через пару секунд опрос статуса тянет ещё
    // старое значение с сервера → закрывает их обратно).
    if (!_mgFormInited) {
      document.getElementById('mg_passthrough').checked = d.passthrough;
      document.getElementById('mg_o_power').checked = !!d.o_power;
      document.getElementById('mg_o_temp').value = d.o_target_t;
      mgToggle();
      _mgFormInited = true;
    }

    document.getElementById('mg_diag').innerHTML =
      `Пакетов OK: ${d.pkt_ok} &nbsp; CRC ошибок: ${d.pkt_crc_err}<br>
       Последний пакет от котла: ${mgAge(d.boiler_age_ms)}<br>
       Последний пакет от пульта: ${mgAge(d.remote_age_ms)}`;

    const edgeColor = (n) => n > 0 ? 'var(--ok)' : 'var(--danger)';
    const edgeLine = (label, n, ageMs) =>
      `<b>${label}:</b> <span style="color:${edgeColor(n)};font-weight:700">${n}</span> фронтов
       &nbsp;<span style="color:var(--muted);font-size:12px">(${mgAge(ageMs)})</span>`;

    document.getElementById('mg_edges').innerHTML =
      edgeLine('boilerRead (GPIO36)', d.boiler_edges, d.boiler_edge_age_ms) + '<br>' +
      edgeLine('remoteRead (GPIO39, пока не подключен)', d.remote_edges, d.remote_edge_age_ms);

    document.getElementById('mg_probe_edges').innerHTML =
      edgeLine('GPIO14', d.probe14_edges, d.probe14_edge_age_ms) + '<br>' +
      edgeLine('GPIO15', d.probe15_edges, d.probe15_edge_age_ms) + '<br>' +
      edgeLine('GPIO33 (remoteEn)', d.en33_edges, d.en33_edge_age_ms) + '<br>' +
      edgeLine('GPIO32 (boilerAnswer)', d.answer32_edges, d.answer32_edge_age_ms);

    document.getElementById('mg_test_pulse_status').textContent = d.test_pulse_active
      ? `идёт, осталось ${(d.test_pulse_remaining_ms/1000).toFixed(1)}с`
      : 'не запущен';

    const errPct = (ok, err) => (ok + err) > 0 ? (100 * err / (ok + err)).toFixed(1) : '—';
    document.getElementById('mg_burner_stats').innerHTML =
      `<b>🔥 Горелка ВКЛ:</b> ${d.pkt_ok_burner_on} OK / ${d.pkt_crc_err_burner_on} ошибок
        (${errPct(d.pkt_ok_burner_on, d.pkt_crc_err_burner_on)}%)<br>
       <b>⚪ Горелка ВЫКЛ:</b> ${d.pkt_ok_burner_off} OK / ${d.pkt_crc_err_burner_off} ошибок
        (${errPct(d.pkt_ok_burner_off, d.pkt_crc_err_burner_off)}%)`;
  } catch(e) {}
}

async function loadMgTypeStats() {
  try {
    const r = await fetch('/api/mastergas/type-stats');
    const d = await r.json();
    const errPct = (ok, err) => (ok + err) > 0 ? (100 * err / (ok + err)).toFixed(1) : '—';
    document.getElementById('mg_type_stats').innerHTML = d.types.map(t =>
      `<b>${t.name}</b><br>
       &nbsp;&nbsp;🔥 вкл: ${t.ok_burner_on} OK / ${t.err_burner_on} ошибок (${errPct(t.ok_burner_on, t.err_burner_on)}%)<br>
       &nbsp;&nbsp;⚪ выкл: ${t.ok_burner_off} OK / ${t.err_burner_off} ошибок (${errPct(t.ok_burner_off, t.err_burner_off)}%)`
    ).join('<br>');
  } catch(e) {}
}

async function mgResetDiag() {
  await fetch('/api/mastergas/diag/reset', {method:'POST'});
  loadMgStatus();
}

async function mgTestPulse() {
  if (!confirm('Отключить штатный пульт от шины на 3 секунды (без ответа котлу)?')) return;
  const r = await fetch('/api/mastergas/test/remote-en-pulse', {method:'POST'});
  if (!r.ok) { alert('Уже идёт другой тест'); return; }
  // Чаще опрашиваем статус на время импульса, чтобы видеть live-реакцию щупов
  let ticks = 0;
  const fast = setInterval(() => {
    loadMgStatus();
    if (++ticks > 8) clearInterval(fast);   // ~4с на 500мс — с запасом
  }, 500);
}

async function loadMgRaw() {
  try {
    const r = await fetch('/api/mastergas/raw');
    const d = await r.json();
    document.getElementById('mg_raw_status').textContent = d.raw_status || '—';
    document.getElementById('mg_raw_ext').textContent = d.raw_ext || '—';
    document.getElementById('mg_raw_remote').textContent = d.raw_remote || '—';
  } catch(e) {}
}

function mgAgeShort(ms) {
  if (ms < 60000) return (ms/1000).toFixed(0) + 'с назад';
  if (ms < 3600000) return (ms/60000).toFixed(0) + 'мин назад';
  return (ms/3600000).toFixed(1) + 'ч назад';
}

async function loadMgAnomalies() {
  try {
    const r = await fetch('/api/mastergas/anomalies');
    const d = await r.json();
    document.getElementById('mg_anomaly_status').innerHTML = d.learning
      ? `⏳ Идёт обучение baseline, осталось ~${(d.learning_remaining_ms/3600000).toFixed(1)}ч. Аномалий пока не фиксируем.`
      : `✅ Baseline зафиксирован. Аномалий поймано: <b>${d.count}</b>`;
    const list = document.getElementById('mg_anomaly_list');
    if (!d.items || !d.items.length) {
      list.textContent = d.learning ? 'обучение...' : 'аномалий не было';
    } else {
      list.innerHTML = d.items.map(a =>
        `<div style="margin-bottom:4px">
          <b>${a.slot}</b> = 0x${a.value} &nbsp;<span style="opacity:.7">(${mgAgeShort(a.age_ms)})</span><br>
          пакет: ${a.packet}
        </div>`).join('');
    }
  } catch(e) {}
}

async function loadMgCrcFails() {
  try {
    const r = await fetch('/api/mastergas/crcfails');
    const d = await r.json();
    const list = document.getElementById('mg_crcfail_list');
    if (!d.items || !d.items.length) {
      list.textContent = 'ошибок не было';
    } else {
      list.innerHTML = d.items.map(f =>
        `<div style="margin-bottom:4px">
          ${f.burner ? '🔥' : '⚪'} пакет: <b>${f.packet}</b>
          &nbsp;ожидали CRC=0x${f.expected_crc}
          &nbsp;<span style="opacity:.7">(${mgAgeShort(f.age_ms)})</span>
        </div>`).join('');
    }
  } catch(e) {}
}

async function mgResetAnomalies() {
  if (!confirm('Сбросить baseline и начать сутки обучения заново?')) return;
  await fetch('/api/mastergas/anomalies/reset', {method:'POST'});
  loadMgAnomalies();
}

async function mgSave() {
  const data = {
    passthrough:    document.getElementById('mg_passthrough').checked,
    override_power: document.getElementById('mg_o_power').checked,
    override_temp:  +document.getElementById('mg_o_temp').value,
  };
  const r = await fetch('/api/mastergas/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Сохранено');
}

loadMgStatus();
loadMgRaw();
loadMgAnomalies();
loadMgCrcFails();
loadMgTypeStats();
setInterval(loadMgStatus, 2000);
setInterval(loadMgRaw, 5000);
setInterval(loadMgAnomalies, 15000);
setInterval(loadMgCrcFails, 5000);
setInterval(loadMgTypeStats, 5000);
</script>
)html";

// ── Config ───────────────────────────────────────────────────────────────────
bool MasterGasDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/mastergas.json", doc)) return false;
    masterGasCfg.enabled        = doc["enabled"]        | true;
    masterGasCfg.passthrough    = doc["passthrough"]    | true;
    masterGasCfg.override_power = doc["override_power"] | true;
    masterGasCfg.override_temp  = doc["override_temp"]  | 50;
    masterGasCfg.pub_interval   = doc["pub_interval"]   | 10000;
    return true;
}

bool MasterGasDevice::saveConfig() {
    JsonDocument doc;
    doc["enabled"]        = masterGasCfg.enabled;
    doc["passthrough"]    = masterGasCfg.passthrough;
    doc["override_power"] = masterGasCfg.override_power;
    doc["override_temp"]  = masterGasCfg.override_temp;
    doc["pub_interval"]   = masterGasCfg.pub_interval;
    return ConfigManager::saveJson("/mastergas.json", doc);
}

// ── Протокол (порт OpenMasterGas) ─────────────────────────────────────────────
// Чек-сумма: сумма первых 6 байт, инвертировать, +1.
uint8_t MasterGasDevice::_calcCrc(const uint8_t* buf) {
    uint8_t sum = 0;
    for (int n = 0; n < 6; n++) sum += buf[n];
    return (uint8_t)((sum ^ 0xFF) + 1);
}

// Не блокирует: возвращает false, если 7 байт ещё не накопилось.
// При ложном старте (первый байт не 0xF9/0xFA) съедает один байт и ждёт
// следующего вызова — как и в оригинальном протоколе; при потоке мусора
// возможна временная десинхронизация до следующего валидного заголовка.
//
// ВАЖНО про ~50% "CRC-ошибок" именно при работающей горелке (в простое —
// стабильно 0%): это НЕ порча данных на линии. По дампам забракованных
// пакетов (см. /api/mastergas/crcfails) видно, что buf[0..4] всегда
// F9 80 01 28 32 — то есть валидный заголовок "ответ пульта" (не котла!)
// с реальным power/target_temp, а "хвост" (buf[5]/buf[6], которые мы
// трактуем как T_воздуха+checksum) меняется независимо друг от друга и
// слишком быстро (за 1-2с) для комнатного датчика — похоже на отдельный,
// нерасшифрованный вид полезной нагрузки под тем же заголовком (что именно
// это — неизвестно, не домысливать), который просто не проходит нашу
// checksum, потому что байт[6] там не checksum, а данные. Т.е. это не брак,
// а неучтённый подтип пакета — можно спокойно игнорировать до тех пор, пока
// не понадобится его реально расшифровать.
bool MasterGasDevice::_waitPacket(HardwareSerial& serial, uint8_t* buf, bool trackBurnerStats) {
    if (serial.available() < 7) return false;

    int c = serial.read();
    if (c != 0xF9 && c != 0xFA) return false;
    buf[0] = (uint8_t)c;
    for (int n = 1; n < 7; n++) buf[n] = (uint8_t)serial.read();

    MasterGasPktType ptype = _classifyPkt(buf);
    uint8_t burnerIdx = masterGasState.burner_on ? 1 : 0;

    if (buf[6] == _calcCrc(buf)) {
        masterGasState.pkt_ok++;
        _typeOk[ptype][burnerIdx]++;
        if (trackBurnerStats) {
            if (masterGasState.burner_on) masterGasState.pkt_ok_burner_on++;
            else                          masterGasState.pkt_ok_burner_off++;
        }
        return true;
    }
    masterGasState.pkt_crc_err++;
    _typeCrcErr[ptype][burnerIdx]++;
    if (trackBurnerStats) {
        if (masterGasState.burner_on) masterGasState.pkt_crc_err_burner_on++;
        else                          masterGasState.pkt_crc_err_burner_off++;
    }
    {
        MasterGasCrcFail& f = _crcFailBuf[_crcFailHead];
        f.ts       = millis();
        memcpy(f.packet, buf, 7);
        f.expectedCrc = _calcCrc(buf);
        f.burnerOn = masterGasState.burner_on;
        _crcFailHead = (_crcFailHead + 1) % MASTERGAS_CRCFAIL_BUF_LEN;
        if (_crcFailCount < MASTERGAS_CRCFAIL_BUF_LEN) _crcFailCount++;
    }
    return false;
}

bool MasterGasDevice::_isFromBoiler(const uint8_t* buf) {
    return buf[0] == 0xFA || (buf[0] == 0xF9 && buf[1] == 0x00);
}

void MasterGasDevice::_parsePacket(const uint8_t* buf) {
    if (buf[0] == 0xFA) {
        // Расширенные данные, offset 0x00..0x14 с шагом 4 — смысл части полей
        // не расшифрован (см. апстрим), храним как есть для диагностики.
        uint8_t offset = buf[1];
        if (offset <= sizeof(masterGasState.raw_ext) - 4) {
            for (int n = 0; n < 4; n++) {
                masterGasState.raw_ext[offset + n] = buf[2 + n];
                if (offset + n < 24) _trackUnknown(4 + offset + n, buf[2 + n], buf);
            }
        }
        masterGasState.last_boiler_rx_ms = millis();
        return;
    }

    if (buf[0] == 0xF9 && buf[1] == 0x00) {
        // Состояние котла: бит 6 байта [2] — горелка, [3] — T теплоносителя,
        // [4] — T ГВС (все — прямое значение в °C, без масштаба)
        masterGasState.burner_on    = (buf[2] >> 6) & 1;
        masterGasState.coolant_temp = buf[3];
        masterGasState.water_temp   = buf[4];
        for (int n = 0; n < 4; n++) masterGasState.raw_status[n] = buf[2 + n];
        _trackUnknown(0, buf[2], buf);   // остальные 7 бит байта[2], кроме горелки
        _trackUnknown(1, buf[5], buf);   // байт[5] целиком неизвестен
        masterGasState.last_boiler_rx_ms = millis();
        return;
    }

    if (buf[0] == 0xF9 && buf[1] == 0x80) {
        // Ответ пульта: [2] бит0 — питание, [3] — уставка °C, [5] — T воздуха
        masterGasState.remote_power       = buf[2] & 0x01;
        masterGasState.remote_target_temp = buf[3];
        masterGasState.remote_air_temp    = buf[5];
        _remoteRaw2 = buf[2];
        _remoteRaw4 = buf[4];
        _trackUnknown(2, buf[2], buf);   // остальные 7 бит байта[2], кроме питания
        _trackUnknown(3, buf[4], buf);   // байт[4] целиком неизвестен
        masterGasState.last_remote_rx_ms  = millis();
        return;
    }
}

// Формат ответа пульта на 0xF9/0x00 — байты [4],[5] взяты из наблюдаемых
// реальных ответов пульта (см. апстрим), их смысл не расшифрован, но котёл
// на них не реагирует отказом — используем как константу.
void MasterGasDevice::_sendReply() {
    uint8_t reply[7] = {0xF9, 0x80, 0x00, 0x00, 0x28, 0x18, 0x00};
    reply[2] = masterGasCfg.override_power ? 0x01 : 0x00;
    reply[3] = masterGasCfg.override_temp;
    reply[6] = _calcCrc(reply);
    _remoteSerial.write(reply, 7);
    _remoteSerial.flush();
}

// ── Init ─────────────────────────────────────────────────────────────────────
void MasterGasDevice::init() {
    loadConfig();
    if (!masterGasCfg.enabled) { Serial.println("[MASTERGAS] Disabled"); return; }

    _learningStartMs = millis();

    pinMode(MASTERGAS_REMOTE_EN_PIN, OUTPUT);
    digitalWrite(MASTERGAS_REMOTE_EN_PIN, HIGH); // пульт подключен к шине штатно

    pinMode(MASTERGAS_LED_R_PIN, OUTPUT);
    pinMode(MASTERGAS_LED_G_PIN, OUTPUT);
    pinMode(MASTERGAS_LED_B_PIN, OUTPUT);

    // Инверсия сигнала — как в апстриме (OpenMasterGas): линия boilerRead
    // (общая шина) идёт БЕЗ инверсии, а линия пульта (remoteRead) и передатчик
    // ответа на шину (boilerAnswer) — ИНВЕРТИРОВАНЫ (там же rx_remote и Serial1
    // поднимались с invert=true). Это свойство их согласующей электроники, а
    // не протокола — если на реальной плате схема интерфейса другая, первым
    // делом при "пустых"/битых пакетах проверяйте именно этот флаг.
    _boilerSerial.begin(MASTERGAS_BAUD, SERIAL_8N1, MASTERGAS_BOILER_RX_PIN, -1);
    _remoteSerial.begin(MASTERGAS_BAUD, SERIAL_8N1, MASTERGAS_REMOTE_RX_PIN, MASTERGAS_ANSWER_TX_PIN, true);

    // Счётчики фронтов вешаем ПОСЛЕ HardwareSerial::begin() — вход GPIO-матрицы
    // может фанаутиться на несколько потребителей одновременно (UART RX +
    // обычное изменение уровня), но начинаем слушать заведомо после того, как
    // UART уже сконфигурировал этот пин.
    // Если boilerRead/remoteRead временно указывают на один и тот же физический
    // пин (см. диагностический дубль-тест в MasterGasTypes.h) — вешаем только
    // один interrupt на него, иначе второй attachInterrupt тихо перезапишет
    // первый и один из счётчиков просто замрёт.
    attachInterrupt(digitalPinToInterrupt(MASTERGAS_BOILER_RX_PIN), _onBoilerEdge, CHANGE);
#if MASTERGAS_REMOTE_RX_PIN != MASTERGAS_BOILER_RX_PIN
    attachInterrupt(digitalPinToInterrupt(MASTERGAS_REMOTE_RX_PIN), _onRemoteEdge, CHANGE);
#endif

    // Временная диагностика "не перепутаны ли ноги" — см. комментарий у
    // объявления счётчиков выше. 14/15 никем не заняты, задаём им direction
    // явно; 33/32 УЖЕ наши выходы (remoteEn/boilerAnswer) — pinMode НЕ трогаем,
    // просто вешаем interrupt поверх.
    // INPUT_PULLDOWN, а не голый INPUT — свободный floating-вход на КМОП сам
    // по себе шумит от наводок (десятки тысяч "фронтов" в секунду без всякого
    // внешнего сигнала). Слабая подтяжка к GND даёт пину определённое состояние
    // покоя: если после неё счётчик всё равно активно растёт — это уже реальный
    // внешний сигнал, а не самовозбуждение.
    // Пины-щупы не должны совпадать с уже занятыми boilerRead/remoteRead —
    // сейчас boilerRead временно переехал на GPIO15 (см. MasterGasTypes.h),
    // так что щуп на том же пине пропускаем: второй attachInterrupt на тот
    // же GPIO просто перезапишет _onBoilerEdge, и boiler_edges встанет.
#if MASTERGAS_PROBE_PIN_A != MASTERGAS_BOILER_RX_PIN && MASTERGAS_PROBE_PIN_A != MASTERGAS_REMOTE_RX_PIN
    pinMode(MASTERGAS_PROBE_PIN_A, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(MASTERGAS_PROBE_PIN_A), _onProbe14Edge, CHANGE);
#endif
#if MASTERGAS_PROBE_PIN_B != MASTERGAS_BOILER_RX_PIN && MASTERGAS_PROBE_PIN_B != MASTERGAS_REMOTE_RX_PIN
    pinMode(MASTERGAS_PROBE_PIN_B, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(MASTERGAS_PROBE_PIN_B), _onProbe15Edge, CHANGE);
#endif
    attachInterrupt(digitalPinToInterrupt(MASTERGAS_REMOTE_EN_PIN), _onEn33Edge, CHANGE);
    attachInterrupt(digitalPinToInterrupt(MASTERGAS_ANSWER_TX_PIN), _onAnswer32Edge, CHANGE);

    WebHandler::registerTab({"mastergas", "Котёл 🔥", "🔥", MASTERGAS_TAB_HTML});

    extern AsyncWebServer server;

    server.on("/api/mastergas/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["burner"]      = masterGasState.burner_on;
        doc["coolant_t"]   = masterGasState.coolant_temp;
        doc["water_t"]     = masterGasState.water_temp;
        doc["r_power"]     = masterGasState.remote_power;
        doc["r_target_t"]  = masterGasState.remote_target_temp;
        doc["r_air_t"]     = masterGasState.remote_air_temp;
        doc["passthrough"] = masterGasCfg.passthrough;
        doc["o_power"]     = masterGasCfg.override_power;
        doc["o_target_t"]  = masterGasCfg.override_temp;
        doc["pkt_ok"]      = masterGasState.pkt_ok;
        doc["pkt_crc_err"] = masterGasState.pkt_crc_err;
        doc["boiler_age_ms"] = masterGasState.last_boiler_rx_ms
            ? (int32_t)(millis() - masterGasState.last_boiler_rx_ms) : -1;
        doc["remote_age_ms"] = masterGasState.last_remote_rx_ms
            ? (int32_t)(millis() - masterGasState.last_remote_rx_ms) : -1;

        // Сырые фронты на ногах — независимо от UART framing, см. коммент
        // у attachInterrupt() выше. Если это осталось на нуле, а котёл точно
        // работает — сигнал физически не доходит до GPIO (до делителя/моста
        // или после него — заземление, обрыв, не тот пин).
        portENTER_CRITICAL(&_edgeMux);
        uint32_t boilerEdges = _boilerEdges, remoteEdges = _remoteEdges;
        uint32_t boilerEdgeMs = _lastBoilerEdgeMs, remoteEdgeMs = _lastRemoteEdgeMs;
        portEXIT_CRITICAL(&_edgeMux);
        doc["boiler_edges"] = boilerEdges;
        doc["remote_edges"] = remoteEdges;
        doc["boiler_edge_age_ms"] = boilerEdgeMs ? (int32_t)(millis() - boilerEdgeMs) : -1;
        doc["remote_edge_age_ms"] = remoteEdgeMs ? (int32_t)(millis() - remoteEdgeMs) : -1;

        // Временные щупы "не перепутаны ли ноги" — см. комментарий у ISR выше
        portENTER_CRITICAL(&_edgeMux);
        uint32_t p14 = _probe14Edges, p15 = _probe15Edges;
        uint32_t en33 = _en33Edges, ans32 = _answer32Edges;
        uint32_t p14Ms = _lastProbe14EdgeMs, p15Ms = _lastProbe15EdgeMs;
        uint32_t en33Ms = _lastEn33EdgeMs, ans32Ms = _lastAnswer32EdgeMs;
        portEXIT_CRITICAL(&_edgeMux);
        doc["probe14_edges"] = p14;
        doc["probe15_edges"] = p15;
        doc["en33_edges"]    = en33;
        doc["answer32_edges"] = ans32;
        doc["probe14_edge_age_ms"]  = p14Ms  ? (int32_t)(millis() - p14Ms)  : -1;
        doc["probe15_edge_age_ms"]  = p15Ms  ? (int32_t)(millis() - p15Ms)  : -1;
        doc["en33_edge_age_ms"]     = en33Ms ? (int32_t)(millis() - en33Ms) : -1;
        doc["answer32_edge_age_ms"] = ans32Ms ? (int32_t)(millis() - ans32Ms) : -1;

        doc["test_pulse_active"] = _testPulseActive;
        doc["test_pulse_remaining_ms"] = _testPulseActive
            ? (int32_t)MASTERGAS_TEST_PULSE_MS - (int32_t)(millis() - _testPulseStartMs) : 0;

        doc["pkt_ok_burner_on"]       = masterGasState.pkt_ok_burner_on;
        doc["pkt_ok_burner_off"]      = masterGasState.pkt_ok_burner_off;
        doc["pkt_crc_err_burner_on"]  = masterGasState.pkt_crc_err_burner_on;
        doc["pkt_crc_err_burner_off"] = masterGasState.pkt_crc_err_burner_off;

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/mastergas/diag/reset", HTTP_POST, [](AsyncWebServerRequest* req) {
        masterGasState.pkt_ok      = 0;
        masterGasState.pkt_crc_err = 0;
        masterGasState.pkt_ok_burner_on       = 0;
        masterGasState.pkt_ok_burner_off      = 0;
        masterGasState.pkt_crc_err_burner_on  = 0;
        masterGasState.pkt_crc_err_burner_off = 0;
        memset(_typeOk, 0, sizeof(_typeOk));
        memset(_typeCrcErr, 0, sizeof(_typeCrcErr));
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/mastergas/test/remote-en-pulse", HTTP_POST, [](AsyncWebServerRequest* req) {
        if (_testPulseActive) { req->send(409, "application/json", "{\"status\":\"already_running\"}"); return; }
        _testPulseActive  = true;
        _testPulseStartMs = millis();
        digitalWrite(MASTERGAS_REMOTE_EN_PIN, LOW);
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/mastergas/raw", HTTP_GET, [](AsyncWebServerRequest* req) {
        static const char* H = "0123456789ABCDEF";
        char hexExt[49] = {0};
        for (int i = 0; i < 24; i++) {
            hexExt[i*2]   = H[masterGasState.raw_ext[i] >> 4];
            hexExt[i*2+1] = H[masterGasState.raw_ext[i] & 0x0F];
        }
        char hexStatus[9] = {0};
        for (int i = 0; i < 4; i++) {
            hexStatus[i*2]   = H[masterGasState.raw_status[i] >> 4];
            hexStatus[i*2+1] = H[masterGasState.raw_status[i] & 0x0F];
        }
        JsonDocument doc;
        doc["raw_ext"]    = hexExt;
        doc["raw_status"] = hexStatus;
        char hexRemote[5] = {0};
        hexRemote[0] = H[_remoteRaw2 >> 4]; hexRemote[1] = H[_remoteRaw2 & 0x0F];
        hexRemote[2] = H[_remoteRaw4 >> 4]; hexRemote[3] = H[_remoteRaw4 & 0x0F];
        doc["raw_remote"] = hexRemote;   // buf[2]+buf[4] последнего ответа пульта
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    static const char* SLOT_NAMES[MASTERGAS_UNKNOWN_COUNT] = {
        "boiler buf[2]", "boiler buf[5]", "remote buf[2]", "remote buf[4]",
        "ext[0]","ext[1]","ext[2]","ext[3]","ext[4]","ext[5]","ext[6]","ext[7]",
        "ext[8]","ext[9]","ext[10]","ext[11]","ext[12]","ext[13]","ext[14]","ext[15]",
        "ext[16]","ext[17]","ext[18]","ext[19]","ext[20]","ext[21]","ext[22]","ext[23]"
    };

    server.on("/api/mastergas/anomalies", HTTP_GET, [](AsyncWebServerRequest* req) {
        static const char* H = "0123456789ABCDEF";
        JsonDocument doc;
        doc["learning"] = !_learningDone;
        doc["learning_remaining_ms"] = _learningDone ? 0
            : (int32_t)MASTERGAS_LEARN_MS - (int32_t)(millis() - _learningStartMs);
        doc["count"] = _anomalyCount;
        JsonArray arr = doc["items"].to<JsonArray>();
        // От новых к старым
        for (uint8_t i = 0; i < _anomalyCount; i++) {
            uint8_t idx = (_anomalyHead + MASTERGAS_ANOMALY_BUF_LEN - 1 - i) % MASTERGAS_ANOMALY_BUF_LEN;
            const MasterGasAnomaly& a = _anomalyBuf[idx];
            JsonObject o = arr.add<JsonObject>();
            o["age_ms"] = (int32_t)(millis() - a.ts);
            o["slot"]   = SLOT_NAMES[a.slot];
            char hv[3] = { H[a.value >> 4], H[a.value & 0x0F], 0 };
            o["value"] = hv;
            char hp[15] = {0};
            for (int n = 0; n < 7; n++) { hp[n*2] = H[a.packet[n]>>4]; hp[n*2+1] = H[a.packet[n]&0x0F]; }
            o["packet"] = hp;
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/mastergas/anomalies/reset", HTTP_POST, [](AsyncWebServerRequest* req) {
        memset(_unknownMask, 0, sizeof(_unknownMask));
        _learningDone    = false;
        _learningStartMs = millis();
        _anomalyHead  = 0;
        _anomalyCount = 0;
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/mastergas/crcfails", HTTP_GET, [](AsyncWebServerRequest* req) {
        static const char* H = "0123456789ABCDEF";
        JsonDocument doc;
        doc["count"] = _crcFailCount;
        JsonArray arr = doc["items"].to<JsonArray>();
        for (uint8_t i = 0; i < _crcFailCount; i++) {
            uint8_t idx = (_crcFailHead + MASTERGAS_CRCFAIL_BUF_LEN - 1 - i) % MASTERGAS_CRCFAIL_BUF_LEN;
            const MasterGasCrcFail& f = _crcFailBuf[idx];
            JsonObject o = arr.add<JsonObject>();
            o["age_ms"]  = (int32_t)(millis() - f.ts);
            o["burner"]  = f.burnerOn;
            char hp[15] = {0};
            for (int n = 0; n < 7; n++) { hp[n*2] = H[f.packet[n]>>4]; hp[n*2+1] = H[f.packet[n]&0x0F]; }
            o["packet"] = hp;
            char hc[3] = { H[f.expectedCrc >> 4], H[f.expectedCrc & 0x0F], 0 };
            o["expected_crc"] = hc;
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/mastergas/type-stats", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc["types"].to<JsonArray>();
        for (int t = 0; t < PKT_TYPE_COUNT; t++) {
            JsonObject o = arr.add<JsonObject>();
            o["name"]           = PKT_TYPE_NAMES[t];
            o["ok_burner_on"]   = _typeOk[t][1];
            o["err_burner_on"]  = _typeCrcErr[t][1];
            o["ok_burner_off"]  = _typeOk[t][0];
            o["err_burner_off"] = _typeCrcErr[t][0];
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* hSave = new AsyncCallbackJsonWebHandler("/api/mastergas/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            masterGasCfg.passthrough    = o["passthrough"]    | true;
            masterGasCfg.override_power = o["override_power"] | true;
            masterGasCfg.override_temp  = o["override_temp"]  | 50;
            MasterGasDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSave);

    Serial.println("[MASTERGAS] Init OK");
}

// ── Loop ─────────────────────────────────────────────────────────────────────
void MasterGasDevice::loop() {
    if (!masterGasCfg.enabled) return;

    if (_testPulseActive && millis() - _testPulseStartMs >= MASTERGAS_TEST_PULSE_MS) {
        digitalWrite(MASTERGAS_REMOTE_EN_PIN, HIGH);
        _testPulseActive = false;
        Serial.println("[MASTERGAS] Test remoteEn pulse done");
    }

    _ledTick();

    static uint8_t buf[7];

    if (_waitPacket(_boilerSerial, buf, /*trackBurnerStats=*/true)) {
        if (masterGasCfg.passthrough) {
            // Штатный пульт отвечает сам — только слушаем и парсим шину
            _parsePacket(buf);
        } else if (_isFromBoiler(buf)) {
            _parsePacket(buf);

            // Отключаем штатный пульт от шины — дальше отвечаем сами
            digitalWrite(MASTERGAS_REMOTE_EN_PIN, LOW);

            if (buf[0] == 0xF9) {
                // Тайминг из реверс-инжиниринга протокола (см. README
                // OpenMasterGas) — котёл ждёт паузу перед ответом пульта.
                // Суммарно ~34мс, вотчдогу (8с) ничем не грозит.
                delay(4);
                _sendReply();
                delay(30);
            } else {
                // 0xFA (расширенные данные) — ESP их не подменяет, только
                // подсматривает настоящий ответ пульта на его собственной
                // линии (remoteRead), пока тот отключен от шины. Сам запрос
                // при этом остаётся без ответа котлу — так же ведёт себя
                // и апстрим-проект.
                while (_remoteSerial.available()) _remoteSerial.read();
                delay(34);
                uint8_t rbuf[7];
                if (_waitPacket(_remoteSerial, rbuf)) _parsePacket(rbuf);
            }

            digitalWrite(MASTERGAS_REMOTE_EN_PIN, HIGH);
        }
    }

#ifdef MODULE_MQTT
    if (millis() - _lastPub >= masterGasCfg.pub_interval) {
        _lastPub = millis();
        _publishMqtt();
    }
#endif
}

#ifdef MODULE_MQTT
void MasterGasDevice::_publishMqtt() {
    JsonDocument doc;
    doc["burner"]     = masterGasState.burner_on;
    doc["coolant_t"]  = masterGasState.coolant_temp;
    doc["water_t"]    = masterGasState.water_temp;
    doc["r_power"]    = masterGasState.remote_power;
    doc["r_target_t"] = masterGasState.remote_target_temp;
    doc["r_air_t"]    = masterGasState.remote_air_temp;
    String payload; serializeJson(doc, payload);
    String topic = String(baseCfg.device_name) + "/mastergas/state";
    MqttHandler::publish(topic.c_str(), payload.c_str(), true);
}
#endif

// ── LED-индикация ─────────────────────────────────────────────────────────────
// Красный: heartbeat loop() в passthrough (1 раз/с) — доказывает, что основной
//   цикл жив, а не завис где-то до вотчдога. В override — частое мигание
//   (каждые 150мс), чтобы режим "плата реально управляет котлом" было видно
//   издалека без веб-интерфейса.
// Зелёный: здоровье шины котла — не горит (валидных пакетов не было вообще),
//   мигает (пакеты идут, но давно не свежие или много CRC-ошибок), горит ровно
//   (свежие пакеты, чисто).
// Синий: сеть — не горит (нет ни ETH, ни WiFi), мигает (AP-fallback, нет
//   аплинка), горит ровно (есть IP, веб поднят).
#define MASTERGAS_LED_STALE_MS      5000   // нет свежего пакета — считаем шину подвисшей
#define MASTERGAS_LED_ERR_PCT       3      // % CRC-ошибок, выше которого зелёный мигает
#define MASTERGAS_LED_BLINK_MS      300    // период мигания зелёного/синего

void MasterGasDevice::_ledTick() {
    uint32_t now = millis();

    // Красный — heartbeat/override
    static bool     ledR         = false;
    static uint32_t ledRToggleMs = 0;
    uint32_t redPeriod = masterGasCfg.passthrough ? 500 : 150;
    if (now - ledRToggleMs >= redPeriod) {
        ledRToggleMs = now;
        ledR = !ledR;
        digitalWrite(MASTERGAS_LED_R_PIN, ledR);
    }

    // Зелёный — здоровье шины котла
    bool boilerHeard = masterGasState.pkt_ok > 0;
    uint32_t totalPkts = masterGasState.pkt_ok + masterGasState.pkt_crc_err;
    bool boilerStale = !boilerHeard || (now - masterGasState.last_boiler_rx_ms > MASTERGAS_LED_STALE_MS);
    bool boilerNoisy = totalPkts > 0
        && (masterGasState.pkt_crc_err * 100 / totalPkts) > MASTERGAS_LED_ERR_PCT;

    static bool     ledG         = false;
    static uint32_t ledGToggleMs = 0;
    if (!boilerHeard) {
        digitalWrite(MASTERGAS_LED_G_PIN, LOW);
    } else if (boilerStale || boilerNoisy) {
        if (now - ledGToggleMs >= MASTERGAS_LED_BLINK_MS) {
            ledGToggleMs = now;
            ledG = !ledG;
            digitalWrite(MASTERGAS_LED_G_PIN, ledG);
        }
    } else {
        digitalWrite(MASTERGAS_LED_G_PIN, HIGH);
    }

    // Синий — сеть (sysState трогаем под мьютексом, как и весь остальной код)
    bool netUp, apMode;
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    netUp  = sysState.ethConnected || sysState.wifiConnected;
    apMode = sysState.apMode;
    xSemaphoreGive(coreMutex);

    static bool     ledB         = false;
    static uint32_t ledBToggleMs = 0;
    if (netUp) {
        digitalWrite(MASTERGAS_LED_B_PIN, HIGH);
    } else if (apMode) {
        if (now - ledBToggleMs >= MASTERGAS_LED_BLINK_MS) {
            ledBToggleMs = now;
            ledB = !ledB;
            digitalWrite(MASTERGAS_LED_B_PIN, ledB);
        }
    } else {
        digitalWrite(MASTERGAS_LED_B_PIN, LOW);
    }
}

#endif // DEVICE_MASTERGAS
