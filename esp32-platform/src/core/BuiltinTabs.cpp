#include "BuiltinTabs.h"
#include "WebHandler.h"
#include "CoreTypes.h"

// ============================================================
//  МОНИТОРИНГ — живые значения всех каналов Bus
// ============================================================
static const char MONITOR_TAB_HTML[] PROGMEM = R"html(
<div class="monitor-page" id="monPage">
  <div style="color:var(--muted);text-align:center;padding:20px" id="monLoading">Загрузка каналов...</div>
</div>

<style>
.mon-group{margin-bottom:10px}
.mon-group-title{font-size:10px;font-weight:700;letter-spacing:.1em;text-transform:uppercase;color:var(--muted);margin:14px 0 8px}
.mon-rows{display:flex;flex-direction:column;gap:1px;background:var(--border);border-radius:8px;overflow:hidden}
.mon-row{display:flex;align-items:center;justify-content:space-between;background:var(--card);padding:9px 12px;font-size:13px;gap:10px}
.mon-row .lbl{color:var(--text)}
.mon-row .val{font-weight:700;color:var(--accent);font-family:monospace;white-space:nowrap}
.mon-row .val.stale{color:var(--muted);font-weight:400}
.mon-edit{display:flex;gap:6px;align-items:center;flex-shrink:0}
.mon-edit input{width:56px!important;margin:0!important;padding:5px 7px!important;font-size:12px!important}
.mon-edit button{width:auto!important;margin:0!important;padding:5px 10px!important;font-size:11px!important}
.mon-manual{display:flex;align-items:center;gap:3px;font-size:11px;color:var(--muted);white-space:nowrap;cursor:pointer}
.mon-manual input{margin:0!important;width:auto!important}
.mon-row-gauge{flex-direction:column;align-items:stretch;gap:5px}
.mon-row-top{display:flex;align-items:center;justify-content:space-between;gap:10px}
.mon-gauge{height:5px;border-radius:3px;background:var(--border);overflow:hidden}
.mon-gauge-fill{height:100%;border-radius:3px;transition:width .4s,background-color .4s}
</style>

<script>
// Дашборд сгруппирован по физическим объектам котельной (не по типу
// измерения) — так оператору привычнее ориентироваться на месте.
// Всё, чего нет ни в одной секции (в первую очередь raw.* — некалиброванные
// значения с устройств, включая пока не откалиброванные NTC контуров)
// попадает в отдельную секцию "Прочее (raw)" в конце — полезно для отладки.
const MON_SECTIONS = [
  {title:'🔥 Котёл',                 ids:['t_supply','t_return','t_delta','flow_m3h','power_kw','energy_kwh','volume_m3','p_heat','gas_rate','gas_total_m3']},
  {title:'♨️ Контур отопления 1',    ids:['pump1_setpoint','pump1_feedback','pump1_t_supply','pump1_t_return','pump1_t_delta']},
  {title:'♨️ Контур отопления 2',    ids:['pump2_setpoint','pump2_feedback','pump2_t_supply','pump2_t_return','pump2_t_delta']},
  {title:'🚰 ХВС',                   ids:['water_total_m3','water_rate','water_rate_lph','p_water']},
  {title:'🌡 Температуры',           ids:['t_indoor','t_outdoor']},
  {title:'⚙️ Система',               ids:['uptime','free_heap','rssi','mqtt_status','ip_address','fw_version']},
];
const MON_SECTION_OF = {};
MON_SECTIONS.forEach(s => s.ids.forEach(id => MON_SECTION_OF[id] = s.title));

