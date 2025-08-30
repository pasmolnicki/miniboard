#pragma once

#include <WiFi.h>
#include <WebServer.h>

#include "config.h"
#include "battery.h"
#include "page.h"

// Start the web server, before calling this function turn off
// all bt related tasks for good. This server will persist, as long as
// the user finishes the setup.
void startServer();
void serverTask();