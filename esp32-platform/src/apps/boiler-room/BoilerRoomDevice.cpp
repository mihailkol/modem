#if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)

#include "BoilerRoomDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include "../../devices/kc868-a16/A16Types.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif

#ifdef DEVICE_EKONOM
#include "../../devices/ekonom/EkoNomDevice.h"
#endif

// ============================================================
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM) && defined(MODULE_DATACHANNEL)
 
#include "../../core/DataChannel.h"
#include "BoilerRoomDevice.h"
 
// ── Теплосчётчик EkoNom ──────────────────────────────────────
// Данные актуальны только при heat_valid; логгер получит 0 если
// EkoNom не отвечал — это допустимо, avg за минуту всё равно
// посчитается корректно при наличии хотя бы одного валидного чтения.
// Для критичности можно добавить проверку heat_valid в геттере.
DECLARE_CHANNEL(brr_heat_t_supply,
    "brr.heat.t_supply",  "Теплосчётчик подача",   "°C",   CH_FLOAT, 10,
    []() -> float { return brrState.heat_valid ? brrState.heat_t_supply : NAN; }
);
DECLARE_CHANNEL(brr_heat_t_return,
    "brr.heat.t_return",  "Теплосчётчик обратка",  "°C",   CH_FLOAT, 10,
    []() -> float { return brrState.heat_valid ? brrState.heat_t_return : NAN; }
);
DECLARE_CHANNEL(brr_heat_t_delta,
    "brr.heat.t_delta",   "Теплосчётчик ΔT",       "°C",   CH_FLOAT, 10,
    []() -> float { return brrState.heat_valid ? brrState.heat_t_delta : NAN; }
);
DECLARE_CHANNEL(brr_heat_flow,
    "brr.heat.flow_m3h",  "Расход теплоносителя",  "м³/ч", CH_FLOAT, 1000,
    []() -> float { return brrState.heat_valid ? brrState.heat_flow_m3h : NAN; }
);
DECLARE_CHANNEL(brr_heat_power,
    "brr.heat.power_kw",  "Тепловая мощность",     "кВт",  CH_FLOAT, 100,
    []() -> float { return brrState.heat_valid ? brrState.heat_power_kw : NAN; }
);
// Накопленная энергия и объём — монотонные счётчики
DECLARE_CHANNEL(brr_heat_energy,
    "brr.heat.energy_kwh","Тепловая энергия",      "кВт·ч",CH_COUNTER, 1,
    []() -> float { return brrState.heat_energy_kwh; }
);
DECLARE_CHANNEL(brr_heat_volume,
    "brr.heat.volume_m3", "Объём теплоносителя",   "м³",   CH_COUNTER, 1,
    []() -> float { return brrState.heat_volume_m3; }
);
 
// ── Давление ─────────────────────────────────────────────────
DECLARE_CHANNEL(brr_p_heat,
    "brr.p_heat",  "Давление отопления", "бар", CH_FLOAT, 100,
    []() -> float { return brrState.p_heat; }
);
DECLARE_CHANNEL(brr_p_water,
    "brr.p_water", "Давление ХВС",       "бар", CH_FLOAT, 100,
    []() -> float { return brrState.p_water; }
);
 
// ── Расход воды (мгновенный) ─────────────────────────────────
DECLARE_CHANNEL(brr_water_rate,
    "brr.water.rate_lpm", "Расход воды", "л/мин", CH_FLOAT, 100,
    []() -> float { return brrState.water_rate_lpm; }
);
// Счётчик воды — накопленный объём
DECLARE_CHANNEL(brr_water_total,
    "brr.water.total_m3", "Счётчик воды", "м³", CH_COUNTER, 1,
    []() -> float { return brrState.water_total_m3; }
);
 
// ── Расход газа (мгновенный) ─────────────────────────────────
DECLARE_CHANNEL(brr_gas_rate,
    "brr.gas.rate_m3h",   "Расход газа",   "м³/ч", CH_FLOAT, 1000,
    []() -> float { return brrState.gas_rate_m3h; }
);
// Счётчик газа — накопленный объём
DECLARE_CHANNEL(brr_gas_total,
    "brr.gas.total_m3",   "Счётчик газа",  "м³",   CH_COUNTER, 1,
    []() -> float { return brrState.gas_total_m3; }
);
 
#endif // DEVICE_KC868_A16 && APP_BOILER_ROOM && MODULE_DATACHANNEL

// ── Глобальные данные ─────────────────────────────────────────────────────────
BoilerRoomConfig brrCfg;
BoilerRoomState  brrState;

uint32_t BoilerRoomDevice::_lastCalc = 0;
uint32_t BoilerRoomDevice::_lastPub  = 0;

