# Module Interfaces (data contract)

This file is the agreement between modules. Each owner implements their own module in
`firmware/<module>/`; the shared types eventually live in `firmware/common/`. Change this file
**before** changing a shared type, and tell the team.

Units everywhere: meters, degrees, m/s, milliseconds. Frame: see `architecture.md`
(x forward, y left, angle positive to the left).

## radar → fusion

`radar_frame_t` — one parsed RD-03D frame.

| Field | Meaning |
| --- | --- |
| `timestamp_ms` | when the frame finished arriving |
| `num_targets` | number of valid targets in this frame |
| `targets[]` | fixed-size array (size = module's max target count, confirm in datasheet) |
| `targets[i].x_m`, `y_m` | position in robot frame |
| `targets[i].range_m` | distance from radar |
| `targets[i].angle_deg` | bearing from radar boresight |
| `targets[i].speed_mps` | radial speed (sign convention: document it) |
| `targets[i].valid` | slot holds a real target |

## audio → fusion

`audio_bearing_t` — latest tone estimate.

| Field | Meaning |
| --- | --- |
| `timestamp_ms` | end of the audio window used |
| `tone_present` | tone detected above threshold |
| `bearing_deg` | estimated direction of the tone |
| `confidence` | 0–1, how much to trust the bearing |
| `level_db` | tone level (optional, useful for debugging and rough distance) |

## fusion → control

`follow_target_t`

| Field | Meaning |
| --- | --- |
| `timestamp_ms` | when the decision was made |
| `state` | `TRACKING`, `SEARCHING`, or `LOST` |
| `range_m` | distance to chosen target |
| `angle_deg` | bearing to chosen target |
| `radar_id` | which radar slot was chosen (for debugging) |

## control → motor driver

Left/right wheel commands (normalized −1.0 … 1.0) plus an `enable` flag. The mapping to PWM and
direction pins lives only inside `firmware/control/`.

## Telemetry

One line per message over serial, easy to parse on a PC, e.g. CSV with a type prefix:
`RAD,<t>,<id>,<x>,<y>,...` · `AUD,<t>,<present>,<bearing>,<conf>` · `FUS,<t>,<state>,<range>,<angle>`.
Exact format is decided when `tools/` is written.
