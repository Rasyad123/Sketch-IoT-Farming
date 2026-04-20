#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <DHT.h>

const char* ssid = "Ryan";
const char* password = "12345678";

#define DHTPIN 15
#define DHTTYPE DHT22
#define SOIL_PIN 34
#define LDR_PIN 35
DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

String readDHTTemperature() {
    float t = dht.readTemperature();
    if (isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    }
    return String(t);
}

String readDHTHumidity() {
    float h = dht.readHumidity();
    if (isnan(h)) {
        Serial.println("Failed to read from DHT sensor!");
        return "--";
    }
    return String(h);
}

String readSoilMoisture() {
    int value = analogRead(SOIL_PIN);
    int moisture = (100 - ((value / 4095.00) * 100));
    return String(moisture);
}

String readLDR() {
    int value = analogRead(LDR_PIN);
    int light = (100 - ((value / 4095.00) * 100));
    return String(light);
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  
  <style>
    html { font-family: Arial; text-align: center; }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{ font-size: 1.5rem; }
  </style>
</head>
<body>
  <h2>ESP32 Sensor Server</h2>
  <p>Temperature: <span id="temperature">%TEMPERATURE%</span>&deg;C</p>
  <p>Humidity: <span id="humidity">%HUMIDITY%</span>%</p>
  <p>Soil Moisture: <span id="soil">%SOIL%</span>%</p>
  <p>Light Level: <span id="ldr">%LDR%</span>%</p>
</body>
<script>
function updateData(endpoint, elementId) {
  fetch(endpoint).then(response => response.text()).then(data => {
    document.getElementById(elementId).innerText = data;
  });
}
setInterval(() => { updateData("/temperature", "temperature"); }, 10000);
setInterval(() => { updateData("/humidity", "humidity"); }, 10000);
setInterval(() => { updateData("/soil", "soil"); }, 10000);
setInterval(() => { updateData("/ldr", "ldr"); }, 10000);
</script>
</html>)rawliteral";

String processor(const String& var) {
    if (var == "TEMPERATURE") return readDHTTemperature();
    if (var == "HUMIDITY") return readDHTHumidity();
    if (var == "SOIL") return readSoilMoisture();
    if (var == "LDR") return readLDR();
    return String();
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println(WiFi.localIP());
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html, processor);
    });
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTTemperature().c_str());
    });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readDHTHumidity().c_str());
    });
    server.on("/soil", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readSoilMoisture().c_str());
    });
    server.on("/ldr", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/plain", readLDR().c_str());
    });
    server.begin();
}

void loop() {
}
