// SOS 信号，播放 3 次后停止，LED 常灭

const int ledPin = 2;
unsigned long previousMillis = 0;
int stepIndex = 0;
unsigned long stepDuration = 0;
int playCount = 0;           // 已经完整播放的次数
const int maxPlays = 3;      // 总共播放 3 次

const int sosSequence[] = {
  200, -200,  200, -200,  200, -400,
  600, -200,  600, -200,  600, -400,
  200, -200,  200, -200,  200, -800
};
const int seqLen = sizeof(sosSequence) / sizeof(sosSequence[0]);

bool isPlaying = true;       // 是否还在播放

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  previousMillis = millis();
  stepIndex = 0;
  stepDuration = abs(sosSequence[0]);
  if (sosSequence[0] > 0) digitalWrite(ledPin, HIGH);
  else digitalWrite(ledPin, LOW);
}

void loop() {
  if (!isPlaying) {
    // 已经播完，什么都不做，LED保持灭
    return;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= stepDuration) {
    previousMillis = currentMillis;
    stepIndex++;
    if (stepIndex >= seqLen) {
      // 完成一次完整 SOS
      playCount++;
      if (playCount >= maxPlays) {
        isPlaying = false;
        digitalWrite(ledPin, LOW);   // 最终熄灭
        return;
      }
      stepIndex = 0;   // 重新开始下一轮
    }
    stepDuration = abs(sosSequence[stepIndex]);
    if (sosSequence[stepIndex] > 0) digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);
  }
}