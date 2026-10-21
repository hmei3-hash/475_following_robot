# main — startup and integration

Owners: **All**

## Job

Initialize hardware and modules, create FreeRTOS tasks and queues, run the watchdog.

## Spec

- Init order: config/logging → motors (disabled) → radar → audio → fusion → control → enable.
- One task per module (see rates in `docs/architecture.md`); pin audio to its own core if CPU-bound.
- Build flags / config to run each module alone (e.g. radar-only test mode).
- Watchdog: motors off if the control loop stops updating.

## Bench sketch

[`bench/ota_test/`](bench/ota_test/) — bootloader + OTA test only: prints the running/boot/next
app slot and their OTA state at boot, and accepts firmware over Wi-Fi (ArduinoOTA).

## TODO

- [ ] Choose framework and add build files
- [ ] Test-mode switches for each module
- [ ] Task/queue setup
