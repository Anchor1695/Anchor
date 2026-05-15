// SOS 信号：用 millis() 实现非阻塞时序
// 短闪: 200ms 亮, 200ms 灭
// 长闪: 600ms 亮, 200ms 灭
// 字母间隔: 400ms 灭
// 单词间隔（一轮SOS结束）: 800ms 灭

const int ledPin = 2;        // LED引脚（ESP32板载LED通常为GPIO2）
unsigned long previousMillis = 0;
int state = 0;               // 状态机当前步骤
int stepIndex = 0;           // 当前在SOS序列中的位置
unsigned long stepDuration = 0;

// SOS 序列: 每个元素代表一个步骤的持续时间（ms），正数表示亮，负数表示灭
// 序列: 短闪(亮200), 灭200, 短闪(亮200), 灭200, 短闪(亮200), 灭400(字母间隔),
//       长闪(亮600), 灭200, 长闪(亮600), 灭200, 长闪(亮600), 灭400(字母间隔),
//       短闪(亮200), 灭200, 短闪(亮200), 灭200, 短闪(亮200), 灭800(单词间隔)
const int sosSequence[] = {
  200, -200,  200, -200,  200, -400,
  600, -200,  600, -200,  600, -400,
  200, -200,  200, -200,  200, -800
};
const int seqLen = sizeof(sosSequence) / sizeof(sosSequence[0]);

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  previousMillis = millis();
  stepIndex = 0;
  digitalWrite(ledPin, LOW);
  // 开始第一个步骤
  stepDuration = abs(sosSequence[0]);
  if (sosSequence[0] > 0) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Step: ON");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("Step: OFF");
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= stepDuration) {
    previousMillis = currentMillis;
    // 移动到下一步
    stepIndex++;
    if (stepIndex >= seqLen) {
      stepIndex = 0;  // 循环播放SOS
    }
    // 更新步骤持续时间
    stepDuration = abs(sosSequence[stepIndex]);
    if (sosSequence[stepIndex] > 0) {
      digitalWrite(ledPin, HIGH);
      Serial.println("ON");
    } else {
      digitalWrite(ledPin, LOW);
      Serial.println("OFF");
    }
  }
}