// ============================================================
//  HTML — вкладка мониторинга
// ============================================================
static const char BRR_MONITOR_HTML[] PROGMEM = R"html(
<style>
.brr-page { display:flex; flex-direction:column; gap:10px; }

.brr-card {
  background: var(--card);
  border-radius: 10px;
  padding: 16px;
  border: 1px solid var(--border);
}
.brr-label {
  font-size: 10px; font-weight: 700;
  letter-spacing: .12em; text-transform: uppercase;
  color: var(--muted); margin-bottom: 12px;
}

/* Сетка 2 колонки */
.brr-grid2 {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 8px;
}
.brr-cell {
  background: #111318;
  border: 1px solid var(--border);
  border-radius: 8px;
  padding: 10px 12px;
}
.brr-cell-lbl {
  font-size: 10px; color: var(--muted);
  margin-bottom: 4px;
}
.brr-val {
  font-size: 22px; font-weight: 800; color: var(--accent);
  line-height: 1.1;
}
.brr-val.cool  { color: var(--cool); }
.brr-val.hot   { color: var(--hot); }
.brr-val.ok    { color: var(--ok); }
.brr-unit {
  font-size: 11px; color: var(--muted); margin-left: 2px;
}

/* Большой показатель — мощность теплосчётчика */
.brr-big {
  text-align: center; padding: 4px 0 8px;
}
.brr-big-val {
  font-size: 52px; font-weight: 900;
  color: var(--accent); line-height: 1;
}
.brr-big-unit {
  font-size: 14px; color: var(--muted);
}
.brr-big-sub {
  font-size: 12px; color: var(--muted);
  margin-top: 6px;
}

/* Разделительная строка внутри карточки */
.brr-row {
  display: flex; justify-content: space-between;
  align-items: baseline;
  padding: 5px 0;
  border-bottom: 1px solid var(--border);
  font-size: 13px;
}
.brr-row:last-child { border-bottom: none; }
.brr-row-lbl { color: var(--muted); }
.brr-row-val { font-weight: 700; }

/* Статус свежести данных */
.brr-age {
  font-size: 11px; color: var(--muted);
  text-align: right; margin-top: 8px;
}
.brr-age.stale { color: var(--danger); }
</style>

<div class="brr-page">

  <!-- Теплосчётчик: мощность крупно -->
  <div class="brr-card">
    <div class="brr-label">🔥 ТЕПЛОВАЯ ЭНЕРГИЯ</div>
    <div class="brr-big">
      <div>
        <span class="brr-big-val" id="brr_power">--</span>
        <span class="brr-big-unit"> кВт</span>
      </div>
      <div class="brr-big-sub">
        Накоплено: <b id="brr_energy">--</b> кВт·ч
        &nbsp;|&nbsp;
        Объём: <b id="brr_vol">--</b> м³
      </div>
    </div>

    <div class="brr-grid2" style="margin-top:10px">
      <div class="brr-cell">
        <div class="brr-cell-lbl">Подача</div>
        <div><span class="brr-val hot" id="brr_ts">--</span><span class="brr-unit">°C</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Обратка</div>
        <div><span class="brr-val cool" id="brr_tr">--</span><span class="brr-unit">°C</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Δ температур</div>
        <div><span class="brr-val" id="brr_dt">--</span><span class="brr-unit">°C</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Расход тепл.</div>
        <div><span class="brr-val" id="brr_flow">--</span><span class="brr-unit">м³/ч</span></div>
      </div>
    </div>
    <div class="brr-age" id="brr_heat_age">—</div>
  </div>

  <!-- Давление -->
  <div class="brr-card">
    <div class="brr-label">🌡 ДАВЛЕНИЕ</div>
    <div class="brr-grid2">
      <div class="brr-cell">
        <div class="brr-cell-lbl">Отопление</div>
        <div><span class="brr-val ok" id="brr_ph">--</span><span class="brr-unit">бар</span></div>
      </div>
      <div class="brr-cell">
        <div class="brr-cell-lbl">Вода ХВС</div>
        <div><span class="brr-val ok" id="brr_pw">--</span><span class="brr-unit">бар</span></div>
      </div>
    </div>
  </div>

  <!-- Счётчики -->
  <div class="brr-card">
    <div class="brr-label">💧 ВОДОСНАБЖЕНИЕ</div>
    <div class="brr-row">
      <span class="brr-row-lbl">Расход</span>
      <span class="brr-row-val"><span id="brr_wlpm">--</span> л/мин</span>
    </div>
    <div class="brr-row">
      <span class="brr-row-lbl">Накоплено</span>
      <span class="brr-row-val"><span id="brr_wtotal">--</span> м³</span>
    </div>
  </div>

  <div class="brr-card">
    <div class="brr-label">🔵 ГАЗ</div>
    <div class="brr-row">
      <span class="brr-row-lbl">Расход</span>
      <span class="brr-row-val"><span id="brr_gm3h">--</span> м³/ч</span>
    </div>
    <div class="brr-row">
      <span class="brr-row-lbl">Накоплено</span>
      <span class="brr-row-val"><span id="brr_gtotal">--</span> м³</span>
    </div>
  </div>

  <!-- История -->
  <div class="brr-card" id="hist-panel">

    <!-- Заголовок-переключатель -->
    <div class="brr-label" id="hist-toggle" style="cursor:pointer;user-select:none;margin-bottom:0;display:flex;justify-content:space-between;align-items:center;">
      <span>📈 ИСТОРИЯ</span>
      <span id="hist-arrow" style="font-size:14px;transition:transform .2s">▼</span>
    </div>

    <!-- Тело панели (скрыто по умолчанию) -->
    <div id="hist-body" style="display:none;margin-top:12px">

      <!-- Масштаб + чекбокс диапазона -->
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:10px;flex-wrap:wrap;gap:6px;">
        <div style="display:flex;gap:4px;">
          <button class="hist-scale-btn active" data-level="0" data-last="720">2ч</button>
          <button class="hist-scale-btn" data-level="1" data-last="1440">24ч</button>
          <button class="hist-scale-btn" data-level="2" data-last="2016">2нед</button>
        </div>
        <label style="display:flex;align-items:center;gap:6px;font-size:12px;color:var(--muted);cursor:pointer;">
          <input type="checkbox" id="hist-range-cb" style="cursor:pointer;"> диапазон
        </label>
      </div>

      <!-- Список каналов -->
      <div id="hist-channels" style="display:flex;flex-direction:column;gap:4px;margin-bottom:12px;"></div>

      <!-- Canvas графика -->
      <div style="position:relative;height:220px;">
        <canvas id="hist-chart"></canvas>
        <div id="hist-empty" style="display:none;position:absolute;inset:0;display:flex;align-items:center;justify-content:center;color:var(--muted);font-size:13px;">Нет данных</div>
      </div>

    </div>
  </div>

