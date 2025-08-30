#include "server.h"

// Assign output variables to GPIO pins
const int led = 2;
String ledState = "off";

WebServer* server = nullptr;

// pre-declare server functions
void handleRoot();
void handleLedOn();
void handleLedOff();

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
  server->on("/led/on", handleLedOn);
  server->on("/led/off", handleLedOff);
  server->begin();

  Serial.println("HTTP server started");
}

void serverTask() {
    // Handle incoming client requests
  server->handleClient();
}

// Function to handle turning GPIO 26 on
void handleLedOn() {
  ledState = "on";
  digitalWrite(led, HIGH);
  handleRoot();
}

// Function to handle turning GPIO 26 off
void handleLedOff() {
  ledState = "off";
  digitalWrite(led, LOW);
  handleRoot();
}

// Function to handle the root URL and show the current states
void handleRoot() {
  Serial.println("Returning html...");

  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
  html += ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
  html += ".button2 { background-color: #555555; }</style></head>";
  html += "<body><h1>ESP32 Web Server</h1>";

  html += "<p>Led - State " + ledState + "</p>";
  if (ledState == "off") {
    html += "<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>";
  } else {
    html += "<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>";
  }

  html += "</body></html>";
  server->send(200, "text/html", html);
}