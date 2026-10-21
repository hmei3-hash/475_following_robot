# rd03d_uart_test — RD-03D bench sketch

Standalone Arduino-ESP32 sketch for bring-up of the RD-03D on an ESP32-S3 DevKitC-1.
It reads radar frames over UART and prints every valid target over USB serial.
This is a **bench test only**; the real driver will live in `firmware/radar/` once the
firmware framework is decided (see `docs/decisions.md`).

## Wiring

| RD-03D | ESP32-S3 DevKitC-1 |
| --- | --- |
| 5V | 5V |
| GND | GND |
| TX | GPIO2 (ESP RX) |
| RX | GPIO1 (ESP TX) |

RD-03D I/O is 3.3 V, so no level shifter is needed (see `docs/hardware/rd-03d.md`).

## Build and run

1. Arduino IDE (or arduino-cli) with the ESP32 core, board **ESP32S3 Dev Module**.
2. Open `rd03d_uart_test.ino`, upload.
3. Serial monitor at **115200** baud (USB side). The radar side runs at 256000 baud.

## Config flags (top of the sketch)

| Flag | Default | Meaning |
| --- | --- | --- |
| `USE_MULTI_TARGET` | `false` | Send config commands at boot to switch to multi-target mode |
| `DEBUG_HEX` | `false` | Also print the raw 24-byte payload of each frame |

## Expected output

```
RD-03D test started
#1 T1  x=  -120 mm  y=  1450 mm  v=    0 cm/s  d= 1455.0 mm  ang=  -4.7 deg
#2 no target
```

`WARN: no valid frame in 1 s` means no frame has passed the header/tail check — usually
TX/RX swapped, wrong baud, or no 5 V.

## Frame conventions used here (radar's own frame)

- x = lateral, y = forward, both in mm; speed in cm/s.
- Signed fields use a **sign bit**, not two's complement: bit15 = 1 → positive, bit15 = 0 → negative.
- `ang` = `atan2(x, y)`: 0° straight ahead, positive to the **right** (follows x sign).
- An all-zero 8-byte slot is an empty target and is skipped.

These differ from the robot frame in `docs/interfaces.md` (x forward, y left, angle positive to
the left). The conversion belongs in the real driver, not in this sketch.