<style>
.hist-scale-btn {
  background: #111318; border: 1px solid var(--border);
  color: var(--muted); border-radius: 6px;
  padding: 4px 12px; font-size: 12px; cursor: pointer;
  transition: background .15s, color .15s;
  width: auto !important;
}
.hist-scale-btn.active {
  background: var(--accent); color: #000;
  border-color: var(--accent);
}
.hist-ch-row {
  display: flex; align-items: center; gap:8px;
  font-size: 12px; color: var(--text); cursor: pointer;
}
.hist-ch-dot {
  width: 10px; height: 10px; border-radius: 50%;
  flex-shrink: 0;
}
</style>

<script>
(function() {

// ── Палитра линий ────────────────────────────────────────────
const COLORS = [
  '#4fc3f7','#ef5350','#66bb6a','#ffa726',
  '#ab47bc','#26c6da','#d4e157','#ff7043',
  '#42a5f5','#ec407a','#26a69a','#8d6e63'
];

// ── Состояние ────────────────────────────────────────────────
let chart       = null;
let chartJsPromise = null;
let channels    = [];      // [{id, label, unit, type, color, enabled}]
let currentLevel = 0;
let currentLast  = 720;
let showRange    = false;
let panelOpen    = false;

// ── Переключатель панели ─────────────────────────────────────
document.getElementById('hist-toggle').addEventListener('click', () => {
  panelOpen = !panelOpen;
  document.getElementById('hist-body').style.display = panelOpen ? 'block' : 'none';
  document.getElementById('hist-arrow').style.transform = panelOpen ? 'rotate(180deg)' : '';
  if (panelOpen && channels.length === 0) initHistory();
});

// ── Кнопки масштаба ──────────────────────────────────────────
document.querySelectorAll('.hist-scale-btn').forEach(btn => {
  btn.addEventListener('click', () => {
    document.querySelectorAll('.hist-scale-btn').forEach(b => b.classList.remove('active'));
    btn.classList.add('active');
    currentLevel = +btn.dataset.level;
    currentLast  = +btn.dataset.last;
    loadAndDraw();
  });
});

// ── Чекбокс диапазона ────────────────────────────────────────
document.getElementById('hist-range-cb').addEventListener('change', e => {
  showRange = e.target.checked;
  loadAndDraw();
});

// ── Инициализация: загружаем каналы, потом Chart.js ──────────
async function initHistory() {
  try {
    const resp = await fetch('/api/history/channels');
    const list = await resp.json();

    // Берём только history:true и enabled:true
    channels = list
      .filter(ch => ch.history && ch.enabled)
      .map((ch, i) => ({
        ...ch,
        color:   COLORS[i % COLORS.length],
        visible: ch.type === 0   // по умолчанию показываем только CH_FLOAT
      }));

    renderChannelList();
    loadChartJs();
  } catch(e) {
    console.error('[HIST] init error', e);
  }
}

// ── Список каналов с чекбоксами ──────────────────────────────
function renderChannelList() {
  const el = document.getElementById('hist-channels');
  el.innerHTML = '';
  channels.forEach((ch, i) => {
    const row = document.createElement('label');
    row.className = 'hist-ch-row';
    row.innerHTML = `
      <input type="checkbox" ${ch.visible ? 'checked' : ''} style="cursor:pointer;">
      <span class="hist-ch-dot" style="background:${ch.color}"></span>
      <span>${ch.label}</span>
      <span style="color:var(--muted);font-size:11px;margin-left:auto">${ch.unit}</span>
    `;
    row.querySelector('input').addEventListener('change', e => {
      channels[i].visible = e.target.checked;
      loadAndDraw();
    });
    el.appendChild(row);
  });
}

function loadChartJs() {
    if (chartJsPromise) return chartJsPromise;
    chartJsPromise = new Promise((resolve) => {
        if (window.Chart) { resolve(); return; }
        const s = document.createElement('script');
        s.src = 'https://cdnjs.cloudflare.com/ajax/libs/Chart.js/4.4.1/chart.umd.min.js';
        s.onload = resolve;
        document.head.appendChild(s);
    });
    return chartJsPromise;
}

async function loadAndDraw() {
    await loadChartJs();
    try {
        const url = `/api/history?level=${currentLevel}&last=${currentLast}`;
        const data = await (await fetch(url)).json();
        drawChart(data);
    } catch(e) {
        console.error('[HIST] load error', e);
    }
}

// ── Нормализация 0-1 по массиву ──────────────────────────────
function normalize(arr) {
  const valid = arr.filter(v => v !== null && !isNaN(v));
  if (valid.length === 0) return arr.map(() => null);
  const mn = Math.min(...valid);
  const mx = Math.max(...valid);
  const range = mx - mn;
  if (range === 0) return arr.map(v => v === null ? null : 0.5);
  return arr.map(v => v === null ? null : (v - mn) / range);
}

// ── Форматирование метки времени ─────────────────────────────
function fmtTs(ts) {
  if (!ts) return '?';
  const d = new Date(ts * 1000);
  const pad = n => String(n).padStart(2,'0');
  if (currentLevel === 2) {
    // для 2 недель показываем дату
    return `${pad(d.getDate())}.${pad(d.getMonth()+1)} ${pad(d.getHours())}:${pad(d.getMinutes())}`;
  }
  return `${pad(d.getHours())}:${pad(d.getMinutes())}`;
}

// ── Отрисовка графика ────────────────────────────────────────
function drawChart(data) {
  const emptyEl = document.getElementById('hist-empty');

  if (!data.ts || data.ts.length === 0) {
    emptyEl.style.display = 'flex';
    if (chart) { chart.destroy(); chart = null; }
    return;
  }
  emptyEl.style.display = 'none';

  const labels = data.ts.map(fmtTs);
  const datasets = [];

  channels.forEach(ch => {
    if (!ch.visible) return;

    // Найти канал в ответе
    const src = data.channels.find(c => c.id === ch.id);
    if (!src) return;

    const rawValues = ch.type === 0
      ? (src.avg || src.values || [])
      : (src.values || []);

    const normValues = normalize(rawValues);

    // Основная линия
    datasets.push({
      label:           ch.label,
      data:            normValues,
      borderColor:     ch.color,
      backgroundColor: ch.color + '22',
      borderWidth:     1.5,
      pointRadius:     0,
      pointHoverRadius:4,
      tension:         0.3,
      fill:            false,
      _raw:            rawValues,
      _unit:           ch.unit,
      _chType:         ch.type,
    });

    // Полоса диапазона min/max для CH_FLOAT
    if (showRange && ch.type === 0 && src.min && src.max) {
      const normMin = normalize(src.min);
      const normMax = normalize(src.max);

      datasets.push({
        label:           ch.label + ' min',
        data:            normMin,
        borderColor:     'transparent',
        backgroundColor: ch.color + '18',
        borderWidth:     0,
        pointRadius:     0,
        fill:            '+1',  // fill до следующего dataset (max)
        tension:         0.3,
        _hidden:         true,
      });
      datasets.push({
        label:           ch.label + ' max',
        data:            normMax,
        borderColor:     'transparent',
        backgroundColor: ch.color + '18',
        borderWidth:     0,
        pointRadius:     0,
        fill:            false,
        tension:         0.3,
        _hidden:         true,
      });
    }
  });

  const ctx = document.getElementById('hist-chart').getContext('2d');

  if (chart) { chart.destroy(); }

  chart = new Chart(ctx, {
    type: 'line',
    data: { labels, datasets },
    options: {
      responsive:          true,
      maintainAspectRatio: false,
      animation:           { duration: 200 },
      interaction: { mode: 'index', intersect: false },
      plugins: {
        legend: { display: false },
        tooltip: {
          backgroundColor: '#1a1d23',
          borderColor:     '#2a2d35',
          borderWidth:     1,
          titleColor:      '#aaa',
          bodyColor:       '#eee',
          padding:         10,
          callbacks: {
            label: ctx => {
              const ds = ctx.dataset;
              if (ds._hidden) return null;
              const raw = ds._raw ? ds._raw[ctx.dataIndex] : null;
              if (raw === null || raw === undefined) return null;
              return ` ${ds.label}: ${Number(raw).toFixed(2)} ${ds._unit || ''}`;
            }
          }
        }
      },
      scales: {
        x: {
          ticks: {
            color:    '#666',
            font:     { size: 10 },
            maxTicksLimit: 8,
            maxRotation: 0,
          },
          grid: { color: '#1e2128' }
        },
        y: {
          display: false,   // ось Y скрыта — данные нормализованы
          min: -0.05,
          max:  1.05,
        }
      }
    }
  });
}

})();
</script>

