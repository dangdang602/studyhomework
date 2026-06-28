#include <WiFi.h>
#include <WebServer.h>

// AP热点配置
const char* ap_ssid = "ESP32-032";
const char* ap_pass = "12345678";

// 硬件定义
#define TOUCH_PIN 4    // T0触摸GPIO4
#define TOUCH_THRESHOLD 20

WebServer server(80);

// 生成仪表盘网页
String getDashboardHtml() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ex09 实时触摸传感器仪表盘</title>
    <style>
        body {
            font-family: Arial;
            text-align: center;
            margin-top: 80px;
            background-color: #f5f7fa;
        }
        h1 { color: #2c3e50; }
        .data-box {
            font-size: 72px;
            font-weight: bold;
            color: #e74c3c;
            margin: 40px 0;
        }
        .tip {
            font-size: 18px;
            color: #666;
        }
    </style>
</head>
<body>
    <h1>触摸传感器实时数值仪表盘</h1>
    <div class="data-box" id="touchValue">--</div>
    <p class="tip">手指靠近GPIO4 → 数值变小 | 松开手指 → 数值恢复变大</p>

<script>
    // 循环轮询接口，实时拉取传感器数据（AJAX）
    function updateData() {
        fetch("/data")
            .then(res => res.text())
            .then(val => {
                document.getElementById("touchValue").innerText = val;
            });
    }
    // 每100ms刷新一次数据，实时流畅
    setInterval(updateData, 100);
</script>
</body>
</html>
)rawliteral";
  return html;
}

// 主页：返回仪表盘页面
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", getDashboardHtml());
}

// 数据接口：仅返回触摸原始数值，供网页AJAX拉取
void handleGetData() {
  int touchRaw = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(touchRaw));
}

void setup() {
  Serial.begin(115200);

  // 开启AP热点
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("===== EX09 传感器仪表盘启动 =====");
  Serial.print("热点名称："); Serial.println(ap_ssid);
  Serial.print("网页地址：http://"); Serial.println(WiFi.softAPIP()); // 192.168.4.1

  // 绑定两个路由
  server.on("/", handleRoot);
  server.on("/data", handleGetData);
  server.begin();
}

void loop() {
  server.handleClient(); // 持续处理网页请求
}