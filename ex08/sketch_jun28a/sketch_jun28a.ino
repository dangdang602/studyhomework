#include <WiFi.h>
#include <WebServer.h>

// ========== AP热点配置 ==========
const char* ap_ssid = "ESP32-032";
const char* ap_pass = "12345678";

// ========== 硬件引脚定义 ==========
#define TOUCH_PIN 4    // 触摸T0 GPIO4
#define LED_PIN 2      // 报警LED GPIO2
#define TOUCH_THRESHOLD 400

// ========== 全局状态变量 ==========
bool isArm = false;    // false=撤防，true=布防
bool isAlarm = false;  // 报警锁定标记

WebServer server(80);

// 生成网页HTML
String getHtmlPage() {
  String armState = isArm ? "已布防" : "已撤防";
  String alarmState = isAlarm ? "⚠️ 警报触发中" : "正常无警报";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ex08 物联网安防报警器</title>
<style>
body {font-family:Arial; text-align:center; margin-top:60px;}
.btn {padding:15px 30px; font-size:18px; margin:10px; cursor:pointer;}
.arm {background:#2ecc71; color:white; border:none; border-radius:6px;}
.disarm {background:#e74c3c; color:white; border:none; border-radius:6px;}
.info {font-size:22px; margin:20px 0; font-weight:bold;}
</style>
</head>
<body>
<h1>ESP32 安防报警主机</h1>
<p class="info">系统模式：)rawliteral" + armState + R"rawliteral(</p>
<p class="info">设备状态：)rawliteral" + alarmState + R"rawliteral(</p>

<button class="btn arm" onclick="armSys()">布防 Arm</button>
<button class="btn disarm" onclick="disarmSys()">撤防 Disarm</button>

<script>
function armSys(){
  fetch("/arm");
  location.reload();
}
function disarmSys(){
  fetch("/disarm");
  location.reload();
}
</script>
</body>
</html>
)rawliteral";
  return html;
}

// 主页路由
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", getHtmlPage());
}

// 布防接口
void handleArm() {
  isArm = true;
  server.send(204);
}

// 撤防接口：同时关闭报警
void handleDisarm() {
  isArm = false;
  isAlarm = false;
  digitalWrite(LED_PIN, LOW);
  server.send(204);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 开启AP热点
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("===== EX08安防主机启动 =====");
  Serial.print("热点名称："); Serial.println(ap_ssid);
  Serial.print("访问地址：http://"); Serial.println(WiFi.softAPIP()); //192.168.4.1

  // 绑定网页路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient(); // 持续处理网页请求

  // 读取触摸值
  int touchVal = touchRead(TOUCH_PIN);

  // 核心安防逻辑：仅布防状态下触摸才触发锁定报警
  if(isArm && !isAlarm && touchVal < TOUCH_THRESHOLD){
    isAlarm = true;
  }

  // 报警锁定：持续高频闪烁，直到网页撤防才停止
  if(isAlarm){
    digitalWrite(LED_PIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);
    delay(80);
  }
}