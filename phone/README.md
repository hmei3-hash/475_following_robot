# phone — tone beacon

Owners: audio team (with input from all)

## Job

Make the user's phone play the beacon tone reliably.

## Options

- Simplest: a web page (Web Audio API) that plays a fixed-frequency tone — works on any phone.
- Native app later, if we add BLE control/status (stretch goal).

## Decisions

- Frequency and waveform must match `firmware/audio/` (record the choice in `docs/decisions.md`).
- Volume: loud enough to detect at the target range, but tolerable for people nearby.

## Pitfalls

- Phones may reduce volume or stop audio when the screen locks.
- Phone speaker response at high frequencies varies between models — test several phones.
