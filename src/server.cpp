#include "server.h"

// Assign output variables to GPIO pins
static WebServer* server = nullptr;
static Button bootButton = Button(PIN_SERVER_BUTTON);

// pre-declare server functions
static void handleRoot();
static void handleInfo();
static void handleSave();
inline void sendJson(int code, const String &body) { 
    server->send(code, "application/json", body); 
}

static void reboot() {
    server->close();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(200);
    esp_restart();
}

static void bootKeyboard() {
    auto s = g_settings.get();
    s->boot_type = BOOT_BLE_KEYBOARD; // ensure we boot keyboard next
    g_settings.save();
    reboot();
}

// Starts the web server with AP and returns the IP address
IPAddress startServer() {
    server = new WebServer(80);
    initPage();

    bootButton.onPress(bootKeyboard);
    bootButton.begin();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(SERVER_SSID, SERVER_PASSWORD);

    server->on("/", handleRoot);
    server->on("/info", handleInfo);
    server->on("/save", HTTP_POST, handleSave);
    server->begin();

    return WiFi.softAPIP();
}

void serverTask() {
    server->handleClient();
    bootButton.read();
}

static void handleRoot() {
    server->send(200, "text/html", getPage());
}

static void handleInfo() {
    constexpr const char* JSON_TEMPLATE = R"({"battery_level":%u,"keymap":[%u, %u, %u, %u], "defaultKeymap":[%u, %u, %u, %u]})";

    auto defaultKeymap = DEFAULT_SETTINGS.keypad;
    auto s = g_settings.get();
    char buffer[256];
    snprintf(buffer, sizeof(buffer), JSON_TEMPLATE, 
        readBatteryLevel(), s->keypad[0], s->keypad[1], s->keypad[2], s->keypad[3],
        defaultKeymap[0], defaultKeymap[1], defaultKeymap[2], defaultKeymap[3]);

    dlog_v("/info %s\n", buffer);
    sendJson(200, buffer);
}

// Accept JSON body: {"keymap":[k1,k2,k3,k4]}
static void handleSave() {
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

    dlog_v("Received new keymap: [%d, %d, %d, %d]\n", vals[0], vals[1], vals[2], vals[3]);

    auto s = g_settings.get();
    for (int i=0;i<4;i++) {
        s->keypad[i] = (uint8_t)vals[i];
    }
    s->boot_type = BOOT_BLE_KEYBOARD; // ensure we boot keyboard next
    g_settings.save();
    sendJson(200, "{\"status\":\"ok\",\"rebooting\":true}");

    delay(50);
    reboot();
}

