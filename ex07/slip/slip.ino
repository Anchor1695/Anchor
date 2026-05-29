#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "洗洁精";
const char* password = "i2kqjjej";
const int LED_PIN = 2; // GPIO2，支持 PWM

WebServer server(80);

// 当前亮度值，用于页面初始显示
int currentBrightness = 0;

// 生成 HTML 页面（含滑动条）
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>无极调光器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 80px; }
    .slider { width: 300px; height: 40px; }
    .value { font-size: 24px; margin-top: 20px; }
  </style>
</head>
<body>
  <h1>LED 无极调光</h1>
  <input type="range" id="brightness" class="slider" min="0" max="255" value=")rawliteral";
  html += String(currentBrightness);
  html += R"rawliteral(" oninput="updateBrightness(this.value)">
  <p class="value">亮度值：<span id="valDisplay">)rawliteral";
  html += String(currentBrightness);
  html += R"rawliteral(</span></p>

  <script>
    function updateBrightness(val) {
      // 更新页面显示
      document.getElementById('valDisplay').innerText = val;
      // 发送亮度值到 ESP32
      fetch('/set?value=' + val)
        .then(response => {
          if (!response.ok) console.error('设置失败');
        })
        .catch(error => console.error('网络错误:', error));
    }
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// 处理根路径
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 处理亮度设置请求
void handleSet() {
  if (server.hasArg("value")) {
    int val = server.arg("value").toInt();
    // 限制范围
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    analogWrite(LED_PIN, val);
    currentBrightness = val;
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing 'value' argument");
  }
}

// 404 处理
void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void setup() {
  Serial.begin(115200);
  
  // 初始化 LED 引脚为 PWM 输出
  pinMode(LED_PIN, OUTPUT);
  analogWrite(LED_PIN, currentBrightness); // 初始熄灭

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  // 注册路由
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP 服务器已启动");
}

void loop() {
  server.handleClient();
}