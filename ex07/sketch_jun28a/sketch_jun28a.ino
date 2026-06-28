#include <WiFi.h>
#include <WebServer.h>

// ===================== AP热点配置 =====================
const char* ap_ssid = "ESP32-032"; // 手机搜到的热点名
const char* ap_pass = "12345678";          // 热点密码，必须≥8位

// ===================== PWM LED配置（实验3接口） =====================
const int LED_PIN = 2;
const int PWM_FREQ = 5000;
const int PWM_RES  = 8;
int brightness = 0; // 存储当前亮度 0~255

WebServer server(80);

// 网页页面：内置range滑块+JS fetch实时发送亮度
String getHtmlPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ex07 网页无极调光</title>
  <style>
    body {font-family: Arial; text-align: center; margin-top: 60px;}
    .slider {width: 85%; height: 22px; margin: 35px 0;}
    #num {font-size: 26px; font-weight: bold; color: #333;}
  </style>
</head>
<body>
  <h1>ESP32 AP热点调光器</h1>
  <p>拖动滑块控制LED亮度（范围0~255）</p>
  <!-- 题目要求滑动条 -->
  <input type="range" class="slider" id="slide" min="0" max="255" value=")rawliteral"
  + String(brightness) + R"rawliteral(">
  <p>当前亮度数值：<span id="num">)rawliteral" + String(brightness) + R"rawliteral(</span></p>

<script>
  const slider = document.getElementById("slide");
  const showText = document.getElementById("num");
  // 监听滑块拖动事件
  slider.addEventListener("input", function(){
    let val = this.value;
    showText.innerText = val;
    // fetch异步发送亮度到ESP32
    fetch("/set?bri=" + val);
  })
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

// 接收滑块亮度参数
void handleSetBright() {
  if(server.hasArg("bri")){
    brightness = server.arg("bri").toInt();
    // 限制区间防止溢出
    brightness = constrain(brightness, 0, 255);
    ledcWrite(LED_PIN, brightness);
  }
  server.send(204); // 空响应，不刷新页面
}

void setup() {
  Serial.begin(115200);
  // 初始化PWM通道（实验3同款）
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RES);
  ledcWrite(LED_PIN, 0);

  // 开启AP热点模式
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);

  Serial.println("======== AP热点启动成功 ========");
  Serial.print("热点名称：");
  Serial.println(ap_ssid);
  Serial.print("热点密码：");
  Serial.println(ap_pass);
  Serial.print("网页访问地址：http://");
  Serial.println(WiFi.softAPIP()); // 固定地址：192.168.4.1

  // 绑定网页接口
  server.on("/", handleRoot);
  server.on("/set", handleSetBright);
  server.begin();
}

void loop() {
  server.handleClient(); // 持续处理网页请求
}