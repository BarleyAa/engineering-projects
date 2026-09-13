# Engineering Projects — Agris Miezītis

Electrical Engineering student at Riga Technical University, working in embedded systems and automation. This repository collects my hardware and firmware projects: source, design files and documentation.

📍 Riga, Latvia · [LinkedIn](https://www.linkedin.com/in/agrismiezitis/)

---

## Projects

### ⚡ [AC/DC Power Meter](power-meter/)

A meter for 50 Hz AC and for DC loads, built for up to ±30 A. It works out the full power triangle for AC loads in firmware, and makes a first attempt at telling inductive loads from capacitive ones.

Register-level AVR: the ADC is driven from a timer interrupt and alternates channels inside the ADC ISR, so voltage and current are sampled back-to-back and their phase relationship is kept.

`Arduino / AVR C` · `ACS712` · `op-amp front end` · **complete**

---

### 🌱 [Automated Growhouse](growhouse/)

A greenhouse that waters, ventilates and lights itself. Three separate control loops on one Arduino, working across 5 V logic, a 12 V fan and 230 V mains through a relay, with an irrigation state machine and a 2 °C hysteresis band on the temperature control.

`Arduino` · `3D printing` · `mixed-voltage` · **complete**

---

### 🔌 [NE555 Timer Relay — PCB](timer-relay/)

A single-layer through-hole board for an adjustable-delay timer relay, designed in Altium. Every drill diameter worked out from lead geometry and plating thickness; every trace width calculated from current density.

`Altium Designer` · `22 components` · *design files to be added*

---

### 🚪 [SmartTurn — Retrofit Smart Lock](smartturn/)

A device that makes an ordinary interior door lock smart without modifying the door. About €15–30 in parts against €100–280 for the products on the market. It was a team project and I did the hardware, firmware and mechanical design.

`ESP32` · `servo` · `3D printing` · *firmware to be added*

---

### ⚙️ [DC Motor Control — STM32](motor-control/)

Four-button control of a brushed gearmotor: speed by PWM, LED indication in 25% increments, and reversal sequenced through a stop state to keep back-EMF off the driver.

`STM32 NUCLEO-L432KC` · `TB6612FNG` · *firmware to be added*

---

### 🔊 [Shoebox Speaker](shoebox-speaker/)

A portable amplified speaker built into a shoebox, using a salvaged 8 Ω driver and an LM386 amplifier stage. The box itself is the enclosure, so sealing the driver into the panel is what decides whether there is any low end at all.

`LM386` · `analogue audio` · *currently building*

---

## Also

Two projects that live better as visuals than as code: a 14-part SolidWorks conveyor assembly with motion study and production drawing, and a DIALux room lighting design checked against the required illuminance levels. Both are on my website.
