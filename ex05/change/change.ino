// ========== 双灯反相呼吸（警车双闪效果）==========
#define TOUCH_PIN T0        // 触摸引脚 T0 (GPIO4)
#define LED_A_PIN 5         // 灯 A PWM 引脚
#define LED_B_PIN 18        // 灯 B PWM 引脚（请选择支持 PWM 的 GPIO）
#define TOUCH_THRESHOLD 500  // 触摸阈值，需根据实际读数调整

int speedLevel = 1;
bool lastTouchState = false;
bool breathRestart = false;

struct SpeedParam {
  int stepDelay;
  int stepSize;
};

SpeedParam getSpeedParam() {
  switch (speedLevel) {
    case 1: return {30, 1};   // 慢
    case 2: return {15, 2};   // 中
    case 3: return {5, 8};    // 快
    default: return {20, 1};
  }
}

// 触摸检测与档位切换
void checkTouch() {
  bool currentTouch = (touchRead(TOUCH_PIN) < TOUCH_THRESHOLD);
  if (!lastTouchState && currentTouch) {
    delay(5);
    if (touchRead(TOUCH_PIN) < TOUCH_THRESHOLD) {
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1;
      breathRestart = true;
    }
  }
  lastTouchState = currentTouch;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  Serial.println("双灯反相呼吸灯已启动，触摸 T0 切换速度");
}

void loop() {
  checkTouch();

  SpeedParam p = getSpeedParam();

  // ----- 灯A渐亮，灯B渐暗 (0 -> 255) -----
  for (int brightness = 0; brightness <= 255; brightness += p.stepSize) {
    analogWrite(LED_A_PIN, brightness);          // 灯A变亮
    analogWrite(LED_B_PIN, 255 - brightness);    // 灯B同步变暗
    delay(p.stepDelay);
    checkTouch();
    if (breathRestart) break;
  }
  if (breathRestart) {
    breathRestart = false;
    return;
  }

  // ----- 灯A渐暗，灯B渐亮 (255 -> 0) -----
  for (int brightness = 255; brightness >= 0; brightness -= p.stepSize) {
    analogWrite(LED_A_PIN, brightness);
    analogWrite(LED_B_PIN, 255 - brightness);
    delay(p.stepDelay);
    checkTouch();
    if (breathRestart) break;
  }
  if (breathRestart) {
    breathRestart = false;
    return;
  }
}