// label/юнит/формат/шкала для известных каналов (см. ТЗ п.4).
// gauge:{min,max} — включает полосу-индикатор под значением (диапазоны
// подобраны с Михаилом: отопление 10-90, помещение 10-40, улица -50-40
// с запасом на морозные зимы, дельта -5-40, насосы 0-100%).
const MON_META = {
  t_supply:   {l:'Подача',   u:'°C', gauge:{min:10,max:90}},
  t_return:   {l:'Обратка',  u:'°C', gauge:{min:10,max:90}},
  t_delta:    {l:'Дельта',   u:'°C', gauge:{min:-5,max:40}},
  flow_m3h:   {l:'Расход',   u:'м³/ч'},
  power_kw:   {l:'Мощность', u:'кВт'},
  energy_kwh: {l:'Энергия',  u:'кВт·ч'},
  volume_m3:  {l:'Объём',    u:'м³'},

  p_heat:  {l:'Давление отопления', u:'бар'},
  p_water: {l:'Давление ХВС',       u:'бар'},

  gas_total_m3: {l:'Газ, всего',   u:'м³'},
  gas_rate:     {l:'Газ, расход',  u:'м³/ч'},

  water_total_m3: {l:'Вода, всего',   u:'м³'},
  water_rate:     {l:'Вода, расход',  u:'л/мин'},
  water_rate_lph: {l:'Вода, расход',  u:'л/ч'},

  // pump*_feedback — жёлтый провод насоса Shimge APE, ПО ДАТАШИТУ это не
  // скорость, а потребляемая мощность (0-70Вт, ~1Вт/1% ШИМ) — независимая
  // от уставки величина, не обязана с ней совпадать. Значения выше ~70 —
  // вне спеки насоса, вероятно шум декодирования на Pro Mini, а не
  // реальная мощность (см. чат при вводе в эксплуатацию).
  pump1_setpoint: {l:'Уставка', u:'%', editable:true, gauge:{min:0,max:100}, manualId:'pump1_manual'},
  pump1_feedback: {l:'Потребление', u:'Вт', gauge:{min:0,max:70}},
  pump1_t_supply: {l:'Подача',  u:'°C', gauge:{min:10,max:90}},
  pump1_t_return: {l:'Обратка', u:'°C', gauge:{min:10,max:90}},
  pump1_t_delta:  {l:'Дельта',  u:'°C', gauge:{min:-5,max:40}},
  pump2_setpoint: {l:'Уставка', u:'%', editable:true, gauge:{min:0,max:100}, manualId:'pump2_manual'},
  pump2_feedback: {l:'Потребление', u:'Вт', gauge:{min:0,max:70}},
  pump2_t_supply: {l:'Подача',  u:'°C', gauge:{min:10,max:90}},
  pump2_t_return: {l:'Обратка', u:'°C', gauge:{min:10,max:90}},
  pump2_t_delta:  {l:'Дельта',  u:'°C', gauge:{min:-5,max:40}},

  t_outdoor: {l:'Улица',  u:'°C', gauge:{min:-50,max:40}},
  t_indoor:  {l:'В доме', u:'°C', gauge:{min:10,max:40}},

  uptime:      {l:'Аптайм',     fmt:'uptime'},
  free_heap:   {l:'Free heap',  u:'байт'},
  rssi:        {l:'WiFi RSSI',  u:'dBm'},
  mqtt_status: {l:'MQTT',       fmt:'bool'},
  ip_address:  {l:'IP',         fmt:'ip'},
  fw_version:  {l:'Версия ПО',  fmt:'fwver'},
};

function monFmtUptime(sec){
  sec = Math.floor(sec);
  const d = Math.floor(sec/86400); sec%=86400;
  const h = Math.floor(sec/3600);  sec%=3600;
  const m = Math.floor(sec/60);
  return (d?d+'д ':'')+h+'ч '+m+'м';
}
function monFmtIp(v){
  v = v>>>0;
  return [(v>>>24)&255,(v>>>16)&255,(v>>>8)&255,v&255].join('.');
}
function monFmtFwVer(v){
  return Math.floor(v/10000)+'.'+(Math.floor(v/100)%100)+'.'+(v%100);
}
function monDisplayValue(c, meta){
  if (!c.valid) return '—';
  if (meta.fmt==='uptime') return monFmtUptime(c.value/c.scale);
  if (meta.fmt==='ip')     return monFmtIp(c.value);
  if (meta.fmt==='fwver')  return monFmtFwVer(c.value);
  if (meta.fmt==='bool')   return c.value ? 'да' : 'нет';
  const num = c.scale > 1 ? (c.value / c.scale).toFixed(2) : String(c.value);
  return meta.u ? (num + ' ' + meta.u) : num;
}

