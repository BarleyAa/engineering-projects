# Engineering Projects — Agris Miezītis

Electrical Engineering student at Riga Technical University, working in embedded systems and automation. This repository collects my hardware and firmware projects: source, design files and documentation.

📍 Riga, Latvia · [LinkedIn](https://www.linkedin.com/in/agris-miez%C4%ABtis-199a5a404/)

---

## Projects

### ⚡ [AC/DC Power Meter](power-meter/)

An instrument that measures voltage and current on AC or DC loads and derives the full power triangle in firmware — including whether the load is inductive or capacitive.

Register-level AVR: the ADC is driven from a timer interrupt and alternates channels inside the ADC ISR, so voltage and current are sampled back-to-back and their phase relationship is preserved.

`Arduino / AVR C` · `ACS712` · `op-amp front end` · **complete**

---

### 🌱 [Automated Growhouse](growhouse/)

A greenhouse that waters, ventilates and lights itself. Three independent control loops on one Arduino, spanning 5 V logic, a 12 V fan and 230 V mains through a relay — with an irrigation state machine and a 2 °C hysteresis band on the temperature control.

`Arduino` · `3D printing` · `mixed-voltage` · *firmware to be added*

---

### 🔌 [NE555 Timer Relay — PCB](timer-relay/)

A single-layer through-hole board for an adjustable-delay timer relay, designed in Altium. Every drill diameter derived from lead geometry and plating thickness; every trace sized from current density.

`Altium Designer` · `22 components` · *design files to be added*

---

### 🚪 [SmartTurn — Retrofit Smart Lock](smartturn/)

A device that makes an ordinary interior door lock smart without modifying the door. €20–30 unit cost against €150–500 commercial equivalents. Team project — I owned hardware, firmware and mechanical design.

`ESP32` · `servo` · `3D printing` · *firmware to be added*

---

### ⚙️ [DC Motor Control — STM32](motor-control/)

Four-button control of a brushed gearmotor: speed by PWM, LED indication in 25% increments, and reversal sequenced through a stop state to keep back-EMF off the driver.

`STM32 NUCLEO-L432KC` · `TB6612FNG` · *firmware to be added*

---

## Also

Two projects that live better as visuals than as code — a 14-part SolidWorks conveyor assembly with motion study and production drawing, and a DIALux room lighting design verified against normative illuminance levels. Both are on my website.
