#include <Arduino.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>   
#include <LittleFS.h> // Вбудована бібліотека

const char* ssid = "TP-Link_C2BC";
const char* password = "92636156";

WebServer server(80); 


void handleRoot() {
    File file = LittleFS.open("/index.html", "r");
    
    if (!file) {
        server.send(404, "text/plain", "File Not Found (LittleFS error)");
        return;
    }

    server.streamFile(file, "text/html");

    file.close();
}

void setup() {
    Serial.begin(115200);


    if (!LittleFS.begin(true)) {
        Serial.println("Помилка монтування LittleFS!");
        return;
    }


    Serial.print("Підключення до Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }


    Serial.println("\nWi-Fi підключено!");
    Serial.print("IP-адреса: ");
    Serial.println(WiFi.localIP());


    server.on("/", HTTP_GET, handleRoot);

    // Запуск HTTP-сервера [cite: 48]
    server.begin();
    Serial.println("Сервер працює на порту 80");
}

void loop() {
    server.handleClient();
}