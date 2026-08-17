# NE555 Timer Relay — PCB Design

> **Status: needs your Altium project files.** Drop the schematic, PCB and output job files into `hardware/` and delete this line.

A single-layer through-hole board for an adjustable-delay timer relay. Press the trigger, the relay pulls in, and it drops out after a delay set by a front-panel potentiometer — anywhere from a few seconds to several minutes.

<p align="center">
  <img src="docs/schematic.webp" width="49%" alt="Altium schematic">
  <img src="docs/pcb-layout.webp" width="49%" alt="PCB layout">
</p>

---

## Circuit

| Stage | Part | Role |
|---|---|---|
| Supply | L7812 | 12 V regulation |
| Timing | NE555 | Monostable; delay set by RC |
| Adjustment | 500 kΩ potentiometer | Sets the delay |
| Switching | BC547B | Drives the relay coil |
| Output | 12 V SPDT relay | With 1N4007 flyback diode |
| Indication | Green + red LEDs | State |

22 components total.

---

## The design work

The schematic is the easy part. What this project was actually about:

**Drill diameters** were derived per component rather than assumed — from each part's lead diameter, the plating thickness, and a process tolerance allowance.

**Trace widths** were sized from current density rather than from a default. The board runs 1.00 mm traces, comfortably above the calculated minimum.

**Signal frequencies** were checked to confirm that trace length was irrelevant here. Everything on the board is DC or well under a hertz, so transmission-line effects don't apply and routing could be optimised for layout instead of length. Worth confirming rather than assuming.

---

## TODO

- [ ] Add Altium project files to `hardware/`
- [ ] Export and add Gerbers
- [ ] Photo of the assembled board, if one was built
- [ ] Fix the trace-width table in the original report — the text calculates 0.5 / (30 × 0.035) = 0.48 mm, but the table lists 0.017 mm, which drops the copper thickness term. The board is unaffected; the document isn't.
