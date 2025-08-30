#pragma once

#include "BleKeyboard.h"
#include "ButtonLib.h"
#include "EEPROMSettings.h"
#include "config.h"
#include "battery.h"

void setupKeyboard(EEPROMSettings& settings);
void keyboardTask();