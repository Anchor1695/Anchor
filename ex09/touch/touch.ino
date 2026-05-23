#include <WiFi.h>
#include <WebServer.h>

// ==================== 硬件定义 ====================
#define TOUCH_PIN T0        // 使用 T0 (GPIO4) 作为触摸感应引脚

// ==================== WiFi 配置 ====================
const char* ssid = "剧烈爆炸的洗洁精";
const char* password = "i2kqjjej";

WebServer server(80);

// ==================== 网页 HTML ====================
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>触摸传感器仪表盘</title>
  <style>
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      text-align: center;
      margin-top: 100px;
      background-color: #1a1a2e;
      color: #e0e0e0;
    }
    .dashboard {
      background: #16213e;
      border-radius: 20px;
      padding: 40px;
      margin: 0 auto;
      width: 320px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.5);
    }
    .label {
      font-size: 20px;
      margin-bottom: 20px;
      letter-spacing: 2px;
      color: #a0a0b0;
    }
    .value {
      font-size: 80px;
      font-weight: bold;
      margin: 30px 0;
      transition: color 0.3s;
      color: #4ecca3;
    }
    .value.near {
      color: #f08a5d;   /* 数值较低（手靠近）时变色 */
    }
    .bar {
      width: 100%;
      height: 10px;
      background: #0f3460;
      border-radius: 5px;
      overflow: hidden;
      margin-top: 20px;
    }
    .fill {
      height: 100%;
      width: 50%;
      background: #4ecca3;
      border-radius: 5px;
      transition: width 0.2s, background 0.3s;
    }
    .fill.near {
      background: #f08a5d;
    }
  </style>
</head>
<body>
  <div class="dashboard">
    <div class="label">触摸传感器数值</div>
    <div id="sensorValue" class="value">--</div>
    <div class="bar">
      <div id="progressBar" class="fill" style="width: 50%;"></div>
    </div>
    <p style="margin-top:30px; font-size:14px; color:#888;">手靠近感应区，数值变小</p>
  </div>

  <script>
    const valueDisplay = document.getElementById('sensorValue');
    const progressBar = document.getElementById('progressBar');
    const MAX_RAW = 100;    // 触摸读数大致范围 0~100（实际可自动调整）

    function updateSensor() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          let val = data.value;
          valueDisplay.textContent = val;

          // 根据数值大小改变颜色和进度条
          const percentage = Math.min(100, Math.max(0, Math.round((val / MAX_RAW) * 100)));
          progressBar.style.width = percentage + '%';

          if (val < 25) {
            valueDisplay.classList.add('near');
            progressBar.classList.add('near');
          } else {
            valueDisplay.classList.remove('near');
            progressBar.classList.remove('near');
          }
        })
        .catch(err => {
          valueDisplay.textContent = '--';
          console.error('数据获取失败:', err);
        });
    }

    // 每 200ms 刷新一次数据（足够快且不会过载）
    setInterval(updateSensor, 200);
    updateSensor(); // 页面加载立即执行一次
  </script>
</body>
</html>
)rawliteral";

// ==================== 路由处理 ====================
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleData() {
  int touchValue = touchRead(TOUCH_PIN);
  String json = "{\"value\":" + String(touchValue) + "}";
  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

// ==================== 初始化 ====================
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("仪表盘地址: http://");
  Serial.println(WiFi.localIP());

  // 注册路由
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}