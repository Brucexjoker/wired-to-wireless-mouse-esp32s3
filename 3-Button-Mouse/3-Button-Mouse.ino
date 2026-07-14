/*
  ESP32-S3 Wireless Mouse — 3-Button Variant
  --------------------------------------------
  Relays a wired USB mouse (Left / Right / Middle buttons + scroll wheel)
  to any BLE-paired host as a wireless mouse.

  Hardware:
    - ESP32-S3 SuperMini (single native USB-C port)
    - Wired USB mouse connected via USB-C-to-A OTG adapter
    - Board powered externally (LiPo/5V into VIN) — NOT via the same
      USB-C port used for the mouse, since the S3 has only one native
      USB peripheral and cannot be a USB device (to a PC) and a USB
      Host (to the mouse) on the same port at the same time.

  Libraries required:
    - EspUsbHost v2.1.3+        (github.com/tanakamasayuki/EspUsbHost)
    - HijelHID_BLEMouse         (Arduino Library Manager)
    - Adafruit NeoPixel         (Arduino Library Manager)

  Required board settings (Tools menu, Arduino IDE):
    - USB Mode:        "USB-OTG (TinyUSB)"
    - USB CDC On Boot: "Disabled"   <-- critical for standalone operation;
                                          leaving this Enabled causes a boot
                                          hang when no PC is attached to the
                                          native USB port.

  LED status (onboard WS2812, GPIO48):
    - Slow blue blink   -> BLE advertising, not yet paired
    - Solid green       -> paired and relaying
    - Brief white flash -> wired mouse detected on USB
    - Brief red flash   -> a button click was relayed
*/

#include "EspUsbHost.h"
#include <HijelHID_BLEMouse.h>
#include <Adafruit_NeoPixel.h>

// -------------------------
// Sensitivity
// -------------------------
#define MOUSE_SENSITIVITY 1.5f  // 1.0 = unscaled, >1.0 = faster, <1.0 = slower

// -------------------------
// LED status (onboard WS2812 on GPIO48)
// -------------------------
#define LED_PIN 48
Adafruit_NeoPixel pixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastBlink = 0;
bool blinkOn = false;
const unsigned long BLINK_INTERVAL = 500;

unsigned long flashUntil = 0;
bool flashing = false;

void setPixel(uint8_t r, uint8_t g, uint8_t b) {
  pixel.setPixelColor(0, pixel.Color(r, g, b));
  pixel.show();
}

void triggerFlash(uint8_t r, uint8_t g, uint8_t b, unsigned long durationMs) {
  flashing = true;
  flashUntil = millis() + durationMs;
  setPixel(r, g, b);
}

// -------------------------
// BLE + USB Host
// -------------------------
HijelBLEMouse bleMouse("ESP32-S3 Wireless Mouse", "DIY");
EspUsbHost usbHost;

void setup() {
  pixel.begin();
  pixel.setBrightness(40);
  setPixel(0, 0, 0);

  bleMouse.begin();

  usbHost.onDeviceConnected([](const EspUsbHostDeviceInfo &device) {
    triggerFlash(255, 255, 255, 300);  // white flash = USB mouse detected
  });

  usbHost.onMouse([](const EspUsbHostMouseEvent &event) {
    if (!bleMouse.isPaired()) return;

    if (event.moved) {
      int16_t scaledX = (int16_t)(event.x * MOUSE_SENSITIVITY);
      int16_t scaledY = (int16_t)(event.y * MOUSE_SENSITIVITY);

      // BLE HID mouse reports are signed 8-bit — clamp so fast swipes
      // don't wrap/clip oddly
      scaledX = constrain(scaledX, -127, 127);
      scaledY = constrain(scaledY, -127, 127);

      bleMouse.move(scaledX, scaledY);
    }

    if (event.wheel != 0) {
      bleMouse.addScroll(event.wheel);
    }

    if (event.buttonsChanged) {
      bleMouse.setButton(MouseButton::Left,   event.buttons & 0x01);
      bleMouse.setButton(MouseButton::Right,  event.buttons & 0x02);
      bleMouse.setButton(MouseButton::Middle, event.buttons & 0x04);
      if (event.buttons != 0) {
        triggerFlash(255, 0, 0, 80);  // quick red flash = click relayed
      }
    }
  });

  usbHost.begin();
}

void loop() {
  unsigned long now = millis();

  if (flashing && now >= flashUntil) {
    flashing = false;
  }

  if (!flashing) {
    if (bleMouse.isPaired()) {
      setPixel(0, 60, 0);  // solid green = paired and ready
    } else {
      if (now - lastBlink >= BLINK_INTERVAL) {
        lastBlink = now;
        blinkOn = !blinkOn;
        setPixel(0, 0, blinkOn ? 60 : 0);  // slow blue blink = advertising
      }
    }
  }

  delay(10);
}
