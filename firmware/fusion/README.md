# fusion — choose which radar target is the user

Owners: **All** (integration module)

## Job

Combine the latest `radar_frame_t` and `audio_bearing_t` into one `follow_target_t`:
which radar target is the person with the phone, and where they are.

## Spec

1. **Lock on** — when the tone is present, pick the radar track whose angle is closest to the
   audio bearing (within a gate). Remember its track ID.
2. **Keep tracking** — while locked, follow that radar track by ID even if the audio bearing is
   noisy for a moment. Use audio to confirm, not to steer.
3. **Re-lock** — if the locked track disappears, or audio consistently disagrees, go to `SEARCHING`.
4. **Lost** — no tone or no matching target for longer than a timeout → `LOST` (control stops).
5. **Staleness** — ignore sensor data older than a set age.

## Design decisions to make

- Angle gate width and timeouts (tune on real data).
- Whether to use audio level as a rough distance check.
- State machine diagram: put it in this folder before writing code.

## Pitfalls

- Radar and mic array angles must use the same zero direction (mounting offset).
- Front/back ambiguity of the audio bearing.
- Avoid flipping between two people standing close together (hysteresis).

## Tests

- [ ] Offline: replay logged radar + audio data on PC and check decisions
- [ ] Two people, one phone: correct person chosen
- [ ] User walks behind an obstacle / stops tone → LOST within timeout