// Полоса-индикатор под значением: заливка пропорционально позиции в
// диапазоне gauge.min..max, цвет плавно синий->красный (холодно->горячо).
// Для дельты и насосов диапазон свой, но механика та же — просто позиция
// в отрезке, без привязки к нулю.
function monGaugeFill(c, gauge){
  if (!c.valid) return {pct:0, color:'var(--border)'};
  const v = c.value / c.scale;
  const ratio = Math.max(0, Math.min(1, (v - gauge.min) / (gauge.max - gauge.min)));
  const hue = Math.round(210 - ratio*210);
  return {pct: (ratio*100).toFixed(1), color: 'hsl('+hue+',70%,50%)'};
}

let _monBuilt = 0;  // сколько каналов было при последней сборке DOM

function monManualHtml(meta, byId){
  if (!meta.manualId) return '';
  const mc = byId[meta.manualId];
  const checked = (mc && mc.valid && mc.value) ? 'checked' : '';
  return `<label class="mon-manual"><input type="checkbox" id="monmanual_${meta.manualId}" ${checked}
    onchange="monToggleManual('${meta.manualId}', this)"> ручной</label>`;
}

function monRowHtml(c, meta, byId){
  const val = monDisplayValue(c, meta);
  const topInner = meta.editable
    ? `<span class="lbl">${meta.l||c.id}</span>
       <div class="mon-edit">
         <span class="val" id="monval_${c.id}">${val}</span>
         <input type="number" min="0" max="100" id="moninput_${c.id}" placeholder="%">
         <button onclick="monSet('${c.id}')">→</button>
         ${monManualHtml(meta, byId)}
       </div>`
    : `<span class="lbl">${meta.l||c.id}</span><span class="val" id="monval_${c.id}">${val}</span>`;

  if (!meta.gauge) {
    return `<div class="mon-row">${topInner}</div>`;
  }
  const g = monGaugeFill(c, meta.gauge);
  return `<div class="mon-row mon-row-gauge">
    <div class="mon-row-top">${topInner}</div>
    <div class="mon-gauge"><div class="mon-gauge-fill" id="mongauge_${c.id}" style="width:${g.pct}%;background:${g.color}"></div></div>
  </div>`;
}

function monBuild(list){
  // Каналы вне MON_SECTIONS (в первую очередь raw.*) сюда больше не
  // попадают — они переехали на отдельную скрытую вкладку "Диагностика"
  // (см. BuiltinTabs::registerDiagTab), чтобы не захламлять основной
  // дашборд некалиброванными значениями.
  const byId = {};
  list.forEach(c=>{ byId[c.id] = c; });
  const groups = {};
  list.forEach(c=>{
    const g = MON_SECTION_OF[c.id];
    if (!g) return;
    (groups[g] = groups[g]||[]).push(c);
  });
  const order = MON_SECTIONS.map(s=>s.title);

  let html = '';
  order.forEach(g=>{
    if(!groups[g] || !groups[g].length) return;
    html += `<div class="mon-group"><div class="mon-group-title">${g}</div><div class="mon-rows">`;
    groups[g].forEach(c=>{
      const meta = MON_META[c.id] || {l:c.id};
      html += monRowHtml(c, meta, byId);
    });
    html += `</div></div>`;
  });
  document.getElementById('monPage').innerHTML = html || '<div style="color:var(--muted);padding:20px">Каналы ещё не объявлены</div>';
  _monBuilt = list.length;
}

function monUpdate(list){
  const byId = {};
  list.forEach(c=>{ byId[c.id] = c; });
  list.forEach(c=>{
    const el = document.getElementById('monval_'+c.id);
    if (!el) return;
    const meta = MON_META[c.id] || {l:c.id};
    el.textContent = monDisplayValue(c, meta);
    el.classList.toggle('stale', !c.valid);
    if (meta.gauge) {
      const fillEl = document.getElementById('mongauge_'+c.id);
      if (fillEl) {
        const g = monGaugeFill(c, meta.gauge);
        fillEl.style.width = g.pct + '%';
        fillEl.style.background = g.color;
      }
    }
    if (meta.manualId) {
      const cb = document.getElementById('monmanual_'+meta.manualId);
      const mc = byId[meta.manualId];
      // Не трогаем чекбокс, если пользователь прямо сейчас на нём кликает —
      // избегаем "прыжка" состояния из-за задержки сети между кликом и
      // следующим poll'ом.
      if (cb && document.activeElement !== cb) cb.checked = !!(mc && mc.valid && mc.value);
    }
  });
}

