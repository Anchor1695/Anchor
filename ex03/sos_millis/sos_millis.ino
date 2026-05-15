// SOS 信号：用 millis() 实现非阻塞时序
// 短闪: 200ms 亮, 200ms 灭
// 长闪: 600ms 亮, 200ms 灭
// 字母间隔: 400ms 灭
// 单词间隔（一轮SOS结束）: 800ms 灭

const int ledPin = 2;        // LED引脚（ESP32板载LED通常为GPIO2）
unsigned long previousMillis = 0;
int stepIndex = 0;           // 当前步骤索引
unsigned long stepDuration = 0;

// SOS 序列: 正数=亮，负数=灭（绝对值是持续时间）
const int sosSequence[] = {
  200, -200,  200, -200,  200, -400,   // S: 短-短-短
  600, -200,  600, -200,  600, -400,   // O: 长-长-长
  200, -200,  200, -200,  200, -800    // S: 短-短-短，最后长灭
};
const int seqLen = sizeof(sosSequence) / sizeof(sosSequence[0]);

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  previousMillis = millis();
  stepIndex = 0;
  // 开始执行第一个步骤
  stepDuration = abs(sosSequence[0]);
  if (sosSequence[0] > 0) digitalWrite(ledPin, HIGH);
  else digitalWrite(ledPin, LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= stepDuration) {
    previousMillis = currentMillis;
    // 移到下一步
    stepIndex++;
    if (stepIndex >= seqLen) {
      stepIndex = 0;   // 无限循环
    }
    stepDuration = abs(sosSequence[stepIndex]);
    if (sosSequence[stepIndex] > 0) digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);
  }
}