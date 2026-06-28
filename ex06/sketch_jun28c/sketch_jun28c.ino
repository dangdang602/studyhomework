// 两路LED引脚定义
const int ledA = 2;
const int ledB = 4;

// PWM统一参数（和实验3保持一致）
const int pwmFreq = 5000;
const int pwmRes = 8;  // 0~255

void setup() {
  Serial.begin(115200);
  // 分别给两个引脚绑定独立PWM通道
  ledcAttach(ledA, pwmFreq, pwmRes);
  ledcAttach(ledB, pwmFreq, pwmRes);
  Serial.println("ex06 双通道反相渐变警灯启动");
}

void loop() {
  // 阶段1：A渐亮(0→255)，B同步渐暗(255→0)
  for(int dutyA = 0; dutyA <= 255; dutyA++){
    int dutyB = 255 - dutyA;  // 反相关系
    ledcWrite(ledA, dutyA);
    ledcWrite(ledB, dutyB);
    delay(8);
  }

  // 阶段2：A渐暗(255→0)，B同步渐亮(0→255)
  for(int dutyA = 255; dutyA >= 0; dutyA--){
    int dutyB = 255 - dutyA;
    ledcWrite(ledA, dutyA);
    ledcWrite(ledB, dutyB);
    delay(8);
  }
}