// ===================== 引脚与参数定义 =====================
#define TOUCH_PIN 4        // T0触摸引脚GPIO4
#define LED_PIN 2          // ESP32板载LED引脚
#define THRESHOLD 400       // 触摸触发阈值，可串口观测调整

// PWM基础配置（实验3呼吸灯参数）
const int freq = 5000;
const int resolution = 8;

// 档位全局变量：1=慢速 2=中速 3=快速
uint8_t gear = 1;
bool touchFlag = false;   // 中断触发标记

// ===================== 触摸中断服务函数 =====================
void touchISR() {
  touchFlag = true;
}

// ===================== 初始化 =====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);

  // 绑定PWM（实验3新版引脚直写）
  ledcAttach(LED_PIN, freq, resolution);

  // 绑定触摸中断（实验4中断模式）
  touchAttachInterrupt(TOUCH_PIN, touchISR, THRESHOLD);

  Serial.println("ex05 多档位触摸呼吸灯启动");
  Serial.println("档位1：慢速 | 档位2：中速 | 档位3：快速");
}

// ===================== 主循环呼吸逻辑 =====================
void loop() {
  // 检测触摸中断，切换档位循环
  if (touchFlag) {
    touchFlag = false;
    gear += 1;
    if (gear > 3) gear = 1; // 3档循环 1→2→3→1
    Serial.print("当前切换至档位：");
    Serial.println(gear);
  }

  // 根据档位设置呼吸延时，延时越小呼吸越快
  int breathDelay;
  switch (gear) {
    case 1: breathDelay = 30; break; // 慢速
    case 2: breathDelay = 15; break; // 中速
    case 3: breathDelay = 5;  break; // 快速
  }

  // 呼吸渐亮
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED_PIN, duty);
    delay(breathDelay);
  }
  // 呼吸渐暗
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(LED_PIN, duty);
    delay(breathDelay);
  }
}