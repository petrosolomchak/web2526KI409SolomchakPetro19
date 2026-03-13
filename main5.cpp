#include <Arduino.h>
#include <WiFi.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

using namespace httpsserver;

const char* ssid = "iPhone";
const char* password = "12345678";

SSLCert cert = SSLCert(); 
HTTPSServer secureServer = HTTPSServer(&cert);

void handleRoot(HTTPRequest * req, HTTPResponse * res) {
    // Basic Auth перевірка
    // admin:password -> YWRtaW46cGFzc3dvcmQ=
    if (req->getHeader("Authorization") != "Basic YWRtaW46cGFzc3dvcmQ=") {
        res->setStatusCode(401);
        res->setHeader("WWW-Authenticate", "Basic realm=\"ESP32-Secure\"");
        res->print("Автентифікація провалена!");
        return;
    }

    res->setHeader("Content-Type", "text/html; charset=utf-8");
    res->print("<html><body><h1>Вітаємо, Адмін! Сесія захищена HTTPS.</h1>");
    res->print("<p>Студент: Соломчак Петро</p></body></html>");
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nIP: " + WiFi.localIP().toString());

    // Створення сертифіката (може зайняти час)
    int createRes = createSelfSignedCert(cert, KEYSIZE_2048, "CN=esp32.local,O=Test,C=UA");
    if (createRes != 0) {
        Serial.println("Помилка сертифіката");
        return;
    }

    ResourceNode * nodeRoot = new ResourceNode("/", "GET", &handleRoot);
    secureServer.registerNode(nodeRoot);

    secureServer.start();
    if (secureServer.isRunning()) {
        Serial.println("HTTPS сервер працює на порту 443");
    }
}

void loop() {
    secureServer.loop();
}