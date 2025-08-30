#pragma once

#include <WiFi.h>
#include <WebServer.h>

#include "config.h"
#include "battery.h"
#include "page.h"
#include "template_parser.h"
#include "EEPROMSettings.h"

// Start the web server. Pass current settings so the page can
// modify keypad layout then trigger reboot into BLE keyboard mode.
void startServer(EEPROMSettings &settings);
void serverTask();