# Bluetooth Keyboard

Custom Bluetooth Low Energy keyboard with configurable key mappings

<img src="/docs/images/bt-keyboard-miniboard.jpg" width="700" />

## Assembly

- ESP32 devkit v1 board
- 4x1 keymap
- Caps: 0.1uF ceramic, 100uF electrolytic
- 3.7V Li-ion battery + holder
- Resistors: 47k, 100k ohm
- 3-way switch
- TP4056 charging module

Circuit diagram coming soon

## Configuration

This project uses 3rd party Arduino libraries:
- [NimBLE-Arduino](https://github.com/NimBLEArduino/NimBLE-Arduino) for BLE functionality
- [BleKeyboard](https://github.com/T-vK/ESP32-BLE-Keyboard) for the BLE keyboard implementation (already included in `src` folder)

To run this project, make sure you have downloaded the NimBLE-Arduino library.

## Notes

> [!WARNING]
> DO NOT power ESP32 when batter is plugged, this can cause damage to the microcontroller.
> Instead when uploading the sketch, take out the battery and KEEP THE SWITCH ON (the state in which the battery powers up the board).
> Due to my flawless design, this board features a heater when powered by built-in USB port, that may warm you up quickly or
> your whole house. If desired, then keep the switch off and make yourself comfy with that heat.


Battery life: ~18-19 hours with a 3000mAh battery (ESP32 draws ~160mA)

Sleep mode activates after 5 minutes of idle state. Press any key to wake.

## License

MIT