</div>

<script>
function fmt(v, dec) {
  return (v === null || v === undefined || isNaN(v)) ? '—' : Number(v).toFixed(dec);
}

async function brrUpdate() {
  try {
    const d = await (await fetch('/api/brr/data')).json();

    // Теплосчётчик
    document.getElementById('brr_power').textContent  = fmt(d.heat_power_kw, 1);
    document.getElementById('brr_energy').textContent = fmt(d.heat_energy_kwh, 1);
    document.getElementById('brr_vol').textContent    = fmt(d.heat_volume_m3, 2);
    document.getElementById('brr_ts').textContent     = fmt(d.heat_t_supply, 1);
    document.getElementById('brr_tr').textContent     = fmt(d.heat_t_return, 1);
    document.getElementById('brr_dt').textContent     = fmt(d.heat_t_delta, 1);
    document.getElementById('brr_flow').textContent   = fmt(d.heat_flow_m3h, 3);

    // Возраст данных теплосчётчика
    const ageEl = document.getElementById('brr_heat_age');
    if (d.heat_valid && d.heat_age_sec !== undefined) {
      const age = d.heat_age_sec;
      ageEl.textContent = 'Обновлено ' + age + ' с назад';
      ageEl.className   = 'brr-age' + (age > 120 ? ' stale' : '');
    } else {
      ageEl.textContent = 'Нет данных с теплосчётчика';
      ageEl.className   = 'brr-age stale';
    }

    // Давление
    document.getElementById('brr_ph').textContent = fmt(d.p_heat, 2);
    document.getElementById('brr_pw').textContent = fmt(d.p_water, 2);

    // Вода
    document.getElementById('brr_wlpm').textContent   = fmt(d.water_rate_lpm, 2);
    document.getElementById('brr_wtotal').textContent = fmt(d.water_total_m3, 3);

    // Газ
    document.getElementById('brr_gm3h').textContent   = fmt(d.gas_rate_m3h, 3);
    document.getElementById('brr_gtotal').textContent = fmt(d.gas_total_m3, 3);

  } catch(e) {}
}

