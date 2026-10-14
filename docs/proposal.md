# Project Proposal: Sound-Guided Human-Following Robot

CSE/EE 475 · Team 7 · Oct 13, 2026

## Overview

We propose a low-cost robot that follows one specific person, using their phone as the beacon. The phone plays a tone at a known frequency; a microphone array on the robot finds which direction the sound comes from. A 24 GHz FMCW radar (RD-03D) then measures the distance and angle to that person with higher resolution than audio alone. An ESP32-S3 combines both sensors and drives the robot base to stay a set distance behind the user. The target budget is $200.

## Problem space and applications

People often need to move loads while keeping their hands free: groceries, luggage, tools, lab equipment, or supplies for someone with limited mobility. A robot that simply follows its user removes the need to push or carry. The hard part is reliably knowing *who* to follow and *where* they are.

Common approaches each have gaps:

- **Camera-only tracking** can lose or swap the target in crowds, in poor lighting, or when the person turns away, and it raises privacy concerns.
- **GPS** is too coarse for close following and does not work well indoors.
- **Dedicated tags** (e.g. UWB) work well but require extra hardware the user must carry.

Our approach uses something the user already has, a phone, as the beacon. Sound tells the robot which person is the user; radar gives precise distance and angle and works in the dark without recording images. Applications include shopping and luggage carts, carrying equipment across a campus or worksite, and assistive carriers for people who cannot easily push a cart.

## Example scenarios

1. **Shopping or grocery run.** A user opens the app, starts the tone, and walks through a store. The cart-robot follows about 1 m behind, slows when they stop, and turns with them at aisle ends.
2. **Carrying lab or event equipment.** A student moves gear between buildings. Other people walk nearby, but the robot keeps following only the person whose phone is emitting the tone.
3. **Assistive carrier.** A person using crutches or a walker cannot push a cart. The robot carries their bag and keeps pace without them needing to steer it.

In every scenario, if the tone stops or the user moves out of range, the robot stops in place instead of wandering.

## Related technology

- **Follow-me carts and suitcases** already exist commercially, mostly using UWB tags, cameras, or both. They show demand but rely on a dedicated tag or vision.
- **Vision-based person following** (camera + person detection) is common in research robots; it needs significant compute and struggles with re-identifying the same person.
- **Acoustic source localization** with microphone arrays (time-difference-of-arrival or beamforming) is used in smart speakers and conferencing to find who is talking.
- **Low-cost mmWave radar modules** such as the RD-03D now report multiple targets' distance, angle, and speed over a simple UART interface, making radar practical for small robots.

Our project combines the last two: acoustic direction finding to pick the user, and radar to track them precisely.

## Potential design solutions

The robot splits the job between two sensors: sound decides *which* person to follow, and radar measures *where* that person is. The ESP32-S3 reads the microphones and the radar, picks the radar target that lies in the sound direction, and steers the base to hold a set distance.

- **Beacon:** the phone plays a tone at a fixed frequency. An audible tone is easiest to detect; a near-ultrasonic tone is less noticeable but phone speakers and microphones may respond weakly.
- **Direction finding:** detect the tone with a narrow filter, then compare arrival time or loudness across 2–4 microphones.
- **Ranging and tracking:** the RD-03D reports distance, angle, and speed for multiple targets; we follow the one matched to the sound direction.
- **Control:** a simple controller holds distance and heading; the robot stops if the tone or the target is lost.
- **Platform:** reuse an existing base with a 3D-printed sensor mount; a two-wheel differential base is the fallback.
- **Optional vision:** a Raspberry Pi 5 with a camera could visually confirm the user if time allows.

## Team and roles

| Member | Primary area |
| --- | --- |
| Hongyi Mei | Radar ranging and tracking (RD-03D) |
| Franklin Fu | Audio: microphone array and tone detection |
| Rui Kong | Audio: direction finding and signal processing |
| Bryan Zhang | 3D modeling, mechanical design, and motion control |

All members share ESP32-S3 firmware integration, sensor fusion testing, and the project website.

## Resources needed and budget

See [`budget.md`](budget.md). Target total: $200, reusing an existing robot platform.

Other resources: the CSE 003E lab and 3D printers, a phone app or web page that plays the tone, and possibly a Raspberry Pi 5 with camera (only for the optional vision feature, using hardware we already have).

## Final demo

1. A user starts the tone on their phone; the robot turns toward them and begins following.
2. The user walks a path with turns; the robot keeps a steady following distance (target around 1 m).
3. The user stops; the robot stops without bumping into them.
4. A second person walks nearby; the robot keeps following the user with the phone, not the other person.
5. The user stops the tone or walks out of range; the robot halts safely.

A live display (serial or phone dashboard) will show the radar targets and the estimated sound direction. Exact accuracy and range targets will be set in the PRD.

**Stretch goals:** obstacle avoidance, a phone app that also controls the robot over Bluetooth, and a Raspberry Pi 5 camera to visually confirm the user.

## Risks and open questions

| Risk | Mitigation |
| --- | --- |
| Echoes, crowd noise, and motor noise reduce sound-direction accuracy | Narrow band-pass at the tone frequency; use radar as the main position source and audio only to pick the target |
| Radar sees several people or reflections | Match radar targets to the sound direction; track the chosen target over time |
| Reused platform's motors or size don't fit | Inspect the platform early; fall back to a simple two-wheel differential base |
| Sensor fusion takes longer than planned | Demo each sensor working alone first, then combine |

Open decisions: see [`decisions.md`](decisions.md).
