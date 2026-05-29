#include <math.h>

// ========== 引脚定义 ==========
#define LED_A_PIN 5   // 灯 A PWM 引脚
#define LED_B_PIN 18  // 灯 B PWM 引脚

// ========== 呼吸参数 ==========
float angle = 0.0;          // 正弦波角度
float speed = 0.03;         // 呼吸速度，数值越大越快 (推荐 0.01~0.08)

void setup() {
  Serial.begin(115200);
  pinMode(LED_A_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  Serial.println("双灯同步柔和呼吸灯已启动");
}

void loop() {
  // 递增角度，用于计算正弦波
  angle += speed;
  // 限制角度在 0 ~ 2*PI 范围内，防止浮点数溢出
  if (angle > 2 * PI) {
    angle -= 2 * PI;
  }

  // 利用正弦函数生成 0 ~ 255 的亮度值
  // sin(angle) 范围 [-1, 1]，+1 后变为 [0, 2]，再乘以 127.5 得到 [0, 255]
  int brightness = (sin(angle) + 1.0) * 127.5;

  // 两盏灯设置为相同亮度，实现同步柔和呼吸
  analogWrite(LED_A_PIN, brightness);
  analogWrite(LED_B_PIN, brightness);

  delay(10);  // 控制刷新间隔，10ms 可保证视觉上完全平滑
}