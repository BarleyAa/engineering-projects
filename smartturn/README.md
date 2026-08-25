# SmartTurn — Retrofit Smart Lock

A device that makes an ordinary interior door lock smart without modifying the door. About €15–30 in parts, against €100–280 for the products on the market.

Team project (3 people) at Riga Technical University. **My contribution: hardware, firmware and mechanical design.**

---

## The problem

Most interior doors have no keyhole on the inside — just a thumb-turn. Every commercial smart lock requires replacing the cylinder, which renters are not allowed to do and students cannot afford.

| Product | Price | Requires |
|---|---|---|
| Yale Linus | €200–280 | Full cylinder replacement |
| Nuki | €150–250 | Cylinder work |
| SwitchBot Lock | €100–130 | Retrofit, but expensive |
| **SmartTurn** | **€50–90 (est. selling price)** | Adhesive or screws |

## How it works

A 3D-printed gripper clamps over the existing thumb-turn. A 13 kg-rated MG996R metal-gear servo turns it. An ESP32 provides Wi-Fi and Bluetooth control alongside a physical button on the unit.

The design constraint: it operates from the inside only, and the outside key keeps working exactly as before. The lock is never replaced or bypassed — only turned.

---

## Hardware

| Function | Part | Cost |
|---|---|---|
| Controller | ESP32 dev board | €5–10 |
| Actuator | MG996R servo, 13 kg, 4.8–6.0 V | €3–7 |
| Gripper | 3D printed, PLA/PETG | €1–3 |
| Power | 5 V/3 A USB-C or 4×AA | €3–6 |
| Housing | 3D printed + adhesive or screws | €1–3 |
| Input | Physical button | €0.50–1 |
| Wiring | Jumpers, solder | €1–2 |

**Total BOM: €15–30**

---

## Test results

Prototype tested in normal daily use.

| Aspect | Result |
|---|---|
| Door opening | Reliable; torque is enough for standard locks |
| Servo | Stable, same rotation angle every time, no skipped steps |
| ESP32 control | No hangs; precise PWM generation and command handling |
| Mounting | Good enough, but the housing bends slightly under high load |
| Gripper | Needs fitting to each lock type — otherwise it goes off-axis and slips |

---

## Next steps

- Stiffer housing
- A gripper that adjusts to more than one knob shape without a reprint
- Matter compatibility

## TODO

- [ ] Add ESP32 firmware to `src/`
- [ ] Add gripper and housing STLs to `hardware/`
- [ ] Wiring diagram
- [ ] Photos of the prototype mounted on a door
