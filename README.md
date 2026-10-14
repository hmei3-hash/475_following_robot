# Sound-Guided Human-Following Robot

CSE/EE 475 Embedded Systems Capstone · Autumn 2026 · Team 7

A low-cost robot that follows one specific person, using their phone as a beacon.
The phone plays a tone at a known frequency; a microphone array finds the direction
of the sound; a 24 GHz FMCW radar (RD-03D) measures distance and angle to that person.
An ESP32-S3 fuses both and drives the base to stay a set distance behind the user.

## Team

| Member | Area | Main folders |
| --- | --- | --- |
| Hongyi Mei | Radar ranging and tracking (RD-03D) | `firmware/radar/` |
| Franklin Fu | Audio: mic array, tone detection | `firmware/audio/` |
| Rui Kong | Audio: direction finding, signal processing | `firmware/audio/` |
| Bryan Zhang | 3D modeling, mechanical, motion control | `mechanical/`, `firmware/control/` |
| All | Integration, fusion, testing, website | `firmware/fusion/`, `firmware/main/`, `test/` |

## System at a glance

```
 User + phone ──tone──▶ Mic array (2–4 × MP34DT01-M) ──PDM/I2S──▶ ┌──────────────┐
      ▲                                                            │   ESP32-S3   │
      └──── 24 GHz ────▶ RD-03D radar ──────────UART─────────────▶ │ audio · radar│──PWM──▶ Motor driver ─▶ Base
                                                                   │ fusion · ctrl│
                                     (optional) RPi 5 + camera ──▶ └──────────────┘
```

Sound decides **which** person to follow; radar measures **where** that person is.
See [`docs/architecture.md`](docs/architecture.md) and [`docs/interfaces.md`](docs/interfaces.md).

## Repository layout

```
475_following_robot/
├── docs/                 Proposal, architecture, interfaces, decisions, budget
│   ├── hardware/         Part notes (mics, radar, base)
│   └── notebook/         Weekly lab notebook entries
├── firmware/             ESP32-S3 firmware, one folder per module
│   ├── main/             Startup, task scheduling, integration
│   ├── radar/            RD-03D UART driver + target tracking      (Hongyi)
│   ├── audio/            PDM capture, tone detection, bearing       (Franklin, Rui)
│   ├── fusion/           Match radar target to sound bearing        (all)
│   ├── control/          Follow controller + motor driver + safety  (Bryan)
│   └── common/           Shared types, config, logging
├── mechanical/           CAD, 3D-print files, mounting             (Bryan)
├── phone/                Tone generator (app or web page)
├── vision/               Optional RPi 5 + camera (stretch goal)
├── tools/                PC-side scripts: serial logging, plotting
└── test/                 Test plans and results per module
```

## Development order

1. Each module works **alone** on the bench (radar prints targets, audio prints bearing, base drives by command).
2. Modules talk through the shared types in `docs/interfaces.md`.
3. Fusion: pick the radar target in the sound direction.
4. Closed-loop following, then safety behavior, then stretch goals.

## Key dates

| Date | Deliverable |
| --- | --- |
| Oct 13 | Project proposal on website |
| Oct 22 | PRD on website |
| Oct 29 | Rapid prototypes |
| Nov 3 | Status presentation |
| Dec 10 | Final demo and presentation |
| Dec 15 | Final report (ACM format) |

## Status

Early planning. Open decisions are tracked in [`docs/decisions.md`](docs/decisions.md).
