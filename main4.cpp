#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Preferences.h> // Для збереження в NVRAM (аналог EEPROM)

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
WebServer server(80);
Preferences preferences;

void handleSave() {
    if (server.hasArg("ssid") && server.hasArg("password")) {
        String ssid = server.arg("ssid");
        String pass = server.arg("password");
        
        preferences.begin("wifi-creds", false);
        preferences.putString("ssid", ssid);
        preferences.putString("pass", pass);
        preferences.end();
        
        server.send(200, "text/html", "<html><body><h1>Збережено! ESP32 перезавантажується...</h1></body></html>");
        delay(2000);
        ESP.restart();
    }
}

void setup() {
    Serial.begin(115200);
    LittleFS.begin(true);
    preferences.begin("wifi-creds", true);
    String savedSSID = preferences.getString("ssid", "");
    String savedPass = preferences.getString("pass", "");
    preferences.end();

    if (savedSSID != "") {
        WiFi.begin(savedSSID.c_str(), savedPass.c_str());
        Serial.print("Connecting to WiFi...");
        int retry = 0;
        while (WiFi.status() != WL_CONNECTED && retry < 20) {
            delay(500); Serial.print("."); retry++;
        }
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nStarting Hotspot (Captive Portal)");
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        WiFi.softAP("ESP32_Setup_Petro");

        dnsServer.start(DNS_PORT, "*", apIP); // Всі DNS запити на 192.168.1.1

        server.onNotFound([]() {
            File f = LittleFS.open("/index.html", "r");
            server.streamFile(f, "text/html");
            f.close();
        });
        server.on("/save", HTTP_POST, handleSave);
        server.begin();
    } else {
        Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
    }
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}