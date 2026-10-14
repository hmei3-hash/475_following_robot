# Firmware (ESP32-S3)

One folder per module. Each module:

- has a README with its spec, interface, test plan, and TODO list;
- exposes a small public header (init + get-latest-data style API);
- can be built and tested **alone** with a small bench test before integration;
- only talks to other modules through the types in [`../docs/interfaces.md`](../docs/interfaces.md).

| Folder | Purpose | Owner |
| --- | --- | --- |
| `main/` | startup, task creation, queues, watchdog | All |
| `common/` | shared types, config constants, logging/telemetry | All |
| `radar/` | RD-03D UART driver + target tracking | Hongyi |
| `audio/` | PDM capture, tone detection, bearing estimate | Franklin, Rui |
| `fusion/` | choose which radar target is the user | All |
| `control/` | follow controller, motor driver, safety stop | Bryan |

Framework (ESP-IDF / Arduino-ESP32 / PlatformIO) is still open — see `docs/decisions.md`.
Once chosen, add the build files here and update this README with build/flash commands.

## Conventions

- Units and frame: meters, degrees, m/s, ms; x forward, y left.
- Every published struct carries `timestamp_ms`.
- No module blocks another: publish "latest value", consumers check age.
- Pin numbers and tunable constants live in `common/` config, not scattered in modules.
- Branch per feature (`radar/uart-parser`, `audio/goertzel`, …), merge to `main` via pull request.
