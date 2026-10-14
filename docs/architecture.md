# System Architecture

## Block diagram

```
                 ┌──────────────────────── ESP32-S3 ───────────────────────────┐
 Phone tone ──▶ Mic array ─PDM/I2S─▶ [audio]  tone level + bearing ─┐          │
                 │                                                   ▼          │
 Person ◀─24GHz─▶ RD-03D ───UART───▶ [radar]  target list ───────▶ [fusion] ─▶ [control] ─PWM/DIR─▶ Motor driver ─▶ Base
                 │                                                   ▲          │
                 │                         (optional) [vision] ──────┘          │
                 └──────────────────────────────────────────────────────────────┘
                                         │
                                  [telemetry] ─USB serial / BLE─▶ PC or phone dashboard
```

## Modules

| Module | Input | Output | Owner |
| --- | --- | --- | --- |
| `radar` | RD-03D UART bytes | `radar_frame_t` (list of targets) | Hongyi |
| `audio` | PDM samples from mics | `audio_bearing_t` (tone present, bearing, confidence) | Franklin, Rui |
| `fusion` | latest radar frame + audio bearing | `follow_target_t` (chosen target or "lost") | All |
| `control` | `follow_target_t` | motor commands | Bryan |
| `main` | — | starts tasks, owns queues, watchdog | All |
| `telemetry` (in `common`) | any module | serial/BLE log lines for debugging | All |

Shared data types and units are defined in [`interfaces.md`](interfaces.md). Every module must
compile and be testable on its own before integration.

## Runtime model (proposed)

ESP32-S3 has two cores and runs FreeRTOS (both ESP-IDF and Arduino-ESP32 use it).
Proposed task split — to be confirmed once the framework is chosen:

| Task | Rate | Notes |
| --- | --- | --- |
| audio | continuous DMA, bearing every ~50 ms | heaviest CPU load; consider pinning to its own core |
| radar | per UART frame | parse in a task, not in the ISR |
| fusion + control | fixed rate (e.g. 20–50 Hz) | reads latest data; never blocks on a sensor |
| telemetry | low rate | must not slow down control |

Modules exchange data through FreeRTOS queues or "latest value" mailboxes, each with a timestamp,
so fusion can tell when a sensor's data is stale.

## Coordinate frame

Robot frame: origin at the radar, **x forward, y left, angle positive counter-clockwise (left)**,
distances in meters, angles in degrees. Every module converts to this frame before publishing.
The mic array and radar must be mounted so their zero-angle directions agree (or the offset is
measured and stored in config).

## Safety behavior

- Tone lost or target lost for longer than a timeout → stop.
- Target closer than a minimum distance → stop.
- No new fusion output within a watchdog period → motors off.
- A physical kill switch on the robot.

## Radar-driven constraints (from RD-03D spec, see `hardware/rd-03d.md`)

- **Sensor roles:** radar covers only ±60° azimuth, so audio is the wide-angle "search" sensor and
  radar is the narrow, precise "track" sensor. If the user leaves the radar's view, fusion goes to
  `SEARCHING` and control turns toward the audio bearing until radar re-acquires.
- **Power:** radar needs a clean 5 V (≤ 100 mV ripple, ≥ 200 mA peak). Motors on the same battery
  create large ripple → give the radar its own LDO/regulator, separate from the motor supply.
  Power tree: battery → motor driver; battery → 5 V regulator → radar; → 3.3 V → ESP32-S3 + mics.
- **UART:** 3.3 V levels, direct to ESP32-S3; 256000 bps default. Use the hardware UART with a
  driver RX buffer; parse in a task.
- **Resolution:** 0.75 m range resolution — two people at similar distance may merge into one
  target. Fusion should use angle as well as range, and audio to break ties.
- **Moving platform risk:** the module is designed for a fixed wall mount. On a moving robot,
  static objects move relative to the radar and may appear as targets; vibration also hurts.
  Test the radar on the moving base early (week 3–4), before building fusion on top of it.
- **Mounting:** rigid, unobstructed front, no metal near the antenna, optional metal shield behind
  (back lobe sees the robot's own moving parts). Mount height vs ±30° elevation — check floor clutter.
- **Angle sign:** depends on how the module is rotated when mounted; record the mapping to the
  robot frame (positive = left) once the mount is fixed.
