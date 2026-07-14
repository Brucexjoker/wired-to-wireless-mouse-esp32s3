# 5-Button Mouse Variant

Adds **Back** and **Forward** (side navigation) button support on top of the
base 3-button relay — full Left, Right, Middle, Back, Forward, and scroll
wheel.

See the [main README](../README.md) for full setup instructions: required
libraries, board settings, wiring, power requirements, and the LED status
reference.

## Button bit mapping

Standard 5-button mice report all buttons in a single byte:

| Bit | Mask | Button |
|---|---|---|
| 0 | `0x01` | Left |
| 1 | `0x02` | Right |
| 2 | `0x04` | Middle |
| 3 | `0x08` | Back / Previous |
| 4 | `0x10` | Forward / Next |

If your mouse's side buttons don't respond, or respond swapped, your mouse
may use a different bit layout. Wire a UART/FTDI adapter to GPIO43(TX)/
GPIO44(RX) — the native USB port is occupied by the mouse, so Serial won't
reach a PC through it — and temporarily add this inside the `onMouse`
callback:

```cpp
Serial.printf("buttons: 0x%02X\n", event.buttons);
```

Click each side button alone and note which bit actually changes, then
update the `0x08`/`0x10` masks in the sketch to match.

## Quick summary

1. Install `EspUsbHost` (v2.1.3+), `HijelHID_BLEMouse`, and `Adafruit NeoPixel`.
2. Set **USB Mode → USB-OTG (TinyUSB)** and **USB CDC On Boot → Disabled**
   in the Tools menu.
3. Power the board externally via VIN — not through the USB-C port.
4. Flash `5-Button-Mouse.ino`.
5. Connect the wired mouse to the native USB-C port via an OTG adapter.
6. Pair "ESP32-S3 Wireless Mouse" over Bluetooth on your host device.
