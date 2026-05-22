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