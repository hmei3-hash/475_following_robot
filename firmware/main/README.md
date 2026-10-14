# main — startup and integration

Owners: **All**

## Job

Initialize hardware and modules, create FreeRTOS tasks and queues, run the watchdog.

## Spec

- Init order: config/logging → motors (disabled) → radar → audio → fusion → control → enable.
- One task per module (see rates in `docs/architecture.md`); pin audio to its own core if CPU-bound.
- Build flags / config to run each module alone (e.g. radar-only test mode).
- Watchdog: motors off if the control loop stops updating.

## TODO

- [ ] Choose framework and add build files
- [ ] Test-mode switches for each module
- [ ] Task/queue setup