async function monToggleManual(id, checkbox){
  await fetch('/api/channels/set', {method:'POST', headers:{'Content-Type':'application/json'},
    body: JSON.stringify({id, value: checkbox.checked ? 1 : 0})});
}

async function monLoad(){
  try{
    const r = await fetch('/api/channels');
    const list = await r.json();
    if (list.length !== _monBuilt) monBuild(list);
    else monUpdate(list);
  }catch(e){
    const el = document.getElementById('monLoading');
    if (el) el.textContent = 'Ошибка загрузки';
  }
}

async function monSet(id){
  const input = document.getElementById('moninput_'+id);
  const value = parseFloat(input.value);
  if (isNaN(value)) return;
  await fetch('/api/channels/set', {method:'POST', headers:{'Content-Type':'application/json'},
    body: JSON.stringify({id, value})});
  input.value = '';
}

monLoad();
setInterval(monLoad, 2000);
</script>
)html";

// ============================================================
//  ДИАГНОСТИКА — сырые ADC/DI/DO/raw.* каналы, без калибровок
//  Скрыта из nav по умолчанию (BaseConfig::show_diagnostics) — раньше
//  это был просто хвост "Прочее (raw)" в конце Мониторинга, но по мере
//  ввода в эксплуатацию список рос (импульсы, ADC, 1-Wire, NTC, а теперь
//  ещё все 16 DI/DO) и стал мешать основному дашборду — вынесено отдельно
//  и спрятано за тумблер (см. чат при вводе в эксплуатацию).
// ============================================================
static const char DIAG_TAB_HTML[] PROGMEM = R"html(
<div style="color:var(--muted);font-size:12px;margin-bottom:10px">
  Сырые значения без калибровки — для диагностики проводки/шин. Обычно не нужны в работе.
</div>
<div class="monitor-page" id="diagPage">
  <div style="color:var(--muted);text-align:center;padding:20px" id="diagLoading">Загрузка...</div>
</div>

<script>
// Человекочитаемые подписи для известных raw.*-каналов; для того, чего
// тут нет (например будущих датчиков) — просто покажем сырой id.
const DIAG_META = {
  water_pulses: {l:'Импульсы воды'},
  gas_pulses:   {l:'Импульсы газа'},
  p_heat_adc:   {l:'ADC давление отопления'},
  p_water_adc:  {l:'ADC давление ХВС'},
};
for (let i = 0; i < 4; i++) DIAG_META['ntc'+i+'_adc'] = {l:'NTC'+i+' (raw ADC)'};
for (let i = 0; i < 4; i++) DIAG_META['ow'+i+'_temp']  = {l:'1-Wire ow'+i, u:'°C', scaled:true};
for (let i = 0; i < 16; i++) DIAG_META['din'+i] = {l:'IN'+(i+1), fmt:'onoff'};
for (let i = 0; i < 16; i++) DIAG_META['dout'+i] = {l:'OUT'+(i+1), fmt:'onoff'};

function diagRowHtml(c, meta){
  let val;
  if (!c.valid) val = '—';
  else if (meta.fmt === 'onoff') val = c.value ? 'ON' : 'off';
  else if (meta.scaled && c.scale > 1) val = (c.value / c.scale).toFixed(2) + (meta.u ? ' '+meta.u : '');
  else val = String(c.value) + (meta.u ? ' '+meta.u : '');
  return `<div class="mon-row"><span class="lbl">${meta.l}</span><span class="val" id="diagval_${c.id}">${val}</span></div>`;
}