brrUpdate();
setInterval(brrUpdate, 3000);
</script>
)html";

// ============================================================
//  HTML — вкладка настроек
// ============================================================
static const char BRR_SETTINGS_HTML[] PROGMEM = R"html(
<style>
.brs-card {
  background: var(--card); border-radius: 10px;
  padding: 16px; border: 1px solid var(--border);
  margin-bottom: 10px;
}
.brs-label {
  font-size: 10px; font-weight: 700;
  letter-spacing: .12em; text-transform: uppercase;
  color: var(--muted); margin-bottom: 12px;
}
.brs-row {
  display: flex; align-items: center;
  justify-content: space-between;
  color: var(--text); font-size: 14px;
  margin-bottom: 10px;
}
.brs-row input {
  width: 110px; background: #111318;
  border: 1px solid var(--border); border-radius: 6px;
  padding: 6px 8px; color: var(--text); font-size: 13px;
  text-align: right;
}
</style>

<div class="brs-card">
  <div class="brs-label">⚙️ КОЭФФИЦИЕНТЫ СЧЁТЧИКОВ</div>
  <div class="brs-row">
    <span>Вода, м³/импульс</span>
    <input type="number" id="water_kf" step="0.001" min="0.001">
  </div>
  <div class="brs-row">
    <span>Газ, м³/импульс</span>
    <input type="number" id="gas_kf" step="0.001" min="0.001">
  </div>
