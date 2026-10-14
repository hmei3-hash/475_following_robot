# MP34DT01-M MEMS Microphone — Key Notes

Source: ST datasheet DocID026514 Rev 3 (Sep 2014). Check the datasheet for anything not listed here.

## Electrical

| Parameter | Value |
| --- | --- |
| Output | PDM (1-bit) on DOUT |
| Supply Vdd | 1.64 – 3.6 V (typ 1.8 V); run at 3.3 V with the ESP32-S3 |
| Current | ~0.6 mA normal, 20 µA power-down |
| Clock input | 1 – 3.25 MHz (typ 2.4 MHz), duty cycle 40–60 % |
| Power-down | clock below 0.23 MHz |
| Turn-on time | 10 ms from first clock edge to valid data |
| Logic levels | low ≤ 0.35·Vdd, high ≥ 0.65·Vdd |
| Max input on control pins | Vdd + 0.3 V |

## Acoustic

| Parameter | Value |
| --- | --- |
| Sensitivity | −26 dBFS (−29 to −23) |
| SNR | 61 dB (A-weighted, 1 kHz) |
| Acoustic overload | 120 dBSPL |
| Frequency response | Datasheet plot (normalized at 1 kHz) is flat from ~100 Hz to ~5 kHz, rising a few dB toward 10 kHz. **The plot stops at 10 kHz** — response at near-ultrasonic frequencies (17–20 kHz) is not specified and must be measured. |

## Pins

| Pin | Name | Function |
| --- | --- | --- |
| 1 | Vdd | Supply |
| 2 | LR | Channel select (tie to Vdd or GND — never float) |
| 3 | CLK | Clock input |
| 4 | DOUT | PDM data out |
| 5 | GND | Ground ring |

## Stereo / channel select

Two mics can share one CLK and one DOUT line:

| LR pin | Data valid when |
| --- | --- |
| GND | CLK low |
| Vdd | CLK high |

The other half-cycle DOUT is high-impedance. So one data line carries two mics; 4 mics need 2 data lines.

## Design notes and pitfalls

- 100 nF + 10 µF ceramic decoupling as close as possible to pin 1.
- Top-port package: the sound hole must face the enclosure opening; don't block it.
- ESD- and shock-sensitive part.
- For direction finding, **all mics must share the same clock** so their samples are time-aligned;
  check that the ESP32-S3 I2S PDM mode supports the number of channels you need.
- Mic spacing sets the maximum unambiguous frequency for phase-based methods
  (spacing should be less than half the wavelength; at 343 m/s, 4 kHz → λ ≈ 8.6 cm).
