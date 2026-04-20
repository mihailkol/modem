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

#ifdef MODULE_MODEM
#include "modules/modem/ModemHandler.h"
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

    // Вкладка настроек сети — всегда последняя
    static const char SETTINGS_HTML[] = R"html(
<div class="settings-group">
  <h3>🌐 Сеть</h3>
  <label><input type="checkbox" id="eth_dhcp" onchange="toggleEth()"> DHCP (Ethernet)</label>
  <div id="eth_static" style="display:none">
    <input type="text" name="eth_ip"   placeholder="IP: 192.168.1.100">
    <input type="text" name="eth_mask" placeholder="Маска: 255.255.255.0">
    <input type="text" name="eth_gw"   placeholder="Шлюз: 192.168.1.1">
    <input type="text" name="eth_dns"  placeholder="DNS: 8.8.8.8">
  </div>
  <input type="text"     name="wifi_ssid"   placeholder="WiFi SSID">
  <input type="password" name="wifi_pass"   placeholder="WiFi Password">
  <input type="text"     name="device_name" placeholder="Имя устройства">
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
async function saveSettings() {
  // Берём текущий конфиг как базу, поверх него — только непустые поля
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
}
