# Decisions Log

## Decided

| Date | Decision | Notes |
| --- | --- | --- |
| 2026-10-13 | Main MCU: ESP32-S3 | Dual core, I2S with PDM input, BLE |
| 2026-10-13 | Ranging: Ai-Thinker RD-03D 24 GHz FMCW radar | UART output: x/y/distance/angle/speed per target |
| 2026-10-13 | Microphones: ST MP34DT01-M (PDM) | See `hardware/mp34dt01-m.md` |
| 2026-10-13 | No FPGA in this project | |
| 2026-10-13 | Budget: $200 | Reuse an existing robot platform |

## Open

- [ ] Firmware framework: ESP-IDF vs Arduino-ESP32 vs PlatformIO (affects folder layout and build)
- [ ] Tone design: frequency (audible vs near-ultrasonic), pure tone vs coded/chirped signal
- [ ] Number of microphones (2–4) and array geometry/spacing
- [ ] Which platform to reuse; its motors, encoders, and driver
- [ ] Power: battery type, voltages needed (ESP32-S3 3.3 V, radar supply, motors)
- [ ] Whether to include RPi 5 + camera (stretch)
- [ ] Phone side: native app vs web page for the tone
- [ ] Custom PCB or perfboard
- [ ] Team roles for course chores (leader, treasurer, schedule manager)
