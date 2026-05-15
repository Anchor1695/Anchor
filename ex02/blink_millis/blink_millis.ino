const int ledPin = 2;           // LED引脚
unsigned long previousMillis = 0;
bool ledState = LOW;

// 定义亮和灭的持续时间（毫秒）
const unsigned long onDuration = 100;   // 亮100ms
const unsigned long offDuration = 900;  // 灭900ms

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);    // 初始状态灭
}

void loop() {
  unsigned long currentMillis = millis();

  if (ledState == HIGH) {
    // 当前是亮着的，检查是否该灭了
    if (currentMillis - previousMillis >= onDuration) {
      ledState = LOW;
      digitalWrite(ledPin, LOW);
      previousMillis = currentMillis;   // 记录灭的开始时间
    }
  } else {
    // 当前是灭着的，检查是否该亮了
    if (currentMillis - previousMillis >= offDuration) {
      ledState = HIGH;
      digitalWrite(ledPin, HIGH);
      previousMillis = currentMillis;   // 记录亮的开始时间
    }
  }
}