let _diagBuilt = 0;
function diagBuild(list){
  const raw = list.filter(c => c.id.startsWith('raw.'));
  let html = '<div class="mon-group"><div class="mon-rows">';
  raw.forEach(c=>{
    const shortId = c.id.slice(4); // без "raw."
    const meta = DIAG_META[shortId] || {l:c.id};
    html += diagRowHtml(c, meta);
  });
  html += '</div></div>';
  document.getElementById('diagPage').innerHTML = html || '<div style="color:var(--muted);padding:20px">Нет raw-каналов</div>';
  _diagBuilt = raw.length;
}
function diagUpdate(list){
  list.filter(c => c.id.startsWith('raw.')).forEach(c=>{
    const el = document.getElementById('diagval_'+c.id);
    if (!el) return;
    const shortId = c.id.slice(4);
    const meta = DIAG_META[shortId] || {l:c.id};
    if (!c.valid) el.textContent = '—';
    else if (meta.fmt === 'onoff') el.textContent = c.value ? 'ON' : 'off';
    else if (meta.scaled && c.scale > 1) el.textContent = (c.value / c.scale).toFixed(2) + (meta.u ? ' '+meta.u : '');
    else el.textContent = String(c.value) + (meta.u ? ' '+meta.u : '');
  });
}
async function diagLoad(){
  try{
    const r = await fetch('/api/channels');
    const list = await r.json();
    const rawCount = list.filter(c => c.id.startsWith('raw.')).length;
    if (rawCount !== _diagBuilt) diagBuild(list);
    else diagUpdate(list);
  }catch(e){
    const el = document.getElementById('diagLoading');
    if (el) el.textContent = 'Ошибка загрузки';
  }
}
diagLoad();
setInterval(diagLoad, 2000);
</script>
)html";

// ============================================================
//  НАСТРОЙКИ — сеть, время, бэкап, OTA
//  (порт SETTINGS_HTML из esp32-platform/src/main.cpp, вынесено из
//  main.cpp сюда — см. BuiltinTabs.h)
// ============================================================
static const char SETTINGS_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>📡 Статус сети</h3>
  <div id="net_status" style="font-size:12px;color:var(--muted)">загрузка...</div>
</div>

<div class="settings-group">
  <h3>🌐 Сеть</h3>
  <label><input type="checkbox" id="eth_dhcp" onchange="toggleEth()"> DHCP (Ethernet)</label>
  <div id="eth_static" style="display:none">
    <input type="text" name="eth_ip"   placeholder="IP: 192.168.1.100">
    <input type="text" name="eth_mask" placeholder="Маска: 255.255.255.0">
    <input type="text" name="eth_gw"   placeholder="Шлюз: 192.168.1.1">
    <input type="text" name="eth_dns"  placeholder="DNS: 8.8.8.8">
  </div>
  <label style="margin-top:8px">WiFi SSID (резерв, если пропадёт Ethernet)</label>
  <div style="display:flex;gap:8px">
    <input type="text" name="wifi_ssid" placeholder="SSID" style="margin:0;flex:1">
    <button onclick="scanWifi()" style="width:auto;padding:10px 12px;margin:0">🔍</button>
  </div>
  <div id="wifi_scan_results" style="display:none;margin-top:4px;border:1px solid var(--border);
    border-radius:6px;max-height:160px;overflow-y:auto;background:var(--bg)"></div>
  <input type="password" name="wifi_pass" placeholder="WiFi Password" style="margin-top:8px">
  <input type="text" name="device_name" placeholder="Имя устройства">
</div>

<div class="settings-group" id="time_settings">
  <h3>🕐 Время</h3>
  <div style="display:flex;align-items:center;gap:8px;margin-bottom:8px">
    <span id="st_led" style="width:8px;height:8px;border-radius:50%;background:var(--muted);flex-shrink:0"></span>
    <span id="st_time" style="font-family:monospace;font-weight:700;color:var(--accent)">--:--:--</span>
    <span id="st_src" style="font-size:11px;color:var(--muted)"></span>
  </div>
  <label>Часовой пояс (мин от UTC)</label>
  <input type="text" name="tz_offset_min" placeholder="180 = UTC+3">
  <label>NTP сервер</label>
  <input type="text" name="ntp_server" placeholder="pool.ntp.org">
  <label>MQTT топик времени (опционально, например от Home Assistant)</label>
  <input type="text" name="mqtt_time_topic" placeholder="homeassistant/sensor/time/state">
  <div style="display:flex;gap:12px;margin-top:4px">
    <label><input type="checkbox" id="ntp_enabled"> NTP</label>
    <label><input type="checkbox" id="mqtt_time_enabled"> MQTT</label>
  </div>
