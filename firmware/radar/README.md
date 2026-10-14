# radar — RD-03D driver and target tracking

Owner: **Hongyi Mei**

## Job

Read RD-03D UART frames, turn them into `radar_frame_t` (see `docs/interfaces.md`), and keep
stable IDs for targets across frames so fusion can follow "the same person".

## Spec

1. **UART driver** — configure the ESP32-S3 UART for the RD-03D (baud, pins from `common/` config).
   Receive bytes with a buffer/ring buffer; never parse inside the ISR.
2. **Frame parser** — byte-by-byte state machine: wait for header → collect fixed-length payload →
   check footer → decode fields. A bad frame is dropped and counted, never crashes the parser.
3. **Decode** — convert raw fields to meters / degrees / m/s in the robot frame. Document the
   sign convention exactly as the protocol defines it.
4. **Config commands** (if needed) — switch single/multi-target mode at startup.
5. **Tracking** — associate targets between frames (e.g. nearest-neighbor on position) and
   smooth position (moving average or simple filter). Output a stable ID per track.
6. **Publish** — latest `radar_frame_t` with timestamp.

## Design decisions to make

- Raw frame parsing vs. relying on the module's own tracking IDs.
- Filter type for smoothing (moving average, alpha-beta, Kalman) — start simple.
- How long a track survives without being seen.

## Pitfalls

- Frame boundaries: the parser must resync if it starts mid-frame.
- Signed fields may use a sign bit instead of two's complement — check the protocol doc.
- Logic level of the module's UART vs ESP32-S3 3.3 V I/O.
- Empty target slots must not be treated as a target at (0, 0).
- Static reflections (walls) may show up as targets.

## Bench tests

- [ ] Print raw bytes, identify header/footer by eye
- [ ] Parser unit test with recorded frames (including corrupted and partial frames)
- [ ] Range accuracy at known distances
- [ ] Angle accuracy at known angles
- [ ] Two people: IDs stay stable when they walk past each other

## TODO

- [ ] Fill in `docs/hardware/rd-03d.md` from the datasheet/protocol doc
- [ ] UART bring-up
- [ ] Parser
- [ ] Unit conversion + frame
- [ ] Tracking + smoothing
- [ ] Telemetry output for `tools/` plotter
