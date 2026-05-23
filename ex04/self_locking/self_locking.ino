#define TOUCH_PIN T0        // ESP32 触摸引脚 T0 (对应 GPIO4)
#define LED_PIN   5
#define TOUCH_THRESHOLD 500 // 触摸阈值：读数低于此值视为手指接触（需根据实际调整）

int speedLevel = 1;
bool lastTouchState = false;   // 上一轮的触摸状态（true = 触摸中）
bool breathRestart = false;    // 标志：需要立即重启呼吸周期

struct SpeedParam {
  int stepDelay;   // 每步延时 (ms)
  int stepSize;    // 每步亮度增量
};

SpeedParam getSpeedParam() {
  switch (speedLevel) {
    case 1: return {30, 1};   // 缓慢
    case 2: return {15, 2};   // 中速
    case 3: return {5, 8};    // 急促
    default: return {20, 1};
  }
}

// 检测触摸并切换速度档位
void checkTouch() {
  bool currentTouch = (touchRead(TOUCH_PIN) < TOUCH_THRESHOLD);

  // 上升沿检测 + 消抖：从未触摸变成触摸时，稍作确认
  if (!lastTouchState && currentTouch) {
    delay(5);
    if (touchRead(TOUCH_PIN) < TOUCH_THRESHOLD) {
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1;
      breathRestart = true;   // 标记需要重启呼吸
    }
  }
  lastTouchState = currentTouch;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  // 触摸引脚不需要 pinMode，ESP32 内部自动配置
  Serial.println("呼吸灯已启动，触摸 T0(GPIO4) 切换速度档位");
}

void loop() {
  // 每次 loop 开始时检测触摸（即使没有呼吸循环也会响应）
  checkTouch();

  SpeedParam p = getSpeedParam();

  // ---------- 渐亮 ----------
  for (int brightness = 0; brightness <= 255; brightness += p.stepSize) {
    analogWrite(LED_PIN, brightness);
    delay(p.stepDelay);
    checkTouch();
    if (breathRestart) break;   // 触摸导致速度改变，立刻跳出
  }
  if (breathRestart) {
    breathRestart = false;
    return;                     // 返回 loop 开头，从 0 开始新的呼吸
  }

  // ---------- 渐暗 ----------
  for (int brightness = 255; brightness >= 0; brightness -= p.stepSize) {
    analogWrite(LED_PIN, brightness);
    delay(p.stepDelay);
    checkTouch();
    if (breathRestart) break;
  }
  if (breathRestart) {
    breathRestart = false;
    return;                     // 返回开头，重新渐亮
  }
}