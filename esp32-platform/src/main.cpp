#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "core/CoreTypes.h"
#include "core/ConfigManager.h"
#include "core/NetworkManager.h"
#include "core/WebHandler.h"

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

    // Перезагрузка по запросу из веб-интерфейса
    xSemaphoreTake(coreMutex, portMAX_DELAY);
    bool reboot = sysState.pendingReboot && (millis() - sysState.rebootAt > 500);
    xSemaphoreGive(coreMutex);
    if (reboot) ESP.restart();
}
