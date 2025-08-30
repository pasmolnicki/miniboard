#include "server.h"

// Assign output variables to GPIO pins
static WebServer* server = nullptr;
static EEPROMSettings* g_settings = nullptr; // pointer to modify and persist

// pre-declare server functions
static void handleRoot();
static void handleSwitchLed();
static void handleInfo();
static void handleSave();
static void sendJson(int code, const String &body) { 
    server->send(code, "application/json", body); 
}

void startServer(EEPROMSettings &settings) {
    g_settings = &settings;
    server = new WebServer(80);
    initPage();

    Serial.print("Setting as access point ");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SERVER_SSID, SERVER_PASSWORD);

    Serial.println("");
    Serial.println("ESP32 Wi-Fi Access Point ready!");

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server->on("/", handleRoot);
    server->on("/info", handleInfo);
    server->on("/save", HTTP_POST, handleSave);
    server->begin();

    Serial.println("HTTP server started");
}

void serverTask() {
    server->handleClient();
}

static void handleRoot() {
    server->send(200, "text/html", getPage());
}

static void handleInfo() {
    constexpr const char* JSON_TEMPLATE = R"({"battery_level":%u,"keymap":[%u, %u, %u, %u]})";

    auto s = g_settings->get();
    char buffer[256];
    snprintf(buffer, sizeof(buffer), JSON_TEMPLATE, 
        readBatteryLevel(), s->keypad[0], s->keypad[1], s->keypad[2], s->keypad[3]);
    sendJson(200, buffer);
}

// Accept JSON body: {"keymap":[k1,k2,k3,k4]}
static void handleSave() {
    if (!g_settings) { 
        sendJson(500, "{\"error\":\"settings null\"}"); 
        return; 
    }
    if (!server->hasArg("plain")) { 
        sendJson(400, "{\"error\":\"missing body\"}"); 
        return; 
    }

    String body = server->arg("plain");
    int vals[4];
    int matched = sscanf(body.c_str(), "{\"keymap\":[%d,%d,%d,%d]}", &vals[0], &vals[1], &vals[2], &vals[3]);
    if (matched != 4) { 
        sendJson(400, "{\"error\":\"bad format\"}"); 
        return; 
    }

    Serial.printf("Received new keymap: [%d, %d, %d, %d]\n", vals[0], vals[1], vals[2], vals[3]);

    auto s = g_settings->get();
    for (int i=0;i<4;i++) {
        s->keypad[i] = (uint8_t)vals[i];
    }

    s->boot_type = BOOT_BLE_KEYBOARD; // ensure we boot keyboard next
    g_settings->save();
    sendJson(200, "{\"status\":\"ok\",\"rebooting\":true}");
    
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(500);
    esp_restart();
}

