# 📁 PROJECT EXPORT FOR LLMs

## 📊 Project Information

- **Project Name**: `esp32-platform`
- **Generated On**: 2026-06-04 11:04:24 (Asia/Novosibirsk / GMT+07:00)
- **Total Files Processed**: 39
- **Export Tool**: Easy Whole Project to Single Text File for LLMs v1.1.0
- **Tool Author**: Jota / José Guilherme Pandolfi

### ⚙️ Export Configuration

| Setting | Value |
|---------|-------|
| Language | `en` |
| Max File Size | `1 MB` |
| Include Hidden Files | `false` |
| Output Format | `both` |

## 🌳 Project Structure

```
├── 📁 data/
│   ├── 📄 index.html (11.49 KB)
│   └── 📄 time.json (132 B)
├── 📁 src/
│   ├── 📁 apps/
│   │   ├── 📁 boiler/
│   │   │   ├── 📄 BoilerApp.h (4.98 KB)
│   │   │   ├── 📄 BoilerAppDevice.cpp (26.46 KB)
│   │   │   └── 📄 BoilerAppDevice.h (631 B)
│   │   └── 📁 boiler-room/
│   │       ├── 📄 BoilerRoomApp.h (3.36 KB)
│   │       ├── 📄 BoilerRoomDevice.cpp (21.91 KB)
│   │       └── 📄 BoilerRoomDevice.h (617 B)
│   ├── 📁 core/
│   │   ├── 📄 ConfigManager.cpp (2.31 KB)
│   │   ├── 📄 ConfigManager.h (442 B)
│   │   ├── 📄 CoreTypes.h (2.13 KB)
│   │   ├── 📄 NetworkManager.cpp (5.84 KB)
│   │   ├── 📄 NetworkManager.h (408 B)
│   │   ├── 📄 WebHandler.cpp (11.83 KB)
│   │   └── 📄 WebHandler.h (644 B)
│   ├── 📁 devices/
│   │   ├── 📁 boiler/
│   │   │   ├── 📄 BoilerDevice.cpp (14.85 KB)
│   │   │   ├── 📄 BoilerDevice.h (621 B)
│   │   │   └── 📄 BoilerTypes.h (2.42 KB)
│   │   ├── 📁 boiler-monitor/
│   │   │   ├── 📄 BMonDevice.cpp (23.83 KB)
│   │   │   ├── 📄 BMonDevice.h (1012 B)
│   │   │   └── 📄 BMonTypes.h (5.05 KB)
│   │   ├── 📁 ekonom/
│   │   │   ├── 📄 EkoNomDevice.cpp (6.54 KB)
│   │   │   └── 📄 EkoNomDevice.h (1.79 KB)
│   │   └── 📁 kc868-a16/
│   │       ├── 📄 A16Device.cpp (21.51 KB)
│   │       ├── 📄 A16Device.h (1019 B)
│   │       └── 📄 A16Types.h (3.94 KB)
│   ├── 📁 modules/
│   │   ├── 📁 modem/
│   │   │   ├── 📄 ModemHandler.cpp (51.4 KB)
│   │   │   └── 📄 ModemHandler.h (3.46 KB)
│   │   ├── 📁 mqtt/
│   │   │   ├── 📄 MqttHandler.cpp (6.61 KB)
│   │   │   └── 📄 MqttHandler.h (1.02 KB)
│   │   ├── 📁 rs485/
│   │   │   ├── 📄 Rs485Handler.cpp (23.43 KB)
│   │   │   └── 📄 Rs485Handler.h (2.33 KB)
│   │   ├── 📁 telegram/
│   │   │   ├── 📄 TgHandler.cpp (6.78 KB)
│   │   │   └── 📄 TgHandler.h (860 B)
│   │   └── 📁 time/
│   │       ├── 📄 TimeManager.cpp (11.31 KB)
│   │       └── 📄 TimeManager.h (1.94 KB)
│   └── 📄 main.cpp (14.25 KB)
├── 📄 platformio.ini (4.83 KB)
└── 📄 sdkconfig.defaults (192 B)
```

## 📑 Table of Contents

**Project Files:**

