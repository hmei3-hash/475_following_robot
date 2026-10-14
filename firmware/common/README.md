# common — shared types, config, logging

Owners: **All**

## Contents (to create)

- Shared structs from `docs/interfaces.md` (one header everyone includes).
- `config` — all pin assignments and tunable constants (tone frequency, follow distance,
  timeouts, gains) in one place.
- Logging/telemetry helpers: one-line CSV-style messages for `tools/`.

## Rule

Changing a shared type = update `docs/interfaces.md` first, then tell the team.

## Pin map (fill in once the board is chosen)

| Signal | ESP32-S3 GPIO | Notes |
| --- | --- | --- |
| Radar UART TX/RX | | |
| Mic CLK | | shared by all mics |
| Mic DATA 0 | | mics with LR = GND / Vdd |
| Mic DATA 1 | | if 4 mics |
| Motor L PWM / DIR | | |
| Motor R PWM / DIR | | |
| Kill switch | | |
