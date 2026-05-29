#include <WiFi.h>
#include <WebServer.h>

// ==================== 硬件定义 ====================
#define TOUCH_PIN T0        // 触摸引脚 (GPIO4)
#define LED_PIN   2         // LED 引脚 (GPIO2)
#define TOUCH_THRESHOLD 500  // 触摸阈值（根据实际环境微调）

// ==================== AP 热点配置 ====================
const char* ap_ssid = "ESP32_Alarm";      // 热点名称
const char* ap_password = "12345678";     // 热点密码（至少8位）

WebServer server(80);

// ==================== 状态变量 ====================
bool armed = false;
bool alarmActive = false;

unsigned long lastBlinkTime = 0;
const int blinkInterval = 100;
bool ledState = LOW;

// ==================== 网页生成 ====================
String makePage() {
  String statusText;
  if (alarmActive) {
    statusText = "<span style='color:red; font-weight:bold;'>⚠ 报警中！</span>";
  } else if (armed) {
    statusText = "<span style='color:green;'>● 布防中（待触发）</span>";
  } else {
    statusText = "<span style='color:gray;'>○ 未布防</span>";
  }

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>安防报警器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 80px; }
    .status { font-size: 28px; margin-bottom: 30px; }
    button {
      font-size: 20px; padding: 12px 30px; margin: 10px;
      border: none; border-radius: 8px; cursor: pointer;
    }
    .arm-btn { background-color: #4CAF50; color: white; }
    .disarm-btn { background-color: #f44336; color: white; }
  </style>
</head>
<body>
  <h1>物联网安防报警器</h1>
  <div class="status">)rawliteral" + statusText + R"rawliteral(</div>
  <div>
    <button class="arm-btn" onclick="sendCmd('/arm')">布 防</button>
    <button class="disarm-btn" onclick="sendCmd('/disarm')">撤 防</button>
  </div>
  <p style="margin-top:40px; color:#666;">布防后触摸感应区触发报警，撤防解除</p>
  <script>
    function sendCmd(url) {
      fetch(url)
        .then(response => {
          if (response.ok) {
            location.reload();
          } else {
            console.error('请求失败');
          }
        })
        .catch(error => console.error('网络错误:', error));
    }
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// ==================== 路由处理 ====================
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleArm() {
  Serial.println(">>> 收到布防请求");
  armed = true;
  alarmActive = false;
  digitalWrite(LED_PIN, LOW);
  ledState = LOW;
  server.send(200, "text/plain", "ARMED");
}

void handleDisarm() {
  Serial.println(">>> 收到撤防请求");
  armed = false;
  alarmActive = false;
  digitalWrite(LED_PIN, LOW);
  ledState = LOW;
  server.send(200, "text/plain", "DISARMED");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

// ==================== 触摸检测 ====================
void checkTouch() {
  if (!armed || alarmActive) return;

  int touchValue = touchRead(TOUCH_PIN);
  if (touchValue < TOUCH_THRESHOLD) {
    delay(5);
    if (touchRead(TOUCH_PIN) < TOUCH_THRESHOLD) {
      Serial.println("!!! 触摸触发报警 !!!");
      alarmActive = true;
      lastBlinkTime = millis();
      ledState = LOW;
      digitalWrite(LED_PIN, ledState);
    }
  }
}

// ==================== LED 报警闪烁 (非阻塞) ====================
void updateAlarmLED() {
  if (!alarmActive) return;

  if (millis() - lastBlinkTime >= blinkInterval) {
    lastBlinkTime = millis();
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}

// ==================== 初始化 ====================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // ----- 修改为 AP 模式，ESP32 自己作为热点 -----
  WiFi.mode(WIFI_AP);                      // 设置为 AP 模式
  WiFi.softAP(ap_ssid, ap_password);      // 创建热点
  IPAddress myIP = WiFi.softAPIP();        // 获取本机 IP（通常为 192.168.4.1）
  Serial.print("热点已创建，名称：");
  Serial.println(ap_ssid);
  Serial.print("访问地址: http://");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("安防主机已就绪");
}

void loop() {
  server.handleClient();
  checkTouch();
  updateAlarmLED();
}