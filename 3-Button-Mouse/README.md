# 3-Button Mouse Variant

The base version — relays **Left, Right, Middle, and scroll wheel** from a
wired USB mouse to a BLE-paired host.

Use this variant if:
- Your wired mouse only has the standard 3 buttons, or
- You want the simpler code as a starting point before adding more buttons

See the [main README](../README.md) for full setup instructions: required
libraries, board settings, wiring, power requirements, and the LED status
reference.

## Quick summary

1. Install `EspUsbHost` (v2.1.3+), `HijelHID_BLEMouse`, and `Adafruit NeoPixel`.
2. Set **USB Mode → USB-OTG (TinyUSB)** and **USB CDC On Boot → Disabled**
   in the Tools menu.
3. Power the board externally via VIN — not through the USB-C port.
4. Flash `3-Button-Mouse.ino`.
5. Connect the wired mouse to the native USB-C port via an OTG adapter.
6. Pair "ESP32-S3 Wireless Mouse" over Bluetooth on your host device.
