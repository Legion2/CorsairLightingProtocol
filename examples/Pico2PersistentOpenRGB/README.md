# Pico 2 persistent OpenRGB ARGB controller

This example turns a Raspberry Pi Pico 2 into a two-channel, Corsair Lighting
Node PRO-compatible ARGB controller. It is intended for OpenRGB and restores the
last stable Direct-mode frame from flash when powered without a USB host.

## Connections

| Pico 2 | Function |
| --- | --- |
| GP2 (physical pin 4) | ARGB channel 1 data |
| GP3 (physical pin 5) | ARGB channel 2 data |
| GND | Common ground with the external 5 V LED supply |

Each channel supports up to 204 WS2812B-compatible LEDs in GRB order. Do not
power the LEDs from the Pico's 3.3 V pin. Use an adequately rated 5 V supply and
prefer a 5 V AHCT-family level shifter such as an SN74AHCT125.

## Arduino settings

- Board package: Arduino-Pico 5.6.0 or later
- Board: Raspberry Pi Pico 2
- USB stack: Adafruit TinyUSB
- Flash size: 4 MB, no filesystem
- Library: FastLED 3.10.5 or later

The USB serial and four-byte Corsair device ID are derived from the RP2350's
unique board ID, so multiple controllers remain distinguishable in OpenRGB.

## Persistence behavior

The example saves both 204-pixel channel buffers after the displayed frame has
remained unchanged for 10 seconds. Continuously animated frames therefore do
not cause repeated flash writes. After selecting a static color in OpenRGB,
wait at least 10 seconds before removing power.

OpenRGB's per-channel LED counts remain host-side configuration because its
Lighting Node PRO driver does not read them from the controller. Resize the two
zones in OpenRGB to match the connected hardware and ensure OpenRGB's size
configuration is saved.
