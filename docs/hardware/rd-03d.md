# RD-03D 24 GHz Radar — Key Notes

Ai-Thinker RD-03D: 24 GHz FMCW mmWave module (S5KM312CL SoC, 1 TX / 2 RX).
Outputs tracked targets over UART: x, y, distance, angle, speed.

> TODO (Hongyi): fill in from the official datasheet / protocol document — do not guess these values.

| Item | Value |
| --- | --- |
| Supply voltage | TODO |
| UART logic level | TODO (confirm 3.3 V compatible with ESP32-S3) |
| Baud rate | TODO |
| Frame header / footer bytes | TODO |
| Max targets per frame | TODO |
| Field encoding (sign bit, units) | TODO |
| Update rate | TODO |
| Detection range | TODO |
| Field of view (azimuth) | TODO |
| Single- vs multi-target mode command | TODO |

## Things to measure on the bench

- Range error vs true distance (0.5 m, 1 m, 2 m, 3 m …).
- Angle error vs true angle across the field of view.
- What happens with two people at similar range.
- Static clutter (walls, furniture): does it appear as targets?
- Effect of the robot's own motion and vibration.
