# vision — optional RPi 5 + camera (stretch goal)

Status: **not decided**. Only start after the core robot follows reliably.

## Possible use

- Visually confirm that the chosen radar target is a person.
- Obstacle detection.

## Interface idea

RPi 5 sends a small message to the ESP32-S3 over UART (e.g. "person at angle X, confidence C"),
which fusion can use as an extra check. Keep it optional: the robot must work without it.
