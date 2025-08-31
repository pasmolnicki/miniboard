# Miniboard

Custom Bluetooth Low Energy keypad with configurable key mappings

<img src="/docs/images/bt-keyboard-miniboard.jpg" width="700" />

## Features

- 4 customizable buttons
- Battery level monitoring
- Light sleep mode for power saving
- HTTP server for button layout setup (see [Keypad setup](#keypad-setup))
- Memory leak (~2kB) when put to sleep mode
- Heater mode when board powered by USB and power switch turned off

## Assembly

- ESP32 devkit v1 board
- 4x1 keymap
- Caps: 0.1uF ceramic, 100uF electrolytic
- 3.7V Li-ion battery + holder
- Resistors: 47k, 100k ohm
- 3-way switch
- push button
- TP4056 charging module

Circuit diagram coming soon

## Setup

This project uses 3rd party Arduino libraries:
- [NimBLE-Arduino](https://github.com/NimBLEArduino/NimBLE-Arduino) for BLE functionality
- [BleKeyboard](https://github.com/T-vK/ESP32-BLE-Keyboard) for the BLE keyboard implementation (already included in `src` folder)

To run this project, make sure you have downloaded the NimBLE-Arduino library, and
added it to the Arduino libraries folder.
Also you must have installed the ESP32 board package for the Arduino IDE.

> [!WARNING]
> DO NOT power ESP32 when battery is plugged, this can cause damage to the microcontroller.
> Instead when uploading the sketch, take out the battery and KEEP THE SWITCH ON (the state in which the battery powers up the board).
> Due to my flawless design, this board features a heater when powered by built-in USB port, that may warm you up quickly or
> your whole house. If desired, then keep the switch off and make yourself comfy with that heat.

Setup steps:
1. Download the project files and open the Arduino IDE.
2. Install the required libraries (see Configuration section).
3. Connect the ESP32 board to your computer. (see the warning section)
4. Select the correct board and port in the Arduino IDE.
5. Upload the sketch to the ESP32.

## Keypad setup

<img src="/docs/images/miniboard-keypad-setup-page.png" />

To configure the keypad layout, you can use the built-in HTTP server to access the web interface. From there, you can customize the key mappings for each button.

Follow these steps:
1. Press the server button
2. Connect to the `Miniboard AP` network, default password is `miniboard123`
3. Open a web browser and go to `http://192.168.4.1`

After finishing the configuration, press `Save & Reboot` button to commit the changes.
The microcontroller will reboot with the new key mappings.

## Notes

Battery life: ~18-19 hours with a 3000mAh battery (ESP32 draws ~160mA)

Sleep mode activates after 15 minutes of idle state. Press any key to wake.

## License

MIT
