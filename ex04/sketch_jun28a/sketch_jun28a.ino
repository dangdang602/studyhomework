/*
ex04 触摸自锁LED开关
关联实验：基础IO点灯、触摸引脚读取
功能：单次触摸翻转LED，防抖+上升沿边缘检测，自锁保持状态
*/
#define TOUCH_PIN 4    // T0对应GPIO4触摸引脚
#define LED_PIN 2      // ESP32板载LED引脚
#define THRESHOLD 400   // 触摸阈值，可根据串口读数微调

// 作业要求：布尔型状态变量存储LED状态
bool ledState = false;
// 用于边缘检测，记录上一次触摸状态
bool lastTouchFlag = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // 上电默认熄灭
}

void loop() {
  int touchValue = touchRead(TOUCH_PIN);
  bool nowTouch = (touchValue < THRESHOLD);

  // 边缘检测：上一时刻未触摸，当前刚触摸（触发瞬间）
  if (nowTouch == true && lastTouchFlag == false)
  {
    delay(50); // 软件防抖延时，过滤手抖杂波
    touchValue = touchRead(TOUCH_PIN);
    if (touchValue < THRESHOLD)
    {
      ledState = !ledState;       // 翻转LED状态
      digitalWrite(LED_PIN, ledState);
      Serial.print("触摸触发，LED状态：");
      Serial.println(ledState ? "点亮" : "熄灭");
    }
  }

  lastTouchFlag = nowTouch; // 更新历史触摸状态
  delay(20);
}