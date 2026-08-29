# Shoebox Speaker

A portable amplified speaker built into a shoebox: a salvaged 8 Ω full-range
driver, an LM386 amplifier stage on perfboard, and a 9 V battery.

The box is the enclosure, which is the part that actually matters. The driver
has to be sealed into the panel properly, because if air can move freely
between the front and the back of the cone the two halves of the wave cancel
each other and the low end disappears.

**Status: early build.** The parts are chosen and the plan below is what I am
working to. Photos and measurements go in as I get them.

---

## Parts

| Part | Notes |
|---|---|
| Enclosure | A shoebox, roughly shoe sized |
| Driver | 8 Ω small full-range, salvaged |
| Amplifier | LM386 audio amplifier IC |
| Power | 9 V battery |
| Passives | Resistors and capacitors from the LM386 reference circuit |
| Input | Aux cable from a phone or a laptop |
| Mounting | Hot glue and tape |

---

## The plan

1. Cut a hole in one side of the box to match the driver diameter.
2. Mount the driver and seal the edges so the box works as a proper enclosure.
3. Build the LM386 circuit on perfboard: the gain setting resistor and
   capacitor, the input coupling capacitor, and the output coupling capacitor
   to the driver.
4. Wire the battery to the amplifier supply pins, watching the polarity.
5. Wire the aux input, tip and ring to the input pin and sleeve to ground.
6. Test it with a phone before gluing anything down.
7. Glue the board inside the box and close the lid.

---

## Notes to myself

- Check the battery polarity before the first power up.
- Start with the volume low so the driver does not get pushed past its rating.
- The LM386 runs at a gain of 20 with nothing between pins 1 and 8. A capacitor
  across those two pins takes it to 200, which is almost certainly more than a
  9 V battery and a cardboard box can do anything useful with.
- A cardboard enclosure will flex. If the low end sounds weak once it is
  sealed, the panel is probably moving instead of the cone.

## TODO

- [ ] Build the amplifier stage and get it working on the bench
- [ ] Cut and seal the driver panel
- [ ] Photos in `docs/`
- [ ] Measure what the enclosure does to the low end
- [ ] Schematic in `hardware/`
