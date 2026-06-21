/*
 * ESP32 SOS 信号灯 - 非阻塞 millis() 实现
 * 适用于 Arduino IDE
 * 短闪 = 200ms，长闪 = 600ms
 * SOS = 三短 · 三长 · 三短，每次结束停顿 2 秒
 */

const int ledPin = 2;   // 板载 LED 通常接 GPIO2，可自行修改

// 时间常量（单位：毫秒）
const unsigned long SHORT_FLASH = 200;
const unsigned long LONG_FLASH  = 600;
const unsigned short GAP_SHORT  = 200;   // 短闪之间的间隔
const unsigned short GAP_LONG   = 600;   // 字母之间的间隔（S与O，O与S）
const unsigned long PAUSE_END   = 2000;  // 一轮结束后长熄灭

// SOS 闪灯类型序列：0=短闪，1=长闪
const bool flashType[9] = {
  0, 0, 0,   // S
  1, 1, 1,   // O
  0, 0, 0    // S
};

// 状态机
enum LedState { LED_ON, LED_OFF };
LedState state = LED_OFF;
int step = 0;                    // 当前闪灯序号 (0~8)
unsigned long lastTime = 0;      // 上次切换时刻

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);     // 初始熄灭
  // 让第一次循环立即点亮第一个短闪
  lastTime = millis();
  digitalWrite(ledPin, HIGH);
  state = LED_ON;
}

void loop() {
  unsigned long now = millis();

  if (state == LED_ON) {
    // 当前灯亮，计算应该持续的时间
    unsigned long duration = flashType[step] ? LONG_FLASH : SHORT_FLASH;
    if (now - lastTime >= duration) {
      digitalWrite(ledPin, LOW);     // 熄灭
      lastTime = now;
      state = LED_OFF;
    }
  } else { // LED_OFF
    // 计算本次熄灭应持续多久（间隔）
    unsigned long gap;
    if (step == 8) {                 // 最后一个闪灯结束 → 最终长停顿
      gap = PAUSE_END;
    } else if (step == 2 || step == 5) { // 字母间停顿（第3个闪灯后、第6个闪灯后）
      gap = GAP_LONG;
    } else {
      gap = GAP_SHORT;               // 字母内部短停顿
    }

    if (now - lastTime >= gap) {
      // 进入下一个闪灯
      step++;
      if (step >= 9) {
        step = 0;                    // 新一轮循环
      }
      // 点亮 LED，开始下一次闪灯
      digitalWrite(ledPin, HIGH);
      lastTime = now;
      state = LED_ON;
    }
  }
}