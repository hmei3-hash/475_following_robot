# RD-03D 24 GHz Radar — Key Notes

Source: Ai-Thinker *Rd-03D Specification* V1.0.0 (2023-08-31).
Ai-Thinker RD-03D: 24 GHz FMCW mmWave module (S5KM312CL SoC, 1 TX / 2 RX, max sweep bandwidth 0.25 GHz).
Reports multiple tracked targets over UART.

## Electrical

| Item | Value | Note |
| --- | --- | --- |
| Supply | 5 V single supply | |
| Current | 92 mA average; supply must handle ≥ 200 mA peak | |
| Supply ripple | ≤ 100 mV | LDO recommended; DC-DC only with ripple control |
| I/O level | 3.3 V | direct connect to ESP32-S3, no level shifter |
| VOL / VOH | ≤ 0.33 V / ≥ 2.7 V | |
| Interface | UART | default **256000 bps** |
| Reference circuit | 100 nF + 10 µF on 5 V; 10 kΩ pull-ups to 3.3 V on TX and RX | 10–100 Ω series resistors on I/O suggested for EMI/ESD |
| Connector | 1×4P, 1.25 mm pitch: 5V, GND, TX, RX | also 7 pads: RX TX GND DEBUG DP DM 5V |
| Size | 15.0 × 44.0 mm | |

## Sensing

| Item | Value |
| --- | --- |
| Max range | 8 m (vendor open-field test, wall-mounted at 1.5–2 m) |
| Range resolution | 0.75 m |
| Ranging accuracy | 0.15 m |
| Field of view | azimuth ±60°, elevation ±30° |
| Angle reference | 0° = antenna-plane normal; sign of "positive angle" depends on mounting orientation (spec Fig. 9 a/b) |
| Configurable (vendor tool) | detection range, reporting interval, target retention time |

## UART data protocol

The specification does not include the protocol; the values below are what the bench sketch
(`firmware/radar/bench/rd03d_uart_test/`) implements. Re-check against Ai-Thinker's RD-03D
protocol document.

| Item | Value |
| --- | --- |
| Frame | `AA FF 03 00` · 24-byte payload · `55 CC` (30 bytes) |
| Max targets per frame | 3, 8 bytes each |
| Per-target fields | x (int16), y (int16), speed (int16), distance resolution (uint16), all little-endian |
| Units | x, y in mm; speed in cm/s; resolution in mm |
| Sign encoding | sign bit, not two's complement: bit15 = 1 → positive, 0 → negative; magnitude = low 15 bits |
| Empty slot | all 8 bytes zero |
| Axes | x = lateral, y = forward (radar's own frame) |
| Default mode at power-on | single-target |
| Multi-target commands | enter config `FD FC FB FA 04 00 FF 00 01 00 04 03 02 01` → multi-target `FD FC FB FA 02 00 90 00 04 03 02 01` → exit config `FD FC FB FA 02 00 FE 00 04 03 02 01` |
| Report rate | TODO (measure) |
| Minimum detection range | TODO (measure) |

## Installation notes from the spec

- Antenna faces the scene with nothing in front; no metal parts near the antenna.
- Mount rigidly — vibration degrades detection.
- The back lobe can see moving objects behind the radar; a metal shield/backplane behind it helps.
- Moving non-human objects (fans, curtains, plants) and large strong reflectors degrade results.
- Keep away from other 24 GHz radars.

## Things to measure on the bench

- Range and angle error at known positions (0.5–3 m, every 15°).
- Two people closer than 0.75 m in range: one target or two?
- Behavior while the radar itself is moving (on the robot) — see `docs/architecture.md` risks.
- Static clutter (walls, furniture) and floor reflections at the robot's mount height.
