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
