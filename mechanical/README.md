# mechanical — base, mounts, enclosure

Owner: **Bryan Zhang**

## Job

Reuse an existing robot platform and design 3D-printed mounts for the electronics and sensors.

## Spec

- Radar mount: facing forward, rigid, at roughly person-torso height if possible; nothing metal in front of it.
- Mic array mount: fixed, known geometry (spacing from `firmware/audio/`), sound ports open, away from motors; same zero direction as the radar.
- Electronics bay: ESP32-S3, motor driver, battery, accessible USB port and kill switch.
- Vibration isolation for mics if motor noise is a problem.

## TODO

- [ ] Identify and measure the reused platform (size, motors, payload)
- [ ] Decide fallback base if it doesn't fit
- [ ] CAD: radar mount, mic array plate, electronics bay
- [ ] Print, fit, iterate

Store CAD source files and exported STLs here (e.g. `cad/`, `stl/`).
