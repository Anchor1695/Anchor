// 引脚定义（根据实际连接的LED引脚修改，ESP32板载LED通常是GPIO2）
const int ledPin = 2;

// 亮灭时间（毫秒）
const unsigned long onDuration = 100;   // 亮100ms
const unsigned long offDuration = 900;  // 灭900ms

unsigned long previousMillis = 0;
bool ledState = LOW;   // 初始状态为灭

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);   // 确保起始时LED熄灭
  Serial.begin(115200);        // 初始化串口通信，波特率115200
  Serial.println("程序启动，LED开始以1Hz频率闪烁（亮100ms，灭900ms）");
}

void loop() {
  unsigned long currentMillis = millis();

  if (ledState == HIGH) {
    // LED当前亮着，检查是否该灭了
    if (currentMillis - previousMillis >= onDuration) {
      ledState = LOW;
      digitalWrite(ledPin, LOW);
      previousMillis = currentMillis;
      Serial.println("LED OFF");   // 灭的时候打印
    }
  } else {
    // LED当前灭着，检查是否该亮了
    if (currentMillis - previousMillis >= offDuration) {
      ledState = HIGH;
      digitalWrite(ledPin, HIGH);
      previousMillis = currentMillis;
      Serial.println("LED ON");    // 亮的时候打印
    }
  }
}