</div>

<div class="brs-card">
  <div class="brs-label">⚙️ ДАВЛЕНИЕ ОТОПЛЕНИЯ (4-20мА)</div>
  <div class="brs-row">
    <span>4мА = 0 бар (min)</span>
    <input type="number" id="ph_min" step="0.1">
  </div>
  <div class="brs-row">
    <span>20мА = N бар (max)</span>
    <input type="number" id="ph_max" step="0.1">
  </div>
</div>

<div class="brs-card">
  <div class="brs-label">⚙️ ДАВЛЕНИЕ ВОДЫ ХВС (4-20мА)</div>
  <div class="brs-row">
    <span>4мА = 0 бар (min)</span>
    <input type="number" id="pw_min" step="0.1">
  </div>
  <div class="brs-row">
    <span>20мА = N бар (max)</span>
    <input type="number" id="pw_max" step="0.1">
  </div>
</div>

<div class="brs-card">
  <div class="brs-label">⚙️ ОБЩИЕ</div>
  <div class="brs-row">
    <span>Интервал MQTT, сек</span>
    <input type="number" id="pub_interval" step="1" min="5">
  </div>
</div>

<button onclick="brsSave()">💾 Сохранить</button>
<div class="info-row" id="brs_msg"></div>

<script>
async function brsLoad() {
  const d = await (await fetch('/api/brr/config')).json();
  document.getElementById('water_kf').value    = d.water_m3_per_pulse;
  document.getElementById('gas_kf').value      = d.gas_m3_per_pulse;
  document.getElementById('ph_min').value      = d.p_heat_min;
  document.getElementById('ph_max').value      = d.p_heat_max;
  document.getElementById('pw_min').value      = d.p_water_min;
  document.getElementById('pw_max').value      = d.p_water_max;
  document.getElementById('pub_interval').value = Math.round(d.pub_interval / 1000);
}

async function brsSave() {
  const body = {
    water_m3_per_pulse: +document.getElementById('water_kf').value,
    gas_m3_per_pulse:   +document.getElementById('gas_kf').value,
    p_heat_min:         +document.getElementById('ph_min').value,
    p_heat_max:         +document.getElementById('ph_max').value,
    p_water_min:        +document.getElementById('pw_min').value,
    p_water_max:        +document.getElementById('pw_max').value,
    pub_interval:       +document.getElementById('pub_interval').value * 1000,
  };
  const r = await fetch('/api/brr/save', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(body)
  });
  document.getElementById('brs_msg').textContent = r.ok ? '✅ Сохранено' : '❌ Ошибка';
  setTimeout(() => document.getElementById('brs_msg').textContent = '', 3000);
}

