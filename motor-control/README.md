# DC Motor Control — STM32

Four-button control of a brushed DC gearmotor: speed up, speed down, reverse, emergency stop.

---

## Control

| Input | Effect |
|---|---|
| Speed up / down | PWM duty cycle in 25% increments |
| Reverse | Sequenced **stop → change direction → start** |
| Stop | Immediate halt |

Reversal is deliberately not a live switch. Reversing a spinning motor puts its back-EMF across the driver; sequencing through a stop state avoids that.

## Indication

- **4 LEDs** — one per 25% speed increment
- **1 RGB LED** — current direction

---

## Hardware

| Function | Part |
|---|---|
| Controller | NUCLEO-L432KC |
| Dev board | Blume Nucleo development board |
| Driver | TB6612FNG H-bridge |
| Motor | GB37Y3530 gearmotor |
| Power | 2000 mAh, 7.4 V, 15C (14.8 Wh) |
| Input | 4 push buttons |

---

## TODO

- [ ] Add firmware to `src/`
- [ ] Wiring diagram
- [ ] Add the demo video — 35 seconds of it running
