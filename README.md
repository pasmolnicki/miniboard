# Bluetooth Keyboard

Custom BLE keyboard with configurable key mappings

<img src="/docs/images/bt-keyboard-miniboard.jpg" width="300" />

## Assembly

- ESP32 dev board
- 4x1 keymap
- Caps: 0.1uF ceramic, 100uF electrolytic
- 3.7V Li-ion battery + holder
- Resistors: 47k, 100k ohm
- 3-way switch
- TP4056 charging module

Circuit diagram coming soon

## Notes

Battery life: ~18-19 hours with a 3000mAh battery (ESP32 draws ~160mA)

Sleep mode activates after 5 minutes of idle state. Press any key to wake.

## License

MIT