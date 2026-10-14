# tools — PC-side helpers

Scripts that run on a laptop, not on the robot.

## Planned

- Serial logger: save telemetry lines from the ESP32-S3 to CSV.
- Live plotter: radar targets (x/y scatter) + audio bearing arrow, top-down view.
- Offline replay: feed logged radar + audio data into a copy of the fusion logic for tuning.
- Audio viewer: spectrum of recorded PCM to check the tone and noise.

Message format: see `docs/interfaces.md` → Telemetry.
