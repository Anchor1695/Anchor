// 引脚定义（根据你实际连接的LED引脚修改，默认用内置LED或GPIO2）
const int ledPin = 2;    // ESP32 开发板上通常 LED_BUILTIN 对应 GPIO2

// 变量记录
unsigned long previousMillis = 0;   // 上一次LED状态改变的时间
const long interval = 500;          // 间隔 500ms（亮500ms，灭500ms = 1Hz）

int ledState = LOW;                 // LED初始状态

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  // 如果当前时间与上次记录时间之差 >= 间隔时间
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   // 保存本次切换时间

    // 翻转LED状态
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
  // 其他非阻塞任务可以放在这里，不会影响LED闪烁
}