- [📄 data/index.html](#📄-data-index-html)
- [📄 data/time.json](#📄-data-time-json)
- [📄 src/apps/boiler/BoilerApp.h](#📄-src-apps-boiler-boilerapp-h)
- [📄 src/apps/boiler/BoilerAppDevice.cpp](#📄-src-apps-boiler-boilerappdevice-cpp)
- [📄 src/apps/boiler/BoilerAppDevice.h](#📄-src-apps-boiler-boilerappdevice-h)
- [📄 src/apps/boiler-room/BoilerRoomApp.h](#📄-src-apps-boiler-room-boilerroomapp-h)
- [📄 src/apps/boiler-room/BoilerRoomDevice.cpp](#📄-src-apps-boiler-room-boilerroomdevice-cpp)
- [📄 src/apps/boiler-room/BoilerRoomDevice.h](#📄-src-apps-boiler-room-boilerroomdevice-h)
- [📄 src/core/ConfigManager.cpp](#📄-src-core-configmanager-cpp)
- [📄 src/core/ConfigManager.h](#📄-src-core-configmanager-h)
- [📄 src/core/CoreTypes.h](#📄-src-core-coretypes-h)
- [📄 src/core/NetworkManager.cpp](#📄-src-core-networkmanager-cpp)
- [📄 src/core/NetworkManager.h](#📄-src-core-networkmanager-h)
- [📄 src/core/WebHandler.cpp](#📄-src-core-webhandler-cpp)
- [📄 src/core/WebHandler.h](#📄-src-core-webhandler-h)
- [📄 src/devices/boiler/BoilerDevice.cpp](#📄-src-devices-boiler-boilerdevice-cpp)
- [📄 src/devices/boiler/BoilerDevice.h](#📄-src-devices-boiler-boilerdevice-h)
- [📄 src/devices/boiler/BoilerTypes.h](#📄-src-devices-boiler-boilertypes-h)
- [📄 src/devices/boiler-monitor/BMonDevice.cpp](#📄-src-devices-boiler-monitor-bmondevice-cpp)
- [📄 src/devices/boiler-monitor/BMonDevice.h](#📄-src-devices-boiler-monitor-bmondevice-h)
- [📄 src/devices/boiler-monitor/BMonTypes.h](#📄-src-devices-boiler-monitor-bmontypes-h)
- [📄 src/devices/ekonom/EkoNomDevice.cpp](#📄-src-devices-ekonom-ekonomdevice-cpp)
- [📄 src/devices/ekonom/EkoNomDevice.h](#📄-src-devices-ekonom-ekonomdevice-h)
- [📄 src/devices/kc868-a16/A16Device.cpp](#📄-src-devices-kc868-a16-a16device-cpp)
- [📄 src/devices/kc868-a16/A16Device.h](#📄-src-devices-kc868-a16-a16device-h)
- [📄 src/devices/kc868-a16/A16Types.h](#📄-src-devices-kc868-a16-a16types-h)
- [📄 src/modules/modem/ModemHandler.cpp](#📄-src-modules-modem-modemhandler-cpp)
- [📄 src/modules/modem/ModemHandler.h](#📄-src-modules-modem-modemhandler-h)
- [📄 src/modules/mqtt/MqttHandler.cpp](#📄-src-modules-mqtt-mqtthandler-cpp)
- [📄 src/modules/mqtt/MqttHandler.h](#📄-src-modules-mqtt-mqtthandler-h)
- [📄 src/modules/rs485/Rs485Handler.cpp](#📄-src-modules-rs485-rs485handler-cpp)
- [📄 src/modules/rs485/Rs485Handler.h](#📄-src-modules-rs485-rs485handler-h)
- [📄 src/modules/telegram/TgHandler.cpp](#📄-src-modules-telegram-tghandler-cpp)
- [📄 src/modules/telegram/TgHandler.h](#📄-src-modules-telegram-tghandler-h)
- [📄 src/modules/time/TimeManager.cpp](#📄-src-modules-time-timemanager-cpp)
- [📄 src/modules/time/TimeManager.h](#📄-src-modules-time-timemanager-h)
- [📄 src/main.cpp](#📄-src-main-cpp)
- [📄 platformio.ini](#📄-platformio-ini)

---

## 📈 Project Statistics

| Metric | Count |
|--------|-------|
| Total Files | 39 |
| Total Directories | 17 |
| Text Files | 38 |
| Binary Files | 1 |
| Total Size | 304.02 KB |

### 📄 File Types Distribution

| Extension | Count |
|-----------|-------|
| `.h` | 20 |
| `.cpp` | 15 |
| `.html` | 1 |
| `.json` | 1 |
| `.ini` | 1 |
| `.defaults` | 1 |

## 💻 File Code Contents

### <a id="📄-data-index-html"></a>📄 `data/index.html`

**File Info:**
- **Size**: 11.49 KB
- **Extension**: `.html`
- **Language**: `html`
- **Location**: `data/index.html`
- **Relative Path**: `data`
- **Created**: 2026-02-18 20:59:52 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-26 01:38:11 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `de8f5946e0a1bf971c8ea727f6df1032`
- **SHA256**: `578fe39b769e75bb72e720d154c3fd97e0b9715ede5866e4a5355d3287b8d842`
- **Encoding**: UTF-8

**File code content:**

```html
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0">
<title>ESP32 Platform</title>
<style>
:root {
  --bg:       #1a1c22;
  --card:     #22252e;
  --border:   #2e3240;
  --text:     #d0d4e0;
  --muted:    #6b7494;
  --accent:   #f0a500;
  --hot:      #e05252;
  --cool:     #5a9fd4;
  --warm:     #e0a040;
  --ok:       #3ec97a;
  --danger:   #e05252;
  --nav-bg:   #13151a;
}

* { box-sizing: border-box; margin: 0; padding: 0; }
html, body { height: 100%; background: var(--bg); color: var(--text);
  font-family: -apple-system, 'Segoe UI', sans-serif; font-size: 14px; }

/* ─── NAV ─────────────────────────────────────────────────── */
nav {
  background: var(--nav-bg);
  display: flex;
  justify-content: center;
  overflow-x: auto;
  scrollbar-width: none;
  border-bottom: 1px solid var(--border);
  position: sticky; top: 0; z-index: 100;
}
nav::-webkit-scrollbar { display: none; }

.tab-btn {
  flex: 0 0 auto;
  width: auto;
  background: none; border: none;
  color: var(--muted);
  padding: 14px 20px;
  font-size: 13px; font-weight: 600;
  cursor: pointer;
  border-bottom: 2px solid transparent;
  transition: color .2s, border-color .2s;
  white-space: nowrap;
}
.tab-btn:hover { color: var(--text); }
.tab-btn.active { color: var(--accent); border-bottom-color: var(--accent); }

/* ─── STATUS BAR ──────────────────────────────────────────── */
.status-bar {
  display: flex; align-items: center; gap: 16px;
  padding: 10px 16px;
  background: var(--card);
  border-bottom: 1px solid var(--border);
  font-size: 12px;
  flex-wrap: wrap;
  justify-content: center;
}
.status-dot { display: flex; align-items: center; gap: 5px; }
.dot { width: 7px; height: 7px; border-radius: 50%; background: var(--muted); }
.dot.on  { background: var(--ok); box-shadow: 0 0 5px var(--ok); }
.dot.off { background: var(--muted); }
.status-online { color: var(--ok); font-weight: 700;
  font-size: 11px; letter-spacing: .08em; }

/* ─── TAB CONTENT ─────────────────────────────────────────── */
.tab-pane { display: none; padding: 16px; max-width: 560px; margin: 0 auto; }
.tab-pane.active { display: block; }
#loading { text-align: center; color: var(--muted); padding: 40px 0; }

/* ─── MONITOR CARDS ───────────────────────────────────────── */
.monitor-page { display: flex; flex-direction: column; gap: 10px; }

.m-card {
  background: var(--card);
  border-radius: 10px;
  padding: 16px;
  border: 1px solid var(--border);
}
.m-card-label {
  font-size: 10px; font-weight: 700;
  letter-spacing: .12em; text-transform: uppercase;
  color: var(--muted); margin-bottom: 12px;
}

/* Power card */
.m-power-big {
  text-align: center;
  font-size: 52px; font-weight: 800;
  color: var(--accent); line-height: 1;
  margin-bottom: 6px;
}
.m-power-big .unit { font-size: 22px; font-weight: 600; margin-left: 4px; }
.m-power-sub { text-align: center; color: var(--muted); font-size: 13px; margin-bottom: 2px; }
.m-power-sub b { color: var(--text); }

/* Sensors row */
.m-sensors-row {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 8px;
}
.m-sensors-row.two { grid-template-columns: repeat(2, 1fr); }

.m-sensor {
  background: #1a1c22;
  border-radius: 8px;
  padding: 12px 10px;
  text-align: center;
  border: 1px solid var(--border);
}
.m-sensor-label {
  font-size: 9px; font-weight: 700;
  letter-spacing: .1em; text-transform: uppercase;
  color: var(--muted); margin-bottom: 6px;
}
.m-sensor-val {
  font-size: 24px; font-weight: 800;
  line-height: 1; margin-bottom: 4px;
}
.m-sensor-val.hot    { color: var(--hot); }
.m-sensor-val.cool   { color: var(--cool); }
.m-sensor-val.warm   { color: var(--warm); }
.m-sensor-val.accent { color: var(--accent); }
.m-sensor-icon { font-size: 12px; color: var(--hot); }
.m-sensor-icon.cool { color: var(--cool); }
.m-sensor-sub { font-size: 11px; color: var(--muted); }

canvas#powerChart { width: 100%; display: block; }

/* ─── SETTINGS ────────────────────────────────────────────── */
.settings-group {
  background: var(--card);
  border-radius: 10px;
  padding: 16px;
  border: 1px solid var(--border);
  margin-bottom: 12px;
}
.settings-group h3 {
  font-size: 13px; font-weight: 700;
  color: var(--text); margin-bottom: 12px;
}
.settings-group label {
  display: block; color: var(--muted);
  font-size: 12px; margin-bottom: 8px;
}
input[type="text"],
input[type="number"],
input[type="password"],
input[type="file"] {
  width: 100%; padding: 10px 12px;
  background: var(--bg); border: 1px solid var(--border);
  border-radius: 6px; color: var(--text);
  font-size: 13px; margin-bottom: 8px;
  outline: none; transition: border-color .2s;
}
input:focus { border-color: var(--accent); }
input[type="checkbox"] { width: auto; margin-right: 6px; }

.toggle-row {
  display: flex !important; align-items: center;
  justify-content: space-between;
  color: var(--text) !important;
  font-size: 14px !important;
  margin-bottom: 12px !important;
}

button {
  width: 100%; padding: 11px;
  background: var(--accent); border: none;
  border-radius: 6px; color: #111;
  font-size: 13px; font-weight: 700;
  cursor: pointer; margin-bottom: 8px;
  transition: opacity .2s;
}
button:hover { opacity: .85; }
button.btn-danger { background: var(--danger); color: #fff; }
button.btn-secondary { background: var(--border); color: var(--text); }

.info-row { font-size: 12px; color: var(--muted); margin-top: 4px; }
.muted { color: var(--muted); }
</style>
</head>
<body>

<nav id="navBar">
  <span class="tab-btn" style="color:var(--muted);padding:14px 16px;font-size:12px">загрузка...</span>
</nav>

<div class="status-bar" id="statusBar">
  <div class="status-dot"><div class="dot" id="dot_wifi"></div>Wi-Fi</div>
  <div class="status-dot"><div class="dot" id="dot_eth"></div>ETH <span id="eth_ip" style="font-size:10px;color:var(--muted)"></span></div>
  <div class="status-dot"><div class="dot" id="dot_mqtt"></div>MQTT</div>
  <div class="status-dot"><div class="dot" id="dot_tg"></div>Telegram</div>
  <div class="status-dot"><div class="dot" id="dot_modem"></div>Модем</div>
  <div id="sys_time" style="font-size:11px;color:var(--muted);font-family:monospace;padding:0 8px">--:--:--</div>
  <div class="status-online" id="status_online">●</div>
  <div style="font-size:10px;color:var(--muted);margin-left:8px" id="status_build"></div>
</div>

<div id="tabsContainer">
  <div id="loading">Загрузка интерфейса...</div>
</div>

<script>
let _activeTab = null;

// ── Загрузка nav и первой вкладки ──────────────────────────
async function initUI() {
  try {
    const r = await fetch('/api/nav');
    const tabs = await r.json();
    if (!tabs.length) return;

    // Строим nav
    const nav = document.getElementById('navBar');
    nav.innerHTML = '';
    tabs.forEach((t, i) => {
      const btn = document.createElement('button');
      btn.className = 'tab-btn' + (i === 0 ? ' active' : '');
      btn.textContent = (t.icon ? t.icon + ' ' : '') + t.label;
      btn.onclick = () => openTab(t.id, btn);
      nav.appendChild(btn);
    });

    // Строим пустые panes
    const container = document.getElementById('tabsContainer');
    container.innerHTML = '';
    tabs.forEach((t, i) => {
      const pane = document.createElement('div');
      pane.className = 'tab-pane' + (i === 0 ? ' active' : '');
      pane.id = 'tab_' + t.id;
      pane.innerHTML = '<div style="color:var(--muted);padding:30px 0;text-align:center">Загрузка...</div>';
      container.appendChild(pane);
    });

    // Загружаем первую вкладку
    await loadTab(tabs[0].id);
    _activeTab = tabs[0].id;
  } catch(e) {
    document.getElementById('tabsContainer').innerHTML =
      '<div style="color:#e05252;padding:20px">Ошибка загрузки интерфейса</div>';
  }
}

// ── Загрузка HTML вкладки ──────────────────────────────────
async function loadTab(id) {
  const pane = document.getElementById('tab_' + id);
  if (!pane || pane.dataset.loaded) return;
  try {
    const r = await fetch('/api/tab?id=' + id);
    pane.innerHTML = await r.text();
    pane.dataset.loaded = '1';
    // Запускаем скрипты внутри вкладки
    pane.querySelectorAll('script').forEach(s => {
      const ns = document.createElement('script');
      ns.textContent = s.textContent;
      document.body.appendChild(ns);
    });
  } catch(e) {
    pane.innerHTML = '<div style="color:var(--muted);padding:20px">Ошибка загрузки вкладки</div>';
  }
}

// ── Переключение вкладок ───────────────────────────────────
async function openTab(id, btn) {
  document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
  document.getElementById('tab_' + id).classList.add('active');
  if (btn) btn.classList.add('active');
  await loadTab(id);
  _activeTab = id;
}

// ── Статус-бар ─────────────────────────────────────────────
async function updateStatus() {
  try {
    const r = await fetch('/api/status');
    const d = await r.json();
    const set = (id, on) => {
      const el = document.getElementById(id);
      if (el) { el.classList.toggle('on', on); el.classList.toggle('off', !on); }
    };
    set('dot_wifi', d.wifi);
    set('dot_eth',  d.eth);
      const ethIp = document.getElementById('eth_ip');
      if (ethIp) ethIp.textContent = d.eth ? '(' + (d.eth_ip || '') + ')' : '';
    set('dot_mqtt', d.mqtt);
    set('dot_tg',   d.tg);
      const dotModem = document.getElementById('dot_modem');
      if (dotModem) {
        if (!d.modem_enabled) {
          dotModem.classList.remove('on', 'off');  // серый — дефолт
        } else {
          const reg = d.modem_creg == 1 || d.modem_creg == 5;
          dotModem.classList.toggle('on', reg);
          dotModem.classList.toggle('off', !reg);
        }
      }
    const online = d.eth || d.wifi;
    const el = document.getElementById('status_online');
    if (el) { el.textContent = online ? 'ONLINE' : 'OFFLINE';
              el.style.color = online ? 'var(--ok)' : 'var(--danger)'; }
    const bld = document.getElementById('status_build');
    if (bld && d.build) bld.textContent = 'fw: ' + d.build;
  } catch(e) {}
}

async function updateTime() {
  try {
    const d = await (await fetch('/api/time')).json();
    const srcs = {ntp:'🛰',mqtt:'🏠',gsm:'📡',rtc:'⏱',none:''};
    document.getElementById('sys_time').textContent =
      (srcs[d.source] || '') + ' ' + (d.time_str || '--:--:--');
    document.getElementById('sys_time').style.color =
      d.synced ? 'var(--text)' : 'var(--muted)';
  } catch(e) {}
}
updateTime();
setInterval(updateTime, 5000);
 

initUI();
updateStatus();
setInterval(updateStatus, 5000);
</script>
</body>
</html>

```

---

### <a id="📄-data-time-json"></a>📄 `data/time.json`

**File Info:**
- **Size**: 132 B
- **Extension**: `.json`
- **Language**: `json`
- **Location**: `data/time.json`
- **Relative Path**: `data`
- **Created**: 2026-05-24 06:51:33 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-24 06:52:45 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `1c7ba559490247a04fbd4a1eb0e808ec`
- **SHA256**: `f22d330324cf072645d2a84d0c247c47ad3f9c7c595b0b22b50c4bae33cd4cfd`
- **Encoding**: ASCII

**File code content:**

```json
{
  "tz_offset_min": 420,
  "ntp_enabled": true,
  "mqtt_enabled": false,
  "ntp_server": "pool.ntp.org",
  "mqtt_topic": ""
}
```

---

### <a id="📄-src-apps-boiler-boilerapp-h"></a>📄 `src/apps/boiler/BoilerApp.h`

**File Info:**
- **Size**: 4.98 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/apps/boiler/BoilerApp.h`
- **Relative Path**: `src/apps/boiler`
- **Created**: 2026-02-18 04:57:08 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-22 18:41:55 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `bb5186fcf9f4eb4770fe7b2fc945be09`
- **SHA256**: `af0066205903efd8a95d0779f5b1fd73afd842b54aac53f5c1a633f6f7b24b45`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER)

#include <Arduino.h>

// ============================================================
//  ПРИВЯЗКА ДАТЧИКОВ К КАНАЛАМ A16
// ============================================================
#define BOILER_OW_SUPPLY    0   // HT1 (pin 33) — подача
#define BOILER_OW_RETURN    1   // HT2 (pin 32) — обратка

#define BOILER_ADC_P1       0   // ADC CH1 (pin 36) — давление подачи 4-20мА
#define BOILER_ADC_P2       1   // ADC CH2 (pin 39) — давление обратки 4-20мА

#define BOILER_DIN_FLOW1    0   // DIN1 — импульсный расходомер (сухой контакт)
#define BOILER_DIN_FLOW2    1   // DIN2 — расходомер Холла

// ============================================================
//  КОНФИГ КОТЛА — сохраняется в /boiler.json
// ============================================================
struct BoilerAppConfig {
    // Уставки температуры
    float t_max         = 85.0f;   // Аварийное отключение
    float t_alarm       = 80.0f;   // Порог оповещения
    float t_hyst        = 2.0f;    // Гистерезис
    float t_min         = 40.0f;   // Защита от замерзания

    // Калибровка давления (4-20мА → бар)
    float p_min         = 0.0f;    // 4мА = 0 бар
    float p_max         = 10.0f;   // 20мА = 10 бар

    // Расходомеры — импульсов на литр (настраивается после замера)
    float flow1_liter_per_pulse = 1.0f;   // сухой контакт
    float flow2_liter_per_pulse = 1.0f;   // Холл

    // Интервал публикации MQTT (мс)
    uint32_t pub_interval = 10000;
};

// ============================================================
//  СОСТОЯНИЕ КОТЛА — runtime
// ============================================================
#define BOILER_HIST_SIZE 60  // 60 × 10сек = 10 минут
#define BOILER_HIST1H_SIZE 360  // 360 × 10сек = 1 час
#define BOILER_POWER_ON_KW 0.5f // порог "котёл работает", кВт

struct BoilerAppState {
    // Температуры
    float t_supply  = 0.0f;
    float t_return  = 0.0f;
    float t_delta   = 0.0f;

    // Давление
    float p_supply  = 0.0f;
    float p_return  = 0.0f;
    float p_delta   = 0.0f;

    // Расход
    float flow1_lpm = 0.0f;   // л/мин расходомер 1
    float flow2_lpm = 0.0f;   // л/мин расходомер 2
    float flow_lpm  = 0.0f;   // итоговый (среднее или выбранный)

    // Счётчики импульсов (снапшот для расчёта расхода)
    uint32_t flow1_last = 0;
    uint32_t flow2_last = 0;

    // Мощность
    float power_kw  = 0.0f;
    float energy_kwh = 0.0f;  // суточный накопитель

    // Аварии
    bool alarm_high   = false;
    bool alarm_low    = false;
    bool alarm_sensor = false;

    // История мощности
    float    hist_power[BOILER_HIST_SIZE] = {};
    uint8_t  hist_head  = 0;
    uint8_t  hist_count = 0;

    
    // Аналитика за последний час
    float    hist1h_power[BOILER_HIST1H_SIZE] = {};  // мощность, кВт
    float    hist1h_delta[BOILER_HIST1H_SIZE] = {};  // дельта T, °C
    uint16_t hist1h_head  = 0;
    uint16_t hist1h_count = 0;
 
    // Расчётные показатели (обновляются каждые 10 сек)
    float    power_avg_1h    = 0.0f;  // средняя мощность за час, кВт
    float    power_peak_1h   = 0.0f;  // пиковая мощность за час, кВт
    float    delta_avg_1h    = 0.0f;  // средняя дельта T за час, °C
    uint16_t cycles_1h       = 0;     // количество включений за час
    uint16_t runtime_min     = 0;     // время работы за час, мин
    uint16_t idle_min        = 0;     // время простоя за час, мин
    uint8_t  duty_pct        = 0;     // duty cycle, %
    uint16_t cycle_dur_min   = 0;     // длительность текущего цикла, мин
    bool     burner_on       = false; // котёл сейчас работает
    uint32_t cycle_start_ms  = 0;     // millis() начала текущего цикла
 
    void pushHistory1h(float p, float dt) {
        hist1h_power[hist1h_head] = p;
        hist1h_delta[hist1h_head] = dt;
        hist1h_head = (hist1h_head + 1) % BOILER_HIST1H_SIZE;
        if (hist1h_count < BOILER_HIST1H_SIZE) hist1h_count++;
    }

    void pushHistory(float p) {
        hist_power[hist_head] = p;
        hist_head = (hist_head + 1) % BOILER_HIST_SIZE;
        if (hist_count < BOILER_HIST_SIZE) hist_count++;
    }
};

extern BoilerAppConfig boilerAppCfg;
extern BoilerAppState  boilerAppState;

#endif // DEVICE_KC868_A16 && APP_BOILER

```

---

### <a id="📄-src-apps-boiler-boilerappdevice-cpp"></a>📄 `src/apps/boiler/BoilerAppDevice.cpp`

**File Info:**
- **Size**: 26.46 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/apps/boiler/BoilerAppDevice.cpp`
- **Relative Path**: `src/apps/boiler`
- **Created**: 2026-02-18 04:58:28 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-28 18:54:14 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `4458277a21c57b45cc1f5ac6a2257e9a`
- **SHA256**: `1a3a32306f3acfea507531f0743494cefe9171e83e7bb37a64d7c8e13128e09d`
- **Encoding**: UTF-8

**File code content:**

```cpp
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER)
#include "BoilerAppDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include "../../devices/kc868-a16/A16Types.h"
#include "../../devices/kc868-a16/A16Device.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../../modules/telegram/TgHandler.h"
#endif

#ifdef DEVICE_EKONOM
#include "../../devices/ekonom/EkoNomDevice.h"
#endif

BoilerAppConfig boilerAppCfg;
BoilerAppState  boilerAppState;

uint32_t BoilerAppDevice::_lastCalc    = 0;
uint32_t BoilerAppDevice::_lastHistory = 0;
uint32_t BoilerAppDevice::_lastPub     = 0;

// ============================================================
//  HTML — вкладка мониторинга (повторяет макет)
// ============================================================
static const char BOILER_MON_HTML[] PROGMEM = R"html(
<div class="monitor-page">

  <div class="m-card">
    <div class="m-card-label">ПРОИЗВОДИТЕЛЬНОСТЬ</div>
    <div class="m-power-big"><span id="b_power">--</span><span class="unit">кВт</span></div>
    <div class="m-power-sub">Σ <span id="b_energy">--</span> кВт·ч <span class="muted">(сутки)</span></div>
    <div class="m-power-sub">
      Поток 1: <b id="b_flow1">--</b> л/мин &nbsp;|&nbsp;
      Поток 2: <b id="b_flow2">--</b> л/мин
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">СИСТЕМА ОТОПЛЕНИЯ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА</div>
        <div class="m-sensor-val hot" id="b_tsup">--°</div>
        <div class="m-sensor-icon">▲</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА</div>
        <div class="m-sensor-val cool" id="b_tret">--°</div>
        <div class="m-sensor-icon">•</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Т</div>
        <div class="m-sensor-val accent" id="b_tdelta">--°</div>
        <div class="m-sensor-sub muted">расчёт</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ДАВЛЕНИЕ В КОНТУРЕ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА P1</div>
        <div class="m-sensor-val accent" id="b_psup">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА P2</div>
        <div class="m-sensor-val accent" id="b_pret">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Р</div>
        <div class="m-sensor-val accent" id="b_pdelta">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
    </div>
  </div>

  <div class="card" style="margin-top:10px">
  <div class="section-label">АНАЛИТИКА (последний час)</div>

  <div style="display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:10px">
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_avg">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Ср. мощность кВт</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_peak">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Пик мощности кВт</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_dt">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Ср. ΔT °C</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_duty">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Duty cycle %</div>
    </div>
  </div>

  <div style="display:grid;grid-template-columns:1fr 1fr 1fr;gap:8px">
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_cyc">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Включений</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_run">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Работа мин</div>
    </div>
    <div style="background:#111318;border-radius:8px;padding:10px 8px;text-align:center">
      <div style="font-size:20px;font-weight:800;color:var(--accent)" id="a_idle">—</div>
      <div style="font-size:10px;color:var(--muted);margin-top:2px">Простой мин</div>
    </div>
  </div>

  <div style="margin-top:8px;padding:8px 10px;background:#111318;border-radius:8px;
              display:flex;align-items:center;gap:8px;font-size:13px">
    <span id="a_burner_led" style="width:10px;height:10px;border-radius:50%;
          background:var(--muted);flex-shrink:0"></span>
    <span id="a_burner_txt">—</span>
    <span style="margin-left:auto;color:var(--muted);font-size:11px">
      Текущий цикл: <b id="a_cycdur">—</b> мин
    </span>
  </div>
</div>

  <div class="m-card">
    <div class="m-card-label">ИСТОРИЯ МОЩНОСТИ</div>
    <canvas id="powerChart" height="80"></canvas>
  </div>

</div>

<script>
async function updateBoiler() {
  try {
    const r = await fetch('/api/boiler/data');
    const d = await r.json();
    document.getElementById('b_power').textContent   = d.power_kw.toFixed(1);
    document.getElementById('b_energy').textContent  = d.energy_kwh.toFixed(1);
    document.getElementById('b_flow1').textContent   = d.flow1_lpm.toFixed(1);
    document.getElementById('b_flow2').textContent   = d.flow2_lpm.toFixed(1);
    document.getElementById('b_tsup').textContent    = d.t_supply.toFixed(1)+'°';
    document.getElementById('b_tret').textContent    = d.t_return.toFixed(1)+'°';
    document.getElementById('b_tdelta').textContent  = d.t_delta.toFixed(1)+'°';
    document.getElementById('b_psup').textContent    = d.p_supply.toFixed(2);
    document.getElementById('b_pret').textContent    = d.p_return.toFixed(2);
    document.getElementById('b_pdelta').textContent  = d.p_delta.toFixed(2);
    drawChart(d.history || []);
    
    // Аналитика
    if (d.power_avg_1h !== undefined) {
      document.getElementById('a_avg').textContent    = d.power_avg_1h.toFixed(1);
      document.getElementById('a_peak').textContent   = d.power_peak_1h.toFixed(1);
      document.getElementById('a_dt').textContent     = d.delta_avg_1h.toFixed(1);
      document.getElementById('a_duty').textContent   = d.duty_pct + '%';
      document.getElementById('a_cyc').textContent    = d.cycles_1h;
      document.getElementById('a_run').textContent    = d.runtime_min;
      document.getElementById('a_idle').textContent   = d.idle_min;
      document.getElementById('a_cycdur').textContent = d.cycle_dur_min;
      const led = document.getElementById('a_burner_led');
      led.style.background = d.burner_on ? 'var(--ok)' : 'var(--muted)';
      led.style.boxShadow  = d.burner_on ? '0 0 6px var(--ok)' : 'none';
      document.getElementById('a_burner_txt').textContent =
        d.burner_on ? 'Горелка работает' : 'Горелка выключена';
      }


  } catch(e) {}
}

function drawChart(data) {
  const canvas = document.getElementById('powerChart');
  if (!canvas || !data.length) return;
  const ctx = canvas.getContext('2d');
  canvas.width = canvas.offsetWidth;
  const W = canvas.width, H = canvas.height;
  ctx.clearRect(0,0,W,H);
  const max = Math.max(...data, 1);
  const step = W / (data.length - 1 || 1);
  ctx.beginPath();
  ctx.strokeStyle = '#f0a500';
  ctx.lineWidth = 2;
  data.forEach((v,i) => {
    const x = i * step;
    const y = H - (v / max * H * 0.85) - 4;
    i === 0 ? ctx.moveTo(x,y) : ctx.lineTo(x,y);
  });
  ctx.stroke();
  ctx.lineTo(W,H); ctx.lineTo(0,H); ctx.closePath();
  ctx.fillStyle = 'rgba(240,165,0,0.08)';
  ctx.fill();
}

updateBoiler();
setInterval(updateBoiler, 2000);
</script>
)html";

// ============================================================
//  HTML — вкладка уставок
// ============================================================
static const char BOILER_CFG_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>🌡 Температурные уставки</h3>
  <label>Макс. температура (аварийное отключение):
    <input type="number" id="t_max" step="0.5"> °C
  </label>
  <label>Порог оповещения:
    <input type="number" id="t_alarm" step="0.5"> °C
  </label>
  <label>Гистерезис:
    <input type="number" id="t_hyst" step="0.1"> °C
  </label>
  <label>Мин. температура (защита от замерзания):
    <input type="number" id="t_min" step="0.5"> °C
  </label>
</div>
<div class="settings-group">
  <h3>💧 Расходомеры</h3>
  <label>Расходомер 1 (сухой контакт) — литров на импульс:
    <input type="number" id="flow1_lpp" step="0.01">
  </label>
  <label>Расходомер 2 (Холл) — литров на импульс:
    <input type="number" id="flow2_lpp" step="0.01">
  </label>
</div>
<div class="settings-group">
  <h3>📊 Давление</h3>
  <label>Датчик давления: 4мА = <input type="number" id="p_min" step="0.1"> бар</label>
  <label>Датчик давления: 20мА = <input type="number" id="p_max" step="0.5"> бар</label>
</div>
<div class="settings-group">
  <h3>📡 MQTT</h3>
  <label>Интервал публикации:
    <input type="number" id="pub_interval" step="1000"> мс
  </label>
</div>
<button onclick="saveBoilerCfg()">💾 Сохранить уставки</button>

<script>
async function loadBoilerCfg() {
  const r = await fetch('/api/boiler/config');
  const d = await r.json();
  document.getElementById('t_max').value        = d.t_max;
  document.getElementById('t_alarm').value      = d.t_alarm;
  document.getElementById('t_hyst').value       = d.t_hyst;
  document.getElementById('t_min').value        = d.t_min;
  document.getElementById('flow1_lpp').value    = d.flow1_liter_per_pulse;
  document.getElementById('flow2_lpp').value    = d.flow2_liter_per_pulse;
  document.getElementById('p_min').value        = d.p_min;
  document.getElementById('p_max').value        = d.p_max;
  document.getElementById('pub_interval').value = d.pub_interval;


  if (d.power_avg_1h  !== undefined) {
    document.getElementById('a_avg').textContent    = d.power_avg_1h.toFixed(1);
    document.getElementById('a_peak').textContent   = d.power_peak_1h.toFixed(1);
    document.getElementById('a_dt').textContent     = d.delta_avg_1h.toFixed(1);
    document.getElementById('a_duty').textContent   = d.duty_pct + '%';
    document.getElementById('a_cyc').textContent    = d.cycles_1h;
    document.getElementById('a_run').textContent    = d.runtime_min;
    document.getElementById('a_idle').textContent   = d.idle_min;
    document.getElementById('a_cycdur').textContent = d.cycle_dur_min;
    const led = document.getElementById('a_burner_led');
    led.style.background = d.burner_on ? 'var(--ok)' : 'var(--muted)';
    led.style.boxShadow  = d.burner_on ? '0 0 6px var(--ok)' : 'none';
    document.getElementById('a_burner_txt').textContent =
      d.burner_on ? 'Горелка работает' : 'Горелка выключена';
  }

  }
async function saveBoilerCfg() {
  const data = {
    t_max:                +document.getElementById('t_max').value,
    t_alarm:              +document.getElementById('t_alarm').value,
    t_hyst:               +document.getElementById('t_hyst').value,
    t_min:                +document.getElementById('t_min').value,
    flow1_liter_per_pulse:+document.getElementById('flow1_lpp').value,
    flow2_liter_per_pulse:+document.getElementById('flow2_lpp').value,
    p_min:                +document.getElementById('p_min').value,
    p_max:                +document.getElementById('p_max').value,
    pub_interval:         +document.getElementById('pub_interval').value,
  };
  const r = await fetch('/api/boiler/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Уставки сохранены');
}
loadBoilerCfg();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BoilerAppDevice::init() {
    loadConfig();

    // Регистрируем вкладки — мониторинг первым
    WebHandler::registerTab({"monitor",   "Мониторинг", "🔥", BOILER_MON_HTML});
    WebHandler::registerTab({"boiler_cfg","Уставки",    "⚙️",  BOILER_CFG_HTML});

    extern AsyncWebServer server;

    // Данные мониторинга
    server.on("/api/boiler/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_supply"]   = boilerAppState.t_supply;
        doc["t_return"]   = boilerAppState.t_return;
        doc["t_delta"]    = boilerAppState.t_delta;
        doc["p_supply"]   = boilerAppState.p_supply;
        doc["p_return"]   = boilerAppState.p_return;
        doc["p_delta"]    = boilerAppState.p_delta;
        doc["flow1_lpm"]  = boilerAppState.flow1_lpm;
        doc["flow2_lpm"]  = boilerAppState.flow2_lpm;
        doc["power_kw"]   = boilerAppState.power_kw;
        doc["energy_kwh"] = boilerAppState.energy_kwh;

        
        // Аналитика
        doc["power_avg_1h"]  = boilerAppState.power_avg_1h;
        doc["power_peak_1h"] = boilerAppState.power_peak_1h;
        doc["delta_avg_1h"]  = boilerAppState.delta_avg_1h;
        doc["cycles_1h"]     = boilerAppState.cycles_1h;
        doc["runtime_min"]   = boilerAppState.runtime_min;
        doc["idle_min"]      = boilerAppState.idle_min;
        doc["duty_pct"]      = boilerAppState.duty_pct;
        doc["cycle_dur_min"] = boilerAppState.cycle_dur_min;
        doc["burner_on"]     = boilerAppState.burner_on;
    
        JsonArray hist = doc["history"].to<JsonArray>();
        uint8_t start = (boilerAppState.hist_head - boilerAppState.hist_count
                         + BOILER_HIST_SIZE) % BOILER_HIST_SIZE;
        for (uint8_t i = 0; i < boilerAppState.hist_count; i++)
            hist.add(boilerAppState.hist_power[(start+i) % BOILER_HIST_SIZE]);

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Конфиг
    server.on("/api/boiler/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_max"]                = boilerAppCfg.t_max;
        doc["t_alarm"]              = boilerAppCfg.t_alarm;
        doc["t_hyst"]               = boilerAppCfg.t_hyst;
        doc["t_min"]                = boilerAppCfg.t_min;
        doc["flow1_liter_per_pulse"]= boilerAppCfg.flow1_liter_per_pulse;
        doc["flow2_liter_per_pulse"]= boilerAppCfg.flow2_liter_per_pulse;
        doc["p_min"]                = boilerAppCfg.p_min;
        doc["p_max"]                = boilerAppCfg.p_max;
        doc["pub_interval"]         = boilerAppCfg.pub_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/boiler/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            boilerAppCfg.t_max                 = o["t_max"]                 | 85.0f;
            boilerAppCfg.t_alarm               = o["t_alarm"]               | 80.0f;
            boilerAppCfg.t_hyst                = o["t_hyst"]                | 2.0f;
            boilerAppCfg.t_min                 = o["t_min"]                 | 40.0f;
            boilerAppCfg.flow1_liter_per_pulse = o["flow1_liter_per_pulse"] | 1.0f;
            boilerAppCfg.flow2_liter_per_pulse = o["flow2_liter_per_pulse"] | 1.0f;
            boilerAppCfg.p_min                 = o["p_min"]                 | 0.0f;
            boilerAppCfg.p_max                 = o["p_max"]                 | 10.0f;
            boilerAppCfg.pub_interval          = o["pub_interval"]          | 10000;
            BoilerAppDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

#ifdef MODULE_TELEGRAM
    TgHandler::onCommand = [](const String& cmd, const String& chat) {
        if (cmd == "/boiler") {
            String msg = "🔥 *Котёл*\n";
            msg += "Подача: "  + String(boilerAppState.t_supply, 1) + "°C\n";
            msg += "Обратка: " + String(boilerAppState.t_return, 1) + "°C\n";
            msg += "P1/P2: "   + String(boilerAppState.p_supply, 2)
                               + "/" + String(boilerAppState.p_return, 2) + " бар\n";
            msg += "Мощность: "+ String(boilerAppState.power_kw, 1) + " кВт";
            TgHandler::sendMessage(msg, chat);
        }
    };
#endif

    Serial.println("[BOILER] App init OK");
}

void BoilerAppDevice::_updateAnalytics() {
    auto& s = boilerAppState;
 
    // 1. Записать в часовой буфер
    s.pushHistory1h(s.power_kw, s.t_delta);
 
    // 2. Подсчёт показателей по буферу
    float sum_p = 0, peak_p = 0, sum_dt = 0;
    uint16_t on_ticks = 0;  // тиков (по 10 сек) когда котёл работал
    uint16_t prev_on = 0;
    uint16_t cyc = 0;
    uint16_t n = s.hist1h_count;
 
    for (uint16_t i = 0; i < n; i++) {
        // Идём от старых к новым
        uint16_t idx = (s.hist1h_head - n + i + BOILER_HIST1H_SIZE) % BOILER_HIST1H_SIZE;
        float p  = s.hist1h_power[idx];
        float dt = s.hist1h_delta[idx];
 
        sum_p  += p;
        sum_dt += dt;
        if (p > peak_p) peak_p = p;
 
        bool on = (p >= BOILER_POWER_ON_KW);
        if (on) on_ticks++;
        // Считаем включения: переход 0→1
        if (on && !prev_on) cyc++;
        prev_on = on;
    }
 
    if (n > 0) {
        s.power_avg_1h  = sum_p  / n;
        s.delta_avg_1h  = sum_dt / n;
        s.power_peak_1h = peak_p;
        s.cycles_1h     = cyc;
        // on_ticks × 10 сек → минуты
        s.runtime_min   = (on_ticks * 10) / 60;
        s.idle_min       = ((n - on_ticks) * 10) / 60;
        s.duty_pct      = (uint8_t)((on_ticks * 100) / n);
    }
 
    // 3. Текущий цикл
    bool now_on = (s.power_kw >= BOILER_POWER_ON_KW);
    if (now_on && !s.burner_on) {
        // Включился
        s.burner_on      = true;
        s.cycle_start_ms = millis();
    } else if (!now_on && s.burner_on) {
        // Выключился
        s.burner_on = false;
        s.cycle_start_ms = millis();
    }
    if (s.cycle_start_ms > 0) {
        s.cycle_dur_min = (uint16_t)((millis() - s.cycle_start_ms) / 60000);
    }
}

// ============================================================
//  LOOP
// ============================================================
void BoilerAppDevice::loop() {
    if (millis() - _lastCalc > 2000) {
        _lastCalc = millis();
        updateFromA16();
        calcPower();
        checkAlarms();
    }

    if (millis() - _lastHistory > 10000) {
        _lastHistory = millis();
        boilerAppState.pushHistory(boilerAppState.power_kw);
        _updateAnalytics(); 
        boilerAppState.energy_kwh += boilerAppState.power_kw * (10.0f / 3600.0f);
    }

    #ifdef MODULE_MQTT
        if (millis() - _lastPub > boilerAppCfg.pub_interval) {
            _lastPub = millis();
            publishMqtt();
        }
    #endif

    #ifdef DEVICE_EKONOM
        updateFromEkoNom();
    #endif
}

// ============================================================
//  ДАННЫЕ ИЗ A16
// ============================================================
void BoilerAppDevice::updateFromA16() {
    // Температуры из 1-Wire (читает A16Device)
    extern A16State a16State;
    if (a16State.ow_count > BOILER_OW_SUPPLY && a16State.ow_ok[BOILER_OW_SUPPLY])
        boilerAppState.t_supply = a16State.ow_temp[BOILER_OW_SUPPLY];
    if (a16State.ow_count > BOILER_OW_RETURN && a16State.ow_ok[BOILER_OW_RETURN])
        boilerAppState.t_return = a16State.ow_temp[BOILER_OW_RETURN];
    boilerAppState.t_delta = boilerAppState.t_supply - boilerAppState.t_return;

    // Давление из ADC (уже сконвертировано A16Device в физические единицы)
    boilerAppState.p_supply = roundf(a16State.adc_value[BOILER_ADC_P1] * 10.0f) / 10.0f;
    boilerAppState.p_return = roundf(a16State.adc_value[BOILER_ADC_P2] * 10.0f) / 10.0f;    
    boilerAppState.p_delta  = boilerAppState.p_supply - boilerAppState.p_return;

    // Расход из счётчиков импульсов DIN
    // Расход (л/мин) = (импульсов за интервал) × (л/имп) / (dt в минутах)
    static uint32_t lastFlowCalc = 0;
    uint32_t now = millis();
    float dt_min = (now - lastFlowCalc) / 60000.0f;
    if (dt_min > 0 && lastFlowCalc > 0) {
        uint32_t d1 = a16State.din_counter[BOILER_DIN_FLOW1] - boilerAppState.flow1_last;
        uint32_t d2 = a16State.din_counter[BOILER_DIN_FLOW2] - boilerAppState.flow2_last;
        boilerAppState.flow1_lpm = (d1 * boilerAppCfg.flow1_liter_per_pulse) / dt_min;
        boilerAppState.flow2_lpm = (d2 * boilerAppCfg.flow2_liter_per_pulse) / dt_min;
    }
    boilerAppState.flow1_last = a16State.din_counter[BOILER_DIN_FLOW1];
    boilerAppState.flow2_last = a16State.din_counter[BOILER_DIN_FLOW2];
    lastFlowCalc = now;

    // Итоговый расход — берём больший из двух
    boilerAppState.flow_lpm = max(boilerAppState.flow1_lpm, boilerAppState.flow2_lpm);
}

#ifdef DEVICE_EKONOM
void BoilerAppDevice::updateFromEkoNom() {
    if (!ekoNomData.valid) return;
    boilerAppState.t_supply  = ekoNomData.t_supply;
    boilerAppState.t_return  = ekoNomData.t_return;
    boilerAppState.t_delta   = ekoNomData.t_delta;
    float lpm = ekoNomData.flow_m3h * 1000.0f / 60.0f;
    boilerAppState.flow1_lpm = lpm;   // отображается в UI
    boilerAppState.flow2_lpm = 0.0f;  // второго расходомера нет
    boilerAppState.flow_lpm  = lpm;   // используется для расчёта мощности 
    boilerAppState.power_kw  = ekoNomData.power_kw;
}
#endif

void BoilerAppDevice::calcPower() {
    // P(кВт) = m_dot(кг/с) × Cp(кДж/кг·К) × ΔT(К)
    // m_dot = flow_lpm / 60 (л/с = кг/с для воды)
    float m_dot = boilerAppState.flow_lpm / 60.0f;
    boilerAppState.power_kw = m_dot * 4.186f * boilerAppState.t_delta;
    if (boilerAppState.power_kw < 0) boilerAppState.power_kw = 0;
}

void BoilerAppDevice::checkAlarms() {
    bool wasHigh = boilerAppState.alarm_high;
    boilerAppState.alarm_high   = boilerAppState.t_supply > boilerAppCfg.t_max;
    boilerAppState.alarm_low    = boilerAppState.t_supply < boilerAppCfg.t_min
                                  && boilerAppState.t_supply > 1.0f;
    boilerAppState.alarm_sensor = !a16State.ow_ok[BOILER_OW_SUPPLY]
                                  || !a16State.ow_ok[BOILER_OW_RETURN];

#ifdef MODULE_TELEGRAM
    if (boilerAppState.alarm_high && !wasHigh) {
        TgHandler::sendMessage("🚨 *АВАРИЯ*: Температура подачи "
            + String(boilerAppState.t_supply, 1) + "°C > "
            + String(boilerAppCfg.t_max, 1) + "°C!");
    }
    if (!boilerAppState.alarm_high && wasHigh)
        TgHandler::sendMessage("✅ Температура в норме");
#endif
}

void BoilerAppDevice::publishMqtt() {
#ifdef MODULE_MQTT
    String b = String(baseCfg.device_name) + "/boiler/";
    char buf[16];
    dtostrf(boilerAppState.t_supply,  1, 1, buf); MqttHandler::publish((b+"t_supply").c_str(),  buf);
    dtostrf(boilerAppState.t_return,  1, 1, buf); MqttHandler::publish((b+"t_return").c_str(),  buf);
    dtostrf(boilerAppState.p_supply,  1, 2, buf); MqttHandler::publish((b+"p_supply").c_str(),  buf);
    dtostrf(boilerAppState.p_return,  1, 2, buf); MqttHandler::publish((b+"p_return").c_str(),  buf);
    dtostrf(boilerAppState.flow1_lpm, 1, 1, buf); MqttHandler::publish((b+"flow1_lpm").c_str(), buf);
    dtostrf(boilerAppState.flow2_lpm, 1, 1, buf); MqttHandler::publish((b+"flow2_lpm").c_str(), buf);
    dtostrf(boilerAppState.power_kw,  1, 2, buf); MqttHandler::publish((b+"power_kw").c_str(),  buf);
#endif
}

// ============================================================
//  CONFIG
// ============================================================
bool BoilerAppDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/boiler.json", doc)) return false;
    boilerAppCfg.t_max                 = doc["t_max"]                 | 85.0f;
    boilerAppCfg.t_alarm               = doc["t_alarm"]               | 80.0f;
    boilerAppCfg.t_hyst                = doc["t_hyst"]                | 2.0f;
    boilerAppCfg.t_min                 = doc["t_min"]                 | 40.0f;
    boilerAppCfg.flow1_liter_per_pulse = doc["flow1_liter_per_pulse"] | 1.0f;
    boilerAppCfg.flow2_liter_per_pulse = doc["flow2_liter_per_pulse"] | 1.0f;
    boilerAppCfg.p_min                 = doc["p_min"]                 | 0.0f;
    boilerAppCfg.p_max                 = doc["p_max"]                 | 10.0f;
    boilerAppCfg.pub_interval          = doc["pub_interval"]          | 10000;
    return true;
}

bool BoilerAppDevice::saveConfig() {
    JsonDocument doc;
    doc["t_max"]                 = boilerAppCfg.t_max;
    doc["t_alarm"]               = boilerAppCfg.t_alarm;
    doc["t_hyst"]                = boilerAppCfg.t_hyst;
    doc["t_min"]                 = boilerAppCfg.t_min;
    doc["flow1_liter_per_pulse"] = boilerAppCfg.flow1_liter_per_pulse;
    doc["flow2_liter_per_pulse"] = boilerAppCfg.flow2_liter_per_pulse;
    doc["p_min"]                 = boilerAppCfg.p_min;
    doc["p_max"]                 = boilerAppCfg.p_max;
    doc["pub_interval"]          = boilerAppCfg.pub_interval;
    return ConfigManager::saveJson("/boiler.json", doc);
}

#endif

```

---

### <a id="📄-src-apps-boiler-boilerappdevice-h"></a>📄 `src/apps/boiler/BoilerAppDevice.h`

**File Info:**
- **Size**: 631 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/apps/boiler/BoilerAppDevice.h`
- **Relative Path**: `src/apps/boiler`
- **Created**: 2026-02-18 04:57:12 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-22 18:41:55 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `35c75a808e7aed1c4dc2c9f87a230417`
- **SHA256**: `731077e579cf55c297271b051a6a619833e8df1624c3ac282bd66c7ab0a8dd03`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER)

#include "BoilerApp.h"

class BoilerAppDevice {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

private:
    static void updateFromA16();   // читает данные из a16State
    static void calcPower();
    static void checkAlarms();
    static void publishMqtt();

    static uint32_t _lastCalc;
    static uint32_t _lastHistory;
    static uint32_t _lastPub;

    static void _updateAnalytics();

    #ifdef DEVICE_EKONOM
        static void updateFromEkoNom();
    #endif
};

#endif

```

---

### <a id="📄-src-apps-boiler-room-boilerroomapp-h"></a>📄 `src/apps/boiler-room/BoilerRoomApp.h`

**File Info:**
- **Size**: 3.36 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/apps/boiler-room/BoilerRoomApp.h`
- **Relative Path**: `src/apps/boiler-room`
- **Created**: 2026-06-02 15:59:30 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-06-02 16:08:46 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `11d71b0d8d22e8c49c1cdca2d6308628`
- **SHA256**: `addb5adccc72279bfcdca496bdf4b3af6240ba4d35e2c29b52d007bf80f053e5`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)

#include <Arduino.h>

// ============================================================
//  ПРИВЯЗКА ДАТЧИКОВ К КАНАЛАМ A16
// ============================================================

// Цифровые входы
#define BRR_DIN_WATER       0   // DI1 — счётчик воды (импульсный)
#define BRR_DIN_GAS         1   // DI2 — счётчик газа (импульсный)

// Аналоговые входы (4-20мА)
#define BRR_ADC_P_HEAT      0   // ADC CH1 — давление отопления
#define BRR_ADC_P_WATER     1   // ADC CH2 — давление воды ХВС

// ============================================================
//  КОНФИГ — сохраняется в /boilerroom.json
// ============================================================
struct BoilerRoomConfig {
    // Коэффициенты счётчиков
    float water_m3_per_pulse    = 0.01f;  // м³/импульс
    float gas_m3_per_pulse      = 0.01f;  // м³/импульс

    // Калибровка давления (4-20мА → бар)
    float p_heat_min            = 0.0f;   // 4мА = 0 бар
    float p_heat_max            = 10.0f;  // 20мА = 10 бар
    float p_water_min           = 0.0f;
    float p_water_max           = 10.0f;

    // Интервал публикации MQTT (мс)
    uint32_t pub_interval       = 10000;
};

// ============================================================
//  СОСТОЯНИЕ — runtime
// ============================================================
struct BoilerRoomState {
    // Теплосчётчик (из EkoNomDevice)
    float    heat_t_supply   = 0.0f;   // температура подачи, °C
    float    heat_t_return   = 0.0f;   // температура обратки, °C
    float    heat_t_delta    = 0.0f;   // разность, °C
    float    heat_flow_m3h   = 0.0f;   // расход теплоносителя, м³/ч
    float    heat_power_kw   = 0.0f;   // тепловая мощность, кВт
    float    heat_energy_kwh = 0.0f;   // накопленная энергия, кВт·ч
    float    heat_volume_m3  = 0.0f;   // накопленный объём, м³
    bool     heat_valid      = false;  // данные актуальны
    uint32_t heat_updated_ms = 0;      // millis() последнего обновления

    // Давление отопления
    float    p_heat          = 0.0f;   // бар

    // Давление воды ХВС
    float    p_water         = 0.0f;   // бар

    // Счётчик воды
    uint32_t water_pulses    = 0;      // импульсы (снапшот последнего цикла)
    uint32_t water_pulses_prev = 0;
    float    water_total_m3  = 0.0f;  // накопленный объём, м³
    float    water_rate_lpm  = 0.0f;  // текущий расход, л/мин

    // Счётчик газа
    uint32_t gas_pulses      = 0;
    uint32_t gas_pulses_prev = 0;
    float    gas_total_m3    = 0.0f;  // накопленный объём, м³
    float    gas_rate_m3h    = 0.0f;  // текущий расход, м³/ч
};

extern BoilerRoomConfig brrCfg;
extern BoilerRoomState  brrState;

#endif // DEVICE_KC868_A16 && APP_BOILER_ROOM
```

---

### <a id="📄-src-apps-boiler-room-boilerroomdevice-cpp"></a>📄 `src/apps/boiler-room/BoilerRoomDevice.cpp`

**File Info:**
- **Size**: 21.91 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/apps/boiler-room/BoilerRoomDevice.cpp`
- **Relative Path**: `src/apps/boiler-room`
- **Created**: 2026-06-02 15:59:44 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-06-02 16:08:39 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `8f5f3c68d32a4ba4f2cbb5bbb390679e`
- **SHA256**: `9adebdee7bcdfa1a9facb335d5ed4be7d00bb727f2fc4c21bb2a74d8713567db`
- **Encoding**: UTF-8

**File code content:**

```cpp
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
```

---

### <a id="📄-src-apps-boiler-room-boilerroomdevice-h"></a>📄 `src/apps/boiler-room/BoilerRoomDevice.h`

**File Info:**
- **Size**: 617 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/apps/boiler-room/BoilerRoomDevice.h`
- **Relative Path**: `src/apps/boiler-room`
- **Created**: 2026-06-02 15:59:38 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-06-02 16:13:15 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `bf3b778788228315de102fc04fde9df3`
- **SHA256**: `f5b54332e20ad6174559c8f39ca537e5c2a88adf5d548859ea2dfcd0f7d02ac5`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)

#include "BoilerRoomApp.h"

class BoilerRoomDevice {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

private:
    static void _updateFromA16();       // давление + импульсы
    static void _updateFromEkoNom();    // теплосчётчик
    static void _calcRates(uint32_t dt_ms);        // расходы по счётчикам
    static void _publishMqtt();

    static uint32_t _lastCalc;
    static uint32_t _lastPub;
};

#endif
```

---

### <a id="📄-src-core-configmanager-cpp"></a>📄 `src/core/ConfigManager.cpp`

**File Info:**
- **Size**: 2.31 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/core/ConfigManager.cpp`
- **Relative Path**: `src/core`
- **Created**: 2026-02-17 12:55:32 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:55:32 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `b06a2b68f36164ce6a617bd9d33380d8`
- **SHA256**: `14de2f4c13e57787a6b0c9c86db5c892102c08d44507e18d6b6f5580a6a87dd9`
- **Encoding**: ASCII

**File code content:**

```cpp
#include "ConfigManager.h"

BaseConfig   baseCfg;
SystemState  sysState;
SemaphoreHandle_t coreMutex = xSemaphoreCreateMutex();

void ConfigManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("[CFG] LittleFS mount failed!");
        return;
    }
    Serial.println("[CFG] LittleFS OK");
}

bool ConfigManager::loadJson(const char* path, JsonDocument& doc) {
    if (!LittleFS.exists(path)) return false;
    File f = LittleFS.open(path, "r");
    if (!f) return false;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    return !err;
}

bool ConfigManager::saveJson(const char* path, JsonDocument& doc) {
    File f = LittleFS.open(path, "w");
    if (!f) return false;
    bool ok = serializeJson(doc, f) > 0;
    f.close();
    return ok;
}

bool ConfigManager::loadBase() {
    JsonDocument doc;
    if (!loadJson("/base.json", doc)) return false;

    baseCfg.eth_dhcp = doc["eth_dhcp"] | true;
    strlcpy(baseCfg.eth_ip,       doc["eth_ip"]   | "192.168.1.100", sizeof(baseCfg.eth_ip));
    strlcpy(baseCfg.eth_mask,     doc["eth_mask"] | "255.255.255.0", sizeof(baseCfg.eth_mask));
    strlcpy(baseCfg.eth_gw,       doc["eth_gw"]   | "192.168.1.1",  sizeof(baseCfg.eth_gw));
    strlcpy(baseCfg.eth_dns,      doc["eth_dns"]  | "8.8.8.8",      sizeof(baseCfg.eth_dns));
    strlcpy(baseCfg.wifi_ssid,    doc["wifi_ssid"]| "",              sizeof(baseCfg.wifi_ssid));
    strlcpy(baseCfg.wifi_pass,    doc["wifi_pass"]| "",              sizeof(baseCfg.wifi_pass));
    strlcpy(baseCfg.web_user,     doc["web_user"] | "admin",         sizeof(baseCfg.web_user));
    strlcpy(baseCfg.web_pass,     doc["web_pass"] | "",              sizeof(baseCfg.web_pass));
    strlcpy(baseCfg.device_name,  doc["device_name"] | "esp32-device", sizeof(baseCfg.device_name));
    return true;
}

bool ConfigManager::saveBase() {
    JsonDocument doc;
    doc["eth_dhcp"]     = baseCfg.eth_dhcp;
    doc["eth_ip"]       = baseCfg.eth_ip;
    doc["eth_mask"]     = baseCfg.eth_mask;
    doc["eth_gw"]       = baseCfg.eth_gw;
    doc["eth_dns"]      = baseCfg.eth_dns;
    doc["wifi_ssid"]    = baseCfg.wifi_ssid;
    doc["wifi_pass"]    = baseCfg.wifi_pass;
    doc["web_user"]     = baseCfg.web_user;
    doc["web_pass"]     = baseCfg.web_pass;
    doc["device_name"]  = baseCfg.device_name;
    return saveJson("/base.json", doc);
}

```

---

### <a id="📄-src-core-configmanager-h"></a>📄 `src/core/ConfigManager.h`

**File Info:**
- **Size**: 442 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/core/ConfigManager.h`
- **Relative Path**: `src/core`
- **Created**: 2026-02-17 12:55:16 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:55:16 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `8080f0c3cea4dd179d96516c6688d45c`
- **SHA256**: `e4a299e12d63d2c60282202af412ae5b95db9f33e138bb1998ca49c4c8b5f937`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "CoreTypes.h"

class ConfigManager {
public:
    static void begin();

    // Core сеть
    static bool loadBase();
    static bool saveBase();

    // Хелпер: загрузить произвольный JSON-файл в JsonDocument
    static bool loadJson(const char* path, JsonDocument& doc);
    static bool saveJson(const char* path, JsonDocument& doc);
};

```

---

### <a id="📄-src-core-coretypes-h"></a>📄 `src/core/CoreTypes.h`

**File Info:**
- **Size**: 2.13 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/core/CoreTypes.h`
- **Relative Path**: `src/core`
- **Created**: 2026-02-17 12:55:08 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:55:08 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `e3eaedf32b8ea5a2cbf5f8a3362a1b09`
- **SHA256**: `156cf64315bda5bed0ff44f3ccdc85ac0a0dda8df029dc60755b0206ca0b70c9`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#include <Arduino.h>
#include <freertos/semphr.h>

// ============================================================
//  BASE CONFIG — сетевая часть, общая для всех устройств
// ============================================================
struct BaseConfig {
    // Ethernet
    bool eth_dhcp       = true;
    char eth_ip[16]     = "192.168.1.100";
    char eth_mask[16]   = "255.255.255.0";
    char eth_gw[16]     = "192.168.1.1";
    char eth_dns[16]    = "8.8.8.8";

    // WiFi
    char wifi_ssid[32]  = "";
    char wifi_pass[32]  = "";

    // Web auth
    char web_user[16]   = "admin";
    char web_pass[32]   = "";

    // Device name (используется в mDNS, MQTT client ID, AP SSID)
    char device_name[32] = "esp32-device";
};

// ============================================================
//  SYSTEM STATE — runtime, не сохраняется
// ============================================================
struct SystemState {
    // Сеть
    bool ethConnected   = false;
    bool wifiConnected  = false;
    bool apMode         = false;

    // Модули
    bool mqttConnected  = false;
    bool tgConnected    = false;

    // Управление
    bool pendingReboot  = false;
    uint32_t rebootAt   = 0;
};

// ============================================================
//  WEB TAB — регистрация вкладок модулями/устройствами
// ============================================================
struct WebTab {
    const char* id;       // "telegram", "boiler" — уникальный ID
    const char* label;    // Отображаемое имя
    const char* icon;     // Эмодзи или пусто
    const char* html;     // HTML-контент (PROGMEM или статическая строка)
    // API-маршруты регистрирует сам модуль в своём init()
};

// ============================================================
//  GLOBALS
// ============================================================
extern BaseConfig       baseCfg;
extern SystemState      sysState;
extern SemaphoreHandle_t coreMutex;

```

---

### <a id="📄-src-core-networkmanager-cpp"></a>📄 `src/core/NetworkManager.cpp`

**File Info:**
- **Size**: 5.84 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/core/NetworkManager.cpp`
- **Relative Path**: `src/core`
- **Created**: 2026-02-18 03:18:42 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-06-04 10:51:51 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `fe0a17c48533a51501608836ff46d3cd`
- **SHA256**: `2d170c101900fe5386af862451f1b87d322949d40f07a4f8546baffd66686183`
- **Encoding**: UTF-8

**File code content:**

```cpp
#include "NetworkManager.h"
#ifdef DEVICE_KC868_A16
#include "../devices/kc868-a16/A16Types.h"
#endif
#ifdef DEVICE_BOILER_MONITOR
#include "../devices/boiler-monitor/BMonTypes.h"
#endif

RTC_DATA_ATTR static uint8_t _ethRebootCount = 0;

DNSServer  NetworkManager::_dns;
uint32_t   NetworkManager::_lastEthCheck = 0;
// ETH watchdog
static bool     _ethWdArmed   = true;
static uint8_t  _ethWdPhySt   = 0;      // 0=idle, 1=phy low, 2=phy high
static uint32_t _ethWdTimer   = 0;

void NetworkManager::begin() {
    WiFi.onEvent(onWiFiEvent);

    // Статический IP для Ethernet если нужен
    if (!baseCfg.eth_dhcp) {
        IPAddress ip, mask, gw, dns;
        if (ip.fromString(baseCfg.eth_ip) && mask.fromString(baseCfg.eth_mask)) {
            gw.fromString(baseCfg.eth_gw);
            dns.fromString(baseCfg.eth_dns);
            ETH.config(ip, gw, mask, dns);
            Serial.println("[NET] ETH: static IP configured");
        }
    }

    // Режим тактирования ETH зависит от платы — каждый device определяет свой
    #ifdef DEVICE_KC868_A16
        ETH.begin(A16_ETH_ADDR, A16_ETH_POWER_PIN, A16_ETH_MDC_PIN, A16_ETH_MDIO_PIN,
                ETH_PHY_LAN8720, A16_ETH_CLK_MODE);
    #elif defined(DEVICE_BOILER_MONITOR)
        pinMode(16, OUTPUT);
        digitalWrite(16, HIGH);
        delay(500);
        ETH.begin(BMON_ETH_ADDR, BMON_ETH_POWER_PIN, BMON_ETH_MDC_PIN, BMON_ETH_MDIO_PIN, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN);
    #else
        ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN,
                ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN);
    #endif

    Serial.printf("[NET] Device name: %s\n", baseCfg.device_name);
}

void NetworkManager::_ethPhyReset() {
    Serial.println("[NET] ETH watchdog: resetting PHY");
    #ifdef ETH_POWER_PIN
    digitalWrite(ETH_POWER_PIN, LOW);
    #endif
}

void NetworkManager::loop() {
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool connected      = sysState.ethConnected || sysState.wifiConnected;
    bool apActive       = sysState.apMode;
    xSemaphoreGive(coreMutex);

    // ETH watchdog
    if (_ethWdArmed) {
        if (sysState.ethConnected) {
            _ethWdArmed    = false;
            _ethRebootCount = 0;
        } else if (_ethWdPhySt == 0 && millis() - _ethWdTimer > 30000) {
            if (_ethRebootCount < 3) {
                _ethPhyReset();
                _ethWdPhySt = 1;
                _ethWdTimer = millis();
            } else {
                Serial.println("[NET] ETH watchdog: giving up after 3 attempts");
                _ethWdArmed = false;
                // AP поднимется штатно через существующую логику
            }
        } else if (_ethWdPhySt == 1 && millis() - _ethWdTimer > 200) {
            // PHY был LOW 200мс — поднимаем
            #ifdef ETH_POWER_PIN
            digitalWrite(ETH_POWER_PIN, HIGH);
            #endif
            _ethWdPhySt = 2;
            _ethWdTimer = millis();
        } else if (_ethWdPhySt == 2 && millis() - _ethWdTimer > 30000) {
            // Ждали 30 сек после PHY reset — не помогло
            _ethRebootCount++;
            _ethWdPhySt = 0;
            _ethWdTimer = millis();
            Serial.printf("[NET] ETH watchdog: PHY reset attempt %d failed\n", _ethRebootCount);
        }
    }

    // Runtime watchdog — если сеть пропала надолго, перезагружаемся
    static uint32_t _netLostTimer    = 0;
    static bool     _netWasConnected = false;

    if (connected) {
        _netWasConnected = true;
        _netLostTimer    = millis();
    } else if (_netWasConnected && millis() - _netLostTimer > 300000) {
        Serial.println("[NET] Network lost for 5 min, rebooting");
        ESP.restart();
    }

    // Captive portal DNS
#ifdef MODULE_CAPTIVE_PORTAL
    if (apActive) _dns.processNextRequest();
#endif

    // Запуск AP если нет сети 30 сек
    static uint32_t lastCheck = 0;
    if (millis() - lastCheck < 2000) return;
    lastCheck = millis();

    if (!connected && !apActive && (millis() - _lastEthCheck > 30000)) {
        startAP();
    }
}

void NetworkManager::startAP() {
    Serial.println("[NET] Starting AP...");
    WiFi.mode(WIFI_AP);

    // SSID = device_name
    WiFi.softAP(baseCfg.device_name);

#ifdef MODULE_CAPTIVE_PORTAL
    _dns.start(53, "*", WiFi.softAPIP());
#endif

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    sysState.apMode = true;
    xSemaphoreGive(coreMutex);

    Serial.printf("[NET] AP started: %s / %s\n",
                  baseCfg.device_name, WiFi.softAPIP().toString().c_str());
}

void NetworkManager::onWiFiEvent(WiFiEvent_t event) {
    xSemaphoreTake(coreMutex, portMAX_DELAY);

    switch (event) {
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.printf("[NET] ETH IP: %s\n", ETH.localIP().toString().c_str());
            sysState.ethConnected = true;
            if (WiFi.getMode() & WIFI_MODE_STA) {
                WiFi.disconnect(true);
                sysState.wifiConnected = false;
            }
            _ethRebootCount = 0;
            _ethWdArmed     = false;
            break;

        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("[NET] ETH lost, trying WiFi...");
            sysState.ethConnected = false;
            _lastEthCheck = millis();
            if (strlen(baseCfg.wifi_ssid) > 0)
                WiFi.begin(baseCfg.wifi_ssid, baseCfg.wifi_pass);
            break;

        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.printf("[NET] WiFi IP: %s\n", WiFi.localIP().toString().c_str());
            sysState.wifiConnected = true;
            sysState.apMode = false;
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            sysState.wifiConnected = false;
            break;

        default: break;
    }

    xSemaphoreGive(coreMutex);
}

```

---

### <a id="📄-src-core-networkmanager-h"></a>📄 `src/core/NetworkManager.h`

**File Info:**
- **Size**: 408 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/core/NetworkManager.h`
- **Relative Path**: `src/core`
- **Created**: 2026-02-17 12:55:36 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-22 07:20:17 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `e3b658371ac67347e154019fc95a96c7`
- **SHA256**: `51dfe1fc3a5a8bb16a6220215490ac8b61b8ab26df3b17c52a297a398043ecc5`
- **Encoding**: ASCII

**File code content:**

```text
#pragma once
#include <WiFi.h>
#include <ETH.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include "CoreTypes.h"
#include "ConfigManager.h"

class NetworkManager {
public:
    static void begin();
    static void loop();

private:
    static void startAP();
    static void onWiFiEvent(WiFiEvent_t event);
    static void _ethPhyReset();
    static DNSServer  _dns;
    static uint32_t   _lastEthCheck;
};

```

---

### <a id="📄-src-core-webhandler-cpp"></a>📄 `src/core/WebHandler.cpp`

**File Info:**
- **Size**: 11.83 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/core/WebHandler.cpp`
- **Relative Path**: `src/core`
- **Created**: 2026-02-18 22:12:02 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-22 14:25:59 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `445e6aeb0a906ff1de0f6908403a8b14`
- **SHA256**: `e05cf81541002f01990cc58d0d296fd35963c59ef247e3ea617577ff5079f1fc`
- **Encoding**: UTF-8

**File code content:**

```cpp
#include "WebHandler.h"
#include "ConfigManager.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ETH.h>
#include <WiFi.h>

#ifdef MODULE_MODEM
extern int  _modemCreg;
extern bool _modemEnabled;
#endif

static const char BUILD_TIME[] = __DATE__ " " __TIME__;

std::vector<WebTab> WebHandler::_tabs;

void WebHandler::registerTab(const WebTab& tab) {
    _tabs.push_back(tab);
    Serial.printf("[WEB] Tab registered: %s\n", tab.id);
}

// Генерирует nav из зарегистрированных вкладок
String WebHandler::buildNav() {
    String nav = "";
    for (size_t i = 0; i < _tabs.size(); i++) {
        String cls = (i == 0) ? "tab-btn active" : "tab-btn";
        nav += "<button class=\"" + String(cls) + "\" onclick=\"openTab(event,'";
        nav += _tabs[i].id;
        nav += "')\">";
        if (_tabs[i].icon && strlen(_tabs[i].icon) > 0) {
            nav += String(_tabs[i].icon) + " ";
        }
        nav += _tabs[i].label;
        nav += "</button>";
    }
    return nav;
}

void WebHandler::init(AsyncWebServer& server) {

    // ── /api/nav — список вкладок для фронтенда ──────────────
    server.on("/api/nav", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (size_t i = 0; i < _tabs.size(); i++) {
            JsonObject t = arr.add<JsonObject>();
            t["id"]    = _tabs[i].id;
            t["label"] = _tabs[i].label;
            t["icon"]  = _tabs[i].icon ? _tabs[i].icon : "";
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/tab?id=xxx — HTML-контент вкладки ────────────────
    server.on("/api/tab", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!req->hasParam("id")) { req->send(400); return; }
        String id = req->getParam("id")->value();
        for (auto& tab : _tabs) {
            if (id == tab.id) {
                AsyncWebServerResponse* resp = req->beginResponse(200, "text/html", tab.html);
                resp->addHeader("Cache-Control", "no-store");
                req->send(resp);
                return;
            }
        }
        req->send(404, "text/plain", "Tab not found");
    });

    // ── /api/status — состояние системы ──────────────────────
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        doc["eth"]   = sysState.ethConnected;
        doc["eth_ip"] = sysState.ethConnected ? ETH.localIP().toString() : "";
        doc["wifi"]  = sysState.wifiConnected;
        doc["ap"]    = sysState.apMode;
        doc["mqtt"]  = sysState.mqttConnected;
        doc["tg"]    = sysState.tgConnected;
        doc["uptime"] = millis() / 1000;
        doc["build"] = BUILD_TIME;
        xSemaphoreGive(coreMutex);
        #ifdef MODULE_MODEM
        doc["modem_enabled"] = _modemEnabled;
        doc["modem_creg"]    = _modemCreg;
        #endif
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/net-status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["eth_connected"]  = sysState.ethConnected;
        doc["wifi_connected"] = sysState.wifiConnected;
        doc["ap_mode"]        = sysState.apMode;
        doc["eth_ip"]         = sysState.ethConnected ? ETH.localIP().toString() : "";
        doc["wifi_ip"]        = sysState.wifiConnected ? WiFi.localIP().toString() : "";
        doc["ap_ip"]          = sysState.apMode ? WiFi.softAPIP().toString() : "";
        doc["wifi_rssi"]      = sysState.wifiConnected ? WiFi.RSSI() : 0;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/wifi-scan", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc["networks"].to<JsonArray>();
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; i++) {
            JsonObject net = arr.add<JsonObject>();
            net["ssid"] = WiFi.SSID(i);
            net["rssi"] = WiFi.RSSI(i);
            net["enc"]  = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
        }
        WiFi.scanDelete();
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/syslog", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        if (LittleFS.exists("/syslog.txt")) {
            File f = LittleFS.open("/syslog.txt", "r");
            if (f) { doc["log"] = f.readString(); f.close(); }
        } else { doc["log"] = ""; }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/syslog/clear", HTTP_POST, [](AsyncWebServerRequest* req) {
        LittleFS.remove("/syslog.txt");
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    // ── /api/get-config — текущий base конфиг ────────────────
    server.on("/api/get-config", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!req->authenticate(baseCfg.web_user, baseCfg.web_pass) && strlen(baseCfg.web_pass) > 0)
            return req->requestAuthentication();
        if (LittleFS.exists("/base.json"))
            req->send(LittleFS, "/base.json", "application/json");
        else
            req->send(404, "application/json", "{\"error\":\"no_config\"}");
    });

    // ── /api/save-config — сохранение base конфига ───────────
    auto* cfgHandler = new AsyncCallbackJsonWebHandler(
        "/api/save-config",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            if (!req->authenticate(baseCfg.web_user, baseCfg.web_pass) && strlen(baseCfg.web_pass) > 0)
                return req->requestAuthentication();

            JsonObject obj = json.as<JsonObject>();
            baseCfg.eth_dhcp = obj["eth_dhcp"] | true;
            strlcpy(baseCfg.eth_ip,      obj["eth_ip"]      | "", sizeof(baseCfg.eth_ip));
            strlcpy(baseCfg.eth_mask,    obj["eth_mask"]    | "", sizeof(baseCfg.eth_mask));
            strlcpy(baseCfg.eth_gw,      obj["eth_gw"]      | "", sizeof(baseCfg.eth_gw));
            strlcpy(baseCfg.eth_dns,     obj["eth_dns"]     | "", sizeof(baseCfg.eth_dns));
            strlcpy(baseCfg.wifi_ssid,   obj["wifi_ssid"]   | "", sizeof(baseCfg.wifi_ssid));
            strlcpy(baseCfg.wifi_pass,   obj["wifi_pass"]   | "", sizeof(baseCfg.wifi_pass));
            strlcpy(baseCfg.web_user,    obj["web_user"]    | "admin", sizeof(baseCfg.web_user));
            strlcpy(baseCfg.web_pass,    obj["web_pass"]    | "", sizeof(baseCfg.web_pass));
            strlcpy(baseCfg.device_name, obj["device_name"] | "esp32-device", sizeof(baseCfg.device_name));

            if (ConfigManager::saveBase())
                req->send(200, "application/json", "{\"status\":\"ok\"}");
            else
                req->send(500, "application/json", "{\"status\":\"error\"}");
        }
    );
    server.addHandler(cfgHandler);

    // ── /api/reboot ───────────────────────────────────────────
    server.on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "text/plain", "Rebooting...");
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        sysState.pendingReboot = true;
        sysState.rebootAt = millis();
        xSemaphoreGive(coreMutex);
    });

    server.on("/api/download-config", HTTP_GET, [](AsyncWebServerRequest* req) {
        if (!LittleFS.exists("/base.json")) { req->send(404); return; }
        req->send(LittleFS, "/base.json", "application/json");
    });

    // ── /api/backup — все конфиги в одном файле ───────────────
    server.on("/api/backup", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        const char* files[] = {"base", "mqtt", "tg", "boiler", "device"};
        for (auto name : files) {
            String path = String("/") + name + ".json";
            JsonDocument part;
            if (ConfigManager::loadJson(path.c_str(), part))
                doc[name] = part;
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // ── /api/restore — восстановить из backup.json ────────────
    auto* restoreH = new AsyncCallbackJsonWebHandler("/api/restore",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            const char* files[] = {"base", "mqtt", "tg", "boiler", "device"};
            for (auto name : files) {
                if (obj[name].is<JsonObject>()) {
                    JsonDocument part;
                    part.set(obj[name]);
                    ConfigManager::saveJson((String("/") + name + ".json").c_str(), part);
                }
            }
            // Перечитываем base сразу
            ConfigManager::loadBase();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
            xSemaphoreTake(coreMutex, portMAX_DELAY);
            sysState.pendingReboot = true;
            sysState.rebootAt = millis() + 1000;
            xSemaphoreGive(coreMutex);
        }
    );
    server.addHandler(restoreH);

    // ── OTA Update ────────────────────────────────────────────
#ifdef MODULE_OTA
    server.on("/update", HTTP_POST,
        [](AsyncWebServerRequest* req) {
            bool fail = Update.hasError();
            auto* resp = req->beginResponse(200, "text/plain", fail ? "FAIL" : "OK");
            resp->addHeader("Connection", "close");
            req->send(resp);
            if (!fail) {
                xSemaphoreTake(coreMutex, portMAX_DELAY);
                sysState.pendingReboot = true;
                sysState.rebootAt = millis();
                xSemaphoreGive(coreMutex);
            }
        },
        handleOtaUpload
    );
#endif

    // ── Captive portal 404 ────────────────────────────────────
    server.onNotFound([](AsyncWebServerRequest* req) {
        bool ap = false;
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        ap = sysState.apMode;
        xSemaphoreGive(coreMutex);

        if (ap) req->redirect("http://192.168.4.1");
        else    req->send(404, "text/plain", "Not Found");
    });

    // ── Статика (index.html + assets) ─────────────────────────
    // Фильтр: отдаём статику только если путь НЕ начинается с /api/
    server.serveStatic("/", LittleFS, "/")
          .setDefaultFile("index.html")
          .setFilter([](AsyncWebServerRequest* req) {
              return !req->url().startsWith("/api/");
          });
}

void WebHandler::handleOtaUpload(AsyncWebServerRequest* req, String filename,
                                  size_t index, uint8_t* data, size_t len, bool final) {
    if (!index) {
        int cmd = (filename.indexOf("littlefs") > -1 || filename.indexOf("spiffs") > -1)
                  ? U_SPIFFS : U_FLASH;
        Serial.printf("[OTA] Start: %s (%s)\n", filename.c_str(),
                      cmd == U_FLASH ? "firmware" : "filesystem");
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) Update.printError(Serial);
    }
    if (Update.write(data, len) != len) Update.printError(Serial);
    if (final) {
        if (Update.end(true)) Serial.printf("[OTA] Done: %u bytes\n", index + len);
        else Update.printError(Serial);
    }
}

```

---

### <a id="📄-src-core-webhandler-h"></a>📄 `src/core/WebHandler.h`

**File Info:**
- **Size**: 644 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/core/WebHandler.h`
- **Relative Path**: `src/core`
- **Created**: 2026-02-17 12:55:58 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:55:58 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `d3f61b65224e94a0539f45d5742fc823`
- **SHA256**: `a5872b78f6ba9d2182676d0fbf3f4d7adf67e158c83cb31e2c75eca029ffb076`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Update.h>
#include <vector>
#include "CoreTypes.h"

class WebHandler {
public:
    // Вызвать ДО init() — модули регистрируют свои вкладки
    static void registerTab(const WebTab& tab);

    static void init(AsyncWebServer& server);

private:
    static std::vector<WebTab> _tabs;

    // Генерация nav-бара из зарегистрированных вкладок
    static String buildNav();

    // OTA upload
    static void handleOtaUpload(AsyncWebServerRequest*, String, size_t, uint8_t*, size_t, bool);
};

```

---

### <a id="📄-src-devices-boiler-boilerdevice-cpp"></a>📄 `src/devices/boiler/BoilerDevice.cpp`

**File Info:**
- **Size**: 14.85 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/devices/boiler/BoilerDevice.cpp`
- **Relative Path**: `src/devices/boiler`
- **Created**: 2026-02-17 12:58:58 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:58:58 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `e2b1df6b8fe8af69f647d8d6f22d2b59`
- **SHA256**: `74894e2522e6a602eef3e87e64c1170021e75e3e238e16d38f0d39d8af3fbe16`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef DEVICE_BOILER
#include "BoilerDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../../modules/telegram/TgHandler.h"
#endif

BoilerConfig boilerCfg;
BoilerState  boilerState;

uint32_t BoilerDevice::_lastRead   = 0;
uint32_t BoilerDevice::_lastPub    = 0;
uint32_t BoilerDevice::_lastEnergy = 0;

// ============================================================
//  HTML ВКЛАДОК (PROGMEM)
// ============================================================

// --- Мониторинг (повторяет макет) ---
static const char BOILER_MONITOR_HTML[] PROGMEM = R"html(
<div class="monitor-page">

  <div class="m-card">
    <div class="m-card-label">ПРОИЗВОДИТЕЛЬНОСТЬ</div>
    <div class="m-power-big"><span id="b_power">--</span><span class="unit">кВт</span></div>
    <div class="m-power-sub">Σ <span id="b_energy">--</span> кВт·ч <span class="muted">(сутки)</span></div>
    <div class="m-power-sub">Поток: <b id="b_flow">--</b> л/мин</div>
  </div>

  <div class="m-card">
    <div class="m-card-label">СИСТЕМА ОТОПЛЕНИЯ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА</div>
        <div class="m-sensor-val hot" id="b_tsup">--°</div>
        <div class="m-sensor-icon">▲</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА</div>
        <div class="m-sensor-val cool" id="b_tret">--°</div>
        <div class="m-sensor-icon">•</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Т</div>
        <div class="m-sensor-val accent" id="b_tdelta">--°</div>
        <div class="m-sensor-sub muted">расчёт</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ОКРУЖАЮЩАЯ СРЕДА</div>
    <div class="m-sensors-row two">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОМЕЩЕНИЕ</div>
        <div class="m-sensor-val warm" id="b_troom">--°</div>
        <div class="m-sensor-icon cool">▼</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">УЛИЦА</div>
        <div class="m-sensor-val cool" id="b_tout">--°</div>
        <div class="m-sensor-icon cool">▼</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ДАВЛЕНИЕ В КОНТУРЕ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА P1</div>
        <div class="m-sensor-val accent" id="b_psup">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА P2</div>
        <div class="m-sensor-val accent" id="b_pret">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Р</div>
        <div class="m-sensor-val accent" id="b_pdelta">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ИСТОРИЯ МОЩНОСТИ</div>
    <canvas id="powerChart" height="80"></canvas>
  </div>

</div>

<script>
async function updateBoiler() {
  try {
    const r = await fetch('/api/boiler/data');
    const d = await r.json();
    document.getElementById('b_power').textContent  = d.power_kw.toFixed(1);
    document.getElementById('b_energy').textContent = d.energy_kwh.toFixed(1);
    document.getElementById('b_flow').textContent   = d.flow_lpm.toFixed(1);
    document.getElementById('b_tsup').textContent   = d.t_supply.toFixed(1)+'°';
    document.getElementById('b_tret').textContent   = d.t_return.toFixed(1)+'°';
    document.getElementById('b_tdelta').textContent = d.t_delta.toFixed(1)+'°';
    document.getElementById('b_troom').textContent  = d.t_room.toFixed(1)+'°';
    document.getElementById('b_tout').textContent   = d.t_outdoor.toFixed(1)+'°';
    document.getElementById('b_psup').textContent   = d.p_supply.toFixed(2);
    document.getElementById('b_pret').textContent   = d.p_return.toFixed(2);
    document.getElementById('b_pdelta').textContent = d.p_delta.toFixed(2);
    drawChart(d.history || []);
  } catch(e) {}
}

function drawChart(data) {
  const canvas = document.getElementById('powerChart');
  if (!canvas || !data.length) return;
  const ctx = canvas.getContext('2d');
  canvas.width = canvas.offsetWidth;
  const W = canvas.width, H = canvas.height;
  ctx.clearRect(0,0,W,H);
  const max = Math.max(...data, 1);
  const step = W / (data.length - 1 || 1);
  ctx.beginPath();
  ctx.strokeStyle = '#f0a500';
  ctx.lineWidth = 2;
  data.forEach((v,i) => {
    const x = i * step;
    const y = H - (v / max * H * 0.85) - 4;
    i === 0 ? ctx.moveTo(x,y) : ctx.lineTo(x,y);
  });
  ctx.stroke();

  // Fill under line
  ctx.lineTo(W, H); ctx.lineTo(0, H); ctx.closePath();
  ctx.fillStyle = 'rgba(240,165,0,0.08)';
  ctx.fill();
}

updateBoiler();
setInterval(updateBoiler, 2000);
</script>
)html";

// --- Уставки ---
static const char BOILER_THRESHOLDS_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>Пороги срабатывания</h3>
  <label>Макс. температура (аварийное отключение):
    <input type="number" id="t_max" step="0.5">°C
  </label>
  <label>Порог оповещения:
    <input type="number" id="t_alarm" step="0.5">°C
  </label>
  <label>Гистерезис:
    <input type="number" id="t_hyst" step="0.1">°C
  </label>
  <label>Мин. температура (защита от замерзания):
    <input type="number" id="t_min" step="0.5">°C
  </label>
  <label>Интервал публикации MQTT:
    <input type="number" id="pub_interval" step="1000"> мс
  </label>
  <button onclick="saveThresholds()">💾 Сохранить уставки</button>
</div>
<script>
async function loadThresholds() {
  const r = await fetch('/api/boiler/config');
  const d = await r.json();
  document.getElementById('t_max').value          = d.t_max;
  document.getElementById('t_alarm').value        = d.t_alarm;
  document.getElementById('t_hyst').value         = d.t_hyst;
  document.getElementById('t_min').value          = d.t_min;
  document.getElementById('pub_interval').value   = d.pub_interval;
}
async function saveThresholds() {
  const data = {
    t_max:        +document.getElementById('t_max').value,
    t_alarm:      +document.getElementById('t_alarm').value,
    t_hyst:       +document.getElementById('t_hyst').value,
    t_min:        +document.getElementById('t_min').value,
    pub_interval: +document.getElementById('pub_interval').value,
  };
  const r = await fetch('/api/boiler/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Уставки сохранены');
}
loadThresholds();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BoilerDevice::init() {
    loadConfig();

    // Регистрируем вкладки устройства
    // Мониторинг — первым, чтобы стоял первой вкладкой
    WebHandler::registerTab({"monitor",    "Мониторинг", "🔥", BOILER_MONITOR_HTML});
    WebHandler::registerTab({"thresholds", "Уставки",    "⚙️",  BOILER_THRESHOLDS_HTML});

    // API данных
    extern AsyncWebServer server;
    server.on("/api/boiler/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_supply"]   = boilerState.t_supply;
        doc["t_return"]   = boilerState.t_return;
        doc["t_delta"]    = boilerState.t_delta;
        doc["t_room"]     = boilerState.t_room;
        doc["t_outdoor"]  = boilerState.t_outdoor;
        doc["p_supply"]   = boilerState.p_supply;
        doc["p_return"]   = boilerState.p_return;
        doc["p_delta"]    = boilerState.p_delta;
        doc["power_kw"]   = boilerState.power_kw;
        doc["energy_kwh"] = boilerState.energy_kwh;
        doc["flow_lpm"]   = boilerState.flow_lpm;

        // История мощности — последние N точек в порядке времени
        JsonArray hist = doc["history"].to<JsonArray>();
        uint8_t start = (boilerState.hist_head - boilerState.hist_count + TEMP_HISTORY_SIZE) % TEMP_HISTORY_SIZE;
        for (uint8_t i = 0; i < boilerState.hist_count; i++) {
            hist.add(boilerState.hist_power[(start + i) % TEMP_HISTORY_SIZE]);
        }

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/boiler/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_max"]        = boilerCfg.t_max;
        doc["t_alarm"]      = boilerCfg.t_alarm;
        doc["t_hyst"]       = boilerCfg.t_hyst;
        doc["t_min"]        = boilerCfg.t_min;
        doc["pub_interval"] = boilerCfg.pub_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/boiler/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            boilerCfg.t_max        = obj["t_max"]        | 85.0f;
            boilerCfg.t_alarm      = obj["t_alarm"]      | 80.0f;
            boilerCfg.t_hyst       = obj["t_hyst"]       | 2.0f;
            boilerCfg.t_min        = obj["t_min"]        | 40.0f;
            boilerCfg.pub_interval = obj["pub_interval"] | 10000;
            BoilerDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

#ifdef MODULE_TELEGRAM
    // Добавляем команды котла в Telegram
    TgHandler::onCommand = [](const String& cmd, const String& chat) {
        if (cmd == "/temp") {
            String msg = "🌡 Котёл:\n";
            msg += "Подача: " + String(boilerState.t_supply, 1) + "°C\n";
            msg += "Обратка: " + String(boilerState.t_return, 1) + "°C\n";
            msg += "Мощность: " + String(boilerState.power_kw, 1) + " кВт";
            TgHandler::sendMessage(msg, chat);
        }
    };
#endif

    Serial.println("[BOILER] Device init OK");
}

// ============================================================
//  LOOP
// ============================================================
void BoilerDevice::loop() {
    // Опрос датчиков каждые 2 сек
    if (millis() - _lastRead > 2000) {
        _lastRead = millis();
        readSensors();
        checkAlarms();
    }

    // История мощности каждые 10 сек
    if (millis() - _lastEnergy > 10000) {
        _lastEnergy = millis();
        boilerState.pushHistory(boilerState.power_kw, millis() / 1000);
        // Накопление энергии: P(кВт) × dt(ч)
        boilerState.energy_kwh += boilerState.power_kw * (10.0f / 3600.0f);
    }

    // MQTT публикация
#ifdef MODULE_MQTT
    if (millis() - _lastPub > boilerCfg.pub_interval) {
        _lastPub = millis();
        publishMqtt();
    }
#endif

    // safe reboot
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (millis() - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}

void BoilerDevice::readSensors() {
    // TODO: заменить на реальное чтение датчиков
    // Пример для DS18B20 на OneWire, NTC, ADS1115 и т.д.
    // boilerState.t_supply  = ds.getTempCByIndex(0);
    // boilerState.t_return  = ds.getTempCByIndex(1);

    // Расчёт производных
    boilerState.t_delta = boilerState.t_supply - boilerState.t_return;
    boilerState.p_delta = boilerState.p_supply - boilerState.p_return;

    // Расчёт мощности: P = m_dot × Cp × ΔT
    // m_dot (кг/с) = flow_lpm / 60 * плотность воды (~1 кг/л)
    // Cp воды = 4.186 кДж/(кг·К)
    float m_dot = boilerState.flow_lpm / 60.0f;
    boilerState.power_kw = m_dot * 4.186f * boilerState.t_delta;
    if (boilerState.power_kw < 0) boilerState.power_kw = 0;
}

void BoilerDevice::checkAlarms() {
    bool wasAlarmHigh = boilerState.alarm_high;

    boilerState.alarm_high = boilerState.t_supply > boilerCfg.t_max;
    boilerState.alarm_low  = boilerState.t_supply < boilerCfg.t_min
                             && boilerState.t_supply > 1.0f; // 0 = датчик не подключён

    // Оповещение при новой аварии
#ifdef MODULE_TELEGRAM
    if (boilerState.alarm_high && !wasAlarmHigh) {
        TgHandler::sendMessage(
            "🚨 *АВАРИЯ*: Температура подачи " +
            String(boilerState.t_supply, 1) + "°C превысила " +
            String(boilerCfg.t_max, 1) + "°C!"
        );
    }
    if (!boilerState.alarm_high && wasAlarmHigh) {
        TgHandler::sendMessage("✅ Температура в норме");
    }
#endif
}

void BoilerDevice::publishMqtt() {
#ifdef MODULE_MQTT
    String base = String(baseCfg.device_name) + "/";
    char buf[12];

    dtostrf(boilerState.t_supply,  1, 1, buf); MqttHandler::publish((base+"t_supply").c_str(),  buf);
    dtostrf(boilerState.t_return,  1, 1, buf); MqttHandler::publish((base+"t_return").c_str(),  buf);
    dtostrf(boilerState.t_room,    1, 1, buf); MqttHandler::publish((base+"t_room").c_str(),    buf);
    dtostrf(boilerState.t_outdoor, 1, 1, buf); MqttHandler::publish((base+"t_outdoor").c_str(), buf);
    dtostrf(boilerState.power_kw,  1, 2, buf); MqttHandler::publish((base+"power_kw").c_str(),  buf);
    dtostrf(boilerState.p_supply,  1, 2, buf); MqttHandler::publish((base+"p_supply").c_str(),  buf);
    dtostrf(boilerState.p_return,  1, 2, buf); MqttHandler::publish((base+"p_return").c_str(),  buf);
#endif
}

bool BoilerDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/device.json", doc)) return false;
    boilerCfg.t_max        = doc["t_max"]        | 85.0f;
    boilerCfg.t_alarm      = doc["t_alarm"]      | 80.0f;
    boilerCfg.t_hyst       = doc["t_hyst"]       | 2.0f;
    boilerCfg.t_min        = doc["t_min"]        | 40.0f;
    boilerCfg.pub_interval = doc["pub_interval"] | 10000;
    return true;
}

bool BoilerDevice::saveConfig() {
    JsonDocument doc;
    doc["t_max"]        = boilerCfg.t_max;
    doc["t_alarm"]      = boilerCfg.t_alarm;
    doc["t_hyst"]       = boilerCfg.t_hyst;
    doc["t_min"]        = boilerCfg.t_min;
    doc["pub_interval"] = boilerCfg.pub_interval;
    return ConfigManager::saveJson("/device.json", doc);
}

#endif // DEVICE_BOILER

```

---

### <a id="📄-src-devices-boiler-boilerdevice-h"></a>📄 `src/devices/boiler/BoilerDevice.h`

**File Info:**
- **Size**: 621 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/devices/boiler/BoilerDevice.h`
- **Relative Path**: `src/devices/boiler`
- **Created**: 2026-02-17 12:57:58 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:57:58 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `b9351fb6d5d3f9e9f00615c7eb5fdaf6`
- **SHA256**: `a60e5fd8ce10944a620c461f37edf95d6ff449de1600a86b6fceb2fe02321a5a`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef DEVICE_BOILER

#include "BoilerTypes.h"
#include "../../core/CoreTypes.h"

class BoilerDevice {
public:
    static void init();   // Регистрирует вкладки, API, инициализирует датчики
    static void loop();   // Опрос датчиков, логика управления
    static bool loadConfig();
    static bool saveConfig();

private:
    static void readSensors();
    static void checkAlarms();
    static void publishMqtt();

    static uint32_t _lastRead;
    static uint32_t _lastPub;
    static uint32_t _lastEnergy;
};

#endif // DEVICE_BOILER

```

---

### <a id="📄-src-devices-boiler-boilertypes-h"></a>📄 `src/devices/boiler/BoilerTypes.h`

**File Info:**
- **Size**: 2.42 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/devices/boiler/BoilerTypes.h`
- **Relative Path**: `src/devices/boiler`
- **Created**: 2026-02-17 12:57:52 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:57:52 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `5cd499f27260791077b69f12c06831bb`
- **SHA256**: `15887e1fa3cc4e047296c6b9138cce9b5700d6916536f48499f6d7000bc62040`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef DEVICE_BOILER

#include <Arduino.h>

// ============================================================
//  КОНФИГ КОТЛА — уставки, сохраняются в /device.json
// ============================================================
struct BoilerConfig {
    float t_max         = 85.0f;   // Аварийное отключение
    float t_alarm       = 80.0f;   // Порог оповещения
    float t_hyst        = 2.0f;    // Гистерезис
    float t_min         = 40.0f;   // Защита от замерзания
    uint32_t pub_interval = 10000; // Интервал MQTT публикации (мс)
};

// ============================================================
//  СОСТОЯНИЕ КОТЛА — runtime, не сохраняется
// ============================================================

#define TEMP_HISTORY_SIZE 60  // 60 × 10сек = 10 минут

struct BoilerState {
    // Температуры
    float t_supply      = 0.0f;   // Подача
    float t_return      = 0.0f;   // Обратка
    float t_room        = 0.0f;   // Помещение
    float t_outdoor     = 0.0f;   // Улица
    float t_delta       = 0.0f;   // Дельта (расчёт)

    // Давление
    float p_supply      = 0.0f;   // Давление подача (бар)
    float p_return      = 0.0f;   // Давление обратка (бар)
    float p_delta       = 0.0f;   // Дельта давления

    // Производительность
    float power_kw      = 0.0f;   // Текущая мощность кВт
    float energy_kwh    = 0.0f;   // Накопленная энергия за сутки кВт·ч
    float flow_lpm      = 0.0f;   // Поток л/мин

    // Аварии
    bool alarm_high     = false;
    bool alarm_low      = false;
    bool alarm_sensor   = false;

    // История мощности (кольцевой буфер)
    float   hist_power[TEMP_HISTORY_SIZE] = {};
    uint32_t hist_ts[TEMP_HISTORY_SIZE]   = {};
    uint8_t  hist_head  = 0;
    uint8_t  hist_count = 0;

    void pushHistory(float power, uint32_t ts) {
        hist_power[hist_head] = power;
        hist_ts[hist_head]    = ts;
        hist_head = (hist_head + 1) % TEMP_HISTORY_SIZE;
        if (hist_count < TEMP_HISTORY_SIZE) hist_count++;
    }
};

extern BoilerConfig boilerCfg;
extern BoilerState  boilerState;

#endif // DEVICE_BOILER

```

---

### <a id="📄-src-devices-boiler-monitor-bmondevice-cpp"></a>📄 `src/devices/boiler-monitor/BMonDevice.cpp`

**File Info:**
- **Size**: 23.83 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/devices/boiler-monitor/BMonDevice.cpp`
- **Relative Path**: `src/devices/boiler-monitor`
- **Created**: 2026-03-20 07:06:03 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-09 05:48:40 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `4d6ff40402c6cc680c1b698383d362f4`
- **SHA256**: `8a0cac0dcc683ae765442d166699e1238fcc7c9dabec096b5feaf0467b9d18ac`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef DEVICE_BOILER_MONITOR

#include "BMonDevice.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../../modules/telegram/TgHandler.h"
#endif

// ============================================================
//  GLOBALS
// ============================================================
BMonConfig bmonCfg;
BMonState  bmonState;

static Adafruit_ADS1115 ads;
static bool adsOk = false;

uint32_t BMonDevice::_lastRead    = 0;
uint32_t BMonDevice::_lastFlow    = 0;
uint32_t BMonDevice::_lastHistory = 0;
uint32_t BMonDevice::_lastPub     = 0;
uint32_t BMonDevice::_pulse1_prev = 0;
uint32_t BMonDevice::_pulse2_prev = 0;

// ============================================================
//  ISR — счётчики расходомеров (IRAM)
// ============================================================
static void IRAM_ATTR onFlow1() { bmonState.pulse1_count++; }
static void IRAM_ATTR onFlow2() { bmonState.pulse2_count++; }

// ============================================================
//  HTML ВКЛАДОК (PROGMEM)
// ============================================================

static const char BMON_MONITOR_HTML[] PROGMEM = R"html(
<div class="monitor-page">

  <div class="m-card">
    <div class="m-card-label">ПРОИЗВОДИТЕЛЬНОСТЬ</div>
    <div class="m-power-big"><span id="bm_power">--</span><span class="unit">кВт</span></div>
    <div class="m-power-sub">Σ <span id="bm_energy">--</span> кВт·ч <span class="muted">(сессия)</span></div>
    <div class="m-power-sub">
      Поток 1: <b id="bm_flow1">--</b> л/мин &nbsp;|&nbsp;
      Поток 2: <b id="bm_flow2">--</b> л/мин
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ТЕМПЕРАТУРЫ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА</div>
        <div class="m-sensor-val hot" id="bm_tsup">--°</div>
        <div class="m-sensor-icon">▲</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА</div>
        <div class="m-sensor-val cool" id="bm_tret">--°</div>
        <div class="m-sensor-icon cool">▼</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Т</div>
        <div class="m-sensor-val accent" id="bm_tdelta">--°</div>
        <div class="m-sensor-sub muted">расчёт</div>
      </div>
    </div>
    <div class="m-sensors-row two" style="margin-top:8px">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОМЕЩЕНИЕ</div>
        <div class="m-sensor-val warm" id="bm_troom">--°</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">УЛИЦА</div>
        <div class="m-sensor-val cool" id="bm_tout">--°</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ДАВЛЕНИЕ В КОНТУРЕ</div>
    <div class="m-sensors-row">
      <div class="m-sensor">
        <div class="m-sensor-label">ПОДАЧА P1</div>
        <div class="m-sensor-val accent" id="bm_psup">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ОБРАТКА P2</div>
        <div class="m-sensor-val accent" id="bm_pret">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
      <div class="m-sensor">
        <div class="m-sensor-label">ДЕЛЬТА Р</div>
        <div class="m-sensor-val accent" id="bm_pdelta">--</div>
        <div class="m-sensor-sub muted">бар</div>
      </div>
    </div>
  </div>

  <div class="m-card">
    <div class="m-card-label">ИСТОРИЯ МОЩНОСТИ (10 мин)</div>
    <canvas id="bmPowerChart" height="80"></canvas>
  </div>

</div>

<script>
function fmtT(v) { return (v === null || v === undefined) ? '--°' : v.toFixed(1) + '°'; }
function fmtP(v) { return (v === null || v === undefined) ? '--'  : v.toFixed(2); }
function fmtF(v) { return (v === null || v === undefined) ? '--'  : v.toFixed(2); }

async function updateBMon() {
  try {
    const r = await fetch('/api/bmon/data');
    const d = await r.json();
    document.getElementById('bm_power').textContent  = d.power_kw.toFixed(1);
    document.getElementById('bm_energy').textContent = d.energy_kwh.toFixed(2);
    document.getElementById('bm_flow1').textContent  = fmtF(d.flow1_lpm);
    document.getElementById('bm_flow2').textContent  = fmtF(d.flow2_lpm);
    document.getElementById('bm_tsup').textContent   = fmtT(d.t_supply);
    document.getElementById('bm_tret').textContent   = fmtT(d.t_return);
    document.getElementById('bm_tdelta').textContent = fmtT(d.t_delta);
    document.getElementById('bm_troom').textContent  = fmtT(d.t_room);
    document.getElementById('bm_tout').textContent   = fmtT(d.t_outdoor);
    document.getElementById('bm_psup').textContent   = fmtP(d.p_supply);
    document.getElementById('bm_pret').textContent   = fmtP(d.p_return);
    document.getElementById('bm_pdelta').textContent = fmtP(d.p_delta);
    drawBMChart(d.history || []);
  } catch(e) {}
}

function drawBMChart(data) {
  const canvas = document.getElementById('bmPowerChart');
  if (!canvas || data.length < 2) return;
  const ctx = canvas.getContext('2d');
  canvas.width = canvas.offsetWidth;
  const W = canvas.width, H = canvas.height;
  ctx.clearRect(0,0,W,H);
  const max = Math.max(...data, 1);
  const step = W / (data.length - 1);
  ctx.beginPath();
  ctx.strokeStyle = '#f0a500';
  ctx.lineWidth = 2;
  data.forEach((v,i) => {
    const x = i * step;
    const y = H - (v / max * H * 0.85) - 4;
    i === 0 ? ctx.moveTo(x,y) : ctx.lineTo(x,y);
  });
  ctx.stroke();
  ctx.lineTo(W,H); ctx.lineTo(0,H); ctx.closePath();
  ctx.fillStyle = 'rgba(240,165,0,0.08)';
  ctx.fill();
}

updateBMon();
setInterval(updateBMon, 2000);
</script>
)html";

// --- Уставки ---
static const char BMON_SETTINGS_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>🌡 Температурные пороги</h3>
  <label>Макс. температура подачи (авария):
    <input type="number" id="bm_t_max" step="0.5">°C
  </label>
  <label>Порог оповещения:
    <input type="number" id="bm_t_alarm" step="0.5">°C
  </label>
  <label>Мин. температура улицы (заморозка):
    <input type="number" id="bm_t_min" step="0.5">°C
  </label>
</div>

<div class="settings-group">
  <h3>💧 Расходомеры</h3>
  <label>Расходомер 1 — л/имп:
    <input type="number" id="bm_flow1_lpi" step="0.001" min="0.001">
  </label>
  <label>Расходомер 2 — л/имп:
    <input type="number" id="bm_flow2_lpi" step="0.001" min="0.001">
  </label>
</div>

<div class="settings-group">
  <h3>📡 Публикация</h3>
  <label>Интервал MQTT (мс):
    <input type="number" id="bm_pub_interval" step="1000" min="1000">
  </label>
  <label>Коэффициент B датчика NTC:
    <input type="number" id="bm_ntc_b" step="10" min="1000" max="6000">
  </label>
</div>

<button onclick="saveBMonCfg()">💾 Сохранить уставки</button>

<script>
async function loadBMonCfg() {
  const r = await fetch('/api/bmon/config');
  const d = await r.json();
  document.getElementById('bm_t_max').value        = d.t_max;
  document.getElementById('bm_t_alarm').value      = d.t_alarm;
  document.getElementById('bm_t_min').value        = d.t_min;
  document.getElementById('bm_flow1_lpi').value    = d.flow1_lpi;
  document.getElementById('bm_flow2_lpi').value    = d.flow2_lpi;
  document.getElementById('bm_pub_interval').value = d.pub_interval;
  document.getElementById('bm_ntc_b').value        = d.ntc_b;
}
async function saveBMonCfg() {
  const data = {
    t_max:        +document.getElementById('bm_t_max').value,
    t_alarm:      +document.getElementById('bm_t_alarm').value,
    t_min:        +document.getElementById('bm_t_min').value,
    flow1_lpi:    +document.getElementById('bm_flow1_lpi').value,
    flow2_lpi:    +document.getElementById('bm_flow2_lpi').value,
    pub_interval: +document.getElementById('bm_pub_interval').value,
    ntc_b:        +document.getElementById('bm_ntc_b').value,
  };
  const r = await fetch('/api/bmon/save', {
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify(data)
  });
  if (r.ok) alert('Уставки сохранены');
}
loadBMonCfg();
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void BMonDevice::init() {
    loadConfig();

    // I2C
    Wire.begin(BMON_PIN_SDA, BMON_PIN_SCL);

    // ADS1115
    adsOk = ads.begin(BMON_ADS_ADDR);
    if (adsOk) {
        ads.setGain(BMON_ADS_GAIN);
        Serial.println("[BMON] ADS1115 OK");
    } else {
        Serial.println("[BMON] ADS1115 NOT FOUND — check wiring!");
        bmonState.alarm_sensor = true;
    }

    // Прерывания расходомеров — FALLING (замыкание контакта / фронт Холла)
    pinMode(BMON_PIN_FLOW1, INPUT_PULLUP);
    pinMode(BMON_PIN_FLOW2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BMON_PIN_FLOW1), onFlow1, FALLING);
    attachInterrupt(digitalPinToInterrupt(BMON_PIN_FLOW2), onFlow2, FALLING);
    Serial.println("[BMON] Flow interrupts attached");
    Serial.printf("[BMON] GPIO2=%d GPIO4=%d\n", digitalRead(BMON_PIN_FLOW1), digitalRead(BMON_PIN_FLOW2));

    // Аналоговые входы давления
    pinMode(BMON_PIN_PRESS1, INPUT);
    pinMode(BMON_PIN_PRESS2, INPUT);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);  // 0–3.3V диапазон

    // Регистрируем вкладки UI
    WebHandler::registerTab({"monitor",  "Мониторинг", "🔥", BMON_MONITOR_HTML});
    WebHandler::registerTab({"bmon_cfg", "Уставки",    "⚙️",  BMON_SETTINGS_HTML});

    // API
    extern AsyncWebServer server;

    server.on("/api/bmon/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        // Температуры: NaN → null в JSON
        if (!isnan(bmonState.t_supply))  doc["t_supply"]  = bmonState.t_supply;
        else                             doc["t_supply"]  = nullptr;
        if (!isnan(bmonState.t_return))  doc["t_return"]  = bmonState.t_return;
        else                             doc["t_return"]  = nullptr;
        if (!isnan(bmonState.t_room))    doc["t_room"]    = bmonState.t_room;
        else                             doc["t_room"]    = nullptr;
        if (!isnan(bmonState.t_outdoor)) doc["t_outdoor"] = bmonState.t_outdoor;
        else                             doc["t_outdoor"] = nullptr;
        doc["t_delta"]    = bmonState.t_delta;
        doc["p_supply"]   = bmonState.p_supply;
        doc["p_return"]   = bmonState.p_return;
        doc["p_delta"]    = bmonState.p_delta;
        doc["flow1_lpm"]  = bmonState.flow1_lpm;
        doc["flow2_lpm"]  = bmonState.flow2_lpm;
        doc["power_kw"]   = bmonState.power_kw;
        doc["energy_kwh"] = bmonState.energy_kwh;
        doc["alarm_high"] = bmonState.alarm_high;
        doc["alarm_low"]  = bmonState.alarm_low;
        doc["alarm_sensor"] = bmonState.alarm_sensor;
        doc["pulse1_count"] = bmonState.pulse1_count;
        doc["pulse2_count"] = bmonState.pulse2_count;

        JsonArray hist = doc["history"].to<JsonArray>();
        uint8_t start = (bmonState.hist_head - bmonState.hist_count + BMON_HISTORY_SIZE) % BMON_HISTORY_SIZE;
        for (uint8_t i = 0; i < bmonState.hist_count; i++)
            hist.add(bmonState.hist_power[(start + i) % BMON_HISTORY_SIZE]);

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/bmon/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["t_max"]        = bmonCfg.t_max;
        doc["t_alarm"]      = bmonCfg.t_alarm;
        doc["t_min"]        = bmonCfg.t_min;
        doc["flow1_lpi"]    = bmonCfg.flow1_lpi;
        doc["flow2_lpi"]    = bmonCfg.flow2_lpi;
        doc["pub_interval"] = bmonCfg.pub_interval;
        doc["ntc_b"]        = bmonCfg.ntc_b;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/bmon/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            bmonCfg.t_max        = obj["t_max"]        | 85.0f;
            bmonCfg.t_alarm      = obj["t_alarm"]      | 80.0f;
            bmonCfg.t_min        = obj["t_min"]        | 5.0f;
            bmonCfg.flow1_lpi    = obj["flow1_lpi"]    | 1.0f;
            bmonCfg.flow2_lpi    = obj["flow2_lpi"]    | 1.0f;
            bmonCfg.pub_interval = obj["pub_interval"] | 10000;
            bmonCfg.ntc_b        = obj["ntc_b"]        | BMON_NTC_B;
            BMonDevice::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

#ifdef MODULE_TELEGRAM
    TgHandler::onCommand = [](const String& cmd, const String& chat) {
        if (cmd == "/temp") {
            String msg = "🌡 Котёл:\n";
            if (!isnan(bmonState.t_supply))
                msg += "Подача: " + String(bmonState.t_supply, 1) + "°C\n";
            if (!isnan(bmonState.t_return))
                msg += "Обратка: " + String(bmonState.t_return, 1) + "°C\n";
            msg += "Мощность: " + String(bmonState.power_kw, 1) + " кВт\n";
            msg += "P1: " + String(bmonState.p_supply, 2) + " бар | ";
            msg += "P2: " + String(bmonState.p_return, 2) + " бар";
            TgHandler::sendMessage(msg, chat);
        }
        if (cmd == "/flow") {
            String msg = "💧 Расход:\n";
            msg += "Расходомер 1: " + String(bmonState.flow1_lpm, 2) + " л/мин\n";
            msg += "Расходомер 2: " + String(bmonState.flow2_lpm, 2) + " л/мин";
            TgHandler::sendMessage(msg, chat);
        }
    };
#endif

    Serial.println("[BMON] Device init OK");
}

// ============================================================
//  LOOP
// ============================================================
void BMonDevice::loop() {
    uint32_t now = millis();

    static uint32_t lastDbg = 0;
    if (millis() - lastDbg > 2000) {
        lastDbg = millis();
        Serial.printf("[BMON] GPIO2=%d GPIO4=%d\n", 
            digitalRead(BMON_PIN_FLOW1), digitalRead(BMON_PIN_FLOW2));
    }

    // Опрос температур и давления каждые 2 сек
    if (now - _lastRead > 2000) {
        _lastRead = now;
        readTemperatures();
        readPressure();
        checkAlarms();
    }

    // Расчёт расхода каждые 5 сек (считаем импульсы за интервал)
    if (now - _lastFlow > 5000) {
        calcFlow();
        _lastFlow = now;
    }

    // История мощности каждые 10 сек
    if (now - _lastHistory > 10000) {
        _lastHistory = now;
        bmonState.pushHistory(bmonState.power_kw);
        // Накопление энергии: P(кВт) × dt(ч)
        bmonState.energy_kwh += bmonState.power_kw * (10.0f / 3600.0f);
    }

    // MQTT публикация
#ifdef MODULE_MQTT
    if (now - _lastPub > bmonCfg.pub_interval) {
        _lastPub = now;
        publishMqtt();
    }
#endif

    // Перезагрузка
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (now - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}

// ============================================================
//  ЧТЕНИЕ NTC ЧЕРЕЗ ADS1115
//  Схема: VCC — Rref — нода — NTC — GND
//  Vin на ADS = напряжение на NTC
//  R_ntc = Rref * Vin / (VCC - Vin)
//  T = 1 / (1/T0 + ln(R/R0)/B) - 273.15
// ============================================================
static float ntcToTemp(int16_t raw, float ntc_b) {
    if (raw <= 0) return NAN;  // обрыв или КЗ
    // ADS1115 с GAIN_ONE: 1 LSB = 0.125 мВ, макс 32767 = 4.096V
    // Но VCC шильда = 3.3V, значит реальный Vin ≤ 3.3V
    float vin  = raw * 0.000125f;           // вольты
    if (vin <= 0.05f || vin >= 3.25f) return NAN; // обрыв/КЗ
    float r_ntc = BMON_NTC_RREF * vin / (3.3f - vin);
    float lnR   = logf(r_ntc / BMON_NTC_R0);
    float tempK = 1.0f / (1.0f / BMON_NTC_T0 + lnR / ntc_b);
    return tempK - 273.15f;
}

void BMonDevice::readTemperatures() {
    if (!adsOk) {
        bmonState.alarm_sensor = true;
        return;
    }
    bmonState.alarm_sensor = false;

    int16_t raw;
    raw = ads.readADC_SingleEnded(3); bmonState.t_supply  = ntcToTemp(raw, bmonCfg.ntc_b); // NTC1
    raw = ads.readADC_SingleEnded(2); bmonState.t_return  = ntcToTemp(raw, bmonCfg.ntc_b); // NTC2
    raw = ads.readADC_SingleEnded(1); bmonState.t_room    = ntcToTemp(raw, bmonCfg.ntc_b); // NTC3
    raw = ads.readADC_SingleEnded(0); bmonState.t_outdoor = ntcToTemp(raw, bmonCfg.ntc_b); // NTC4
      
    // Дельта только если оба датчика живые
    if (!isnan(bmonState.t_supply) && !isnan(bmonState.t_return))
        bmonState.t_delta = bmonState.t_supply - bmonState.t_return;
    else
        bmonState.t_delta = 0.0f;
}

// ============================================================
//  ЧТЕНИЕ ДАВЛЕНИЯ 4-20мА / 150 Ом
//  U = I × 150; I_min=4мА→0.6V, I_max=20мА→3.0V
//  P = (U - U_min) / (U_max - U_min) × P_max
// ============================================================
static float adcToPressure(int rawAdc) {
    float u = rawAdc / BMON_ADC_BITS * BMON_ADC_VREF;
    if (u < BMON_PRESS_U_MIN) return 0.0f;  // ниже 4мА — обрыв или 0
    float p = (u - BMON_PRESS_U_MIN) / (BMON_PRESS_U_MAX - BMON_PRESS_U_MIN) * BMON_PRESS_BAR_MAX;
    if (p < 0.0f) p = 0.0f;
    if (p > BMON_PRESS_BAR_MAX) p = BMON_PRESS_BAR_MAX;
    return p;
}

void BMonDevice::readPressure() {
    // Среднее из нескольких замеров для фильтрации шума АЦП ESP32
    const int N = 8;
    int32_t sum1 = 0, sum2 = 0;
    for (int i = 0; i < N; i++) {
        sum1 += analogRead(BMON_PIN_PRESS1);
        sum2 += analogRead(BMON_PIN_PRESS2);
    }
    bmonState.p_supply = adcToPressure(sum1 / N);
    bmonState.p_return = adcToPressure(sum2 / N);
    bmonState.p_delta  = bmonState.p_supply - bmonState.p_return;
}

// ============================================================
//  РАСЧЁТ РАСХОДА
//  Вызывается каждые 5 сек, считает импульсы за интервал
// ============================================================
void BMonDevice::calcFlow() {
    const float dt_min = 5.0f / 60.0f;  // 5 сек в минутах

    // Атомарно читаем счётчики (ISR пишет volatile)
    noInterrupts();
    uint32_t p1 = bmonState.pulse1_count;
    uint32_t p2 = bmonState.pulse2_count;
    interrupts();

    uint32_t diff1 = p1 - _pulse1_prev;
    uint32_t diff2 = p2 - _pulse2_prev;
    _pulse1_prev = p1;
    _pulse2_prev = p2;

    // л/мин = (импульсы × л/имп) / dt_мин
    bmonState.flow1_lpm = (diff1 * bmonCfg.flow1_lpi) / dt_min;
    bmonState.flow2_lpm = (diff2 * bmonCfg.flow2_lpi) / dt_min;

    // Тепловая мощность по расходомеру 1:
    // P(кВт) = m_dot(кг/с) × Cp(кДж/кг·К) × ΔT(К)
    // m_dot = flow_lpm / 60 (кг/с при плотности воды 1 кг/л)
    float m_dot = bmonState.flow1_lpm / 60.0f;
    if (!isnan(bmonState.t_delta) && bmonState.t_delta > 0)
        bmonState.power_kw = m_dot * 4.186f * bmonState.t_delta;
    else
        bmonState.power_kw = 0.0f;
}

// ============================================================
//  АВАРИИ
// ============================================================
void BMonDevice::checkAlarms() {
    bool wasHigh = bmonState.alarm_high;
    bool wasLow  = bmonState.alarm_low;

    bmonState.alarm_high = !isnan(bmonState.t_supply) &&
                            bmonState.t_supply > bmonCfg.t_max;

    bmonState.alarm_low  = !isnan(bmonState.t_outdoor) &&
                            bmonState.t_outdoor < bmonCfg.t_min;

#ifdef MODULE_TELEGRAM
    if (bmonState.alarm_high && !wasHigh) {
        TgHandler::sendMessage(
            "🚨 *АВАРИЯ*: Температура подачи " +
            String(bmonState.t_supply, 1) + "°C > " +
            String(bmonCfg.t_max, 1) + "°C!"
        );
    }
    if (!bmonState.alarm_high && wasHigh) {
        TgHandler::sendMessage("✅ Температура подачи в норме");
    }
    if (bmonState.alarm_low && !wasLow) {
        TgHandler::sendMessage(
            "❄️ *ВНИМАНИЕ*: Температура улицы " +
            String(bmonState.t_outdoor, 1) + "°C — риск замерзания!"
        );
    }
    if (bmonState.alarm_sensor && !wasHigh) {  // используем флаг для дебаунса
        TgHandler::sendMessage("⚠️ Ошибка ADS1115 — проверьте датчики");
    }
#endif
}

// ============================================================
//  MQTT
// ============================================================
void BMonDevice::publishMqtt() {
#ifdef MODULE_MQTT
    String base = String(baseCfg.device_name) + "/";
    char buf[12];

    auto pub = [&](const char* topic, float val, int dec = 1) {
        if (!isnan(val)) {
            dtostrf(val, 1, dec, buf);
            MqttHandler::publish((base + topic).c_str(), buf);
        }
    };

    pub("t_supply",  bmonState.t_supply);
    pub("t_return",  bmonState.t_return);
    pub("t_room",    bmonState.t_room);
    pub("t_outdoor", bmonState.t_outdoor);
    pub("t_delta",   bmonState.t_delta);
    pub("p_supply",  bmonState.p_supply, 2);
    pub("p_return",  bmonState.p_return, 2);
    pub("flow1_lpm", bmonState.flow1_lpm, 2);
    pub("flow2_lpm", bmonState.flow2_lpm, 2);
    pub("power_kw",  bmonState.power_kw, 2);
#endif
}

// ============================================================
//  КОНФИГ
// ============================================================
bool BMonDevice::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/device.json", doc)) return false;
    bmonCfg.t_max        = doc["t_max"]        | 85.0f;
    bmonCfg.t_alarm      = doc["t_alarm"]      | 80.0f;
    bmonCfg.t_min        = doc["t_min"]        | 5.0f;
    bmonCfg.flow1_lpi    = doc["flow1_lpi"]    | 1.0f;
    bmonCfg.flow2_lpi    = doc["flow2_lpi"]    | 1.0f;
    bmonCfg.pub_interval = doc["pub_interval"] | 10000;
    bmonCfg.ntc_b        = doc["ntc_b"]        | BMON_NTC_B;
    return true;
}

bool BMonDevice::saveConfig() {
    JsonDocument doc;
    doc["t_max"]        = bmonCfg.t_max;
    doc["t_alarm"]      = bmonCfg.t_alarm;
    doc["t_min"]        = bmonCfg.t_min;
    doc["flow1_lpi"]    = bmonCfg.flow1_lpi;
    doc["flow2_lpi"]    = bmonCfg.flow2_lpi;
    doc["pub_interval"] = bmonCfg.pub_interval;
    doc["ntc_b"]        = bmonCfg.ntc_b;
    return ConfigManager::saveJson("/device.json", doc);
}

#endif // DEVICE_BOILER_MONITOR
```

---

### <a id="📄-src-devices-boiler-monitor-bmondevice-h"></a>📄 `src/devices/boiler-monitor/BMonDevice.h`

**File Info:**
- **Size**: 1012 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/devices/boiler-monitor/BMonDevice.h`
- **Relative Path**: `src/devices/boiler-monitor`
- **Created**: 2026-03-20 07:05:55 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-03-20 07:12:23 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `c0a107774e71e3a30230d807c1ea9c37`
- **SHA256**: `6c06e724a1d3768667f44512f361a1ff6430cbefbcf51caffedce6683441f1c3`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef DEVICE_BOILER_MONITOR

#include "BMonTypes.h"
#include "../../core/CoreTypes.h"

class BMonDevice {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

private:
    static void readTemperatures();   // ADS1115 → NTC → °C
    static void readPressure();       // GPIO36/39 ADC → бар
    static void calcFlow();           // импульсы → л/мин, мощность
    static void checkAlarms();
    static void publishMqtt();

    static uint32_t _lastRead;        // опрос датчиков
    static uint32_t _lastFlow;        // расчёт расхода
    static uint32_t _lastHistory;     // запись в историю
    static uint32_t _lastPub;         // MQTT публикация

    // Снимок счётчиков для расчёта л/мин
    static uint32_t _pulse1_prev;
    static uint32_t _pulse2_prev;
};

#endif // DEVICE_BOILER_MONITOR
```

---

### <a id="📄-src-devices-boiler-monitor-bmontypes-h"></a>📄 `src/devices/boiler-monitor/BMonTypes.h`

**File Info:**
- **Size**: 5.05 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/devices/boiler-monitor/BMonTypes.h`
- **Relative Path**: `src/devices/boiler-monitor`
- **Created**: 2026-03-20 07:05:48 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-03-20 12:31:12 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `ac4d23ea41c87942007b294eb4ac7678`
- **SHA256**: `e27baac2360549146c25ead54b119b02a591e7f578f86c969b49f29a7a790a1c`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef DEVICE_BOILER_MONITOR

#include <Arduino.h>

// ============================================================
//  ПИНЫ ПЛАТЫ WT32-ETH01 + шильд boiler-monitor
// ============================================================
#define BMON_PIN_SDA        33
#define BMON_PIN_SCL        32
#define BMON_PIN_FLOW1       2   // расходомер 1 (оптопара)
#define BMON_PIN_FLOW2       4   // расходомер 2 (оптопара)
#define BMON_PIN_PRESS1     36   // давление P1 4-20мА (input only)
#define BMON_PIN_PRESS2     39   // давление P2 4-20мА (input only)

// ETH WT32-ETH01
#define BMON_ETH_ADDR        1
#define BMON_ETH_POWER_PIN  16
#define BMON_ETH_MDC_PIN    23
#define BMON_ETH_MDIO_PIN   18

// ============================================================
//  ADS1115
// ============================================================
#define BMON_ADS_ADDR       0x48  // ADDR → GND
#define BMON_ADS_GAIN       GAIN_ONE  // ±4.096V — достаточно для 3.3V

// ============================================================
//  NTC — B-параметрическая модель
//  R0=10k при T0=25°C, Rref=10k (резистор делителя)
// ============================================================
#define BMON_NTC_B          3950.0f
#define BMON_NTC_R0         10000.0f
#define BMON_NTC_T0         298.15f   // 25°C в Кельвинах
#define BMON_NTC_RREF       10000.0f
#define BMON_ADS_COUNTS     32767.0f  // макс. значение ADS1115 при GAIN_ONE

// ============================================================
//  Давление 4-20мА / 150 Ом
//  U_min = 0.004 * 150 = 0.6V
//  U_max = 0.020 * 150 = 3.0V
//  Диапазон датчика: 0–10 бар
// ============================================================
#define BMON_PRESS_U_MIN    0.6f
#define BMON_PRESS_U_MAX    3.0f
#define BMON_PRESS_BAR_MAX  10.0f
#define BMON_ADC_VREF       3.3f
#define BMON_ADC_BITS       4095.0f   // ESP32 ADC 12-bit

// ============================================================
//  КОНФИГ — уставки, сохраняются в /device.json
// ============================================================
struct BMonConfig {
    // Расходомеры: литров на импульс
    float flow1_lpi     = 1.0f;   // л/имп расходомер 1
    float flow2_lpi     = 1.0f;   // л/имп расходомер 2

    // Пороги аварий
    float t_max         = 85.0f;  // аварийное превышение температуры подачи
    float t_alarm       = 80.0f;  // порог оповещения
    float t_min         = 5.0f;   // защита от замерзания (улица)

    // Интервал публикации MQTT (мс)
    uint32_t pub_interval = 10000;

    // Коэффициент B для NTC (можно уточнить под конкретный датчик)
    float ntc_b         = BMON_NTC_B;
};

// ============================================================
//  СОСТОЯНИЕ — runtime, не сохраняется
// ============================================================

#define BMON_HISTORY_SIZE   60   // 60 × 10 сек = 10 минут

struct BMonState {
    // Температуры (°C), NaN = датчик не подключён
    float t_supply      = NAN;   // NTC1 — подача
    float t_return      = NAN;   // NTC2 — обратка
    float t_room        = NAN;   // NTC3 — помещение
    float t_outdoor     = NAN;   // NTC4 — улица
    float t_delta       = 0.0f;  // подача - обратка

    // Давление (бар)
    float p_supply      = 0.0f;  // P1
    float p_return      = 0.0f;  // P2
    float p_delta       = 0.0f;  // P1 - P2

    // Расход и мощность
    float flow1_lpm     = 0.0f;  // л/мин расходомер 1
    float flow2_lpm     = 0.0f;  // л/мин расходомер 2
    float power_kw      = 0.0f;  // тепловая мощность по расходомеру 1
    float energy_kwh    = 0.0f;  // накопленная энергия за сессию

    // Аварии
    bool alarm_high     = false;
    bool alarm_low      = false;  // защита от замерзания
    bool alarm_sensor   = false;  // ошибка чтения ADS1115

    // Счётчики импульсов (volatile — из ISR)
    volatile uint32_t pulse1_count = 0;
    volatile uint32_t pulse2_count = 0;

    // История мощности (кольцевой буфер)
    float    hist_power[BMON_HISTORY_SIZE] = {};
    uint8_t  hist_head  = 0;
    uint8_t  hist_count = 0;

    void pushHistory(float power) {
        hist_power[hist_head] = power;
        hist_head = (hist_head + 1) % BMON_HISTORY_SIZE;
        if (hist_count < BMON_HISTORY_SIZE) hist_count++;
    }
};

extern BMonConfig bmonCfg;
extern BMonState  bmonState;

#endif // DEVICE_BOILER_MONITOR
```

---

### <a id="📄-src-devices-ekonom-ekonomdevice-cpp"></a>📄 `src/devices/ekonom/EkoNomDevice.cpp`

**File Info:**
- **Size**: 6.54 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/devices/ekonom/EkoNomDevice.cpp`
- **Relative Path**: `src/devices/ekonom`
- **Created**: 2026-05-22 16:16:26 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-06-04 11:04:24 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `570970471b8ae568e0b31195d7ba40b2`
- **SHA256**: `6e608c3ed7bd6d1f978194acc9d393955e62e39534bfe2f215b561653a40de75`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef DEVICE_EKONOM

#include "EkoNomDevice.h"
#include "../../modules/rs485/Rs485Handler.h"

// ── Глобальные данные ────────────────────────────────────────────────────────
EkoNomData ekoNomData;

// ── Статика ──────────────────────────────────────────────────────────────────
EkoNomDevice::State    EkoNomDevice::_state        = IDLE;
uint32_t               EkoNomDevice::_lastPollMs   = 0;
uint32_t               EkoNomDevice::_sentMs       = 0;
uint8_t                EkoNomDevice::_pollInterval = 30;

// ── Запросы (CRC предрассчитан и проверен на реальном приборе) ───────────────
// R1: 01 03 00 00 00 0C 45 CF  — рег 0x00-0x0B: энергия, темп., дельта, объём
// R2: 01 03 00 0C 00 02 04 08  — рег 0x0C-0x0D: расход
// R3: 01 03 00 0E 00 02 A5 C8  — рег 0x0E-0x0F: мощность
static const uint8_t REQ_R1[] = {0x01,0x03,0x00,0x00,0x00,0x0C,0x45,0xCF};
static const uint8_t REQ_R2[] = {0x01,0x03,0x00,0x0C,0x00,0x02,0x04,0x08};
static const uint8_t REQ_R3[] = {0x01,0x03,0x00,0x0E,0x00,0x02,0xA5,0xC8};

static const uint16_t TIMEOUT_MS = 1000;

// ── Вспомогательные ──────────────────────────────────────────────────────────
uint32_t EkoNomDevice::_reg32(const uint8_t* d, int offset) {
    return ((uint32_t)d[offset]   << 24)
         | ((uint32_t)d[offset+1] << 16)
         | ((uint32_t)d[offset+2] <<  8)
         |  (uint32_t)d[offset+3];
}

// ── Парсинг ──────────────────────────────────────────────────────────────────
// R1 ответ: 01 03 18 [24 байта] CRC CRC
// Раскладка по регистрам (каждый параметр = 2 регистра = 4 байта):
//   offset 3:  0x00-0x01 — энергия       (÷100 кВт·ч)
//   offset 7:  0x02-0x03 — зарезервировано
//   offset 11: 0x04-0x05 — T подачи      (÷100 °C)
//   offset 15: 0x06-0x07 — T обратки     (÷100 °C)
//   offset 19: 0x08-0x09 — ΔT            (÷100 °C)
//   offset 23: 0x0A-0x0B — объём         (÷100 м³)
bool EkoNomDevice::_parseR1(const uint8_t* d, size_t len) {
    if (len < 27) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x18) return false;
    ekoNomData.energy_kwh = _reg32(d,  3) / 100.0f;
    ekoNomData.t_supply   = _reg32(d, 11) / 100.0f;
    ekoNomData.t_return   = _reg32(d, 15) / 100.0f;
    ekoNomData.t_delta    = _reg32(d, 19) / 100.0f;
    ekoNomData.volume_m3  = _reg32(d, 23) / 100.0f;
    return true;
}

// R2 ответ: 01 03 04 [4 байта] CRC CRC — расход (÷10000 м³/ч)
bool EkoNomDevice::_parseR2(const uint8_t* d, size_t len) {
    if (len < 9) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x04) return false;
    ekoNomData.flow_m3h = _reg32(d, 3) / 10000.0f;
    return true;
}

// R3 ответ: 01 03 04 [4 байта] CRC CRC — мощность (÷100 кВт)
bool EkoNomDevice::_parseR3(const uint8_t* d, size_t len) {
    if (len < 9) return false;
    if (d[0] != 0x01 || d[1] != 0x03 || d[2] != 0x04) return false;
    ekoNomData.power_kw = _reg32(d, 3) / 100.0f;
    return true;
}

// ── Callback от Rs485Handler ─────────────────────────────────────────────────
void EkoNomDevice::_onFrame(const uint8_t* data, size_t len) {
    switch (_state) {
        case WAIT_R1:
            if (_parseR1(data, len)) {
                _state = WAIT_R2;
                Rs485Handler::sendBytes(REQ_R2, sizeof(REQ_R2));
                _sentMs = millis();
            } else {
                Serial.printf("[EKONOM] R1 parse error, len=%d\n", len);
                _state = IDLE;
            }
            break;

        case WAIT_R2:
            if (_parseR2(data, len)) {
                _state = WAIT_R3;
                Rs485Handler::sendBytes(REQ_R3, sizeof(REQ_R3));
                _sentMs = millis();
            } else {
                Serial.printf("[EKONOM] R2 parse error, len=%d\n", len);
                _state = IDLE;
            }
            break;

        case WAIT_R3:
            if (_parseR3(data, len)) {
                ekoNomData.valid      = true;
                ekoNomData.updated_ms = millis();
                Serial.printf("[EKONOM] Ts=%.2f Tr=%.2f dT=%.2f P=%.2f kW F=%.4f m3/h E=%.2f kWh\n",
                    ekoNomData.t_supply, ekoNomData.t_return, ekoNomData.t_delta,
                    ekoNomData.power_kw, ekoNomData.flow_m3h, ekoNomData.energy_kwh);
            } else {
                Serial.printf("[EKONOM] R3 parse error, len=%d\n", len);
            }
            _state = IDLE;
            break;

        default:
            break;
    }
}

// ── INIT ─────────────────────────────────────────────────────────────────────
void EkoNomDevice::init(uint8_t pollIntervalSec) {
    _pollInterval = pollIntervalSec;
    Rs485Handler::onFrame = _onFrame;
    Serial.printf("[EKONOM] Init OK, poll every %ds\n", _pollInterval);
}

// ── LOOP ─────────────────────────────────────────────────────────────────────
void EkoNomDevice::loop() {
    uint32_t now = millis();

    // Таймаут ожидания ответа
    if (_state != IDLE && (now - _sentMs) > TIMEOUT_MS) {
        Serial.printf("[EKONOM] Timeout in state %d\n", (int)_state);
        _state = IDLE;
    }

    // Запуск нового цикла опроса
    if (_state == IDLE && (now - _lastPollMs) >= (uint32_t)_pollInterval * 1000) {
        _lastPollMs = now;
        _state = WAIT_R1;
        Rs485Handler::sendBytes(REQ_R1, sizeof(REQ_R1));
        _sentMs = now;
    }
}

#endif // DEVICE_EKONOM
```

---

### <a id="📄-src-devices-ekonom-ekonomdevice-h"></a>📄 `src/devices/ekonom/EkoNomDevice.h`

**File Info:**
- **Size**: 1.79 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/devices/ekonom/EkoNomDevice.h`
- **Relative Path**: `src/devices/ekonom`
- **Created**: 2026-05-22 16:15:32 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-22 16:26:12 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `925a3475f13d56d0d01fa760d6af27ee`
- **SHA256**: `a1a0aecfc212a6273ea8c283c4b64d76112269a59ee39250b688855171ce52f8`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef DEVICE_EKONOM

#include <Arduino.h>

// ── Данные теплосчётчика ─────────────────────────────────────────────────────
struct EkoNomData {
    float    energy_kwh  = 0;    // накопленная энергия, кВт·ч
    float    t_supply    = 0;    // температура подачи, °C
    float    t_return    = 0;    // температура обратки, °C
    float    t_delta     = 0;    // разность температур, °C
    float    volume_m3   = 0;    // объём, м³
    float    flow_m3h    = 0;    // расход, м³/ч
    float    power_kw    = 0;    // мощность, кВт
    uint32_t updated_ms  = 0;    // millis() последнего успешного опроса
    bool     valid       = false;
};

extern EkoNomData ekoNomData;

// ── Драйвер ──────────────────────────────────────────────────────────────────
class EkoNomDevice {
public:
    static void init(uint8_t pollIntervalSec = 30);
    static void loop();

private:
    enum State { IDLE, WAIT_R1, WAIT_R2, WAIT_R3 };
    static State    _state;
    static uint32_t _lastPollMs;
    static uint32_t _sentMs;
    static uint8_t  _pollInterval;

    static void    _onFrame(const uint8_t* data, size_t len);
    static bool    _parseR1(const uint8_t* data, size_t len);
    static bool    _parseR2(const uint8_t* data, size_t len);
    static bool    _parseR3(const uint8_t* data, size_t len);
    static uint32_t _reg32(const uint8_t* d, int offset);
};

#endif // DEVICE_EKONOM
```

---

### <a id="📄-src-devices-kc868-a16-a16device-cpp"></a>📄 `src/devices/kc868-a16/A16Device.cpp`

**File Info:**
- **Size**: 21.51 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/devices/kc868-a16/A16Device.cpp`
- **Relative Path**: `src/devices/kc868-a16`
- **Created**: 2026-02-18 03:18:58 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-28 18:54:14 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `fd819ed46366db3d6e78b044d213f773`
- **SHA256**: `86e05fc8ded668f87b79fcfbf2d75a6bb4183cdf31eb11f88c2f9110f6eca98c`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef DEVICE_KC868_A16
#include "A16Device.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_MQTT
#include "../../modules/mqtt/MqttHandler.h"
#endif

A16Config a16Cfg;
A16State  a16State;

uint32_t A16Device::_lastPoll   = 0;
uint32_t A16Device::_lastOwRead = 0;
bool     A16Device::_owScanned  = false;

// OneWire на трёх пинах
static OneWire         ow1(A16_OW_PIN1);
static OneWire         ow2(A16_OW_PIN2);
static OneWire         ow3(A16_OW_PIN3);
static DallasTemperature dt1(&ow1);
static DallasTemperature dt2(&ow2);
static DallasTemperature dt3(&ow3);

// ============================================================
//  HTML ВКЛАДКИ (PROGMEM)
// ============================================================
static const char A16_PERIPH_HTML[] PROGMEM = R"html(
<div class="a16-page">

  <!-- ЦИФРОВЫЕ ВХОДЫ -->
  <div class="a16-section">
    <div class="a16-section-title">ЦИФРОВЫЕ ВХОДЫ</div>
    <div class="a16-grid16" id="din_grid">
      <!-- генерируется JS -->
    </div>
  </div>

  <!-- ЦИФРОВЫЕ ВЫХОДЫ -->
  <div class="a16-section">
    <div class="a16-section-title">РЕЛЕ ВЫХОДЫ</div>
    <div class="a16-grid16" id="dout_grid">
      <!-- генерируется JS -->
    </div>
  </div>

  <!-- АНАЛОГОВЫЕ ВХОДЫ -->
  <div class="a16-section">
    <div class="a16-section-title">АНАЛОГОВЫЕ ВХОДЫ</div>
    <div class="a16-grid4" id="adc_grid">
      <!-- генерируется JS -->
    </div>
  </div>

  <!-- 1-WIRE ДАТЧИКИ -->
  <div class="a16-section">
    <div class="a16-section-title">1-WIRE ТЕМПЕРАТУРА</div>
    <div id="ow_grid">
      <!-- генерируется JS -->
    </div>
    <button class="a16-btn-sm" onclick="owScan()">🔍 Сканировать шину</button>
  </div>

</div>

<style>
.a16-page { display:flex; flex-direction:column; gap:12px; }
.a16-section {
  background: var(--card);
  border-radius:10px;
  padding:14px;
  border:1px solid var(--border);
}
.a16-section-title {
  font-size:10px; font-weight:700;
  letter-spacing:.12em; text-transform:uppercase;
  color:var(--muted); margin-bottom:12px;
}
.a16-grid16 {
  display:grid;
  grid-template-columns: repeat(8,1fr);
  gap:6px;
}
.a16-grid4 {
  display:grid;
  grid-template-columns: repeat(2,1fr);
  gap:8px;
}
@media(max-width:400px) {
  .a16-grid16 { grid-template-columns: repeat(4,1fr); }
}

/* Ячейка входа */
.din-cell {
  background:#111318;
  border:1px solid var(--border);
  border-radius:6px;
  padding:6px 4px;
  text-align:center;
  cursor:default;
  transition: border-color .2s;
}
.din-cell.on {
  border-color:var(--ok);
  background:rgba(62,201,122,.1);
}
.din-num { font-size:9px; color:var(--muted); margin-bottom:3px; }
.din-dot { width:10px; height:10px; border-radius:50%;
  background:var(--border); margin:0 auto 3px; transition:background .2s; }
.din-cell.on .din-dot { background:var(--ok); box-shadow:0 0 6px var(--ok); }
.din-cnt { font-size:9px; color:var(--muted); }

/* Ячейка выхода */
.dout-cell {
  background:#111318;
  border:1px solid var(--border);
  border-radius:6px;
  padding:6px 4px;
  text-align:center;
  cursor:pointer;
  transition: all .15s;
  user-select:none;
}
.dout-cell:active { opacity:.7; }
.dout-cell.on {
  border-color:var(--accent);
  background:rgba(240,165,0,.1);
}
.dout-num { font-size:9px; color:var(--muted); margin-bottom:3px; }
.dout-dot { width:10px; height:10px; border-radius:50%;
  background:var(--border); margin:0 auto 3px; transition:background .2s; }
.dout-cell.on .dout-dot { background:var(--accent); box-shadow:0 0 6px var(--accent); }
.dout-lbl { font-size:9px; color:var(--muted); }

/* Аналог */
.adc-cell {
  background:#111318;
  border:1px solid var(--border);
  border-radius:8px;
  padding:12px;
}
.adc-lbl { font-size:10px; color:var(--muted); margin-bottom:4px; }
.adc-val { font-size:22px; font-weight:800; color:var(--accent); }
.adc-unit { font-size:11px; color:var(--muted); margin-left:2px; }
.adc-bar { height:4px; background:var(--border); border-radius:2px; margin-top:8px; }
.adc-bar-fill { height:100%; background:var(--accent);
  border-radius:2px; transition:width .5s; }

/* 1-Wire */
.ow-row {
  display:flex; align-items:center; gap:10px;
  background:#111318; border:1px solid var(--border);
  border-radius:6px; padding:10px 12px; margin-bottom:6px;
}
.ow-temp { font-size:20px; font-weight:800; color:var(--cool); margin-left:auto; }
.ow-addr { font-size:10px; color:var(--muted); font-family:monospace; }
.ow-lbl  { font-size:12px; color:var(--text); }

.a16-btn-sm {
  width:auto; padding:6px 14px;
  font-size:12px; margin-top:8px;
  background:var(--border); color:var(--text);
}
</style>

<script>
let a16Labels = { din:[], dout:[], adc:[], ow:[] };

// Генерация сеток при загрузке
async function initA16() {
  const r = await fetch('/api/a16/config');
  const cfg = await r.json();
  a16Labels = { din: cfg.din_label, dout: cfg.dout_label,
                adc: cfg.adc_label, ow:  cfg.ow_label };

  // DIN сетка
  const dinGrid = document.getElementById('din_grid');
  dinGrid.innerHTML = '';
  for (let i=0; i<16; i++) {
    dinGrid.innerHTML += `
      <div class="din-cell" id="din_${i}">
        <div class="din-num">${i+1}</div>
        <div class="din-dot"></div>
        <div class="din-cnt" id="cnt_${i}">0</div>
      </div>`;
  }

  // DOUT сетка
  const doutGrid = document.getElementById('dout_grid');
  doutGrid.innerHTML = '';
  for (let i=0; i<16; i++) {
    doutGrid.innerHTML += `
      <div class="dout-cell" id="dout_${i}" onclick="toggleOut(${i})">
        <div class="dout-num">${i+1}</div>
        <div class="dout-dot"></div>
        <div class="dout-lbl">${cfg.dout_label[i]||''}</div>
      </div>`;
  }

  // ADC сетка
  const adcGrid = document.getElementById('adc_grid');
  adcGrid.innerHTML = '';
  for (let i=0; i<4; i++) {
    adcGrid.innerHTML += `
      <div class="adc-cell">
        <div class="adc-lbl">${cfg.adc_label[i]||'АЦП '+(i+1)}</div>
        <div><span class="adc-val" id="adc_v${i}">--</span>
             <span class="adc-unit" id="adc_u${i}"></span></div>
        <div class="adc-bar"><div class="adc-bar-fill" id="adc_b${i}" style="width:0%"></div></div>
      </div>`;
  }

  updateA16();
}

async function updateA16() {
  try {
    const r = await fetch('/api/a16/data');
    const d = await r.json();

    // DIN
    for (let i=0; i<16; i++) {
      const cell = document.getElementById('din_'+i);
      if (!cell) continue;
      cell.classList.toggle('on', d.din[i]);
      document.getElementById('cnt_'+i).textContent = d.din_counter[i];
    }

    // DOUT
    for (let i=0; i<16; i++) {
      const cell = document.getElementById('dout_'+i);
      if (cell) cell.classList.toggle('on', d.dout[i]);
    }

    // ADC
    for (let i=0; i<4; i++) {
      const el = document.getElementById('adc_v'+i);
      const un = document.getElementById('adc_u'+i);
      const bar = document.getElementById('adc_b'+i);
      const pct = Math.min(100, Math.max(0, d.adc_pct[i]));
      if (bar) bar.style.width = pct+'%';
      if (i < 2) {
        // 4-20мА каналы — показываем мА
        const ma = d.adc_ma ? d.adc_ma[i] : null;
        if (el) el.textContent = ma != null ? ma.toFixed(2) : '--';
        if (un) un.textContent = 'мА';
      } else {
        // 0-5В каналы — показываем вольты
        if (el) el.textContent = d.adc_value[i].toFixed(2);
        if (un) un.textContent = 'В';
      }
    }

    // 1-Wire
    const owGrid = document.getElementById('ow_grid');
    if (d.ow && d.ow.length) {
      owGrid.innerHTML = d.ow.map((s,i) => `
        <div class="ow-row">
          <div>
            <div class="ow-lbl">${a16Labels.ow[i]||'Датчик '+(i+1)}</div>
            <div class="ow-addr">${s.addr}</div>
          </div>
          <div class="ow-temp">${s.ok ? s.temp.toFixed(1)+'°C' : '—'}</div>
        </div>`).join('');
    } else {
      owGrid.innerHTML = '<div style="color:var(--muted);font-size:12px">Датчики не найдены. Нажмите Сканировать.</div>';
    }
  } catch(e) {}
}

async function toggleOut(ch) {
  await fetch('/api/a16/toggle', {
    method:'POST',
    headers:{'Content-Type':'application/json'},
    body: JSON.stringify({ch})
  });
}

async function owScan() {
  await fetch('/api/a16/ow-scan', {method:'POST'});
  setTimeout(updateA16, 3000);
}

initA16();
setInterval(updateA16, 1000);
</script>
)html";

// ============================================================
//  INIT
// ============================================================
void A16Device::init() {
    loadConfig();

    Wire.begin(A16_I2C_SDA, A16_I2C_SCL);

    // Все выходы выключены при старте
    writePCF(A16_PCF_OUT_0, 0xFF);  // PCF8574: лог.1 = реле выключено
    writePCF(A16_PCF_OUT_1, 0xFF);

    // 1-Wire
    dt1.begin(); dt2.begin(); dt3.begin();
    dt1.setWaitForConversion(false);
    dt2.setWaitForConversion(false);
    dt3.setWaitForConversion(false);

    // Регистрация вкладки
    WebHandler::registerTab({"periph", "Периферия", "🔌", A16_PERIPH_HTML});

    // ── API ──────────────────────────────────────────────────
    extern AsyncWebServer server;

    // Данные
    server.on("/api/a16/data", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;

        // DIN
        JsonArray din = doc["din"].to<JsonArray>();
        JsonArray cnt = doc["din_counter"].to<JsonArray>();
        for (int i = 0; i < 16; i++) {
            din.add(a16State.din[i]);
            cnt.add(a16State.din_counter[i]);
        }

        // DOUT
        JsonArray dout = doc["dout"].to<JsonArray>();
        for (int i = 0; i < 16; i++) dout.add(a16State.dout[i]);

        // ADC
        JsonArray adcV = doc["adc_value"].to<JsonArray>();
        JsonArray adcP = doc["adc_pct"].to<JsonArray>();
        JsonArray adcR = doc["adc_raw"].to<JsonArray>();
        JsonArray adcM = doc["adc_ma"].to<JsonArray>();  
        for (int i = 0; i < 4; i++) {
            adcV.add(a16State.adc_value[i]);
            adcR.add(a16State.adc_raw[i]);
            adcM.add(a16State.adc_ma[i]); 
            float range = a16Cfg.adc_max[i] - a16Cfg.adc_min[i];
            float pct = (range > 0)
                ? (a16State.adc_value[i] - a16Cfg.adc_min[i]) / range * 100.0f
                : 0;
            adcP.add(constrain(pct, 0, 100));
        }

        // 1-Wire
        JsonArray ow = doc["ow"].to<JsonArray>();
        for (int i = 0; i < a16State.ow_count; i++) {
            JsonObject s = ow.add<JsonObject>();
            char addr[17];
            snprintf(addr, sizeof(addr), "%016llX", a16State.ow_addr[i]);
            s["addr"] = addr;
            s["temp"] = a16State.ow_temp[i];
            s["ok"]   = a16State.ow_ok[i];
        }

        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Конфиг (метки)
    server.on("/api/a16/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray dl = doc["din_label"].to<JsonArray>();
        JsonArray ql = doc["dout_label"].to<JsonArray>();
        JsonArray al = doc["adc_label"].to<JsonArray>();
        JsonArray ol = doc["ow_label"].to<JsonArray>();
        for (int i = 0; i < 16; i++) { dl.add(a16Cfg.din_label[i]); ql.add(a16Cfg.dout_label[i]); }
        for (int i = 0; i < 4;  i++) al.add(a16Cfg.adc_label[i]);
        for (int i = 0; i < A16_MAX_OW_SENSORS; i++) ol.add(a16Cfg.ow_label[i]);
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Переключение выхода
    auto* toggleH = new AsyncCallbackJsonWebHandler("/api/a16/toggle",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            int ch = json["ch"] | -1;
            if (ch >= 0 && ch < 16) A16Device::toggleOutput(ch);
            req->send(200, "application/json", "{\"ok\":true}");
        }
    );
    server.addHandler(toggleH);

    // Сброс счётчика
    auto* resetH = new AsyncCallbackJsonWebHandler("/api/a16/reset-counter",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            int ch = json["ch"] | -1;
            if (ch >= 0 && ch < 16) a16State.din_counter[ch] = 0;
            req->send(200, "application/json", "{\"ok\":true}");
        }
    );
    server.addHandler(resetH);

    // Сканирование 1-Wire
    server.on("/api/a16/ow-scan", HTTP_POST, [](AsyncWebServerRequest* req) {
        A16Device::scanOneWire();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    Serial.println("[A16] Device init OK");
}

// ============================================================
//  LOOP
// ============================================================
void A16Device::loop() {
    if (millis() - _lastPoll > a16Cfg.poll_interval) {
        _lastPoll = millis();
        readDigitalInputs();
        readAnalog();
    }

    // 1-Wire: первое сканирование при старте, потом каждые 5 сек
    if (!_owScanned) { scanOneWire(); _owScanned = true; }
    if (millis() - _lastOwRead > 5000) {
        _lastOwRead = millis();
        readOneWire();
    }

    // Безопасная перезагрузка
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (millis() - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}

// ============================================================
//  ЦИФРОВЫЕ ВХОДЫ
// ============================================================
void A16Device::readDigitalInputs() {
    // Входы читаются через PCF8574, не напрямую с GPIO
    uint8_t byte0 = readPCF(A16_PCF_DI_0);  // IN1-IN8
    uint8_t byte1 = readPCF(A16_PCF_DI_1);  // IN9-IN16

    for (int i = 0; i < 8; i++) {
        // PCF8574: лог.0 = вход активен (оптопара)
        bool state = !((byte0 >> i) & 1);
        if (state && !a16State.din_prev[i]) a16State.din_counter[i]++;
        a16State.din_prev[i] = state;
        a16State.din[i] = state;
    }
    for (int i = 0; i < 8; i++) {
        bool state = !((byte1 >> i) & 1);
        if (state && !a16State.din_prev[i+8]) a16State.din_counter[i+8]++;
        a16State.din_prev[i+8] = state;
        a16State.din[i+8] = state;
    }
}

// ============================================================
//  АНАЛОГОВЫЕ ВХОДЫ
// ============================================================
void A16Device::readAnalog() {
    const uint8_t pins[4] = {A16_ADC_CH1, A16_ADC_CH3, A16_ADC_CH4, A16_ADC_CH2};
    for (int i = 0; i < 4; i++) {
        // Среднее из 4 измерений для стабильности
        int32_t sum = 0;
        for (int j = 0; j < 4; j++) sum += analogRead(pins[i]);
        a16State.adc_raw[i] = sum / 4;

        float raw = a16State.adc_raw[i];

        switch (a16Cfg.adc_mode[i]) {
            case AnalogMode::MODE_4_20MA: {
                // Скользящее среднее по буферу
                if (i < 2) {
                    a16State.adc_ma_buf[i][a16State.adc_ma_idx[i]] = raw;
                    a16State.adc_ma_idx[i] = (a16State.adc_ma_idx[i] + 1) % 10;
                    if (a16State.adc_ma_idx[i] == 0) a16State.adc_ma_full[i] = true;
                    uint8_t cnt = a16State.adc_ma_full[i] ? 10 : a16State.adc_ma_idx[i];
                    int32_t sum = 0;
                    for (uint8_t k = 0; k < cnt; k++) sum += a16State.adc_ma_buf[i][k];
                    raw = (cnt > 0) ? (sum / cnt) : raw;
                }

                float ma = 4.0f + (raw - 557.0f) / (4402.0f - 557.0f) * 16.0f;
                ma = constrain(ma, 4.0f, 20.0f);
                a16State.adc_ma[i] = ma;

                float pct = (ma - 4.0f) / 16.0f;
                a16State.adc_value[i] = a16Cfg.adc_min[i] +
                                        pct * (a16Cfg.adc_max[i] - a16Cfg.adc_min[i]);
                break;
            }
            case AnalogMode::MODE_0_5V: {
                // 0-5В через делитель: Vout = Vin * R2/(R1+R2)
                // По схеме делитель 510/100 → коэф ~0.164
                // Но ESP32 ADC до 3.3В, поэтому 5В → 0.82В → ~1020
                float volt = raw / 4095.0f * 3.3f / 0.164f;
                a16State.adc_value[i] = constrain(volt, 0.0f, 5.0f);
                break;
            }
            default:
                a16State.adc_value[i] = raw;
        }
    }
}

// ============================================================
//  1-WIRE
// ============================================================
void A16Device::scanOneWire() {
    a16State.ow_count = 0;
    uint8_t addr[8];

    auto scan = [&](OneWire& ow) {
        ow.reset_search();
        while (ow.search(addr) && a16State.ow_count < A16_MAX_OW_SENSORS) {
            if (OneWire::crc8(addr, 7) != addr[7]) continue;
            uint64_t id = 0;
            for (int i = 0; i < 8; i++) id = (id << 8) | addr[i];
            a16State.ow_addr[a16State.ow_count++] = id;
        }
    };

    scan(ow1); scan(ow2); scan(ow3);
    Serial.printf("[A16] 1-Wire: found %d sensors\n", a16State.ow_count);
}

void A16Device::readOneWire() {
    if (a16State.ow_count == 0) return;

    dt1.requestTemperatures();
    dt2.requestTemperatures();
    dt3.requestTemperatures();
    delay(100);  // DS18B20 conversion time (async mode)

    for (int i = 0; i < a16State.ow_count; i++) {
        uint8_t addr[8];
        for (int b = 7; b >= 0; b--) {
            addr[7-b] = (a16State.ow_addr[i] >> (b*8)) & 0xFF;
        }
        // Пробуем все три шины
        float t = DEVICE_DISCONNECTED_C;
        DeviceAddress da; memcpy(da, addr, 8);
        if (t == DEVICE_DISCONNECTED_C) t = dt1.getTempC(da);
        if (t == DEVICE_DISCONNECTED_C) t = dt2.getTempC(da);
        if (t == DEVICE_DISCONNECTED_C) t = dt3.getTempC(da);

        a16State.ow_ok[i]   = (t != DEVICE_DISCONNECTED_C);
        a16State.ow_temp[i] = a16State.ow_ok[i] ? t : 0.0f;
    }
}

// ============================================================
//  PCF8574 I/O
// ============================================================
void A16Device::writePCF(uint8_t addr, uint8_t data) {
    Wire.beginTransmission(addr);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t A16Device::readPCF(uint8_t addr) {
    Wire.requestFrom(addr, (uint8_t)1);
    return Wire.available() ? Wire.read() : 0xFF;
}

void A16Device::syncOutputs() {
    // PCF8574: лог.0 = реле включено, лог.1 = выключено
    uint8_t byte0 = 0xFF, byte1 = 0xFF;
    for (int i = 0; i < 8;  i++) if (a16State.dout[i])   byte0 &= ~(1 << i);
    for (int i = 0; i < 8;  i++) if (a16State.dout[i+8]) byte1 &= ~(1 << i);
    writePCF(A16_PCF_OUT_0, byte0);
    writePCF(A16_PCF_OUT_1, byte1);
}

bool A16Device::setOutput(uint8_t ch, bool state) {
    if (ch >= 16) return false;
    a16State.dout[ch] = state;
    syncOutputs();
    return true;
}

bool A16Device::toggleOutput(uint8_t ch) {
    return setOutput(ch, !a16State.dout[ch]);
}

void A16Device::setAllOutputs(uint16_t mask) {
    for (int i = 0; i < 16; i++) a16State.dout[i] = (mask >> i) & 1;
    syncOutputs();
}

// ============================================================
//  CONFIG
// ============================================================
bool A16Device::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/device.json", doc)) {
        // Дефолтные метки
        for (int i = 0; i < 16; i++) {
            snprintf(a16Cfg.din_label[i],  20, "IN%d",  i+1);
            snprintf(a16Cfg.dout_label[i], 20, "OUT%d", i+1);
        }
        snprintf(a16Cfg.adc_label[0], 20, "АЦП 1 (4-20мА)");
        snprintf(a16Cfg.adc_label[1], 20, "АЦП 2 (4-20мА)");
        snprintf(a16Cfg.adc_label[2], 20, "АЦП 3 (0-5В)");
        snprintf(a16Cfg.adc_label[3], 20, "АЦП 4 (0-5В)");
        return false;
    }

    for (int i = 0; i < 16; i++) {
        strlcpy(a16Cfg.din_label[i],  doc["din_label"][i]  | "", 20);
        strlcpy(a16Cfg.dout_label[i], doc["dout_label"][i] | "", 20);
    }
    for (int i = 0; i < 4; i++) {
        strlcpy(a16Cfg.adc_label[i], doc["adc_label"][i] | "", 20);
        a16Cfg.adc_min[i] = doc["adc_min"][i] | 0.0f;
        a16Cfg.adc_max[i] = doc["adc_max"][i] | 10.0f;
    }
    for (int i = 0; i < A16_MAX_OW_SENSORS; i++)
        strlcpy(a16Cfg.ow_label[i], doc["ow_label"][i] | "", 20);
    a16Cfg.poll_interval = doc["poll_interval"] | 1000;
    return true;
}

bool A16Device::saveConfig() {
    JsonDocument doc;
    JsonArray dl = doc["din_label"].to<JsonArray>();
    JsonArray ql = doc["dout_label"].to<JsonArray>();
    JsonArray al = doc["adc_label"].to<JsonArray>();
    JsonArray mn = doc["adc_min"].to<JsonArray>();
    JsonArray mx = doc["adc_max"].to<JsonArray>();
    JsonArray ol = doc["ow_label"].to<JsonArray>();
    for (int i = 0; i < 16; i++) { dl.add(a16Cfg.din_label[i]); ql.add(a16Cfg.dout_label[i]); }
    for (int i = 0; i < 4;  i++) { al.add(a16Cfg.adc_label[i]); mn.add(a16Cfg.adc_min[i]); mx.add(a16Cfg.adc_max[i]); }
    for (int i = 0; i < A16_MAX_OW_SENSORS; i++) ol.add(a16Cfg.ow_label[i]);
    doc["poll_interval"] = a16Cfg.poll_interval;
    return ConfigManager::saveJson("/device.json", doc);
}

#endif // DEVICE_KC868_A16

```

---

### <a id="📄-src-devices-kc868-a16-a16device-h"></a>📄 `src/devices/kc868-a16/A16Device.h`

**File Info:**
- **Size**: 1019 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/devices/kc868-a16/A16Device.h`
- **Relative Path**: `src/devices/kc868-a16`
- **Created**: 2026-02-18 01:32:44 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-18 01:32:44 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `654be0c8c9f1b90da0b536af9618f1b1`
- **SHA256**: `7bc5878151b9ffd50f5188a52ce65f42f624bc6953c02df4650014fb68c17f12`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef DEVICE_KC868_A16

#include "A16Types.h"
#include "../../core/CoreTypes.h"

class A16Device {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Управление выходами — можно вызывать из других модулей
    static bool setOutput(uint8_t ch, bool state);   // ch: 0-15
    static bool toggleOutput(uint8_t ch);
    static void setAllOutputs(uint16_t mask);         // битовая маска

private:
    static void readDigitalInputs();
    static void readAnalog();
    static void readOneWire();
    static void writePCF(uint8_t addr, uint8_t data);
    static uint8_t readPCF(uint8_t addr);
    static void syncOutputs();   // применить a16State.dout → PCF8574
    static void scanOneWire();   // найти датчики на шине

    static uint32_t _lastPoll;
    static uint32_t _lastOwRead;
    static bool     _owScanned;
};

#endif // DEVICE_KC868_A16

```

---

### <a id="📄-src-devices-kc868-a16-a16types-h"></a>📄 `src/devices/kc868-a16/A16Types.h`

**File Info:**
- **Size**: 3.94 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/devices/kc868-a16/A16Types.h`
- **Relative Path**: `src/devices/kc868-a16`
- **Created**: 2026-02-18 03:29:22 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-28 18:54:14 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `a5ba4e5ebfae52702938bfb449413ffd`
- **SHA256**: `949a518a7bc981960a69323960859dcad80e140664b410fd6185e79ad0aceee2`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef DEVICE_KC868_A16

#include <Arduino.h>

// ============================================================
//  ПИНЫ KC868-A16
// ============================================================
// ============================================================
//  ПИНЫ KC868-A16 (проверено по реальному проекту)
// ============================================================

// Ethernet — переопределяем дефолты platformio.ini
#define A16_ETH_ADDR      0
#define A16_ETH_POWER_PIN -1                    // PHY питание не управляется
#define A16_ETH_MDC_PIN   23
#define A16_ETH_MDIO_PIN  18
#define A16_ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT  // важно для Kincony!

// I2C
#define A16_I2C_SDA     4
#define A16_I2C_SCL     5    // не 16!

// Аналоговые входы
#define A16_ADC_CH1     36   // 4-20мА вход 1
#define A16_ADC_CH2     39   // 4-20мА вход 2
#define A16_ADC_CH3     34   // 0-5В вход 3
#define A16_ADC_CH4     35   // 0-5В вход 4

// 1-Wire (DS18B20)
#define A16_OW_PIN1     33   // HT1
#define A16_OW_PIN2     32   // HT2
#define A16_OW_PIN3     14   // HT3

// RS485
#define A16_RS485_RX    16
#define A16_RS485_TX    13

// PCF8574 адреса
#define A16_PCF_DI_0    0x22   // IN1-IN8  (входы)
#define A16_PCF_DI_1    0x21   // IN9-IN16 (входы)
#define A16_PCF_OUT_0   0x24   // OT1-OT8  (выходы/реле)
#define A16_PCF_OUT_1   0x25   // OT9-OT16 (выходы/реле)

// ============================================================
//  ТИПЫ АНАЛОГОВЫХ ВХОДОВ
// ============================================================
enum class AnalogMode : uint8_t {
    MODE_4_20MA,   // 4-20мА → 0-100%
    MODE_0_5V,     // 0-5В → сырое значение
    MODE_RAW       // 0-4095 АЦП без конвертации
};


// ============================================================
//  КОНФИГ A16 — сохраняется в /device.json
// ============================================================
#define A16_MAX_OW_SENSORS  8

struct A16Config {
    // Метки входов (для отображения в UI)
    char din_label[16][20];
    char dout_label[16][20];
    char adc_label[4][20];
    char ow_label[A16_MAX_OW_SENSORS][20];

    // Режимы аналоговых входов
    AnalogMode adc_mode[4] = {
        AnalogMode::MODE_4_20MA,
        AnalogMode::MODE_4_20MA,
        AnalogMode::MODE_0_5V,
        AnalogMode::MODE_0_5V
    };

    // Масштаб аналоговых входов (min/max физической величины)
    float adc_min[4] = {0, 0, 0, 0};
    float adc_max[4] = {10, 10, 5, 5};   // бар, бар, В, В

    // Интервал опроса (мс)
    uint32_t poll_interval = 1000;
};

// ============================================================
//  СОСТОЯНИЕ A16 — runtime
// ============================================================
struct A16State {
    // Цифровые входы
    bool     din[16]          = {};
    uint32_t din_counter[16]  = {};   // счётчики импульсов
    bool     din_prev[16]     = {};   // для детекции фронта

    // Цифровые выходы
    bool dout[16] = {};

    // Аналоговые входы
    int   adc_raw[4]   = {};
    float adc_ma[4]    = {};   
    float adc_value[4] = {};   

    // Буфер скользящего среднего для 4-20мА (каналы 0 и 1)
    static const uint8_t ADC_MA_BUF = 10;
    int   adc_ma_buf[2][10] = {};
    uint8_t adc_ma_idx[2]   = {};
    bool    adc_ma_full[2]  = {};

    // 1-Wire датчики
    uint8_t  ow_count = 0;
    uint64_t ow_addr[A16_MAX_OW_SENSORS]  = {};   // 64-бит ROM адрес
    float    ow_temp[A16_MAX_OW_SENSORS]  = {};
    bool     ow_ok[A16_MAX_OW_SENSORS]    = {};
};

extern A16Config a16Cfg;
extern A16State  a16State;

#endif // DEVICE_KC868_A16

```

---

### <a id="📄-src-modules-modem-modemhandler-cpp"></a>📄 `src/modules/modem/ModemHandler.cpp`

**File Info:**
- **Size**: 51.4 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/modules/modem/ModemHandler.cpp`
- **Relative Path**: `src/modules/modem`
- **Created**: 2026-04-17 05:26:47 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-24 06:08:10 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `a960e634fbab9207ec12351d05b69016`
- **SHA256**: `902410265a877c8f630428e2472cc7b10c09986d59e9f83da2c560b724556fac`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef MODULE_MODEM
#include "ModemHandler.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#ifdef MODULE_MQTT
#include "../mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "../telegram/TgHandler.h"
#endif

// ── Статические данные ───────────────────────────────────────────────────────
ModemConfig modemCfg;
ModemStatus modemStatus;

// Глобальные переменные для доступа из других модулей без include
int  _modemCreg    = -1;
bool _modemEnabled = false;

void (*ModemHandler::onDTMFResult)(const char*, const char*) = nullptr;

static HardwareSerial _sim(2);

// ── Состояние звонка ─────────────────────────────────────────────────────────
enum CallState { CALL_IDLE, CALL_RINGING, CALL_IN_CALL };
static CallState _callState = CALL_IDLE;
static String   _lineBuffer = "";
static String   _modemLog   = "";
static String   _callerNum  = "";
static String   _dtmfSeq    = "";
static uint32_t _callStart  = 0;
static String   _callLogBuffer = "";

// SMS
static bool   _smsNextLine  = false;  // ждём тело SMS после заголовка +CMT:
static String _smsSender    = "";
static String _smsInbox[10];          // кольцевой буфер последних 10 SMS
static uint8_t _smsInboxIdx = 0;

// Неблокирующая задержка между ATA и AT+CMUT=1
static bool     _answerPending    = false;
static uint32_t _answerAt         = 0;

// Флаг: loop() поймал "Call Ready" во время фазы INIT_RST_HIGH
static bool     _callReadyReceived = false;

// Флаг ручного ресета из веб-интерфейса
static bool     _manualRestart = false;

// ── Стейт-машина инициализации ────────────────────────────────────────────────
//
//  INIT_RST_LOW  → держим RST=LOW 200 мс
//  INIT_RST_HIGH → подняли RST, ждём "Call Ready" до 15 сек
//  INIT_CMD      → шлём команды по одной, ждём 500 мс между ними
//  INIT_DONE     → инициализация завершена, loop() работает штатно
//
enum InitState { INIT_RST_LOW, INIT_RST_HIGH, INIT_CMD, INIT_DONE };
static InitState _initState = INIT_RST_LOW;
static bool _simCmdsAllowed = false;  // разрешено отправлять SIM команды

static uint32_t _initTimer   = 0;
static uint8_t  _initCmdIdx  = 0;

static const char* _initCmds[] = {
    "AT",
    "ATE0",
    "AT+CLTS=1",
    "AT&W",
    "AT+CREG=2",
};

static const uint8_t _initCmdsCount = sizeof(_initCmds) / sizeof(_initCmds[0]);

// ── Состояние опросчика ───────────────────────────────────────────────────────
enum PollState { POLL_IDLE, POLL_WAIT_CREG, POLL_WAIT_CSQ, POLL_WAIT_CBC, POLL_WAIT_COPS };
static PollState _pollState = POLL_IDLE;

static uint32_t _pollTimer    = 0;
static uint32_t _pollCmdTimer = 0;
static ModemStatus _newStatus;

// ── Вотчдог регистрации ───────────────────────────────────────────────────────
//
//  WD_OK   → сеть есть, опрос AT+CREG? раз в 30 сек
//  WD_WARN → сеть пропала, опрос каждые 10 сек
//
//  WD_OK  → WD_WARN: creg=0/3 из опроса или URC из потока
//  WD_WARN → WD_OK:  creg=1/5
//  6 плохих ответов подряд → перезагрузка модема
//
enum WdState { WD_OK, WD_WARN };
static WdState  _wdState      = WD_OK;
static uint32_t _wdTimer      = 0;
static uint8_t  _wdFailCount  = 0;
static bool     _wdPollActive  = false;  // true = текущий опрос инициирован вотчдогом
static bool     _wdStartupGrace = true;   // true = ждём после старта

static bool    _simInitDone  = false;  // команды требующие SIM уже отправлены
static uint8_t _simCmdIdx    = 0;
static uint32_t _simCmdTimer = 0;

static const char* _simCmds[] = {
    "AT+CLIP=1",
    "AT+DDET=1,0,0",
    "AT+CMGF=0",
    "AT+CNMI=2,2,0,0,0",
};
static const uint8_t _simCmdsCount = sizeof(_simCmds) / sizeof(_simCmds[0]);

static uint32_t _wdGraceTimer   = 0;
static bool _timeSync = false;

static bool     _smsSending    = false;
static bool _smsCmgfDone = false;  // AT+CMGF=0 отправлен и получен OK
static uint32_t _smsCmdTimer   = 0;
static String   _smsPduPending = "";

static bool   _smsPendingFlag = false;
static String _smsPendingTo   = "";
static String _smsPendingText = "";

#define WD_INTERVAL_OK    30000u   // мс между опросами в норме
#define WD_INTERVAL_WARN  10000u   // мс между опросами при потере сети
#define WD_MAX_FAILS      12        // сколько плохих ответов до перезагрузки


// ── HTML ─────────────────────────────────────────────────────────────────────

static const char MODEM_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <h3>📊 Статус модема</h3>
  <div id="modem_status" style="font-size:12px;color:var(--muted)">загрузка...</div>
</div>

<div class="settings-group">
  <h3>✉️ SMS</h3>
  <div style="display:flex;gap:8px;margin-bottom:8px">
    <input type="text" id="sms_to" placeholder="+79139158466" style="margin:0;flex:1">
    <button onclick="sendSmsWeb()" style="width:auto;padding:10px 16px;margin:0">📤 Отправить</button>
  </div>
  <textarea id="sms_text" rows="3" placeholder="Текст сообщения" style="width:100%;
    box-sizing:border-box;padding:8px;border-radius:6px;border:1px solid var(--border);
    background:var(--bg);color:var(--text);resize:vertical;margin-bottom:8px"></textarea>
  <h4 style="margin:8px 0 4px">📥 Входящие</h4>
  <pre id="sms_inbox" style="font-size:11px;color:var(--muted);height:120px;
    overflow-y:auto;white-space:pre-wrap;background:var(--bg);padding:8px;
    border-radius:6px;border:1px solid var(--border)">загрузка...</pre>
</div>

<div class="settings-group">
  <h3>📋 Системный лог</h3>
  <pre id="sys_log" style="font-size:11px;color:var(--muted);height:160px;overflow-y:auto;
    white-space:pre-wrap;background:var(--bg);padding:8px;border-radius:6px;
    border:1px solid var(--border)"></pre>
  <button onclick="clearSysLog()" style="margin-top:6px">🗑 Очистить</button>
</div>

<div class="settings-group">
  <h3>📟 Терминал</h3>
  <div style="display:flex;gap:8px;margin-bottom:8px">
    <select id="at_preset" onchange="atPresetSelect()" style="margin:0;padding:10px 8px;
      border-radius:6px;border:1px solid var(--border);background:var(--bg);color:var(--text)">
      <option value="AT">AT — проверка связи</option>
      <option value="AT+CREG?">AT+CREG? — регистрация</option>
      <option value="AT+CSQ">AT+CSQ — уровень сигнала</option>
      <option value="AT+CBC">AT+CBC — напряжение</option>
      <option value="AT+COPS?">AT+COPS? — оператор</option>
      <option value="AT+CCLK?">AT+CCLK? — время</option>
      <option value="AT+CMGF?">AT+CMGF? — режим SMS</option>
      <option value="AT+CNMI?">AT+CNMI? — уведомления SMS</option>
      <option value="AT+CLTS?">AT+CLTS? — синхронизация времени</option>
      <option value="AT+GMR">AT+GMR — версия прошивки</option>
      <option value="AT+CMGDA=&quot;DEL ALL&quot;">AT+CMGDA — удалить все SMS</option>
    </select>
    <input type="text" id="at_cmd" placeholder="команда" style="margin:0;flex:1">
    <button onclick="sendAT()" style="width:auto;padding:10px 16px;margin:0">▶</button>
  </div>
  <pre id="modem_log" style="font-size:11px;color:var(--muted);height:220px;overflow-y:auto;
    white-space:pre-wrap;background:var(--bg);padding:8px;border-radius:6px;
    border:1px solid var(--border)"></pre>
  <button onclick="clearModemLog()" style="margin-top:6px">🗑 Очистить</button>
</div>

<div class="settings-group">
  <h3>⚙️ Настройки</h3>
  <label class="toggle-row">
    <span>Включить модем</span>
    <input type="checkbox" id="modem_enabled" onchange="modemToggle()">
  </label>
  <div id="modem_fields">
    <label>Таймаут DTMF (сек)</label>
    <input type="number" name="modem_dtmf_timeout" placeholder="10">
    <label>Интервал опроса статуса (сек)</label>
    <input type="number" name="modem_poll_interval" placeholder="60">
    <div style="display:flex;gap:8px;margin-top:12px">
      <button onclick="saveModem()" style="flex:1">💾 Сохранить</button>
      <button onclick="restartModem()" class="btn-danger" style="flex:1">🔄 Перезагрузить модем</button>
    </div>
  </div>
</div>

<script>
function atPresetSelect() {
  const v = document.getElementById('at_preset').value;
  if (v) document.getElementById('at_cmd').value = v;
}
function signalBars(csq) {
  if (csq == 99 || csq < 0) {
    return '<span style="letter-spacing:2px;font-weight:bold;color:var(--muted)">||||||||</span>';
  }
  // CSQ 0-31: 0-4 низкий, 5-14 средний, 15-31 хороший
  // Переводим в 0-8 делений
  const level = Math.round((Math.min(csq, 31) / 31) * 8);
  let bars = '';
  for (let i = 1; i <= 8; i++) {
    let color;
    if (i > level) {
      color = 'var(--muted)';
    } else if (level <= 2) {
      color = '#e74c3c';  // красный
    } else if (level <= 4) {
      color = '#f39c12';  // жёлтый
    } else {
      color = '#2ecc71';  // зелёный
    }
    bars += `<span style="color:${color}">|</span>`;
  }
  return `<span style="letter-spacing:2px;font-weight:bold">${bars}</span>`;
}
async function loadModem() {
  const r = await fetch('/api/modem/config');
  const d = await r.json();
  document.getElementById('modem_enabled').checked = d.enabled;
  ['dtmf_timeout','poll_interval'].forEach(k => {
    const el = document.querySelector('[name="modem_'+k+'"]');
    if (el) el.value = d[k];
  });
  modemToggle();
}
function modemToggle() {
  document.getElementById('modem_fields').style.display =
    document.getElementById('modem_enabled').checked ? 'block' : 'none';
}
async function saveModem() {
  const d = {
    enabled:       document.getElementById('modem_enabled').checked,
    dtmf_timeout:  +document.querySelector('[name="modem_dtmf_timeout"]').value,
    poll_interval: +document.querySelector('[name="modem_poll_interval"]').value,
  };
  const r = await fetch('/api/modem/save',
    {method:'POST', headers:{'Content-Type':'application/json'}, body:JSON.stringify(d)});
  if (r.ok) alert('Сохранено, перезагрузите устройство');
}
async function sendAT() {
  const cmd = document.getElementById('at_cmd').value.trim();
  if (!cmd) return;
  await fetch('/api/modem/at', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify({cmd})});
  document.getElementById('at_cmd').value = '';
}
document.getElementById('at_cmd').addEventListener('keydown', e => {
  if (e.key === 'Enter') sendAT();
});
async function loadModemLog() {
  const r = await fetch('/api/modem/log');
  const d = await r.json();
  const el = document.getElementById('modem_log');
  const atBottom = el.scrollHeight - el.scrollTop <= el.clientHeight + 5;
  el.textContent = d.log || '';
  if (atBottom) el.scrollTop = el.scrollHeight;
}
async function loadModemStatus() {
  const r = await fetch('/api/modem/status');
  const d = await r.json();
  const regMap = {'-1':'неизвестно','0':'не зарегистрирован','1':'домашняя сеть',
    '2':'поиск...','3':'отказ','5':'роуминг'};
  document.getElementById('modem_status').innerHTML =
    `<b>Сеть:</b> ${regMap[String(d.creg)] || d.creg} &nbsp;
     <b>Сигнал:</b> ${signalBars(d.csq)} &nbsp;
     <b>Питание:</b> ${d.vbat > 0 ? (d.vbat/1000).toFixed(3)+'V' : '—'} &nbsp;
     <b>Оператор:</b> ${d.oper || '—'}`;
}
async function clearModemLog() {
  await fetch('/api/modem/log/clear', {method:'POST'});
  document.getElementById('modem_log').textContent = '';
}
async function loadSysLog() {
  const r = await fetch('/api/syslog');
  const d = await r.json();
  const el = document.getElementById('sys_log');
  const atBottom = el.scrollHeight - el.scrollTop <= el.clientHeight + 5;
  el.textContent = d.log || '';
  if (atBottom) el.scrollTop = el.scrollHeight;
}
async function clearSysLog() {
  await fetch('/api/syslog/clear', {method:'POST'});
  document.getElementById('sys_log').textContent = '';
}
async function restartModem() {
  if (!confirm('Перезагрузить модем?')) return;
  await fetch('/api/modem/restart', {method:'POST'});
}
async function sendSmsWeb() {
  const to   = document.getElementById('sms_to').value.trim();
  const text = document.getElementById('sms_text').value.trim();
  if (!to || !text) return;
  const r = await fetch('/api/modem/sms/send', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({to, text})
  });
  if (r.ok) { alert('SMS отправлена'); document.getElementById('sms_text').value = ''; }
}
async function loadSmsInbox() {
  const r = await fetch('/api/modem/sms/inbox');
  const d = await r.json();
  document.getElementById('sms_inbox').textContent =
    (d.messages || []).map(m => m).join('\n') || 'нет сообщений';
}
loadModem();
loadModemLog();
loadModemStatus();
loadSmsInbox();
loadSysLog();
setInterval(loadModemLog, 2000);
setInterval(loadModemStatus, 10000);
setInterval(loadSysLog, 5000);
setInterval(loadSmsInbox, 10000);
</script>
)html";

// ── Config ───────────────────────────────────────────────────────────────────
bool ModemHandler::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/modem.json", doc)) return false;
    modemCfg.enabled       = doc["enabled"]       | true;
    modemCfg.rx_pin        = doc["rx_pin"]         | 2;
    modemCfg.tx_pin        = doc["tx_pin"]         | 4;
    modemCfg.rst_pin       = doc["rst_pin"]        | 33;
    modemCfg.ri_pin        = doc["ri_pin"]         | -1;
    modemCfg.baud          = doc["baud"]           | 9600;
    modemCfg.dtmf_timeout  = doc["dtmf_timeout"]   | 10;
    modemCfg.poll_interval = doc["poll_interval"]  | 60;
    _modemCreg    = modemStatus.creg;
    _modemEnabled = modemCfg.enabled;
    return true;
}

bool ModemHandler::saveConfig() {
    JsonDocument doc;
    doc["enabled"]       = modemCfg.enabled;
    doc["rx_pin"]        = modemCfg.rx_pin;
    doc["tx_pin"]        = modemCfg.tx_pin;
    doc["rst_pin"]       = modemCfg.rst_pin;
    doc["ri_pin"]        = modemCfg.ri_pin;
    doc["baud"]          = modemCfg.baud;
    doc["dtmf_timeout"]  = modemCfg.dtmf_timeout;
    doc["poll_interval"] = modemCfg.poll_interval;
    return ConfigManager::saveJson("/modem.json", doc);
}

// ── Init ─────────────────────────────────────────────────────────────────────
void ModemHandler::init() {
    loadConfig();
    if (!modemCfg.enabled) { Serial.println("[MODEM] Disabled"); return; }

    if (modemCfg.ri_pin >= 0)
        pinMode(modemCfg.ri_pin, INPUT);

    _sim.begin(modemCfg.baud, SERIAL_8N1, modemCfg.rx_pin, modemCfg.tx_pin);

    WebHandler::registerTab({"modem", "Модем 📞", "📞", MODEM_TAB_HTML});

    extern AsyncWebServer server;

    server.on("/api/modem/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["enabled"]       = modemCfg.enabled;
        doc["rx_pin"]        = modemCfg.rx_pin;
        doc["tx_pin"]        = modemCfg.tx_pin;
        doc["rst_pin"]       = modemCfg.rst_pin;
        doc["ri_pin"]        = modemCfg.ri_pin;
        doc["baud"]          = modemCfg.baud;
        doc["dtmf_timeout"]  = modemCfg.dtmf_timeout;
        doc["poll_interval"] = modemCfg.poll_interval;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* hSave = new AsyncCallbackJsonWebHandler("/api/modem/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            modemCfg.enabled       = o["enabled"]       | true;
            modemCfg.rx_pin        = o["rx_pin"]         | 4;
            modemCfg.tx_pin        = o["tx_pin"]         | 2;
            modemCfg.rst_pin       = o["rst_pin"]        | 33;
            modemCfg.ri_pin        = o["ri_pin"]         | -1;
            modemCfg.baud          = o["baud"]           | 9600;
            modemCfg.dtmf_timeout  = o["dtmf_timeout"]   | 10;
            modemCfg.poll_interval = o["poll_interval"]  | 60;
            ModemHandler::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSave);

    auto* hAT = new AsyncCallbackJsonWebHandler("/api/modem/at",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            String cmd = json["cmd"] | "";
            if (cmd.length() > 0) ModemHandler::sendAT(cmd.c_str());
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hAT);

    server.on("/api/modem/log", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["log"] = _modemLog;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/modem/log/clear", HTTP_POST, [](AsyncWebServerRequest* req) {
        _modemLog = "";
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/modem/status", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["creg"] = modemStatus.creg;
        doc["csq"]  = modemStatus.csq;
        doc["vbat"] = modemStatus.vbat;
        doc["oper"] = modemStatus.oper;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/modem/restart", HTTP_POST, [](AsyncWebServerRequest* req) {
        _manualRestart = true;
        req->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    auto* hSms = new AsyncCallbackJsonWebHandler("/api/modem/sms/send",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            String to   = json["to"]   | "";
            String text = json["text"] | "";
            if (to.length() > 0 && text.length() > 0)
                _smsPendingTo   = json["to"]   | "";
                _smsPendingText = json["text"] | "";
                _smsPendingFlag = true;
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSms);

    server.on("/api/modem/sms/inbox", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        JsonArray arr = doc["messages"].to<JsonArray>();
        for (int i = 0; i < 10; i++) {
            if (_smsInbox[i].length() > 0)
                arr.add(_smsInbox[i]);
        }
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // Запускаем стейт-машину инициализации с RST_LOW (или сразу RST_HIGH если
    // пин не задан)
    if (modemCfg.rst_pin >= 0) {
        pinMode(modemCfg.rst_pin, OUTPUT);
        digitalWrite(modemCfg.rst_pin, LOW);
        _initState = INIT_RST_LOW;
        _addLog("Resetting modem...");
    } else {
        // RST не используется — сразу ждём Call Ready
        _initState = INIT_RST_HIGH;
        _addLog("Waiting for Call Ready...");
    }
    _initTimer = millis();

    // Первый опрос запустим после завершения инициализации (_initTick сам
    // выставит _pollTimer в момент перехода в INIT_DONE)
    Serial.println("[MODEM] Init started (non-blocking)");
}

// ── Стейт-машина инициализации ────────────────────────────────────────────────
void ModemHandler::_initTick() {
    switch (_initState) {

    case INIT_RST_LOW:
        // Держим RST=LOW 200 мс, затем поднимаем
        if (millis() - _initTimer >= 200) {
            digitalWrite(modemCfg.rst_pin, HIGH);
            _initState = INIT_RST_HIGH;
            _initTimer = millis();
            _addLog("Waiting for Call Ready...");
        }
        break;

    case INIT_RST_HIGH:
        // Ждём "Call Ready" — детект идёт в loop() через _callReadyReceived.
        // Таймаут 30 сек — SIM800L может стартовать долго.
        if (_callReadyReceived || millis() - _initTimer >= 30000) {
            if (_callReadyReceived)
                _addLog("Call Ready received");
            else
                _addLog("Call Ready timeout, proceeding anyway");
            _callReadyReceived = false;
            _initCmdIdx = 0;
            _initState  = INIT_CMD;
            _initTimer  = millis();
            sendAT(_initCmds[_initCmdIdx]);
        }
        break;

    case INIT_CMD:
        // Пауза 500 мс после каждой команды, затем следующая
        if (millis() - _initTimer >= 500) {
            _initCmdIdx++;
            if (_initCmdIdx >= _initCmdsCount) {
                _addLog("Init done");
                _initState  = INIT_DONE;
                _pollTimer  = millis();   // первый опрос через poll_interval
                _wdStartupGrace = true;
                _wdGraceTimer   = millis();
                Serial.println("[MODEM] Ready");
            } else {
                sendAT(_initCmds[_initCmdIdx]);
                _initTimer = millis();
            }
        }
        break;

    case INIT_DONE:
        // Ничего — обычный loop() работает
        break;
    }
}

// ── Loop ─────────────────────────────────────────────────────────────────────
void ModemHandler::loop() {
    if (!modemCfg.enabled) return;

    // Читаем все доступные байты из UART
    while (_sim.available()) {
        char c = (char)_sim.read();
        _modemLog += c;
        if (c == '>' && _smsSending && _smsCmgfDone) {
            _sim.read(); // съедаем пробел после '>'
            _sim.print(_smsPduPending);
            _sim.write(0x1A);
            _smsSending    = false;
            _smsPduPending = "";
            _addLog("SMS PDU sent");
            continue;
        }
        if (_modemLog.length() > 4096)
            _modemLog = _modemLog.substring(_modemLog.length() - 4096);

        // SIM800L завершает строки через \r\n или только \r
        // Обрабатываем буфер при любом из них
        if (c == '\n' || c == '\r') {
            _lineBuffer.trim();
            if (_lineBuffer.length() > 0) {
                // "Call Ready" детектируем всегда — нужен и во время INIT_RST_HIGH
                if (_lineBuffer == "Call Ready")
                    _callReadyReceived = true;

                if (_initState == INIT_DONE) {
                    _processLine(_lineBuffer);
                    _parsePollResponse(_lineBuffer);
                }
            }
            _lineBuffer = "";
        } else {
            _lineBuffer += c;
        }
    }

    // Тик инициализации (завершается переходом в INIT_DONE)
    if (_initState != INIT_DONE) {
        _initTick();
        return;   // пока не инициализировались — больше ничего не делаем
    }

    // Ручной ресет из веб-интерфейса
    if (_manualRestart) {
        _manualRestart = false;
        _sysLog("Manual modem restart requested");
        _wdRestart();
        return;
    }

    // Таймаут poll — проверяем здесь, а не внутри _parsePollResponse,
    // чтобы он срабатывал даже если модем полностью молчит
    if (_pollState != POLL_IDLE && millis() - _pollCmdTimer > 15000) {
        _addLog("Poll timeout");
        _pollState = POLL_IDLE;
        _pollTimer = millis();
    }

    if (_smsSending && millis() - _smsCmdTimer > 30000) {
        _addLog("SMS send timeout");
        _sim.write(0x1B); // ESC — отменяем отправку
        _smsSending    = false;
        _smsPduPending = "";
    }
        
    // Отложенная отправка SMS — ждём POLL_IDLE
    if (_smsPendingFlag && _pollState == POLL_IDLE && !_smsSending) {
        _smsPendingFlag = false;
        ModemHandler::sendSms(_smsPendingTo, _smsPendingText);
    }

    _checkAnswerDelay();
    _checkTimeout();
    _wdTick();
    _simInitTick();
    _pollTick();
}

// ── AT helper ────────────────────────────────────────────────────────────────
void ModemHandler::sendAT(const char* cmd) {
    _sim.println(cmd);
    _addLog(String(">> ") + cmd);
}

// ── Парсер входящих строк (звонок) ───────────────────────────────────────────
void ModemHandler::_processLine(const String& line) {
    _addLog("LINE:[" + line + "]"); 
    Serial.println("[MODEM] << " + line);

    if (line == "RING") {
        if (_callState == CALL_IDLE) {
            _callState = CALL_RINGING;
            _callerNum = "";
            _dtmfSeq   = "";
        }
        return;
    }
    if (line.startsWith("+CLIP:") && _callState == CALL_RINGING) {
        _onClip(line); return;
    }
    if (line.startsWith("+DTMF:") && _callState == CALL_IN_CALL) {
        String rest = line.substring(6); rest.trim();
        if (rest.length() > 0) _onDTMF(rest.charAt(0));
        return;
    }
    if (line == "NO CARRIER" || line == "BUSY" || line == "NO ANSWER") {
        _onNoCarrier(); return;
    }
    // URC: модем сам сообщает об изменении регистрации (AT+CREG=2 включён при init)
    // URC формат: +CREG: <stat>,"<lac>","<ci>"  — stat сразу после пробела, без <n>
    // Отличие от ответа на запрос: в URC нет числа перед первой запятой (там буква или кавычка)
    if (line.startsWith("+CREG:") && _pollState == POLL_IDLE) {
        int sp = line.indexOf(' ');
        if (sp < 0) return;
        // stat — первый символ после пробела
        int stat = line.substring(sp + 1, sp + 2).toInt();
        _addLog("URC CREG: " + String(stat));
        _wdHandleCreg(stat);
        modemStatus.creg = stat;
        return;
    }

    if (line.startsWith("+CCLK:")) {
        _parseTime(line);
        return;
    }

    // Входящее SMS — две строки: заголовок и PDU
    _addLog("GOT > at " + String(millis()) + " sending=" + String(_smsSending));

    if (line == ">" && _smsSending && _smsCmgfDone) {
        _sim.print(_smsPduPending);
        _sim.write(0x1A);
        _smsSending    = false;
        _smsPduPending = "";
        _addLog("SMS PDU sent");
        return;
    }
    // OK на AT+CMGF=0 — теперь отправляем AT+CMGS
    if (line == "OK" && _smsSending && !_smsCmgfDone) {
        _smsCmgfDone  = true;
        _smsCmdTimer  = millis();
        // Длина PDU = (длина hex строки / 2) - 1 (без SMSC байта)
        int pduLen = (_smsPduPending.length() / 2) - 1;
        sendAT(("AT+CMGS=" + String(pduLen)).c_str());
        return;
    }

    if (_smsNextLine) {
        _smsNextLine = false;
        _onSmsReceived(_smsSender, line);
        return;
    }
    if (line.startsWith("+CMT:")) {
        // +CMT: "",<len>  — в PDU режиме номер в самом PDU
        _smsSender   = "";
        _smsNextLine = true;
        return;
    }
}

void ModemHandler::_parseTime(const String& line) {
    // Формат: +CCLK: "26/04/21,14:30:00+12"
    int q1 = line.indexOf('"');
    int q2 = line.indexOf('"', q1 + 1);
    if (q1 < 0 || q2 < 0) return;
    String t = line.substring(q1 + 1, q2);
    // yy/MM/dd,hh:mm:ss±zz
    int year  = t.substring(0, 2).toInt() + 2000;
    int month = t.substring(3, 5).toInt();
    int day   = t.substring(6, 8).toInt();
    int hour  = t.substring(9, 11).toInt();
    int min   = t.substring(12, 14).toInt();
    int sec   = t.substring(15, 17).toInt();
    if (year < 2024) { _addLog("CCLK: invalid time"); return; }
    struct tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon  = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min  = min;
    tm.tm_sec  = sec;
    time_t t_unix = mktime(&tm);
    struct timeval tv = { t_unix, 0 };
    settimeofday(&tv, nullptr);
    #ifdef MODULE_TIME
        TimeManager::notifySynced(TimeSource::MODEM);
    #endif
    _timeSync = true;
    _addLog("Time synced: " + t);
}

// ── Парсер ответов опроса ─────────────────────────────────────────────────────
void ModemHandler::_parsePollResponse(const String& line) {
    if (line.startsWith("+CREG:") && _pollState == POLL_WAIT_CREG) {
        // AT+CREG=2 возвращает: +CREG: <n>,<stat>[,<lac>,<ci>[,<AcT>]]
        // AT+CREG=0 возвращает: +CREG: <stat>
        // Нам нужен <stat> — второй элемент при наличии запятой, иначе первый
        int comma = line.indexOf(',');
        if (comma > 0) {
            // Берём следующий символ после запятой (stat — одна цифра)
            _newStatus.creg = line.substring(comma + 1, comma + 2).toInt();
        } else {
            // Нет запятой: +CREG: <stat>
            int space = line.indexOf(' ');
            _newStatus.creg = (space > 0)
                ? line.substring(space + 1, space + 2).toInt()
                : line.substring(7, 8).toInt();
        }
        // Сообщаем вотчдогу результат
        _wdHandleCreg(_newStatus.creg);

        // Если опрос инициирован вотчдогом — только CREG, без CSQ/CBC/COPS
        if (_wdPollActive) {
            _wdPollActive = false;
            modemStatus.creg = _newStatus.creg;
            _pollState = POLL_IDLE;
            // При WD_WARN считаем failures
            if (_wdState == WD_WARN && !_wdStartupGrace) {
                if (_newStatus.creg != 1 && _newStatus.creg != 5) {
                    _wdFailCount++;
                    _addLog("Watchdog: fail " + String(_wdFailCount) + "/" + String(WD_MAX_FAILS));
                    if (_wdFailCount >= WD_MAX_FAILS)
                        _wdRestart();
                }
            }
            return;
        }

        // Опрос инициирован pollTick — тоже считаем failures при WD_WARN
        if (_wdState == WD_WARN) {
            if (_newStatus.creg != 1 && _newStatus.creg != 5) {
                _wdFailCount++;
                _addLog("Watchdog: fail " + String(_wdFailCount) + "/" + String(WD_MAX_FAILS));
                if (_wdFailCount >= WD_MAX_FAILS)
                    _wdRestart();
            }
        }

        _pollState    = POLL_WAIT_CSQ;
        _pollCmdTimer = millis();
        sendAT("AT+CSQ");
        return;
    }
    if (line.startsWith("+CSQ:") && _pollState == POLL_WAIT_CSQ) {
        int s = line.indexOf(' '), c = line.indexOf(',');
        if (s > 0 && c > s) _newStatus.csq = line.substring(s + 1, c).toInt();
        _pollState    = POLL_WAIT_CBC;
        _pollCmdTimer = millis();
        sendAT("AT+CBC");
        return;
    }
    if (line.startsWith("+CBC:") && _pollState == POLL_WAIT_CBC) {
        int c1 = line.indexOf(',');
        int c2 = (c1 > 0) ? line.indexOf(',', c1 + 1) : -1;
        if (c2 > 0) _newStatus.vbat = line.substring(c2 + 1).toInt();
        _pollState    = POLL_WAIT_COPS;
        _pollCmdTimer = millis();
        sendAT("AT+COPS?");
        return;
    }
    if (line.startsWith("+COPS:") && _pollState == POLL_WAIT_COPS) {
        int q1 = line.indexOf('"');
        int q2 = (q1 >= 0) ? line.indexOf('"', q1 + 1) : -1;
        if (q2 > q1)
            strlcpy(_newStatus.oper, line.substring(q1 + 1, q2).c_str(), sizeof(_newStatus.oper));
        else
            strlcpy(_newStatus.oper, "", sizeof(_newStatus.oper));
        _pollState = POLL_IDLE;
        _pollTimer = millis();
        _publishStatusIfChanged();
        return;
    }
}

// ── Запуск цикла опроса ───────────────────────────────────────────────────────
void ModemHandler::_pollTick() {
    if (_smsSending) return;
    if (_callState != CALL_IDLE)   return;
    if (_pollState != POLL_IDLE) return;
    if (millis() - _pollTimer < (uint32_t)modemCfg.poll_interval * 1000) return;
    _newStatus    = ModemStatus();
    _pollState    = POLL_WAIT_CREG;
    _pollCmdTimer = millis();
    sendAT("AT+CREG?");
}

// ── Публикация если изменилось ────────────────────────────────────────────────
void ModemHandler::_publishStatusIfChanged() {
    bool changed =
        _newStatus.creg != modemStatus.creg ||
        _newStatus.csq  != modemStatus.csq  ||
        abs(_newStatus.vbat - modemStatus.vbat) > 50 ||
        strcmp(_newStatus.oper, modemStatus.oper) != 0;

    if (!changed) { _addLog("Status poll: no changes"); return; }

    modemStatus = _newStatus;

    JsonDocument doc;
    doc["creg"] = modemStatus.creg;
    doc["csq"]  = modemStatus.csq;
    doc["vbat"] = modemStatus.vbat;
    doc["oper"] = modemStatus.oper;
    String payload;
    serializeJson(doc, payload);
    _addLog("Status changed: " + payload);

#ifdef MODULE_MQTT
    String topic = String(baseCfg.device_name) + "/modem/status";
    MqttHandler::publish(topic.c_str(), payload.c_str(), true);
#endif
}

// ── Обработка звонка ─────────────────────────────────────────────────────────
void ModemHandler::_onClip(const String& line) {
    int s = line.indexOf('"');
    int e = (s >= 0) ? line.indexOf('"', s + 1) : -1;
    _callerNum = (e > s) ? _normalizePhone(line.substring(s + 1, e)) : "unknown";
    _addLog("Incoming: " + _callerNum);

    // Снимаем трубку, затем AT+CMUT=1 через 500 мс — неблокирующий таймер
    sendAT("ATA");
    _answerPending = true;
    _answerAt      = millis();

    _callStart = millis();
    _dtmfSeq   = "";
}

// Вызывается каждый loop() — отправляет AT+CMUT=1 через 500 мс после ATA
void ModemHandler::_checkAnswerDelay() {
    if (!_answerPending) return;
    if (millis() - _answerAt < 400) return;

    sendAT("AT+CMUT=1");
    _callState     = CALL_IN_CALL;
    _answerPending = false;
    _addLog("Answered, waiting DTMF...");
}

void ModemHandler::_onDTMF(char digit) {
    _dtmfSeq += digit;
    _addLog(String("DTMF: ") + digit + " (seq: " + _dtmfSeq + ")");
}

void ModemHandler::_onNoCarrier() {
    if (_callLogBuffer.length() > 0) {
        _sysLog("=== Call " + _callerNum + " (no DTMF) ===\n" + _callLogBuffer);
        _callLogBuffer = "";
    }
    if (_callState == CALL_IN_CALL && _dtmfSeq.length() > 0) {
        _addLog("Call ended, result: " + _callerNum + " - " + _dtmfSeq);
        _notifyResult();
    } else if (_callState == CALL_IN_CALL) {
        _addLog("Call ended, no DTMF");
    }
    _answerPending = false;
    _callState     = CALL_IDLE;
    _callerNum     = "";
    _dtmfSeq       = "";
}

void ModemHandler::_checkTimeout() {
    if (_callState != CALL_IN_CALL) return;
    if (millis() - _callStart < (uint32_t)modemCfg.dtmf_timeout * 1000) return;
    _addLog("DTMF timeout");
    sendAT("ATH");
    if (_callLogBuffer.length() > 0) {
        _sysLog("=== Call " + _callerNum + " (timeout) ===\n" + _callLogBuffer);
        _callLogBuffer = "";
    }
    // Уведомляем до сброса состояния, чтобы колбэк видел актуальные данные
    if (_dtmfSeq.length() > 0) _notifyResult();
    else _addLog("No DTMF received");
    _callState = CALL_IDLE;
    _callerNum = "";
    _dtmfSeq   = "";
}

void ModemHandler::_notifyResult() {
    if (_callLogBuffer.length() > 0) {
        _sysLog("=== Call " + _callerNum + " ===\n" + _callLogBuffer);
        _callLogBuffer = "";
    }
    String msg = _callerNum + " - " + _dtmfSeq;
    _addLog("Publishing DTMF: " + msg);
#ifdef MODULE_MQTT
    String topic = String(baseCfg.device_name) + "/modem/dtmf";
    MqttHandler::publish(topic.c_str(), msg.c_str());
#endif
#ifdef MODULE_TELEGRAM
    TgHandler::sendMessage("📞 " + msg);
#endif
    if (onDTMFResult) onDTMFResult(_callerNum.c_str(), _dtmfSeq.c_str());
}

// ── Helpers ──────────────────────────────────────────────────────────────────
String ModemHandler::_normalizePhone(const String& phone) {
    String result;
    bool leadingPlusDone = false;
    for (unsigned int i = 0; i < phone.length(); i++) {
        char c = phone.charAt(i);
        if (!leadingPlusDone && c == '+') {
            result += '+';
            leadingPlusDone = true;
            continue;
        }
        if (isdigit(c)) {
            result += c;
            leadingPlusDone = true;   // после первой цифры '+' уже недопустим
        }
    }
    if (result.startsWith("8") && result.length() == 11)
        result = "+7" + result.substring(1);
    return result;
}

// ── Вотчдог: обработка результата creg ───────────────────────────────────────
// Вызывается из _parsePollResponse и при URC +CREG: в _processLine
void ModemHandler::_wdHandleCreg(int stat) {
    bool ok = (stat == 1 || stat == 5);

    if (ok) {
        _modemCreg    = modemStatus.creg;
        _modemEnabled = modemCfg.enabled;
        if (!_simInitDone) {
            _simCmdsAllowed = true;  // ← вместо _simInitDone = false
            _simCmdIdx      = 0;
            _simCmdTimer    = millis() - 500;
        }
        if (_wdState == WD_WARN)
            _addLog("Watchdog: network restored");
        _wdState      = WD_OK;
        _wdFailCount  = 0;
        _wdTimer      = millis();
        // Запускаем команды требующие SIM если ещё не отправляли
        if (!_simInitDone) {
            _simInitDone = false;  // будет выставлен после последней команды
            _simCmdIdx   = 0;
            _simCmdTimer = millis() - 500;  // чтобы первая команда ушла сразу
        }
        // Синхронизируем время при восстановлении сети
        sendAT("AT+CCLK?");
        return;
    }

    // stat == 0 — явная потеря сети → входим в режим восстановления
    // stat == 2 — поиск (временно, ждём) → не трогаем
    // stat == 3 — отказ регистрации → ждём, модем сам повторит попытку
    if (_wdState == WD_OK && stat == 0 && !_wdStartupGrace) {
        _addLog("Watchdog: network lost, entering recovery mode");
        _wdState     = WD_WARN;
        _wdFailCount = 0;
        _wdTimer     = millis();
    }
}

// ── Вотчдог: тик (вызывается из loop каждый цикл) ────────────────────────────
void ModemHandler::_wdTick() {
    if (_smsSending) return;
    if (_callState != CALL_IDLE) return;    // не трогаем во время звонка
    if (_pollState != POLL_IDLE) return;    // poll уже идёт — подождём

    if (_wdStartupGrace) {
        if (millis() - _wdGraceTimer < 120000) return;
        _wdStartupGrace = false;
        _wdTimer = millis();
        _addLog("Watchdog: active");
    }

    uint32_t interval = (_wdState == WD_WARN) ? WD_INTERVAL_WARN : WD_INTERVAL_OK;
    if (millis() - _wdTimer < interval) return;

    _wdTimer = millis();

    // Отправляем AT+CREG? — ответ придёт через _parsePollResponse
    // Используем отдельный мини-опрос: только CREG, без CSQ/CBC/COPS
    _pollState    = POLL_WAIT_CREG;
    _pollCmdTimer = millis();
    _newStatus    = modemStatus;   // сохраняем остальные поля как есть
    _wdPollActive = true;          // этот опрос — только CREG, без CSQ/CBC/COPS
    sendAT("AT+CREG?");
}

// ── Перезагрузка модема вотчдогом ────────────────────────────────────────────
void ModemHandler::_wdRestart() {
    _addLog("Watchdog: restarting modem after " + String(WD_MAX_FAILS) + " failures");
    _smsCmgfDone = false;
    _simInitDone = false;
    _simCmdsAllowed = false;
    _simCmdIdx   = 0;

#ifdef MODULE_MQTT
    JsonDocument doc;
    doc["creg"]         = modemStatus.creg;
    doc["csq"]          = modemStatus.csq;
    doc["vbat"]         = modemStatus.vbat;
    doc["oper"]         = modemStatus.oper;
    doc["reboot_reason"] = "watchdog";
    String payload;
    serializeJson(doc, payload);
    String topic = String(baseCfg.device_name) + "/modem/status";
    MqttHandler::publish(topic.c_str(), payload.c_str(), true);
#endif

    // Сбрасываем состояние вотчдога
    _wdState      = WD_OK;
    _wdFailCount  = 0;
    _wdPollActive = false;
    _wdTimer      = millis();
    _wdStartupGrace = true;
    _wdGraceTimer   = millis();

    // Сбрасываем состояние звонка на случай зависшего IN_CALL
    _callState     = CALL_IDLE;
    _answerPending = false;
    _callerNum     = "";
    _dtmfSeq       = "";

    // Запускаем полный рестарт модема через statemachine
    if (modemCfg.rst_pin >= 0) {
        digitalWrite(modemCfg.rst_pin, LOW);
        _initState = INIT_RST_LOW;
    } else {
        _initState = INIT_RST_HIGH;
    }
    _initTimer  = millis();
    _initCmdIdx = 0;
    _pollState  = POLL_IDLE;
}


// ── Системный лог (важные события → LittleFS /syslog.txt) ──────────────────
void ModemHandler::_sysLog(const String& msg) {
    // Формируем строку: uptime + сообщение
    uint32_t s = millis() / 1000;
    String ts;
    if (_timeSync) {
        time_t now = time(nullptr);
        struct tm* tm = localtime(&now);
        char buf[32];
        strftime(buf, sizeof(buf), "[%d.%m.%Y %H:%M:%S] ", tm);
        ts = String(buf);
    } else {
        uint32_t s = millis() / 1000;
        char buf[16];
        snprintf(buf, sizeof(buf), "[%02lu:%02lu:%02lu] ", s/3600, (s%3600)/60, s%60);
        ts = String(buf);
    }
    String line = ts + msg + "\n";
    Serial.println("[SYSLOG] " + msg);

    // Дописываем в файл, ротация: если > 8 КБ — обрезаем старое
    File f = LittleFS.open("/syslog.txt", "a");
    if (f) {
        f.print(line);
        f.close();
    }

    // Ротация
    File fr = LittleFS.open("/syslog.txt", "r");
    if (fr && fr.size() > 8192) {
        fr.close();
        // Читаем, отрезаем первую половину
        File fr2 = LittleFS.open("/syslog.txt", "r");
        String content = "";
        if (fr2) {
            fr2.seek(fr2.size() / 2);
            // Пропускаем до первого \n чтобы не начинать с середины строки
            while (fr2.available() && fr2.read() != '\n') {}
            while (fr2.available()) content += (char)fr2.read();
            fr2.close();
        }
        File fw = LittleFS.open("/syslog.txt", "w");
        if (fw) { fw.print(content); fw.close(); }
    } else if (fr) {
        fr.close();
    }
}

// ── SMS ──────────────────────────────────────────────────────────────────────
void ModemHandler::_onSmsReceived(const String& sender, const String& pdu) {
    _addLog("SMS received, PDU len: " + String(pdu.length()));
    // Сохраняем в кольцевой буфер для веб-интерфейса
    _smsInbox[_smsInboxIdx] = pdu;
    _smsInboxIdx = (_smsInboxIdx + 1) % 10;
    _publishSms(sender, pdu);
    _sysLog("SMS received: " + pdu.substring(0, 40));
}

void ModemHandler::_publishSms(const String& sender, const String& pdu) {
#ifdef MODULE_MQTT
    JsonDocument doc;
    doc["pdu"] = pdu;
    String payload;
    serializeJson(doc, payload);
    String topic = String(baseCfg.device_name) + "/modem/sms/inbox";
    MqttHandler::publish(topic.c_str(), payload.c_str());
#endif
}

// UTF-8 → UCS2 hex для отправки SMS с кириллицей
String ModemHandler::_utf8ToUcs2Hex(const String& text) {
    String result = "";
    char buf[5];
    int i = 0;
    while (i < (int)text.length()) {
        uint8_t c = (uint8_t)text[i];
        uint32_t cp;
        if (c < 0x80) {
            cp = c; i++;
        } else if ((c & 0xE0) == 0xC0) {
            cp = (c & 0x1F) << 6 | ((uint8_t)text[i+1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = (c & 0x0F) << 12 | ((uint8_t)text[i+1] & 0x3F) << 6
                                   | ((uint8_t)text[i+2] & 0x3F);
            i += 3;
        } else { i++; continue; }
        snprintf(buf, sizeof(buf), "%04X", (unsigned)cp);
        result += buf;
    }
    return result;
}

void ModemHandler::sendSms(const String& to, const String& text) {
    String ucs2 = _utf8ToUcs2Hex(text);
    // Формируем минимальный PDU: нет SMSC + TP-MTI/VPF + номер + UCS2 + текст
    // Длина текста в символах UCS2 (каждый символ = 2 байта = 4 hex символа)
    int charCount = ucs2.length() / 4;

    // Нормализуем номер
    String phone = _normalizePhone(to);
    if (phone.startsWith("+")) phone = phone.substring(1);

    // Тип номера: 91 = international, 81 = unknown
    String toa = phone.startsWith("7") ? "91" : "81";

    // Padding номера до чётной длины
    String phoneHex = "";
    String p = phone;
    if (p.length() % 2) p += "F";
    for (int i = 0; i < (int)p.length(); i += 2) {
        phoneHex += p[i+1];
        phoneHex += p[i];
    }
    // Собираем PDU
    // 00 - нет SMSC
    // 11 - TP-MTI=SMS-SUBMIT, TP-VPF=relative
    // 00 - TP-MR
    // <len> - длина номера в цифрах
    // <toa> - тип номера
    // <phone> - номер
    // 00 - TP-PID
    // 08 - TP-DCS = UCS2
    // AA - TP-VP = 4 дня
    // <len> - длина текста в октетах (charCount * 2)
    // <ucs2> - текст
    String lenPhone = String(phone.length(), HEX);
    if (lenPhone.length() < 2) lenPhone = "0" + lenPhone;
    lenPhone.toUpperCase();

    String lenText = String(charCount * 2, HEX);
    if (lenText.length() < 2) lenText = "0" + lenText;
    lenText.toUpperCase();

    String pdu = "001100" + lenPhone + toa + phoneHex + "0008AA" + lenText + ucs2;    pdu.toUpperCase();

    // PDU длина без SMSC октета (всё кроме первых двух символов "00")
    int pduLen = (pdu.length() / 2) - 1;

    _addLog("Sending SMS to " + to + ", PDU len: " + String(pduLen));
    _smsPduPending = pdu;
    _smsSending    = true;
    _smsCmgfDone   = false;
    _smsCmdTimer   = millis();
    _pollState     = POLL_IDLE;
    _pollTimer     = millis();
    sendAT("AT+CMGF=0");
}

void ModemHandler::_addLog(const String& msg) {
    Serial.println("[MODEM] " + msg);
    _modemLog += msg + "\n";
    if (_modemLog.length() > 4096)
        _modemLog = _modemLog.substring(_modemLog.length() - 4096);

    // Во время звонка буферизуем всё
    if (_callState != CALL_IDLE || _answerPending) {
        _callLogBuffer += msg + "\n";
        return;
    }

    // Вне звонка — только важные события в syslog
    if (msg.startsWith("Watchdog") ||
        msg.startsWith("Init done") ||
        msg.startsWith("Call Ready") ||
        msg.startsWith("Manual") ||
        msg.startsWith("Incoming"))
        _sysLog(msg);
}

void ModemHandler::_simInitTick() {
    if (_simInitDone) return;
    if (!_simCmdsAllowed) return;
    if (_pollState != POLL_IDLE) return;
    if (_smsSending) return;
    if (millis() - _simCmdTimer < 500) return;

    sendAT(_simCmds[_simCmdIdx]);
    _simCmdTimer = millis();
    _simCmdIdx++;
    if (_simCmdIdx >= _simCmdsCount) {
        _simInitDone = true;
        _addLog("SIM init done");
    }
}

#endif // MODULE_MODEM
```

---

### <a id="📄-src-modules-modem-modemhandler-h"></a>📄 `src/modules/modem/ModemHandler.h`

**File Info:**
- **Size**: 3.46 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/modules/modem/ModemHandler.h`
- **Relative Path**: `src/modules/modem`
- **Created**: 2026-04-17 05:26:39 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-08 04:40:19 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `d647ad087f244b0d6b1dcf3faf2fc7c3`
- **SHA256**: `b09c0243e92e29bc85ecde3ecb2342678ccd24b5e1bb85ea805f7c3e68e405cb`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef MODULE_MODEM

#include <HardwareSerial.h>
#include "../../core/CoreTypes.h"

// ── Конфиг (сохраняется в /modem.json) ──────────────────────────────────────
struct ModemConfig {
    bool    enabled        = true;
    int     rx_pin         = 2;
    int     tx_pin         = 4;
    int     rst_pin        = 33;   // RST модема, -1 = не используется
    int     ri_pin         = -1;   // RI,  -1 = не используется
    int     baud           = 9600;
    int     dtmf_timeout   = 10;
    int     poll_interval  = 60;
};

extern ModemConfig modemCfg;

// ── Последнее известное состояние модема ─────────────────────────────────────
struct ModemStatus {
    int     creg     = -1;
    int     csq      = -1;
    int     vbat     = -1;
    char    oper[32] = "";
};

extern ModemStatus modemStatus;

class ModemHandler {
public:
    static void init();
    static void loop();
    static void sendAT(const char* cmd);
    static void (*onDTMFResult)(const char* callerNum, const char* dtmf);
    static bool loadConfig();
    static bool saveConfig();
    static void sendSms(const String& to, const String& text);

private:
    // ── Стейт-машина инициализации ────────────────────────────────────────────
    static void _initTick();
    static void _simInitTick();

    // ── Обработка звонка ──────────────────────────────────────────────────────
    static void _processLine(const String& line);
    static void _onClip(const String& line);
    static void _onDTMF(char digit);
    static void _onNoCarrier();
    static void _checkTimeout();
    static void _checkAnswerDelay();    // неблокирующая задержка после ATA
    static void _notifyResult();

    // ── Опрос статуса ─────────────────────────────────────────────────────────
    static void _pollTick();
    static void _wdTick();
    static void _wdHandleCreg(int stat);
    static void _wdRestart();
    static void _parsePollResponse(const String& line);
    static void _publishStatusIfChanged();
    static void _parseTime(const String& line);

    // ── Вспомогательные ──────────────────────────────────────────────────────
    static void _addLog(const String& msg);
    static void _sysLog(const String& msg);
    static String _normalizePhone(const String& phone);

    // ── SMS ───────────────────────────────────────────────────────────────────
    static void _onSmsReceived(const String& sender, const String& pdu);
    static void _publishSms(const String& sender, const String& pdu);
    static String _utf8ToUcs2Hex(const String& text);
};

#endif // MODULE_MODEM
```

---

### <a id="📄-src-modules-mqtt-mqtthandler-cpp"></a>📄 `src/modules/mqtt/MqttHandler.cpp`

**File Info:**
- **Size**: 6.61 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/modules/mqtt/MqttHandler.cpp`
- **Relative Path**: `src/modules/mqtt`
- **Created**: 2026-02-17 12:57:02 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-24 06:52:45 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `5ee33718fcebb52e58ce5f2dede767c8`
- **SHA256**: `bfafcf8658010dc4aadc8923f347f32751bb76f9020097afa537a344468447c8`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef MODULE_MQTT
#include <vector>
#include "MqttHandler.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

#ifdef MODULE_TIME
#include "../time/TimeManager.h"
#endif

MqttConfig mqttCfg;

void (*MqttHandler::onMessage)(const char*, const char*, unsigned int) = nullptr;

static WiFiClient    _espClient;
static PubSubClient  _client(_espClient);
static uint32_t      _lastRetry = 0;
static std::vector<String> _extraSubs;

void MqttHandler::addSubscription(const char* topic) {
    _extraSubs.push_back(String(topic));
    // Если уже подключены — подписываемся сразу
    if (_client.connected()) _client.subscribe(topic);
}

// HTML вкладки настроек MQTT (PROGMEM)
static const char MQTT_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <label class="toggle-row">
    <span>Включить MQTT</span>
    <input type="checkbox" id="mqtt_enabled" onchange="mqttToggle()">
  </label>
  <div id="mqtt_fields">
    <input type="text"     name="mqtt_server" placeholder="Broker (broker.emqx.io)">
    <input type="number"   name="mqtt_port"   placeholder="Port" value="1883">
    <input type="text"     name="mqtt_user"   placeholder="User (опционально)">
    <input type="password" name="mqtt_pass"   placeholder="Password (опционально)">
    <button onclick="saveMqtt()">💾 Сохранить MQTT</button>
  </div>
</div>
<script>
async function loadMqtt() {
  const r = await fetch('/api/mqtt/config');
  const d = await r.json();
  document.getElementById('mqtt_enabled').checked = d.enabled;
  Object.keys(d).forEach(k => {
    const el = document.querySelector('[name="mqtt_'+k+'"]');
    if (el) el.value = d[k];
  });
  mqttToggle();
}
function mqttToggle() {
  document.getElementById('mqtt_fields').style.display =
    document.getElementById('mqtt_enabled').checked ? 'block' : 'none';
}
async function saveMqtt() {
  const data = {
    enabled: document.getElementById('mqtt_enabled').checked,
    server:  document.querySelector('[name="mqtt_server"]').value,
    port:    +document.querySelector('[name="mqtt_port"]').value,
    user:    document.querySelector('[name="mqtt_user"]').value,
    pass:    document.querySelector('[name="mqtt_pass"]').value,
  };
  const r = await fetch('/api/mqtt/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('MQTT сохранён, перезагружаюсь...');
}
loadMqtt();
</script>
)html";

bool MqttHandler::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/mqtt.json", doc)) return false;
    mqttCfg.enabled = doc["enabled"] | true;
    strlcpy(mqttCfg.server, doc["server"] | "", sizeof(mqttCfg.server));
    mqttCfg.port = doc["port"] | 1883;
    strlcpy(mqttCfg.user, doc["user"] | "", sizeof(mqttCfg.user));
    strlcpy(mqttCfg.pass, doc["pass"] | "", sizeof(mqttCfg.pass));
    return true;
}

bool MqttHandler::saveConfig() {
    JsonDocument doc;
    doc["enabled"] = mqttCfg.enabled;
    doc["server"]  = mqttCfg.server;
    doc["port"]    = mqttCfg.port;
    doc["user"]    = mqttCfg.user;
    doc["pass"]    = mqttCfg.pass;
    return ConfigManager::saveJson("/mqtt.json", doc);
}

void MqttHandler::init() {
    loadConfig();

    // Регистрируем вкладку настроек
    WebHandler::registerTab({"mqtt", "MQTT", "📊", MQTT_TAB_HTML});

    // API
    extern AsyncWebServer server; // объявлен в main.cpp
    server.on("/api/mqtt/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["enabled"] = mqttCfg.enabled;
        doc["server"]  = mqttCfg.server;
        doc["port"]    = mqttCfg.port;
        doc["user"]    = mqttCfg.user;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/mqtt/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            mqttCfg.enabled = obj["enabled"] | true;
            strlcpy(mqttCfg.server, obj["server"] | "", sizeof(mqttCfg.server));
            mqttCfg.port = obj["port"] | 1883;
            strlcpy(mqttCfg.user, obj["user"] | "", sizeof(mqttCfg.user));
            strlcpy(mqttCfg.pass, obj["pass"] | "", sizeof(mqttCfg.pass));
            MqttHandler::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
            xSemaphoreTake(coreMutex, portMAX_DELAY);
            sysState.pendingReboot = true;
            sysState.rebootAt = millis();
            xSemaphoreGive(coreMutex);
        }
    );
    server.addHandler(h);

    if (strlen(mqttCfg.server) > 0 && mqttCfg.enabled) {
        _client.setServer(mqttCfg.server, mqttCfg.port);
        _client.setCallback(_defaultCallback);
    }

    Serial.println("[MQTT] Init OK");
}

void MqttHandler::reconnect() {
    if (millis() - _lastRetry < 5000) return;
    _lastRetry = millis();

    String clientId = String(baseCfg.device_name) + "-" + String(WiFi.macAddress());
    bool ok = (strlen(mqttCfg.user) > 0)
              ? _client.connect(clientId.c_str(), mqttCfg.user, mqttCfg.pass)
              : _client.connect(clientId.c_str());

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    sysState.mqttConnected = ok;
    xSemaphoreGive(coreMutex);

    if (ok) {
        Serial.println("[MQTT] Connected");
        String sub = String(baseCfg.device_name) + "/set/#";
        _client.subscribe(sub.c_str());
        for (auto& s : _extraSubs) _client.subscribe(s.c_str());
    } else {
        Serial.printf("[MQTT] Failed rc=%d\n", _client.state());
    }
}

void MqttHandler::loop() {
    if (!mqttCfg.enabled || strlen(mqttCfg.server) == 0) return;

    bool hasNet = false;
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    hasNet = sysState.ethConnected || sysState.wifiConnected;
    xSemaphoreGive(coreMutex);
    if (!hasNet) return;

    if (!_client.connected()) {
        xSemaphoreTake(coreMutex, portMAX_DELAY);
        sysState.mqttConnected = false;
        xSemaphoreGive(coreMutex);
        reconnect();
    } else {
        _client.loop();
    }
}

void MqttHandler::publish(const char* topic, const char* payload, bool retained) {
    if (_client.connected()) _client.publish(topic, payload, retained);
}

void MqttHandler::_defaultCallback(char* topic, byte* payload, unsigned int length) {
    char buf[length + 1];
    memcpy(buf, payload, length);
    buf[length] = '\0';

#ifdef MODULE_TIME
    TimeManager::onMqttMessage(topic, buf);
#endif

    if (onMessage) {
        onMessage(topic, buf, length);
    }
}

#endif // MODULE_MQTT

```

---

### <a id="📄-src-modules-mqtt-mqtthandler-h"></a>📄 `src/modules/mqtt/MqttHandler.h`

**File Info:**
- **Size**: 1.02 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/modules/mqtt/MqttHandler.h`
- **Relative Path**: `src/modules/mqtt`
- **Created**: 2026-02-17 12:56:36 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-24 06:25:14 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `53ea2e826a1ca4ea17f2ec862b1ab88f`
- **SHA256**: `5adfa52ebea804783a5e5a096e1c130dd26f5a08607a279a251263214c9e8dd2`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef MODULE_MQTT

#include <PubSubClient.h>
#include <WiFi.h>
#include "../core/CoreTypes.h"

// Конфиг MQTT — сохраняется в /mqtt.json
struct MqttConfig {
    char server[64] = "";
    int  port       = 1883;
    char user[32]   = "";
    char pass[32]   = "";
    bool enabled    = true;
};

extern MqttConfig mqttCfg;

class MqttHandler {
public:
    static void init();   // Вызвать после WebHandler::init()
    static void loop();
    static void publish(const char* topic, const char* payload, bool retained = false);
    static bool loadConfig();
    static bool saveConfig();

    // Callback для входящих сообщений — устройство может переопределить
    static void (*onMessage)(const char* topic, const char* payload, unsigned int len);
    static void addSubscription(const char* topic);

private:
    static void reconnect();
    static void _defaultCallback(char* topic, byte* payload, unsigned int length);
};

#endif // MODULE_MQTT

```

---

### <a id="📄-src-modules-rs485-rs485handler-cpp"></a>📄 `src/modules/rs485/Rs485Handler.cpp`

**File Info:**
- **Size**: 23.43 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/modules/rs485/Rs485Handler.cpp`
- **Relative Path**: `src/modules/rs485`
- **Created**: 2026-05-21 19:00:42 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-22 15:00:22 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `e814bd66ded6da96ff75f14f9d51a311`
- **SHA256**: `c38db487f5da5ba8d9b9e9f7728fcb751b0d26f00ec2d07fcb29158a0a5ace12`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef MODULE_RS485
#include "Rs485Handler.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <WiFiServer.h>
#include <WiFiClient.h>

// ── Глобальные данные ────────────────────────────────────────────────────────
Rs485Config rs485Cfg;
Rs485Status rs485Status;
void (*Rs485Handler::onFrame)(const uint8_t*, size_t) = nullptr;

// ── Статические члены ────────────────────────────────────────────────────────
HardwareSerial* Rs485Handler::_serial    = nullptr;
uint32_t        Rs485Handler::_lastByteMs = 0;
uint8_t         Rs485Handler::_rxBuf[256];
size_t          Rs485Handler::_rxLen     = 0;

static WiFiServer*  _tcpServer  = nullptr;
static WiFiClient   _tcpClients[4];
static const uint8_t MAX_CLIENTS = 4;

// ── HTML вкладки ─────────────────────────────────────────────────────────────
static const char RS485_TAB_HTML[] PROGMEM = R"html(
<div class="r4-page">

  <!-- Статус -->
  <div class="r4-card">
    <div class="r4-label">СТАТУС RS485</div>
    <div class="r4-status-row">
      <span class="r4-led" id="r_led"></span>
      <span id="r_status_txt">—</span>
      <span style="margin-left:auto;color:var(--muted);font-size:12px">TCP: <b id="r_tcp">0</b></span>
    </div>
    <div class="r4-counters">
      <div class="r4-cnt"><div class="r4-cnt-val" id="r_rx">0</div><div class="r4-cnt-lbl">RX байт</div></div>
      <div class="r4-cnt"><div class="r4-cnt-val" id="r_tx">0</div><div class="r4-cnt-lbl">TX байт</div></div>
    </div>
  </div>

  <!-- Консоль -->
  <div class="r4-card">
    <div class="r4-label">КОНСОЛЬ (HEX)</div>
    <div id="r_log" class="r4-log"></div>
    <div style="display:flex;gap:6px;margin-top:8px">
      <input type="text" id="r_cmd" placeholder="01 03 00 08 00 02 44 0A"
             style="flex:1;min-width:0;background:#111318;border:1px solid var(--border);border-radius:6px;padding:7px 10px;color:var(--text);font-family:monospace;font-size:12px"
             onkeydown="if(event.key==='Enter')rSend()">
      <button onclick="rSend()"
              style="width:auto;flex-shrink:0;background:var(--accent);color:#111;border:none;border-radius:6px;padding:7px 14px;font-size:14px;font-weight:700;cursor:pointer;margin-bottom:0">▶</button>
    </div>
    <div style="display:flex;gap:8px;margin-top:6px;align-items:center">
      <button onclick="rClear()" class="r4-btn-sm">🗑 Очистить</button>
      <label style="display:flex;align-items:center;gap:5px;font-size:12px">
        <input type="checkbox" id="r_auto" checked> Авто
      </label>
    </div>
  </div>

  <!-- Генератор Modbus -->
  <div class="r4-card">
    <div class="r4-label">ГЕНЕРАТОР MODBUS RTU</div>
    <div style="display:flex;flex-direction:column;gap:6px">
      <select id="g_fc" onchange="gCalc()" style="width:100%;margin-bottom:0;background:#111318;border:1px solid var(--border);border-radius:6px;padding:6px 8px;color:var(--text);font-size:12px">
        <option value="03">FC03 Read Holding Registers</option>
        <option value="04">FC04 Read Input Registers</option>
        <option value="06">FC06 Write Single Register</option>
        <option value="10">FC16 Write Multiple Registers</option>
      </select>
      <div style="display:flex;gap:6px;align-items:center">
        <span style="font-size:11px;color:var(--muted);white-space:nowrap">ID</span>
        <input type="number" id="g_id" value="1" min="1" max="247" oninput="gCalc()"
               style="width:52px!important;min-width:0;margin-bottom:0;background:#111318;border:1px solid var(--border);border-radius:6px;padding:5px 7px;color:var(--text);font-size:12px">
        <span style="font-size:11px;color:var(--muted);white-space:nowrap">Рег.</span>
        <input type="number" id="g_reg" value="8" min="0" max="65535" oninput="gCalc()"
               style="flex:1;width:0!important;min-width:0;margin-bottom:0;background:#111318;border:1px solid var(--border);border-radius:6px;padding:5px 7px;color:var(--text);font-size:12px">
        <span style="font-size:11px;color:var(--muted);white-space:nowrap">N/Val</span>
        <input type="number" id="g_cnt" value="2" min="1" max="125" oninput="gCalc()"
               style="width:52px!important;min-width:0;margin-bottom:0;background:#111318;border:1px solid var(--border);border-radius:6px;padding:5px 7px;color:var(--text);font-size:12px">
      </div>
    </div>
    <div style="display:flex;align-items:center;gap:8px;margin-top:10px;background:#111318;border:1px solid var(--border);border-radius:6px;padding:8px 10px">
      <span style="color:var(--muted);font-size:11px;white-space:nowrap">HEX:</span>
      <span id="g_hex" style="font-family:monospace;font-size:13px;color:var(--accent);flex:1;word-break:break-all;white-space:normal">—</span>
      <button onclick="gUse()" style="width:auto!important;margin-bottom:0;flex-shrink:0;background:var(--border);color:var(--text);border:none;border-radius:6px;padding:5px 10px;font-size:11px;cursor:pointer;white-space:nowrap">→ В консоль</button>
    </div>
    <div style="font-size:11px;color:var(--muted);margin-top:4px" id="g_desc"></div>
  </div>

  <!-- Настройки -->
  <div class="r4-card">
    <div class="r4-label">НАСТРОЙКИ</div>
    <div class="r4-cfg-grid">
      <label>Скорость (baud)</label>
      <select id="c_baud">
        <option>1200</option><option>2400</option><option>4800</option>
        <option>9600</option><option>19200</option><option>38400</option>
        <option>57600</option><option>115200</option>
      </select>
      <label>Биты данных</label>
      <select id="c_data">
        <option value="8">8</option>
        <option value="7">7</option>
      </select>
      <label>Чётность</label>
      <select id="c_parity">
        <option value="0">None</option>
        <option value="1">Even</option>
        <option value="2">Odd</option>
      </select>
      <label>Стоп-биты</label>
      <select id="c_stop">
        <option value="1">1</option>
        <option value="2">2</option>
      </select>
      <label>DE/RE пин (-1 = нет)</label>
      <input type="number" id="c_de" min="-1" max="39">
      <label>TCP порт моста (0 = выкл)</label>
      <input type="number" id="c_tcp" min="0" max="65535">
      <label>Межфреймовый таймаут (мс)</label>
      <input type="number" id="c_ift" min="1" max="100">
    </div>
    <button onclick="rSaveCfg()" style="margin-top:10px" class="r4-btn-save">💾 Сохранить и перезагрузить</button>
  </div>

</div>

<style>
.r4-page{display:flex;flex-direction:column;gap:10px}
.r4-card{background:var(--card);border-radius:10px;padding:14px;border:1px solid var(--border)}
.r4-label{font-size:10px;font-weight:700;letter-spacing:.12em;text-transform:uppercase;color:var(--muted);margin-bottom:10px}
.r4-status-row{display:flex;align-items:center;gap:8px;font-size:13px}
.r4-led{width:10px;height:10px;border-radius:50%;background:var(--muted);flex-shrink:0}
.r4-led.on{background:var(--ok);box-shadow:0 0 6px var(--ok)}
.r4-led.off{background:var(--danger);box-shadow:0 0 6px var(--danger)}
.r4-counters{display:flex;gap:16px;margin-top:10px}
.r4-cnt{text-align:center}
.r4-cnt-val{font-size:20px;font-weight:800;color:var(--accent)}
.r4-cnt-lbl{font-size:10px;color:var(--muted)}
.r4-log{background:#111318;border:1px solid var(--border);border-radius:6px;padding:8px;
        height:180px;overflow-y:auto;font-family:monospace;font-size:11px;
        color:var(--text);word-break:break-all}
.r4-log .rx{color:var(--cool)}
.r4-log .tx{color:var(--warm)}
.r4-log .ts{color:var(--muted);font-size:10px;margin-right:4px}
.r4-send-row{display:flex;gap:6px;margin-top:8px;align-items:center}
.r4-input{flex:1;width:0;background:#111318;border:1px solid var(--border);border-radius:6px;
          padding:7px 10px;color:var(--text);font-family:monospace;font-size:12px}
.r4-btn-send{background:var(--accent);color:#111;border:none;border-radius:6px;
             padding:7px 16px;font-size:14px;font-weight:700;cursor:pointer;flex-shrink:0;white-space:nowrap}
.r4-btn-sm{background:var(--border);color:var(--text);border:none;border-radius:6px;
           padding:5px 10px;font-size:11px;cursor:pointer}
.r4-btn-save{background:var(--accent);color:#111;border:none;border-radius:6px;
             padding:8px 16px;font-size:13px;font-weight:700;cursor:pointer}
.r4-gen-grid{display:grid;grid-template-columns:1fr 1fr;gap:6px 10px;align-items:center}
.r4-gen-grid label{font-size:12px;color:var(--muted)}
.r4-gen-grid select,.r4-gen-grid input[type=number]{
  background:#111318;border:1px solid var(--border);border-radius:6px;
  padding:6px 8px;color:var(--text);font-size:12px;width:100%}
.r4-gen-result{display:flex;align-items:center;gap:8px;margin-top:10px;
               background:#111318;border:1px solid var(--border);border-radius:6px;padding:8px 10px}
.r4-hex-out{font-family:monospace;font-size:13px;color:var(--accent);flex:1;word-break:break-all}
.r4-cfg-grid{display:grid;grid-template-columns:1fr 1fr;gap:6px 10px;align-items:center}
.r4-cfg-grid label{font-size:12px;color:var(--muted)}
.r4-cfg-grid select,.r4-cfg-grid input[type=number]{
  background:#111318;border:1px solid var(--border);border-radius:6px;
  padding:6px 8px;color:var(--text);font-size:12px;width:100%}
</style>

<script>
// ── Консоль ──────────────────────────────────────────────────────────────────
let _rLog=[], _rLastRx='';
function rTs(){const d=new Date();return('0'+d.getHours()).slice(-2)+':'+('0'+d.getMinutes()).slice(-2)+':'+('0'+d.getSeconds()).slice(-2)}
function rAppend(cls,hex){
  _rLog.push('<div><span class="ts">'+rTs()+'</span><span class="'+cls+'">'+(cls==='rx'?'← ':'→ ')+hex+'</span></div>');
  if(_rLog.length>300)_rLog.shift();
  const el=document.getElementById('r_log');
  el.innerHTML=_rLog.join('');
  el.scrollTop=el.scrollHeight;
}
function rClear(){_rLog=[];document.getElementById('r_log').innerHTML=''}
async function rSend(){
  const hex=document.getElementById('r_cmd').value.trim();
  if(!hex)return;
  const r=await fetch('/api/rs485/send',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({hex})});
  if(r.ok)rAppend('tx',hex);
}
async function rPoll(){
  try{
    const d=await(await fetch('/api/rs485/status')).json();
    const led=document.getElementById('r_led');
    led.className='r4-led '+(d.running?'on':'off');
    document.getElementById('r_status_txt').textContent=d.running?'Работает':'Остановлен';
    document.getElementById('r_tcp').textContent=d.tcp_clients;
    document.getElementById('r_rx').textContent=d.rx_bytes;
    document.getElementById('r_tx').textContent=d.tx_bytes;
    if(d.last_rx_hex&&d.last_rx_hex!==_rLastRx){_rLastRx=d.last_rx_hex;rAppend('rx',d.last_rx_hex)}
  }catch(e){}
}

// ── Генератор CRC16 Modbus ────────────────────────────────────────────────────
function crc16(buf){
  let crc=0xFFFF;
  for(let i=0;i<buf.length;i++){
    crc^=buf[i];
    for(let j=0;j<8;j++) crc=(crc&1)?(crc>>>1)^0xA001:(crc>>>1);
  }
  return crc;
}
function toHex2(n){return('0'+n.toString(16).toUpperCase()).slice(-2)}
function gCalc(){
  const fc=parseInt(document.getElementById('g_fc').value,16);
  const id=parseInt(document.getElementById('g_id').value)||1;
  const reg=parseInt(document.getElementById('g_reg').value)||0;
  const cnt=parseInt(document.getElementById('g_cnt').value)||1;
  let buf=[];
  if(fc===3||fc===4){
    buf=[id,fc,(reg>>8)&0xFF,reg&0xFF,(cnt>>8)&0xFF,cnt&0xFF];
  } else if(fc===6){
    buf=[id,fc,(reg>>8)&0xFF,reg&0xFF,(cnt>>8)&0xFF,cnt&0xFF];
  } else if(fc===0x10){
    const bc=cnt*2;
    buf=[id,0x10,(reg>>8)&0xFF,reg&0xFF,(cnt>>8)&0xFF,cnt&0xFF,bc];
    for(let i=0;i<cnt;i++){buf.push(0x00);buf.push(0x00);}
  }
  const c=crc16(buf);
  buf.push(c&0xFF);buf.push((c>>8)&0xFF);
  const hex=buf.map(toHex2).join(' ');
  document.getElementById('g_hex').textContent=hex;
  const descs={3:'Читать Holding Registers',4:'Читать Input Registers',6:'Записать 1 регистр',0x10:'Записать N регистров'};
  document.getElementById('g_desc').textContent=
    (descs[fc]||'')+' | slave='+id+' | reg='+reg+' (0x'+('000'+reg.toString(16).toUpperCase()).slice(-4)+') | '+(fc<=4?'count=':'value=')+cnt;
}
function gUse(){
  const hex=document.getElementById('g_hex').textContent;
  if(hex!=='—')document.getElementById('r_cmd').value=hex;
}

// ── Конфиг ───────────────────────────────────────────────────────────────────
async function rLoadCfg(){
  const d=await(await fetch('/api/rs485/config')).json();
  document.getElementById('c_baud').value=d.baud;
  document.getElementById('c_data').value=d.data_bits;
  document.getElementById('c_parity').value=d.parity;
  document.getElementById('c_stop').value=d.stop_bits;
  document.getElementById('c_de').value=d.de_pin;
  document.getElementById('c_tcp').value=d.tcp_port;
  document.getElementById('c_ift').value=d.inter_frame;
}
async function rSaveCfg(){
  const data={
    baud:+document.getElementById('c_baud').value,
    data_bits:+document.getElementById('c_data').value,
    parity:+document.getElementById('c_parity').value,
    stop_bits:+document.getElementById('c_stop').value,
    de_pin:+document.getElementById('c_de').value,
    tcp_port:+document.getElementById('c_tcp').value,
    inter_frame:+document.getElementById('c_ift').value,
  };
  const r=await fetch('/api/rs485/save',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)});
  if(r.ok){alert('Сохранено. Устройство перезагрузится.');setTimeout(()=>location.reload(),3000);}
}

// ── Init ─────────────────────────────────────────────────────────────────────
rLoadCfg();gCalc();rPoll();
setInterval(()=>{if(document.getElementById('r_auto').checked)rPoll();},500);
</script>
)html";

// ── Вспомогательные функции ──────────────────────────────────────────────────
String Rs485Handler::_toHex(const uint8_t* data, size_t len){
  String s; s.reserve(len*3);
  for(size_t i=0;i<len;i++){if(i)s+=' ';char b[3];snprintf(b,3,"%02X",data[i]);s+=b;}
  return s;
}
size_t Rs485Handler::_fromHex(const String& hex, uint8_t* buf, size_t maxLen){
  size_t count=0; int i=0;
  while(i<(int)hex.length()&&count<maxLen){
    while(i<(int)hex.length()&&hex[i]==' ')i++;
    if(i+1>=(int)hex.length())break;
    char b[3]={hex[i],hex[i+1],0};
    buf[count++]=(uint8_t)strtol(b,nullptr,16);i+=2;
  }
  return count;
}

// ── UART init ────────────────────────────────────────────────────────────────
void Rs485Handler::_initSerial(){
  uint32_t config=SERIAL_8N1;
  if     (rs485Cfg.parity==1&&rs485Cfg.stop_bits==1)config=SERIAL_8E1;
  else if(rs485Cfg.parity==1&&rs485Cfg.stop_bits==2)config=SERIAL_8E2;
  else if(rs485Cfg.parity==2&&rs485Cfg.stop_bits==1)config=SERIAL_8O1;
  else if(rs485Cfg.parity==2&&rs485Cfg.stop_bits==2)config=SERIAL_8O2;
  else if(rs485Cfg.stop_bits==2)                     config=SERIAL_8N2;

  if(!_serial) _serial=new HardwareSerial(2);
  _serial->begin(rs485Cfg.baud, config, rs485Cfg.rx_pin, rs485Cfg.tx_pin);

  if(rs485Cfg.de_pin>=0){
    pinMode(rs485Cfg.de_pin,OUTPUT);
    digitalWrite(rs485Cfg.de_pin,LOW);
  }
  rs485Status.running=true;
  Serial.printf("[RS485] UART2 %d baud RX=%d TX=%d DE=%d\n",
    rs485Cfg.baud,rs485Cfg.rx_pin,rs485Cfg.tx_pin,rs485Cfg.de_pin);
}

// ── Отправка ─────────────────────────────────────────────────────────────────
void Rs485Handler::sendBytes(const uint8_t* data, size_t len){
  if(!_serial||!rs485Status.running)return;
  if(rs485Cfg.de_pin>=0){digitalWrite(rs485Cfg.de_pin,HIGH);delayMicroseconds(200);}
  _serial->write(data,len);
  _serial->flush();
  if(rs485Cfg.de_pin>=0){delayMicroseconds(200);digitalWrite(rs485Cfg.de_pin,LOW);}
  rs485Status.tx_bytes+=len;
  rs485Status.last_tx_hex=_toHex(data,len);
}
void Rs485Handler::sendHex(const String& hexStr){
  uint8_t buf[256];
  size_t len=_fromHex(hexStr,buf,sizeof(buf));
  if(len>0)sendBytes(buf,len);
}

// ── Чтение ───────────────────────────────────────────────────────────────────
void Rs485Handler::_readSerial(){
  if(!_serial)return;
  uint32_t now=millis();
  while(_serial->available()&&_rxLen<sizeof(_rxBuf)){
    _rxBuf[_rxLen++]=_serial->read();
    _lastByteMs=now;
    rs485Status.last_rx_ms=now;
  }
  if(_rxLen>0&&(now-_lastByteMs)>=rs485Cfg.inter_frame){
    rs485Status.rx_bytes+=_rxLen;
    rs485Status.last_rx_hex=_toHex(_rxBuf,_rxLen);
    _broadcastToTcp(_rxBuf,_rxLen);
    if(onFrame)onFrame(_rxBuf,_rxLen);
    _rxLen=0;
  }
}

// ── TCP мост ─────────────────────────────────────────────────────────────────
void Rs485Handler::_broadcastToTcp(const uint8_t* data, size_t len){
  if(!_tcpServer)return;
  uint8_t count=0;
  for(uint8_t i=0;i<MAX_CLIENTS;i++){
    if(_tcpClients[i]&&_tcpClients[i].connected()){_tcpClients[i].write(data,len);count++;}
  }
  rs485Status.tcp_clients=count;
}
void Rs485Handler::_handleTcpClients(){
  if(!_tcpServer)return;
  if(_tcpServer->hasClient()){
    WiFiClient inc=_tcpServer->accept();
    bool placed=false;
    for(uint8_t i=0;i<MAX_CLIENTS;i++){
      if(!_tcpClients[i]||!_tcpClients[i].connected()){_tcpClients[i]=inc;placed=true;break;}
    }
    if(!placed)inc.stop();
  }
  uint8_t count=0;
  for(uint8_t i=0;i<MAX_CLIENTS;i++){
    if(!_tcpClients[i]||!_tcpClients[i].connected())continue;
    count++;
    while(_tcpClients[i].available()){
      uint8_t buf[128];int n=_tcpClients[i].readBytes(buf,sizeof(buf));
      if(n>0)sendBytes(buf,n);
    }
  }
  rs485Status.tcp_clients=count;
}

// ── INIT ─────────────────────────────────────────────────────────────────────
void Rs485Handler::init(){
  loadConfig();
  if(!rs485Cfg.enabled){Serial.println("[RS485] Disabled");return;}
  _initSerial();
  if(rs485Cfg.tcp_port>0){
    _tcpServer=new WiFiServer(rs485Cfg.tcp_port);
    _tcpServer->begin();
    Serial.printf("[RS485] TCP bridge port %d\n",rs485Cfg.tcp_port);
  }
  WebHandler::registerTab({"rs485","RS485","🔌",RS485_TAB_HTML});
  extern AsyncWebServer server;

  server.on("/api/rs485/status",HTTP_GET,[](AsyncWebServerRequest* req){
    JsonDocument doc;
    doc["running"]=rs485Status.running;doc["rx_bytes"]=rs485Status.rx_bytes;
    doc["tx_bytes"]=rs485Status.tx_bytes;doc["tcp_clients"]=rs485Status.tcp_clients;
    doc["last_rx_hex"]=rs485Status.last_rx_hex;doc["last_tx_hex"]=rs485Status.last_tx_hex;
    String out;serializeJson(doc,out);req->send(200,"application/json",out);
  });
  server.on("/api/rs485/config",HTTP_GET,[](AsyncWebServerRequest* req){
    JsonDocument doc;
    doc["baud"]=rs485Cfg.baud;doc["parity"]=rs485Cfg.parity;
    doc["data_bits"]=rs485Cfg.data_bits;doc["stop_bits"]=rs485Cfg.stop_bits;
    doc["de_pin"]=rs485Cfg.de_pin;doc["tcp_port"]=rs485Cfg.tcp_port;
    doc["inter_frame"]=rs485Cfg.inter_frame;
    doc["rx_pin"]=rs485Cfg.rx_pin;doc["tx_pin"]=rs485Cfg.tx_pin;
    String out;serializeJson(doc,out);req->send(200,"application/json",out);
  });
  auto* hSave=new AsyncCallbackJsonWebHandler("/api/rs485/save",
    [](AsyncWebServerRequest* req,JsonVariant& json){
      JsonObject o=json.as<JsonObject>();
      rs485Cfg.baud       =o["baud"]       |9600;
      rs485Cfg.data_bits  =o["data_bits"]  |8;
      rs485Cfg.parity     =o["parity"]     |0;
      rs485Cfg.stop_bits  =o["stop_bits"]  |1;
      rs485Cfg.de_pin     =o["de_pin"]     |-1;
      rs485Cfg.tcp_port   =o["tcp_port"]   |8485;
      rs485Cfg.inter_frame=o["inter_frame"]|4;
      Rs485Handler::saveConfig();
      req->send(200,"application/json","{\"status\":\"ok\"}");
      delay(500); ESP.restart();
    }
  );
  server.addHandler(hSave);
  auto* hSend=new AsyncCallbackJsonWebHandler("/api/rs485/send",
    [](AsyncWebServerRequest* req,JsonVariant& json){
      String hex=json["hex"]|"";
      if(hex.length()>0)Rs485Handler::sendHex(hex);
      req->send(200,"application/json","{\"status\":\"ok\"}");
    }
  );
  server.addHandler(hSend);
  Serial.println("[RS485] Init OK");
}

// ── LOOP ─────────────────────────────────────────────────────────────────────
void Rs485Handler::loop(){
  if(!rs485Status.running)return;
  _readSerial();
  _handleTcpClients();
}

// ── Конфиг ───────────────────────────────────────────────────────────────────
bool Rs485Handler::loadConfig(){
  JsonDocument doc;
  if(!ConfigManager::loadJson("/rs485.json",doc))return false;
  rs485Cfg.enabled    =doc["enabled"]    |true;
  rs485Cfg.rx_pin     =doc["rx_pin"]     |16;
  rs485Cfg.tx_pin     =doc["tx_pin"]     |13;
  rs485Cfg.de_pin     =doc["de_pin"]     |-1;
  rs485Cfg.baud       =doc["baud"]       |9600;
  rs485Cfg.data_bits  =doc["data_bits"]  |8;
  rs485Cfg.parity     =doc["parity"]     |0;
  rs485Cfg.stop_bits  =doc["stop_bits"]  |1;
  rs485Cfg.tcp_port   =doc["tcp_port"]   |8485;
  rs485Cfg.inter_frame=doc["inter_frame"]|4;
  return true;
}
bool Rs485Handler::saveConfig(){
  JsonDocument doc;
  doc["enabled"]=rs485Cfg.enabled;doc["rx_pin"]=rs485Cfg.rx_pin;doc["tx_pin"]=rs485Cfg.tx_pin;
  doc["de_pin"]=rs485Cfg.de_pin;doc["baud"]=rs485Cfg.baud;doc["data_bits"]=rs485Cfg.data_bits;
  doc["parity"]=rs485Cfg.parity;doc["stop_bits"]=rs485Cfg.stop_bits;
  doc["tcp_port"]=rs485Cfg.tcp_port;doc["inter_frame"]=rs485Cfg.inter_frame;
  return ConfigManager::saveJson("/rs485.json",doc);
}

#endif // MODULE_RS485
```

---

### <a id="📄-src-modules-rs485-rs485handler-h"></a>📄 `src/modules/rs485/Rs485Handler.h`

**File Info:**
- **Size**: 2.33 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/modules/rs485/Rs485Handler.h`
- **Relative Path**: `src/modules/rs485`
- **Created**: 2026-05-21 19:00:33 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-22 14:41:34 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `e3c44d35038d74b0c15a33c14a8886fa`
- **SHA256**: `0b691fa760bad61785a5b6cec91ecaa7ac17c664a7149db26756d71f0c31d81f`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef MODULE_RS485

#include <HardwareSerial.h>
#include "../../core/CoreTypes.h"

// ── Конфиг (сохраняется в /rs485.json) ──────────────────────────────────────
struct Rs485Config {
    bool     enabled     = true;
    int      rx_pin      = 16;
    int      tx_pin      = 13;
    int      de_pin      = -1;     // DE/RE пин MAX485, -1 = не используется
    uint32_t baud        = 9600;
    uint8_t  data_bits   = 8;
    uint8_t  stop_bits   = 1;
    uint8_t  parity      = 0;      // 0=none, 1=even, 2=odd
    uint16_t tcp_port    = 8485;   // TCP-мост, 0 = отключён
    uint16_t inter_frame = 4;      // мс тишины = конец фрейма
};

// ── Состояние ────────────────────────────────────────────────────────────────
struct Rs485Status {
    bool     running     = false;
    uint32_t rx_bytes    = 0;
    uint32_t tx_bytes    = 0;
    uint8_t  tcp_clients = 0;
    uint32_t last_rx_ms  = 0;
    String   last_rx_hex;
    String   last_tx_hex;
};

extern Rs485Config rs485Cfg;
extern Rs485Status rs485Status;

class Rs485Handler {
public:
    static void   init();
    static void   loop();
    static bool   loadConfig();
    static bool   saveConfig();

    // Отправить байты в шину (из консоли или другого модуля)
    static void sendBytes(const uint8_t* data, size_t len);
    static void sendHex(const String& hexStr);  // "01 03 00 00 00 02 C4 0B"

    // Callback при получении фрейма из шины
    static void (*onFrame)(const uint8_t* data, size_t len);

private:
    static void _initSerial();
    static void _handleTcpClients();
    static void _readSerial();
    static void _broadcastToTcp(const uint8_t* data, size_t len);
    static String _toHex(const uint8_t* data, size_t len);
    static size_t _fromHex(const String& hex, uint8_t* buf, size_t maxLen);

    static HardwareSerial* _serial;
    static uint32_t        _lastByteMs;
    static uint8_t         _rxBuf[256];
    static size_t          _rxLen;
};

#endif // MODULE_RS485
```

---

### <a id="📄-src-modules-telegram-tghandler-cpp"></a>📄 `src/modules/telegram/TgHandler.cpp`

**File Info:**
- **Size**: 6.78 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/modules/telegram/TgHandler.cpp`
- **Relative Path**: `src/modules/telegram`
- **Created**: 2026-02-17 12:57:38 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:57:38 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `3f2ee1a84c43aa8eae4532b139f99443`
- **SHA256**: `52e6823235ff0ef263b901b1d9dbd804cf4c77d33620d3ed29024da5db9f3612`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef MODULE_TELEGRAM
#include "TgHandler.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

TgConfig tgCfg;
void (*TgHandler::onCommand)(const String&, const String&) = nullptr;

static WiFiClientSecure _secClient;
static UniversalTelegramBot _bot("", _secClient);
static uint32_t _lastCheck = 0;

static const char TG_TAB_HTML[] PROGMEM = R"html(
<div class="settings-group">
  <label class="toggle-row">
    <span>Включить Telegram</span>
    <input type="checkbox" id="tg_enabled" onchange="tgToggle()">
  </label>
  <div id="tg_fields">
    <input type="text" name="tg_token"  placeholder="Bot Token">
    <input type="text" name="tg_group"  placeholder="Group Chat ID (для алертов)">
    <input type="text" name="tg_admin"  placeholder="Admin User ID (для команд)">
    <button onclick="saveTg()">💾 Сохранить</button>
  </div>
  <div class="info-row" id="tg_status">...</div>
</div>
<script>
async function loadTg() {
  const r = await fetch('/api/tg/config');
  const d = await r.json();
  document.getElementById('tg_enabled').checked = d.enabled;
  ['token','group','admin'].forEach(k => {
    const el = document.querySelector('[name="tg_'+k+'"]');
    if (el) el.value = d[k] || '';
  });
  tgToggle();
}
function tgToggle() {
  document.getElementById('tg_fields').style.display =
    document.getElementById('tg_enabled').checked ? 'block' : 'none';
}
async function saveTg() {
  const data = {
    enabled: document.getElementById('tg_enabled').checked,
    token:   document.querySelector('[name="tg_token"]').value,
    group:   document.querySelector('[name="tg_group"]').value,
    admin:   document.querySelector('[name="tg_admin"]').value,
  };
  const r = await fetch('/api/tg/save', {method:'POST',
    headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
  if (r.ok) alert('Telegram сохранён. Перезагрузите устройство.');
}
loadTg();
</script>
)html";

bool TgHandler::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/tg.json", doc)) return false;
    tgCfg.enabled = doc["enabled"] | false;
    strlcpy(tgCfg.token, doc["token"] | "", sizeof(tgCfg.token));
    strlcpy(tgCfg.group, doc["group"] | "", sizeof(tgCfg.group));
    strlcpy(tgCfg.admin, doc["admin"] | "", sizeof(tgCfg.admin));
    return true;
}

bool TgHandler::saveConfig() {
    JsonDocument doc;
    doc["enabled"] = tgCfg.enabled;
    doc["token"]   = tgCfg.token;
    doc["group"]   = tgCfg.group;
    doc["admin"]   = tgCfg.admin;
    return ConfigManager::saveJson("/tg.json", doc);
}

void TgHandler::init() {
    loadConfig();

    WebHandler::registerTab({"telegram", "Telegram", "🤖", TG_TAB_HTML});

    extern AsyncWebServer server;
    server.on("/api/tg/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["enabled"] = tgCfg.enabled;
        doc["token"]   = tgCfg.token;
        doc["group"]   = tgCfg.group;
        doc["admin"]   = tgCfg.admin;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    auto* h = new AsyncCallbackJsonWebHandler("/api/tg/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject obj = json.as<JsonObject>();
            tgCfg.enabled = obj["enabled"] | false;
            strlcpy(tgCfg.token, obj["token"] | "", sizeof(tgCfg.token));
            strlcpy(tgCfg.group, obj["group"] | "", sizeof(tgCfg.group));
            strlcpy(tgCfg.admin, obj["admin"] | "", sizeof(tgCfg.admin));
            TgHandler::saveConfig();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(h);

    if (!tgCfg.enabled || strlen(tgCfg.token) == 0) {
        Serial.println("[TG] Disabled or no token");
        return;
    }

    _secClient.setInsecure();
    _bot.updateToken(tgCfg.token);
    _secClient.setHandshakeTimeout(10000);
    _bot.getUpdates(_bot.last_message_received + 1);
    Serial.println("[TG] Init OK");
}

void TgHandler::sendMessage(const String& msg, const String& chatId) {
    if (!tgCfg.enabled || strlen(tgCfg.token) == 0) return;
    String target = (chatId == "") ? String(tgCfg.group) : chatId;
    if (target.length() > 0) _bot.sendMessage(target, msg, "");
}

void TgHandler::handleMessages(int count) {
    static long lastId = 0;
    for (int i = 0; i < count; i++) {
        long uid = _bot.messages[i].update_id;
        if (uid <= lastId && lastId != 0) continue;

        String chat = _bot.messages[i].chat_id;
        String text = _bot.messages[i].text;

        // Проверка прав
        if (chat != String(tgCfg.admin)) {
            _bot.sendMessage(chat, "⛔ Доступ запрещён. Ваш ID: " + chat, "");
            lastId = uid;
            continue;
        }

        // Базовые команды платформы
        if (text == "/start" || text == "/help") {
            String help = "📟 *" + String(baseCfg.device_name) + "*\n";
            help += "/status — состояние системы\n";
            help += "/reboot — перезагрузка\n";
            _bot.sendMessage(chat, help, "Markdown");
        } else if (text == "/status") {
            String s = "📟 *" + String(baseCfg.device_name) + "*\n";
            s += "Сеть: " + String(sysState.ethConnected ? "ETH ✅" : sysState.wifiConnected ? "WiFi ✅" : "❌") + "\n";
            s += "MQTT: " + String(sysState.mqttConnected ? "✅" : "❌") + "\n";
            s += "Uptime: " + String(millis() / 1000) + "с";
            _bot.sendMessage(chat, s, "Markdown");
        } else if (text == "/reboot") {
            _bot.sendMessage(chat, "🔄 Перезагружаюсь...", "");
            xSemaphoreTake(coreMutex, portMAX_DELAY);
            sysState.pendingReboot = true;
            sysState.rebootAt = millis();
            xSemaphoreGive(coreMutex);
        } else if (onCommand) {
            // Передаём устройству неизвестные команды
            onCommand(text, chat);
        }

        lastId = uid;
        _bot.last_message_received = uid;
    }
}

void TgHandler::loop() {
    if (!tgCfg.enabled || strlen(tgCfg.token) < 10) return;

    bool hasNet = false;
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    hasNet = sysState.ethConnected || sysState.wifiConnected;
    xSemaphoreGive(coreMutex);
    if (!hasNet) return;

    if (millis() - _lastCheck > 5000) {
        int n = _bot.getUpdates(_bot.last_message_received + 1);
        if (n > 0) handleMessages(n);
        _lastCheck = millis();
        _secClient.stop(); // освобождаем TCP соединение
    }

    xSemaphoreTake(coreMutex, portMAX_DELAY);
    sysState.tgConnected = tgCfg.enabled;
    xSemaphoreGive(coreMutex);
}

#endif // MODULE_TELEGRAM

```

---

### <a id="📄-src-modules-telegram-tghandler-h"></a>📄 `src/modules/telegram/TgHandler.h`

**File Info:**
- **Size**: 860 B
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/modules/telegram/TgHandler.h`
- **Relative Path**: `src/modules/telegram`
- **Created**: 2026-02-17 12:57:12 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-02-17 12:57:12 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `7186520b7162ce044a83c843da6df46e`
- **SHA256**: `83116fe0d909a59f840d2dfdfd64bf12eb61af57dfbc22309bc13b72d31872cb`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef MODULE_TELEGRAM

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "../core/CoreTypes.h"

struct TgConfig {
    char token[64]  = "";
    char group[32]  = "";   // Группа/канал для алертов
    char admin[32]  = "";   // ID администратора (управление командами)
    bool enabled    = false;
};

extern TgConfig tgCfg;

class TgHandler {
public:
    static void init();
    static void loop();
    static void sendMessage(const String& msg, const String& chatId = "");
    static bool loadConfig();
    static bool saveConfig();

    // Устройство может добавить свои команды
    static void (*onCommand)(const String& cmd, const String& chatId);

private:
    static void handleMessages(int count);
};

#endif // MODULE_TELEGRAM

```

---

### <a id="📄-src-modules-time-timemanager-cpp"></a>📄 `src/modules/time/TimeManager.cpp`

**File Info:**
- **Size**: 11.31 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/modules/time/TimeManager.cpp`
- **Relative Path**: `src/modules/time`
- **Created**: 2026-05-24 06:03:22 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-25 21:20:55 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `0b980a7afbd15d383b3f13dbc8ae1eab`
- **SHA256**: `2799d1110cb370c57a9b93d35a6fc540bc3a1bee7dfb5491978a944ab1b18c80`
- **Encoding**: UTF-8

**File code content:**

```cpp
#ifdef MODULE_TIME
#include "TimeManager.h"
#include "../../core/ConfigManager.h"
#include "../../core/WebHandler.h"
#include "../../core/CoreTypes.h"
#ifdef MODULE_MQTT
#include "../mqtt/MqttHandler.h"
#endif
#include <ArduinoJson.h>
#include <AsyncJson.h>

#ifdef TIME_SOURCE_NTP
#include <esp_sntp.h>
#endif

// ── Глобальные данные ────────────────────────────────────────────────────────
TimeConfig timeCfg;


// ── Статика ──────────────────────────────────────────────────────────────────
TimeSource  TimeManager::_source      = TimeSource::NONE;
bool        TimeManager::_synced      = false;
uint32_t    TimeManager::_lastNtpMs   = 0;
uint32_t    TimeManager::_lastMqttMs  = 0;

// ── Основной интерфейс ───────────────────────────────────────────────────────
time_t TimeManager::now() {
    return time(nullptr);
}

bool TimeManager::isSynced() {
    return _synced;
}

TimeSource TimeManager::source() {
    return _source;
}

String TimeManager::sourceStr() {
    switch (_source) {
        case TimeSource::MODEM: return "gsm";
        case TimeSource::MQTT:  return "mqtt";
        case TimeSource::NTP:   return "ntp";
        case TimeSource::RTC:   return "rtc";
        default:                return "none";
    }
}

String TimeManager::formatTime(const char* fmt) {
    if (!_synced) return "--:--:--";
    time_t t = now() + (time_t)timeCfg.tz_offset_min * 60L;  // ← offset вручную
    struct tm* tm = gmtime(&t);  // ← gmtime, не localtime
    char buf[32];
    strftime(buf, sizeof(buf), fmt, tm);
    return String(buf);
}

String TimeManager::formatDateTime(const char* fmt) {
    if (!_synced) return "не синхронизировано";
    time_t t = now();
    struct tm* tm = localtime(&t);
    char buf[32];
    strftime(buf, sizeof(buf), fmt, tm);
    return String(buf);
}

// ── Уведомление от внешнего источника (GSM/RTC) ──────────────────────────────
void TimeManager::notifySynced(TimeSource src) {
    // GSM и RTC имеют приоритет над MQTT и NTP
    if (src == TimeSource::MODEM || src == TimeSource::RTC) {
        _source = src;
        _synced = true;
        Serial.printf("[TIME] Synced from %s: %s\n",
            sourceStr().c_str(), formatDateTime().c_str());
    }
}

// ── MQTT источник ─────────────────────────────────────────────────────────────
void TimeManager::onMqttMessage(const char* topic, const char* payload) {
    if (!timeCfg.mqtt_enabled) return;
    if (strcmp(topic, timeCfg.mqtt_topic) != 0) return;
    // Принимаем unix timestamp (число) или ISO строку "2024-01-15T12:34:56"
    String s = String(payload);
    time_t t = 0;
    if (s.length() == 19 && s[4] == '-') {
        // ISO формат: "2024-01-15T12:34:56"
        struct tm tm = {};
        tm.tm_year = s.substring(0, 4).toInt() - 1900;
        tm.tm_mon  = s.substring(5, 7).toInt() - 1;
        tm.tm_mday = s.substring(8, 10).toInt();
        tm.tm_hour = s.substring(11, 13).toInt();
        tm.tm_min  = s.substring(14, 16).toInt();
        tm.tm_sec  = s.substring(17, 19).toInt();
        t = mktime(&tm);
    } else {
        // Unix timestamp
        t = (time_t)s.toInt();
    }
    if (t < 1700000000UL) return;  // sanity check (после 2023)

    struct timeval tv = { t, 0 };
    settimeofday(&tv, nullptr);
    _lastMqttMs = millis();

    // MQTT источник только если нет GSM/RTC
    if (_source == TimeSource::NONE || _source == TimeSource::NTP
        || _source == TimeSource::MQTT) {
        _source = TimeSource::MQTT;
        _synced = true;
        Serial.printf("[TIME] Synced from MQTT: %s\n", formatDateTime().c_str());
    }
}

// ── NTP ──────────────────────────────────────────────────────────────────────
void TimeManager::_syncNtp() {
#ifdef TIME_SOURCE_NTP
    if (!timeCfg.ntp_enabled) return;
    if (_source == TimeSource::MODEM || _source == TimeSource::RTC) return;

    // Формируем POSIX TZ строку: UTC смещение (знак инвертирован в POSIX!)
    // UTC+3 → "UTC-3", UTC-5 → "UTC+5"
    int h = timeCfg.tz_offset_min / 60;
    int m = abs(timeCfg.tz_offset_min % 60);
    char tz_str[16];
    if (m == 0)
        snprintf(tz_str, sizeof(tz_str), "UTC%+d", -h);
    else
        snprintf(tz_str, sizeof(tz_str), "UTC%+d:%02d", -h, m);
    setenv("TZ", tz_str, 1);
    tzset();

    configTime(0, 0, timeCfg.ntp_server);  // offset=0, timezone через TZ env

    Serial.printf("[TIME] NTP sync: %s TZ=%s\n", timeCfg.ntp_server, tz_str);
    _lastNtpMs = millis();
#endif
}

// ── INIT ─────────────────────────────────────────────────────────────────────
void TimeManager::init() {
    loadConfig();

    applyTimezone();

    extern AsyncWebServer server;

    server.on("/api/time/config", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        doc["tz_offset_min"] = timeCfg.tz_offset_min;
        doc["ntp_server"]    = timeCfg.ntp_server;
        doc["mqtt_topic"]    = timeCfg.mqtt_topic;
        doc["ntp_enabled"]   = timeCfg.ntp_enabled;
        doc["mqtt_enabled"]  = timeCfg.mqtt_enabled;
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest* req) {
        JsonDocument doc;
        time_t t = now();
        struct tm* ltm = localtime(&t);
        struct tm* gtm = gmtime(&t);
        char local_buf[32], gmt_buf[32];
        strftime(local_buf, sizeof(local_buf), "%H:%M:%S", ltm);
        strftime(gmt_buf,   sizeof(gmt_buf),   "%H:%M:%S", gtm);
        doc["synced"]     = _synced;
        doc["unix"]       = (uint32_t)t;
        doc["source"]     = sourceStr();
        doc["time_str"]   = formatTime();
        doc["date_str"]   = formatDateTime();
        String out; serializeJson(doc, out);
        req->send(200, "application/json", out);
    });

    // server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest* req) {
    //     JsonDocument doc;
    //     doc["synced"]   = _synced;
    //     doc["unix"]     = (uint32_t)now();
    //     doc["source"]   = sourceStr();
    //     doc["time_str"] = formatTime();
    //     doc["date_str"] = formatDateTime();
    //     doc["tz_applied"] = f.tz_applied;
    //     String out; serializeJson(doc, out);
    //     req->send(200, "application/json", out);
    // });


    auto* hSave = new AsyncCallbackJsonWebHandler("/api/time/save",
        [](AsyncWebServerRequest* req, JsonVariant& json) {
            JsonObject o = json.as<JsonObject>();
            timeCfg.tz_offset_min = o["tz_offset_min"] | 180;
            strlcpy(timeCfg.ntp_server,  o["ntp_server"]  | "pool.ntp.org", sizeof(timeCfg.ntp_server));
            strlcpy(timeCfg.mqtt_topic,  o["mqtt_topic"]  | "", sizeof(timeCfg.mqtt_topic));
            timeCfg.ntp_enabled  = o["ntp_enabled"]  | true;
            timeCfg.mqtt_enabled = o["mqtt_enabled"] | true;
            TimeManager::saveConfig();
            TimeManager::applyTimezone();
            req->send(200, "application/json", "{\"status\":\"ok\"}");
        }
    );
    server.addHandler(hSave);

    // Запускаем NTP если нет более приоритетного источника
    if (timeCfg.ntp_enabled) _syncNtp();

    // Подписка на MQTT топик времени
#ifdef MODULE_MQTT
    if (timeCfg.mqtt_enabled && strlen(timeCfg.mqtt_topic) > 0)
        MqttHandler::addSubscription(timeCfg.mqtt_topic);
#endif

    Serial.println("[TIME] Init OK");
}

// ── LOOP ─────────────────────────────────────────────────────────────────────
void TimeManager::loop() {
    uint32_t now_ms = millis();

#ifdef TIME_SOURCE_NTP
    // Проверяем успешность NTP синхронизации
    if (timeCfg.ntp_enabled
        && (_source == TimeSource::NONE || _source == TimeSource::NTP)
        && !_synced) {
        time_t t = time(nullptr);
        if (t > 1700000000UL) {
            _source = TimeSource::NTP;
            _synced = true;
            Serial.printf("[TIME] NTP synced: %s\n", formatDateTime().c_str());
        }
    }
    // Периодическая ресинхронизация NTP
    if (timeCfg.ntp_enabled
        && _source == TimeSource::NTP
        && (now_ms - _lastNtpMs) > NTP_INTERVAL_MS) {
        _syncNtp();
    }
#endif

    // Если MQTT источник давно не присылал — сбрасываем приоритет
    if (_source == TimeSource::MQTT
        && (now_ms - _lastMqttMs) > MQTT_TIMEOUT_MS) {
        Serial.println("[TIME] MQTT time source lost");
        _source = TimeSource::NONE;
        _synced = false;
    }
}

// ── Конфиг ───────────────────────────────────────────────────────────────────
bool TimeManager::loadConfig() {
    JsonDocument doc;
    if (!ConfigManager::loadJson("/time.json", doc)) return false;
    timeCfg.tz_offset_min = doc["tz_offset_min"] | 180;
    timeCfg.ntp_enabled   = doc["ntp_enabled"]   | true;
    timeCfg.mqtt_enabled  = doc["mqtt_enabled"]  | true;
    strlcpy(timeCfg.ntp_server, doc["ntp_server"] | "pool.ntp.org", sizeof(timeCfg.ntp_server));
    strlcpy(timeCfg.mqtt_topic, doc["mqtt_topic"] | "", sizeof(timeCfg.mqtt_topic));
    return true;
}

bool TimeManager::saveConfig() {
    JsonDocument doc;
    doc["tz_offset_min"] = timeCfg.tz_offset_min;
    doc["ntp_enabled"]   = timeCfg.ntp_enabled;
    doc["mqtt_enabled"]  = timeCfg.mqtt_enabled;
    doc["ntp_server"]    = timeCfg.ntp_server;
    doc["mqtt_topic"]    = timeCfg.mqtt_topic;
    return ConfigManager::saveJson("/time.json", doc);
}

void TimeManager::applyTimezone() {
    long offset_sec = timeCfg.tz_offset_min * 60L;
    // Сбрасываем TZ env чтобы не мешал
    setenv("TZ", "UTC0", 1);
    tzset();
    configTime(offset_sec, 0, timeCfg.ntp_enabled ? timeCfg.ntp_server : "");
    strlcpy(timeCfg.tz_applied, String(offset_sec).c_str(), sizeof(timeCfg.tz_applied));
    Serial.printf("[TIME] configTime offset=%lds server=%s\n", offset_sec, timeCfg.ntp_server);
}

#endif // MODULE_TIME
```

---

### <a id="📄-src-modules-time-timemanager-h"></a>📄 `src/modules/time/TimeManager.h`

**File Info:**
- **Size**: 1.94 KB
- **Extension**: `.h`
- **Language**: `text`
- **Location**: `src/modules/time/TimeManager.h`
- **Relative Path**: `src/modules/time`
- **Created**: 2026-05-24 06:03:10 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-05-25 18:48:21 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `cd04ae097faba234047508f06469529f`
- **SHA256**: `fdeae49bc240ae0aa28a9d8ade33cca80fd3f18180a558c5f31dc943a01380dc`
- **Encoding**: UTF-8

**File code content:**

```text
#pragma once
#ifdef MODULE_TIME

#include <Arduino.h>
#include <time.h>

enum class TimeSource {
    NONE = 0,
    MODEM,    // GSM AT+CCLK (уже реализовано в ModemHandler)
    MQTT,     // топик с unix timestamp
    NTP,      // configTime
    RTC       // внешний RTC модуль (будущее)
};

struct TimeConfig {
    // NTP
    bool     ntp_enabled  = true;
    char     ntp_server[48] = "pool.ntp.org";
    int16_t  tz_offset_min = 180;   // UTC+3 (Москва)

    char tz_applied[16] = "";
    
    // MQTT источник
    bool     mqtt_enabled = true;
    char     mqtt_topic[64] = "homeassistant/sensor/time/state"; // HA time entity
};

extern TimeConfig timeCfg;

class TimeManager {
public:
    static void init();
    static void loop();
    static bool loadConfig();
    static bool saveConfig();

    // Основной интерфейс
    static time_t     now();
    static bool       isSynced();
    static TimeSource source();
    static String     sourceStr();
    static String     formatTime(const char* fmt = "%H:%M:%S");
    static String     formatDateTime(const char* fmt = "%d.%m.%Y %H:%M:%S");

    // Вызывается из ModemHandler когда GSM синхронизировал время (уже есть settimeofday)
    static void notifySynced(TimeSource src);

    // Вызывается из MqttHandler при получении сообщения
    static void onMqttMessage(const char* topic, const char* payload);

    static void applyTimezone();

private:
    static TimeSource  _source;
    static bool        _synced;
    static uint32_t    _lastNtpMs;
    static uint32_t    _lastMqttMs;
    static const uint32_t NTP_INTERVAL_MS  = 3600000UL;  // раз в час
    static const uint32_t MQTT_TIMEOUT_MS  = 120000UL;   // 2 мин без MQTT = не источник

    static void _syncNtp();
};

#endif // MODULE_TIME
```

---

### <a id="📄-src-main-cpp"></a>📄 `src/main.cpp`

**File Info:**
- **Size**: 14.25 KB
- **Extension**: `.cpp`
- **Language**: `cpp`
- **Location**: `src/main.cpp`
- **Relative Path**: `src`
- **Created**: 2026-02-18 22:12:18 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-06-02 16:08:46 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `53158d1f88e200ac4d8591eb20c94a65`
- **SHA256**: `92d243a81a83774b3280b74eff934766af63ae0a8e13e7656679fc374c116105`
- **Encoding**: ASCII

**File code content:**

```cpp
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "core/CoreTypes.h"
#include "core/ConfigManager.h"
#include "core/NetworkManager.h"
#include "core/WebHandler.h"

#ifdef APP_BOILER_ROOM
#include "apps/boiler-room/BoilerRoomDevice.h"
#endif

#ifdef MODULE_MQTT
#include "modules/mqtt/MqttHandler.h"
#endif
#ifdef MODULE_TELEGRAM
#include "modules/telegram/TgHandler.h"
#endif
#ifdef DEVICE_BOILER
#include "devices/boiler/BoilerDevice.h"
#endif
#ifdef DEVICE_KC868_A16
#include "devices/kc868-a16/A16Device.h"
#endif
#ifdef DEVICE_BOILER_MONITOR
#include "devices/boiler-monitor/BMonDevice.h"
#endif

#if defined(DEVICE_KC868_A16) && defined(APP_BOILER)
#include "apps/boiler/BoilerAppDevice.h"
#endif

#ifdef DEVICE_EKONOM
#include "devices/ekonom/EkoNomDevice.h"
#endif

#ifdef MODULE_MODEM
#include "modules/modem/ModemHandler.h"
#endif

#ifdef MODULE_RS485
#include "modules/rs485/Rs485Handler.h"
#endif

#ifdef MODULE_TIME
#include "modules/time/TimeManager.h"
#endif

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);
    Serial.println("\n[BOOT] esp32-platform starting...");

    // 1. Файловая система и конфиги
    ConfigManager::begin();
    ConfigManager::loadBase();

    // 2. Сеть
    NetworkManager::begin();

    // 3. Регистрация вкладок (ПОРЯДОК = порядок в nav)
    //    Устройство регистрирует свои вкладки первыми (Мониторинг, Уставки)
    //    Затем модули добавляют свои (MQTT, Telegram)
    //    Последней — вкладка Настройки (core)

    #ifdef MODULE_TIME
    TimeManager::init();
    #endif

    #ifdef DEVICE_BOILER_MONITOR
    BMonDevice::init();
    #endif

    #ifdef DEVICE_BOILER
    BoilerDevice::init();
    #endif

    #ifdef DEVICE_KC868_A16
    A16Device::init();
    #endif

    #if defined(DEVICE_KC868_A16) && defined(APP_BOILER)
    BoilerAppDevice::init();
    #endif

    #ifdef MODULE_MQTT
    MqttHandler::init();
    #endif

    #ifdef MODULE_TELEGRAM
    TgHandler::init();
    #endif

    #ifdef MODULE_MODEM
    ModemHandler::init();
    #endif

    #ifdef MODULE_RS485
        Rs485Handler::init();
    #endif

    #ifdef DEVICE_EKONOM
        EkoNomDevice::init(30);   // опрос каждые 30 сек
    #endif

    #if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)
      BoilerRoomDevice::init();
    #endif

    // Вкладка настроек сети — всегда последняя

    static const char SETTINGS_HTML[] = R"html(
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
      <label style="margin-top:8px">WiFi SSID</label>
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
      <label>MQTT топик времени</label>
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
        // Сортируем по уровню сигнала
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

    // Время
    fetch('/api/time/config').then(r=>r.json()).then(d=>{
      document.querySelector('[name="tz_offset_min"]').value = d.tz_offset_min || 180;
      document.querySelector('[name="ntp_server"]').value    = d.ntp_server || 'pool.ntp.org';
      document.querySelector('[name="mqtt_time_topic"]').value = d.mqtt_topic || '';
      document.getElementById('ntp_enabled').checked  = d.ntp_enabled;
      document.getElementById('mqtt_time_enabled').checked = d.mqtt_enabled;
    });
    // Статус времени
    fetch('/api/time').then(r=>r.json()).then(d=>{
      const srcs={ntp:'🛰 NTP',mqtt:'🏠 MQTT',gsm:'📡 GSM',rtc:'⏱ RTC',none:''};
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
      const fields = ['eth_ip','eth_mask','eth_gw','eth_dns',
                      'wifi_ssid','wifi_pass','device_name','web_user','web_pass'];
      fields.forEach(k => {
        const el = document.querySelector('[name="'+k+'"]');
        if (el && el.value.trim() !== '') data[k] = el.value.trim();
      });
      const r = await fetch('/api/save-config', {method:'POST',
        headers:{'Content-Type':'application/json'}, body:JSON.stringify(data)});
      if (r.ok) alert('Настройки сохранены');
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

    WebHandler::registerTab({"settings", "Настройки", "⚙️", SETTINGS_HTML});

    // 4. Запуск веб-сервера
    WebHandler::init(server);
    server.begin();

    // 5. OTA ArduinoOTA
    #ifdef MODULE_OTA
    ArduinoOTA.setHostname(baseCfg.device_name);
    ArduinoOTA.onStart([]() { Serial.println("[OTA] Start"); });
    ArduinoOTA.onError([](ota_error_t e) { Serial.printf("[OTA] Error: %u\n", e); });
    ArduinoOTA.begin();
    #endif

    // 6. mDNS: http://<device_name>.local
    MDNS.begin(baseCfg.device_name);
    Serial.printf("[BOOT] Ready at http://%s.local\n", baseCfg.device_name);

    #ifdef MODULE_TELEGRAM
    TgHandler::sendMessage("🟢 " + String(baseCfg.device_name) + " запущен");
    #endif
}

void loop() {
    #ifdef MODULE_OTA
    ArduinoOTA.handle();
    #endif

    NetworkManager::loop();

    #ifdef MODULE_MQTT
    MqttHandler::loop();
    #endif

    #ifdef MODULE_TELEGRAM
    TgHandler::loop();
    #endif

    #ifdef DEVICE_BOILER
    BoilerDevice::loop();
    #endif

    #ifdef DEVICE_KC868_A16
    A16Device::loop();
    #endif

    #ifdef DEVICE_BOILER_MONITOR
    BMonDevice::loop();
    #endif

    #if defined(DEVICE_KC868_A16) && defined(APP_BOILER)
    BoilerAppDevice::loop();
    #endif

    #ifdef MODULE_MODEM
    ModemHandler::loop();
    #endif

    #ifdef MODULE_RS485
        Rs485Handler::loop();
    #endif

    #ifdef DEVICE_EKONOM
        EkoNomDevice::loop();
    #endif

    #ifdef MODULE_TIME
        TimeManager::loop();
    #endif

    #if defined(DEVICE_KC868_A16) && defined(APP_BOILER_ROOM)
    BoilerRoomDevice::loop();
    #endif

    // Перезагрузка по запросу из веб-интерфейса
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (millis() - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}

```

---

### <a id="📄-platformio-ini"></a>📄 `platformio.ini`

**File Info:**
- **Size**: 4.83 KB
- **Extension**: `.ini`
- **Language**: `text`
- **Location**: `platformio.ini`
- **Relative Path**: `root`
- **Created**: 2026-02-18 04:58:44 (Asia/Novosibirsk / GMT+07:00)
- **Modified**: 2026-06-04 10:56:42 (Asia/Novosibirsk / GMT+07:00)
- **MD5**: `7bfcc428951abf6e178bf29408b5ced2`
- **SHA256**: `1f27ab0ae6839d9beb6ad89e831b026632b09871fe4c3e9d75d9d1c77fd937ce`
- **Encoding**: UTF-8

**File code content:**

```text
[env:wt32-eth01]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_speed = 921600
upload_protocol = espota
upload_port = 192.168.88.23

board_build.partitions = min_spiffs.csv
board_build.filesystem = littlefs

lib_deps =
    ottowinter/ESPAsyncWebServer-esphome @ ^3.1.0
    esphome/AsyncTCP-esphome @ ^2.1.1
    bblanchon/ArduinoJson @ ^7.0.0
    knolleary/PubSubClient @ ^2.8
    witnessmenow/UniversalTelegramBot @ ^1.3.0
    paulstoffregen/OneWire @ ^2.3.8
    milesburton/DallasTemperature @ ^3.11.0

build_flags =
    -D CORE_DEBUG_LEVEL=3
    ; ---------- Модули (закомментируй чтобы отключить) ----------
    -D MODULE_TELEGRAM
    -D MODULE_MQTT
    -D MODULE_OTA
    -D MODULE_CAPTIVE_PORTAL
    -D MODULE_RS485
    -D MODULE_TIME
    -D TIME_SOURCE_NTP 
    ; ---------- Устройство (только одно!) ----------------------
    ; -D DEVICE_BOILER
    -D DEVICE_KC868_A16
    -D DEVICE_EKONOM
    ; ---------- Железо -----------------------------------------
    -D ETH_ADDR=1
    -D ETH_POWER_PIN=16
    -D ETH_MDC_PIN=23
    -D ETH_MDIO_PIN=18
    ; ---------- Приложения (поверх device) --------------------
    -D APP_BOILER
    -D APP_EKONOM
    ; ---------- Include paths ----------------------------------
    -I src/core
    -I src/modules/mqtt
    -I src/modules/telegram
    -I src/devices/boiler
    -I src/devices/kc868-a16
    -I src/apps/boiler
    -I src/modules/rs485

[env:modem-device]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_speed = 115200
upload_protocol = espota
upload_port = 192.168.1.89

board_build.partitions = min_spiffs.csv
board_build.filesystem = littlefs

lib_deps =
    ottowinter/ESPAsyncWebServer-esphome @ ^3.1.0
    esphome/AsyncTCP-esphome @ ^2.1.1
    bblanchon/ArduinoJson @ ^7.0.0
    knolleary/PubSubClient @ ^2.8
    witnessmenow/UniversalTelegramBot @ ^1.3.0

build_flags =
    -D CORE_DEBUG_LEVEL=3
    ; ---------- Модули ------------------------------------------
    -D MODULE_TELEGRAM
    -D MODULE_MQTT
    -D MODULE_OTA
    -D MODULE_CAPTIVE_PORTAL
    -D MODULE_MODEM
    ; ---------- Железо -----------------------------------------
    -D ETH_ADDR=1
    -D ETH_POWER_PIN=16
    -D ETH_MDC_PIN=23
    -D ETH_MDIO_PIN=18
    ; ---------- Include paths ----------------------------------
    -I src/core
    -I src/modules/mqtt
    -I src/modules/telegram
    -I src/modules/modem

[env:boiler-monitor]
platform = espressif32@6.9.0
board = wt32-eth01
framework = arduino
monitor_speed = 115200
upload_speed = 115200
upload_protocol = espota
upload_port = 192.168.1.127

board_build.partitions = min_spiffs.csv
board_build.filesystem = littlefs

lib_deps =
    ottowinter/ESPAsyncWebServer-esphome @ ^3.1.0
    esphome/AsyncTCP-esphome @ ^2.1.1
    bblanchon/ArduinoJson @ ^7.0.0
    knolleary/PubSubClient @ ^2.8
    witnessmenow/UniversalTelegramBot @ ^1.3.0
    adafruit/Adafruit ADS1X15 @ ^2.5.0
    adafruit/Adafruit BusIO @ ^1.16.1

build_flags =
    -D CORE_DEBUG_LEVEL=3
    ; ---------- Модули ------------------------------------------
    -D MODULE_TELEGRAM
    -D MODULE_MQTT
    -D MODULE_OTA
    ; ---------- Устройство --------------------------------------
    -D DEVICE_BOILER_MONITOR
    ; ---------- Include paths -----------------------------------
    -I src/core
    -I src/modules/mqtt
    -I src/modules/telegram
    -I src/devices/boiler-monitor

[env:boiler-room]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_speed = 115200
upload_protocol = espota
upload_port = 192.168.88.23

board_build.partitions = min_spiffs.csv
board_build.filesystem = littlefs

lib_deps =
    ottowinter/ESPAsyncWebServer-esphome @ ^3.1.0
    esphome/AsyncTCP-esphome @ ^2.1.1
    bblanchon/ArduinoJson @ ^7.0.0
    knolleary/PubSubClient @ ^2.8
    witnessmenow/UniversalTelegramBot @ ^1.3.0
    paulstoffregen/OneWire @ ^2.3.8
    milesburton/DallasTemperature @ ^3.11.0

build_flags =
    -D CORE_DEBUG_LEVEL=3
    ; ---------- Модули ------------------------------------------
    -D MODULE_MQTT
    -D MODULE_OTA
    -D MODULE_RS485
    -D MODULE_TIME
    -D TIME_SOURCE_NTP
    ; ---------- Устройство --------------------------------------
    -D DEVICE_KC868_A16
    -D DEVICE_EKONOM
    ; ---------- Приложение --------------------------------------
    -D APP_BOILER_ROOM
    ; ---------- Железо (KC868-A16) ------------------------------
    -D ETH_ADDR=0
    -D ETH_POWER_PIN=-1
    -D ETH_MDC_PIN=23
    -D ETH_MDIO_PIN=18
    -D ETH_CLK_MODE=ETH_CLOCK_GPIO17_OUT
    ; ---------- Include paths -----------------------------------
    -I src/core
    -I src/modules/mqtt
    -I src/modules/rs485
    -I src/modules/time
    -I src/devices/kc868-a16
    -I src/devices/ekonom
    -I src/apps/boiler-room
```

---

## 🚫 Binary/Excluded Files

The following files were not included in the text content:

- `sdkconfig.defaults`

