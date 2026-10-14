# audio — PDM capture, tone detection, bearing estimation

Owners: **Franklin Fu**, **Rui Kong**

## Job

Capture audio from 2–4 MP34DT01-M microphones, decide whether the phone's tone is present,
and estimate which direction it comes from. Publish `audio_bearing_t` (see `docs/interfaces.md`).

## Spec

1. **Capture** — ESP32-S3 I2S in PDM receive mode, DMA buffers, all mics on the same clock.
   PDM → PCM conversion (hardware filter if available for your channel count, otherwise software).
2. **Tone detection** — narrow-band detection at the known tone frequency
   (e.g. Goertzel or a single FFT bin), compare against the noise floor → `tone_present`.
3. **Bearing** — estimate direction using differences between mics:
   time difference of arrival (cross-correlation / phase difference at the tone frequency)
   and/or level difference. Output `bearing_deg` and a `confidence`.
4. **Publish** — latest result every analysis window, with timestamp.

## Design decisions to make

- Tone frequency (shared decision with `phone/`) — check the mic response (`docs/hardware/mp34dt01-m.md`).
- Number of mics and geometry (line vs triangle vs square) and spacing.
- Analysis window length: longer = more stable, shorter = faster reaction.
- Sample rate after decimation.

## Pitfalls

- Mic spacing vs wavelength: phase methods become ambiguous when spacing > λ/2.
- Two mics on a line cannot tell front from back — need a third mic or radar to resolve it.
- Motor noise and reflections from walls; test with motors running.
- Check how many PDM channels the ESP32-S3 I2S peripheral actually supports at once.
- Mic turn-on time (10 ms) and DC offset after PDM conversion.
- CPU budget: this is the heaviest module; measure time per window.

## Bench tests

- [ ] Record raw PCM from one mic, view spectrum on PC (`tools/`)
- [ ] Tone detection vs distance and background noise
- [ ] Bearing error at known angles (e.g. every 15°)
- [ ] Behavior with motors running
- [ ] CPU time per window

## TODO

- [ ] One-mic PDM capture
- [ ] Multi-mic synchronized capture
- [ ] Tone detector
- [ ] Bearing estimator
- [ ] Telemetry output
