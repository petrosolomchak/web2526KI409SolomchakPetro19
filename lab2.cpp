#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

const char* ssid = "iPhone";
const char* password = "12345678";
const int LED_PIN = 2; // Вбудований LED

WebServer server(80);
bool state = false;

// Повертаємо JSON зі станом
void getStatus() {
    JsonDocument doc;
    doc["led_on"] = state;
    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out);
}

// Приймаємо JSON і перемикаємо LED
void postControl() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));
        if (doc["command"] == "toggle") {
            state = !state;
            digitalWrite(LED_PIN, state ? HIGH : LOW);
        }
    }
    getStatus();
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    
    if(!LittleFS.begin(true)) return;
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println(WiFi.localIP());

    // Віддача головної сторінки
    server.on("/", HTTP_GET, [](){
        File f = LittleFS.open("/index.html", "r");
        server.streamFile(f, "text/html");
        f.close();
    });

    // API ендпоінти
    server.on("/api/status", HTTP_GET, getStatus);
    server.on("/api/control", HTTP_POST, postControl);

    server.begin();
}

void loop() {
    server.handleClient();
}