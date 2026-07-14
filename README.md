# ESP32-S3 Wireless Mouse

Turn a wired USB mouse into a BLE wireless mouse using an ESP32-S3 SuperMini.
The S3 acts as a **USB Host** to read the wired mouse, then relays every
movement, scroll, and button event over **BLE HID** to whatever device you
pair it with (PC, phone, tablet).

Two variants are included:

| Variant | Folder | Buttons supported |
|---|---|---|
| Basic | [`3-Button-Mouse/`](./3-Button-Mouse) | Left, Right, Middle, scroll wheel |
| Extended | [`5-Button-Mouse/`](./5-Button-Mouse) | Left, Right, Middle, Back, Forward, scroll wheel |

Start with the 3-button version if your mouse doesn't have side buttons, or
if you just want the simpler code to read through first.

## Features

- Full USB Host mouse relay (movement, scroll, clicks) over BLE
- Adjustable sensitivity via a single `#define`
- Onboard RGB LED status feedback (no Serial connection required at runtime)
- Runs standalone off external power — no PC needed after flashing

## Hardware

- **ESP32-S3 SuperMini** (or any S3 board with a single native USB-OTG port
  and an onboard WS2812 status LED on GPIO48)
- A wired **USB mouse**
- A **USB-C-to-A OTG adapter** to connect the wired mouse to the S3's native port
- An external **5V power source** (LiPo + boost converter, or a USB power
  bank wired to the board's VIN pin — **not** through the same USB-C port
  used for the mouse)

### Why external power is required

The ESP32-S3 has only **one native USB peripheral**. It can either act as a
USB *device* (talking to your PC) or a USB *Host* (talking to the mouse) —
not both at once, on the same port. To free the native port for the mouse,
the board must be powered independently.

## Libraries

Install via Arduino IDE **Tools → Manage Libraries**:

| Library | Notes |
|---|---|
| `EspUsbHost` | **Use v2.1.3 or later.** Earlier 1.x releases predate mouse/keyboard HID support entirely. If Library Manager gives you trouble switching versions cleanly, delete the library folder from your sketchbook's `libraries/` directory first, then reinstall. |
| `HijelHID_BLEMouse` | Provides the BLE HID mouse output. Requires NimBLE-Arduino v2.3.8+. |
| `Adafruit NeoPixel` | Drives the onboard status LED. |

## Required Arduino IDE board settings

**Tools menu**, on top of your normal ESP32-S3 board selection:

| Setting | Value | Why |
|---|---|---|
| USB Mode | `USB-OTG (TinyUSB)` | Required for USB Host mode |
| USB CDC On Boot | **`Disabled`** | Critical — leaving this enabled causes the board to hang on boot when powered standalone with no PC attached, since the CDC stack waits for a handshake that never comes |

All other settings (Flash Size, PSRAM, Partition Scheme, etc.) can stay at
your board's normal defaults.

## Wiring

- Wired mouse → USB-C-to-A OTG adapter → S3's native USB-C port
- External 5V power → board's VIN/5V pin (**not** the USB-C port)

## Status LED reference

The onboard WS2812 RGB LED (GPIO48) reports state without needing Serial:

| LED behavior | Meaning |
|---|---|
| Slow blue blink | BLE advertising, not yet paired |
| Solid green | Paired and relaying |
| Brief white flash | Wired mouse detected on USB |
| Brief red flash | A button click was relayed |

## Adjusting sensitivity

Edit the multiplier near the top of either sketch:

```cpp
#define MOUSE_SENSITIVITY 1.5f  // 1.0 = unscaled, >1.0 = faster, <1.0 = slower
```

Reflash after changing it — no OS-side mouse settings needed.

## Troubleshooting

- **BLE never shows up on your PC** — confirm `Serial.println` in `setup()`
  actually runs (temporarily set USB CDC On Boot to `Enabled` and USB Mode
  to a CDC-capable mode *while connected to a PC only*, just to confirm
  boot completes) before assuming it's a BLE-side issue.
- **Wired mouse does nothing once BLE is paired** — almost always the single
  native USB port doing double duty. Confirm the board is powered externally
  and only the mouse (via OTG adapter) is on the native port.
- **Compile errors mentioning `hid_mouse_report_t` or `.task()`** — you're on
  an old/mismatched EspUsbHost version. Fully remove the library folder and
  reinstall v2.1.3+.
- **Side buttons don't respond or are swapped (5-button variant)** — your
  mouse's report format may differ. Wire a UART/FTDI adapter to GPIO43(TX)/
  GPIO44(RX) — since the native USB port is occupied by the mouse — and
  temporarily add `Serial.printf("buttons: 0x%02X\n", event.buttons);`
  inside the `onMouse` callback to find your mouse's real bit layout.

## License

Add a license of your choice (e.g. MIT) before publishing if you want others
to freely reuse this.
