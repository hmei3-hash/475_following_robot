# control — follow controller, motor driver, safety

Owner: **Bryan Zhang**

## Job

Turn `follow_target_t` into wheel commands that keep the user at a set distance and straight ahead,
and stop safely when anything goes wrong.

## Spec

1. **Motor driver layer** — PWM + direction pins for left/right motors (pins from `common/` config).
   Input: normalized wheel commands −1.0…1.0 and an enable flag.
2. **Follow controller** — two loops for a differential base:
   distance error (range − setpoint) → forward speed; angle error → turn rate.
   Start with P control, add I/D only if needed. Mix into left/right wheel commands.
3. **Limits** — max speed, max acceleration (ramp), deadband near setpoint.
4. **Safety** — stop on `LOST`, on too-close range, on stale input (watchdog), and on kill switch.
5. **Manual mode** — drive by commands from serial/BLE for testing without sensors.

## Design decisions to make

- Base: reused platform vs fallback two-wheel differential base (see `mechanical/`).
- Encoders available? (closed-loop wheel speed vs open-loop PWM).
- Follow distance setpoint (proposal says ~1 m) and max speed.

## Pitfalls

- Motor noise couples into audio (electrical and acoustic) — keep wiring apart, add filtering.
- Motor current spikes can brown out the ESP32-S3 — separate supply/regulator.
- Turning in place makes radar targets swing quickly; slow down turn rate.
- Never leave motors running if the control task crashes (watchdog → motors off).

## Tests

- [ ] Manual drive: forward, back, turn
- [ ] Step response: target at fixed 2 m, setpoint 1 m
- [ ] Safety: unplug a sensor → robot stops
