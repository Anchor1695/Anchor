const int ledPin = 2;
unsigned long previousMillis = 0;
int stepIndex = 0;
unsigned long stepDuration = 0;

const int sosSequence[] = {
  200, -200,  200, -200,  200, -400,
  600, -200,  600, -200,  600, -400,
  200, -200,  200, -200,  200, -800
};
const int seqLen = sizeof(sosSequence) / sizeof(sosSequence[0]);

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
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= stepDuration) {
    previousMillis = currentMillis;
    stepIndex++;
    if (stepIndex >= seqLen) {
      // 完成一次完整 SOS，直接重新开始
      stepIndex = 0;   // 重新开始下一轮
    }
    stepDuration = abs(sosSequence[stepIndex]);
    if (sosSequence[stepIndex] > 0) digitalWrite(ledPin, HIGH);
    else digitalWrite(ledPin, LOW);
  }
}