#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <math.h>

const char* ssid = "iPhone";
const char* password = "12345678";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
    Serial.begin(115200);
    LittleFS.begin(true);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println(WiFi.localIP());

    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *r){ r->send(LittleFS, "/index.html", "text/html"); });
    server.begin();
}

void loop() {
    ws.cleanupClients();
    static unsigned long lastUpdate = 0;
    static float angle = 0;

    if (millis() - lastUpdate > 300) {
        lastUpdate = millis();
        int simValue = 100 + (sin(angle) * 50) + random(-2, 3);
        angle += 0.2;

        JsonDocument doc;
        doc["value"] = simValue;
        String json;
        serializeJson(doc, json);
        ws.textAll(json);
    }
}