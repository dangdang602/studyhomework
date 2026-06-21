// ex02 millis实现1Hz LED闪烁，不使用delay阻塞
const int ledPin = 2;
unsigned long prevTime = 0;
const unsigned long interval = 500; // 500ms翻转一次，完整周期1000ms=1Hz
bool ledState = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  unsigned long currTime = millis();
  if (currTime - prevTime >= interval) {
    prevTime = currTime;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    Serial.print("LED状态：");
    Serial.println(ledState);
  }
}