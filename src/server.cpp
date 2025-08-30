#include "server.h"

// Assign output variables to GPIO pins
const int led = 2;
String ledState = "off";

WebServer* server = nullptr;

// pre-declare server functions
void handleRoot();
void handleSwitchLed();
void handleInfo();

void startServer() {
  server = new WebServer(80);

  // Initialize the output variables as outputs
  pinMode(led, OUTPUT);
  // Set outputs to LOW
  digitalWrite(led, LOW);

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
  server->on("/switch_led", handleSwitchLed);
  server->begin();

  Serial.println("HTTP server started");
}

void serverTask() {
    // Handle incoming client requests
  server->handleClient();
}

// Function to handle the root URL and show the current states
void handleRoot() {
  server->send(200, "text/html", PAGE_HTML);
}

void handleSwitchLed() {
  ledState = ledState == "off" ? "on" : "off";
  digitalWrite(led, ledState == "on" ? HIGH : LOW);
  server->send(200, "application/json", "{\"state\":\"" + ledState + "\"}");
}

void handleInfo() {
  constexpr const char info[] = R"(
    {
      "battery_level": %d,
      "keymap": %s,
      "led_state": %s
    }
  )";

  char buffer[sizeof(info) + 64];
  sprintf(buffer, info, readBatteryLevel(), 
    "[]",
    ledState.c_str()
  );

  server->send(200, "application/json", buffer);
}