brsLoad();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BoilerRoomDevice::init() {
    loadConfig();

    // Регистрируем вкладки
    WebHandler::registerTab({"monitor", "Мониторинг", "🏠", BRR_MONITOR_HTML});
    WebHandler::registerTab({"brr_cfg", "Уставки",    "⚙️",  BRR_SETTINGS_HTML});

    extern AsyncWebServer server;

    // ── /api/brr/data — данные мониторинга ───────────────────
    server.on("/api/brr/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;

        // Теплосчётчик
        doc["heat_t_supply"]   = brrState.heat_t_supply;
        doc["heat_t_return"]   = brrState.heat_t_return;
        doc["heat_t_delta"]    = brrState.heat_t_delta;
        doc["heat_flow_m3h"]   = brrState.heat_flow_m3h;
        doc["heat_power_kw"]   = brrState.heat_power_kw;
        doc["heat_energy_kwh"] = brrState.heat_energy_kwh;
        doc["heat_volume_m3"]  = brrState.heat_volume_m3;
        doc["heat_valid"]      = brrState.heat_valid;
        if (brrState.heat_valid && brrState.heat_updated_ms > 0)
            doc["heat_age_sec"] = (millis() - brrState.heat_updated_ms) / 1000;

        // Давление
        doc["p_heat"]  = brrState.p_heat;
        doc["p_water"] = brrState.p_water;

        // Вода
        doc["water_rate_lpm"]  = brrState.water_rate_lpm;
        doc["water_total_m3"]  = brrState.water_total_m3;

        // Газ
        doc["gas_rate_m3h"]  = brrState.gas_rate_m3h;
        doc["gas_total_m3"]  = brrState.gas_total_m3;

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/brr/config — чтение конфига ─────────────────────
    server.on("/api/brr/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["water_m3_per_pulse"] = brrCfg.water_m3_per_pulse;
        doc["gas_m3_per_pulse"]   = brrCfg.gas_m3_per_pulse;
        doc["p_heat_min"]         = brrCfg.p_heat_min;
        doc["p_heat_max"]         = brrCfg.p_heat_max;
        doc["p_water_min"]        = brrCfg.p_water_min;
        doc["p_water_max"]        = brrCfg.p_water_max;
        doc["pub_interval"]       = brrCfg.pub_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/brr/save — сохранение конфига ───────────────────
    AsyncCallbackJsonWebHandler* saveHandler =
        new AsyncCallbackJsonWebHandler("/api/brr/save",
            [](AsyncWebServerRequest* req, JsonVariant& json) {
                JsonObject obj = json.as<JsonObject>();
                brrCfg.water_m3_per_pulse = obj["water_m3_per_pulse"] | brrCfg.water_m3_per_pulse;
                brrCfg.gas_m3_per_pulse   = obj["gas_m3_per_pulse"]   | brrCfg.gas_m3_per_pulse;
                brrCfg.p_heat_min         = obj["p_heat_min"]         | brrCfg.p_heat_min;
                brrCfg.p_heat_max         = obj["p_heat_max"]         | brrCfg.p_heat_max;
                brrCfg.p_water_min        = obj["p_water_min"]        | brrCfg.p_water_min;
                brrCfg.p_water_max        = obj["p_water_max"]        | brrCfg.p_water_max;
                brrCfg.pub_interval       = obj["pub_interval"]       | brrCfg.pub_interval;
                BoilerRoomDevice::saveConfig();
                req->send(200, "application/json", "{\"ok\":true}");
            });
    server.addHandler(saveHandler);

    Serial.println("[BRR] Init OK");
}

// ============================================================
//  LOOP
// ============================================================
void BoilerRoomDevice::loop() {
    uint32_t now = millis();

    // Каждые 2 секунды: опрос A16 + пересчёт расходов
    if (now - _lastCalc >= 2000) {
        uint32_t dt_ms = now - _lastCalc;
        _lastCalc = now;
        _updateFromA16();
        _updateFromEkoNom();
        _calcRates(dt_ms);
    }

    // MQTT публикация
#ifdef MODULE_MQTT
    if (now - _lastPub >= brrCfg.pub_interval) {
        _lastPub = now;
        _publishMqtt();
    }
#endif
}

// ============================================================
//  ДАННЫЕ ИЗ A16
// ============================================================
void BoilerRoomDevice::_updateFromA16() {
    extern A16State a16State;
    extern A16Config a16Cfg;

    // ── Давление (4-20мА → бар) ──────────────────────────────
    // adc_value уже масштабирован A16Device через adc_min/adc_max из конфига.
    // Но у нас свои названия диапазонов — берём напрямую из adc_value,
    // предполагая что в A16Config выставлены верные adc_min/adc_max.
    brrState.p_heat  = a16State.adc_value[BRR_ADC_P_HEAT];
    brrState.p_water = a16State.adc_value[BRR_ADC_P_WATER];

    // ── Счётчики импульсов ────────────────────────────────────
    brrState.water_pulses = a16State.din_counter[BRR_DIN_WATER];
    brrState.gas_pulses   = a16State.din_counter[BRR_DIN_GAS];
}

// ============================================================
//  ДАННЫЕ ИЗ ТЕПЛОСЧЁТЧИКА
// ============================================================
void BoilerRoomDevice::_updateFromEkoNom() {
#ifdef DEVICE_EKONOM
    extern EkoNomData ekoNomData;
    if (!ekoNomData.valid) return;

    brrState.heat_t_supply   = ekoNomData.t_supply;
    brrState.heat_t_return   = ekoNomData.t_return;
    brrState.heat_t_delta    = ekoNomData.t_delta;
    brrState.heat_flow_m3h   = ekoNomData.flow_m3h;
    brrState.heat_power_kw   = ekoNomData.power_kw;
    brrState.heat_energy_kwh = ekoNomData.energy_kwh;
    brrState.heat_volume_m3  = ekoNomData.volume_m3;
    brrState.heat_valid      = true;
    brrState.heat_updated_ms = ekoNomData.updated_ms;
#endif
}

// ============================================================
//  РАСЧЁТ РАСХОДОВ ПО СЧЁТЧИКАМ
// ============================================================
void BoilerRoomDevice::_calcRates(uint32_t dt_ms) {
    float dt_min = dt_ms / 60000.0f;
    float dt_h   = dt_ms / 3600000.0f;

    // Вода
    uint32_t w_delta = brrState.water_pulses - brrState.water_pulses_prev;
    brrState.water_pulses_prev = brrState.water_pulses;
    brrState.water_total_m3   += w_delta * brrCfg.water_m3_per_pulse;
    brrState.water_rate_lpm    = (dt_min > 0)
        ? (w_delta * brrCfg.water_m3_per_pulse * 1000.0f) / dt_min
        : 0.0f;

    // Газ
    uint32_t g_delta = brrState.gas_pulses - brrState.gas_pulses_prev;
    brrState.gas_pulses_prev = brrState.gas_pulses;
    brrState.gas_total_m3   += g_delta * brrCfg.gas_m3_per_pulse;
    brrState.gas_rate_m3h    = (dt_h > 0)
        ? (g_delta * brrCfg.gas_m3_per_pulse) / dt_h
        : 0.0f;
}

// ============================================================
//  MQTT ПУБЛИКАЦИЯ
// ============================================================
void BoilerRoomDevice::_publishMqtt() {
#ifdef MODULE_MQTT
    String b = String(baseCfg.device_name) + "/brr/";
    char buf[16];

    // Теплосчётчик
    if (brrState.heat_valid) {
        dtostrf(brrState.heat_t_supply,   1, 1, buf); MqttHandler::publish((b+"heat/t_supply").c_str(),   buf);
        dtostrf(brrState.heat_t_return,   1, 1, buf); MqttHandler::publish((b+"heat/t_return").c_str(),   buf);
        dtostrf(brrState.heat_t_delta,    1, 1, buf); MqttHandler::publish((b+"heat/t_delta").c_str(),    buf);
        dtostrf(brrState.heat_flow_m3h,   1, 4, buf); MqttHandler::publish((b+"heat/flow_m3h").c_str(),   buf);
        dtostrf(brrState.heat_power_kw,   1, 2, buf); MqttHandler::publish((b+"heat/power_kw").c_str(),   buf);
        dtostrf(brrState.heat_energy_kwh, 1, 1, buf); MqttHandler::publish((b+"heat/energy_kwh").c_str(), buf);
        dtostrf(brrState.heat_volume_m3,  1, 2, buf); MqttHandler::publish((b+"heat/volume_m3").c_str(),  buf);
    }

    // Давление
    dtostrf(brrState.p_heat,  1, 2, buf); MqttHandler::publish((b+"p_heat").c_str(),  buf);
    dtostrf(brrState.p_water, 1, 2, buf); MqttHandler::publish((b+"p_water").c_str(), buf);

    // Вода
    dtostrf(brrState.water_rate_lpm, 1, 2, buf); MqttHandler::publish((b+"water/rate_lpm").c_str(),  buf);
    dtostrf(brrState.water_total_m3, 1, 3, buf); MqttHandler::publish((b+"water/total_m3").c_str(),  buf);

    // Газ
    dtostrf(brrState.gas_rate_m3h, 1, 3, buf); MqttHandler::publish((b+"gas/rate_m3h").c_str(),  buf);
    dtostrf(brrState.gas_total_m3, 1, 3, buf); MqttHandler::publish((b+"gas/total_m3").c_str(),   buf);
#endif
}

// ============================================================
//  CONFIG
// ============================================================
bool BoilerRoomDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/boilerroom.json", doc)) return false;
    brrCfg.water_m3_per_pulse = doc["water_m3_per_pulse"] | 0.01f;
    brrCfg.gas_m3_per_pulse   = doc["gas_m3_per_pulse"]   | 0.01f;
    brrCfg.p_heat_min         = doc["p_heat_min"]         | 0.0f;
    brrCfg.p_heat_max         = doc["p_heat_max"]         | 10.0f;
    brrCfg.p_water_min        = doc["p_water_min"]        | 0.0f;
    brrCfg.p_water_max        = doc["p_water_max"]        | 10.0f;
    brrCfg.pub_interval       = doc["pub_interval"]       | 10000;
    return true;
}

bool BoilerRoomDevice::saveConfig() {
    JsonDocument doc;
    doc["water_m3_per_pulse"] = brrCfg.water_m3_per_pulse;
    doc["gas_m3_per_pulse"]   = brrCfg.gas_m3_per_pulse;
    doc["p_heat_min"]         = brrCfg.p_heat_min;
    doc["p_heat_max"]         = brrCfg.p_heat_max;
    doc["p_water_min"]        = brrCfg.p_water_min;
    doc["p_water_max"]        = brrCfg.p_water_max;
    doc["pub_interval"]       = brrCfg.pub_interval;
    return ConfigManager::saveJson("/boilerroom.json", doc);
}

#endif // DEVICE_KC868_A16 && APP_BOILER_ROOM