</div>

<div class="settings-group">
  <h3>🔒 Веб-интерфейс</h3>
  <input type="text"     name="web_user" placeholder="Пользователь">
  <input type="password" name="web_pass" placeholder="Пароль">
</div>

<div class="settings-group">
  <h3>🔧 Диагностика</h3>
  <label><input type="checkbox" id="show_diagnostics"> Показывать вкладки диагностики (RS485-консоль, сырые ADC/DI/DO)</label>
</div>
<button onclick="saveSettings()">💾 Сохранить настройки</button>

<div class="settings-group" style="margin-top:12px">
  <h3>📂 Резервное копирование</h3>
  <a href="/api/backup" download="backup.json">
    <button type="button">📥 Скачать backup (всё)</button>
  </a>
  <a href="/api/download-config" download="base.json">
    <button type="button" class="btn-secondary">📥 Только сетевые настройки</button>
  </a>
  <label style="margin-top:8px">Восстановить из backup.json:</label>
  <input type="file" id="configUpload" accept=".json">
  <button onclick="uploadConfig()">📤 Восстановить</button>
</div>

<div class="settings-group">
  <h3>⚙️ Системные действия</h3>
  <label>Прошивка или файловая система (.bin):</label>
  <form method="POST" action="/update" enctype="multipart/form-data">
    <input type="file" name="update">
    <button type="submit">🚀 Обновить ПО</button>
  </form>
  <button onclick="fetch('/api/reboot').then(()=>alert('Перезагружаюсь...'))" class="btn-danger">
    🔄 Перезагрузка
  </button>
</div>

<script>
let _currentCfg = {};

async function loadNetStatus() {
  try {
    const r = await fetch('/api/net-status');
    const d = await r.json();
    let html = '';
    if (d.eth_connected)
      html += `<b>ETH:</b> ${d.eth_ip} &nbsp;`;
    else
      html += `<b>ETH:</b> <span style="color:var(--danger)">не подключён</span> &nbsp;`;
    if (d.wifi_connected)
      html += `<b>WiFi:</b> ${d.wifi_ip} (${d.wifi_rssi} dBm) &nbsp;`;
    else if (d.ap_mode)
      html += `<b>AP:</b> ${d.ap_ip} &nbsp;`;
    else
      html += `<b>WiFi:</b> <span style="color:var(--muted)">не подключён</span> &nbsp;`;
    document.getElementById('net_status').innerHTML = html;
  } catch(e) {}
}

async function scanWifi() {
  const btn = event.target;
  btn.textContent = '⏳';
  btn.disabled = true;
  try {
    const r = await fetch('/api/wifi-scan');
    const d = await r.json();
    const container = document.getElementById('wifi_scan_results');
    container.style.display = 'block';
    container.innerHTML = '';
    if (!d.networks || d.networks.length === 0) {
      container.innerHTML = '<div style="padding:8px;color:var(--muted)">Сети не найдены</div>';
      return;
    }
    d.networks.sort((a,b) => b.rssi - a.rssi);
    d.networks.forEach(n => {
      const bars = n.rssi > -50 ? '████' : n.rssi > -65 ? '███░' :
                  n.rssi > -75 ? '██░░' : '█░░░';
      const lock = n.enc ? '🔒' : '🔓';
      const div = document.createElement('div');
      div.style.cssText = 'padding:8px 12px;cursor:pointer;border-bottom:1px solid var(--border);font-size:12px';
      div.innerHTML = `${lock} <b>${n.ssid}</b> <span style="color:var(--muted);float:right">${bars} ${n.rssi} dBm</span>`;
      div.onmouseenter = () => div.style.background = 'var(--border)';
      div.onmouseleave = () => div.style.background = '';
      div.onclick = () => {
        document.querySelector('[name="wifi_ssid"]').value = n.ssid;
        container.style.display = 'none';
        document.querySelector('[name="wifi_pass"]').focus();
      };
      container.appendChild(div);
    });
  } catch(e) {
    alert('Ошибка сканирования');
  } finally {
    btn.textContent = '🔍';
    btn.disabled = false;
  }
}

async function loadSettings() {
  const r = await fetch('/api/get-config');
  if (!r.ok) return;
  _currentCfg = await r.json();
  document.getElementById('eth_dhcp').checked = _currentCfg.eth_dhcp;
  document.getElementById('show_diagnostics').checked = !!_currentCfg.show_diagnostics;
  Object.keys(_currentCfg).forEach(k => {
    const el = document.querySelector('[name="'+k+'"]');
    if (el) el.value = _currentCfg[k];
  });
  toggleEth();
}
function toggleEth() {
  document.getElementById('eth_static').style.display =
    document.getElementById('eth_dhcp').checked ? 'none' : 'block';
}

fetch('/api/time/config').then(r=>r.json()).then(d=>{
  document.querySelector('[name="tz_offset_min"]').value = d.tz_offset_min || 180;
  document.querySelector('[name="ntp_server"]').value    = d.ntp_server || 'pool.ntp.org';
  document.querySelector('[name="mqtt_time_topic"]').value = d.mqtt_topic || '';
  document.getElementById('ntp_enabled').checked  = d.ntp_enabled;
  document.getElementById('mqtt_time_enabled').checked = d.mqtt_enabled;
});
fetch('/api/time').then(r=>r.json()).then(d=>{
  const srcs={ntp:'🛰 NTP',mqtt:'🏠 MQTT',none:''};
  document.getElementById('st_led').style.background = d.synced ? 'var(--ok)' : 'var(--muted)';
  document.getElementById('st_time').textContent = d.time_str || '--:--:--';
  document.getElementById('st_src').textContent  = srcs[d.source] || '';
});

async function saveSettings() {
  const timeData = {
    tz_offset_min: +document.querySelector('[name="tz_offset_min"]').value,
    ntp_server:    document.querySelector('[name="ntp_server"]').value,
    mqtt_topic:    document.querySelector('[name="mqtt_time_topic"]').value,
    ntp_enabled:   document.getElementById('ntp_enabled').checked,
    mqtt_enabled:  document.getElementById('mqtt_time_enabled').checked,
  };
  fetch('/api/time/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(timeData)});

  const data = Object.assign({}, _currentCfg);
  data.eth_dhcp = document.getElementById('eth_dhcp').checked;
  data.show_diagnostics = document.getElementById('show_diagnostics').checked;
  const fields = ['eth_ip','eth_mask','eth_gw','eth_dns',
                  'wifi_ssid','wifi_pass','device_name','web_user','web_pass'];
  fields.forEach(k => {
    const el = document.querySelector('[name="'+k+'"]');
    if (el && el.value.trim() !== '') data[k] = el.value.trim();
  });
  const r = await fetch('/api/save-config', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  // Полная перезагрузка страницы — проще всего гарантированно обновить
  // список вкладок в nav (show_diagnostics могла измениться).
  if (r.ok) { alert('Настройки сохранены'); location.reload(); }
}

async function uploadConfig() {
  const file = document.getElementById('configUpload').files[0];
  if (!file) return alert('Выберите файл!');
  const text = await file.text();
  let data;
  try { data = JSON.parse(text); } catch(e) { return alert('Неверный JSON'); }
  const r = await fetch('/api/restore', {method:'POST',
    headers:{'Content-Type':'application/json'}, body: JSON.stringify(data)});
  if (r.ok) alert('Восстановлено. Перезагрузка...');
}

loadSettings();
loadNetStatus();
setInterval(loadNetStatus, 5000);
</script>
)html";

void BuiltinTabs::registerMonitorTab() {
    WebHandler::registerTab({"monitor", "Мониторинг", "📈", MONITOR_TAB_HTML, nullptr});
}

void BuiltinTabs::registerDiagTab() {
    WebHandler::registerTab({"diag", "Диагностика", "🔧", DIAG_TAB_HTML, &baseCfg.show_diagnostics});
}

void BuiltinTabs::registerSettingsTab() {
    WebHandler::registerTab({"settings", "Настройки", "⚙️", SETTINGS_TAB_HTML, nullptr});
}
