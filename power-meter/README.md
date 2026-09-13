# AC/DC Power Meter

A meter for 50 Hz AC and for DC loads. It samples voltage and current at register level and works out the full power triangle for AC loads in firmware. It also tries to tell an inductive load from a capacitive one, but that part is not reliable yet (see [Known limitations](#known-limitations)).

Built on an ATmega328-class AVR, sampling at register level rather than through the Arduino ADC API.

![Hand-wired perfboard](docs/perfboard.webp)

## Operating range

| | Range | Set by |
|---|---|---|
| Current | ±30 A | ACS712ELC-30A, about 66 mV/A |
| DC voltage | up to about 34 V | 330 kΩ / 56 kΩ divider into the 5 V ADC |
| AC voltage | up to about 75 V RMS | op-amp front end, 0.21 V per ADC count in the firmware |
| Frequency | 50 Hz | about 20 samples per cycle at the ~977 Hz sampling rate |

These are design limits worked out from the parts and the firmware's scale factors, not tested limits.

---

## What it does

Two multimeters can tell you volts and amps. They cannot tell you real power on a reactive load, because as soon as current and voltage fall out of phase, the product of their RMS values stops being the power actually delivered.

For an AC load, the firmware measures both channels continuously and computes:

| Quantity | Symbol | How |
|---|---|---|
| RMS voltage | U<sub>rms</sub> | subtract the mean, then root-mean-square over the sample buffer |
| RMS current | I<sub>rms</sub> | same |
| Real power | P | mean of the instantaneous product u(t)·i(t) |
| Apparent power | S | U<sub>rms</sub> × I<sub>rms</sub> |
| Reactive power | Q | √(S² − P²) |
| Power factor | cos φ | P / S |
| Load type |  | sign of the zero-crossing index difference (not reliable yet, see below) |

---

## How the sampling works

The interesting part is the sampling, not the maths.

`analogRead()` blocks for ~100 µs and can only read one channel at a time, which makes it useless for sampling two waveforms that have to line up. Instead the ADC is configured directly and runs interrupt-driven:

1. **Timer1** is set to CTC mode and fires a compare interrupt at a fixed rate. That interrupt selects the voltage channel and starts a conversion.
2. **The ADC ISR** stores the result. If it just read voltage, it switches the multiplexer to the current channel and immediately starts a second conversion. If it just read current, it advances the buffer index.
3. When both 200-sample buffers are full, a flag hands them to the main loop.

The result is that voltage and current are sampled back-to-back on every tick, so the phase relationship between them is kept, which is what the inductive/capacitive detection is based on.

## Phase detection

`findZCIndex()` walks each buffer looking for the first sample pair that sits either side of the signal's own mean, which is a zero crossing of the AC component. Comparing where voltage crosses against where current crosses gives the phase relationship:

- current crossing **after** voltage → current lags → **inductive**
- current crossing **before** voltage → current leads → **capacitive**
- same index → resistive

That is the idea. As written it is not reliable, for the reasons under Known limitations.

---

## Hardware

| Function | Part | Notes |
|---|---|---|
| Current sensing | **ACS712** | Hall-effect. Chosen because it works for AC *and* DC, unlike a shunt-plus-differential-amp arrangement |
| Voltage sensing (DC) | 56 kΩ / 330 kΩ divider | Simple resistive scaling |
| Voltage sensing (AC) | Op-amp front end | Biases the waveform to mid-rail and scales it into the ADC input range |
| Controller | ATmega328-class AVR | Arduino-compatible board |
| Construction | Hand-soldered perfboard | |

<p align="center">
  <img src="docs/acs712.webp" width="45%" alt="ACS712 sensor soldered to the board">
  <img src="docs/waveform.webp" width="45%" alt="Captured waveform">
</p>

---

## Building

Open `src/power_meter.ino` in the Arduino IDE and flash to an ATmega328-based board. Output is plain text over serial at **115200 baud**.

Channel assignment:

- `ADC0`: voltage
- `ADC1`: current

---

## Known limitations

These are documented rather than hidden. They are the things I would fix next.

### The buffers are not double-buffered

`bufferReady` is set the moment `sampleIndex` wraps, but the ADC interrupt immediately starts writing to `bufferU[0]` again while `processBuffer()` is still reading it. Every result is therefore computed over a buffer that is partly being overwritten as it is read.

At roughly 1 ms per sample pair a full buffer takes about 200 ms to fill, and processing plus the serial output takes about 10–20 ms, so about 5–10% of each buffer is next-cycle data. For a steady periodic signal the effect is small, but it is still a real race condition. Two alternating buffers with the ISR writing one while the main loop reads the other would remove it entirely.

### Scaling cuts off the decimals too early

```c
int32_t diffU = ((int32_t)bufferU[i] - (int32_t)averageU) * SCALE_U;
```

The multiply gives a float, but the assignment cuts it back to an integer, so a difference of 4 ADC counts × 0.21 becomes 0, not 0.84. Because it cuts toward zero rather than to the nearest value, the error always goes the same way, so RMS and power come out too low rather than just noisy.

It is worst for current. With the 30 A ACS712, `SCALE_I` is about 0.077 A per count, so every current sample is cut to whole amps and any sample under 1 A becomes 0. A load drawing less than about 0.7 A reads as zero current and zero power, and the power factor divides by zero and prints `nan`. Voltage is cut to whole volts the same way. Accumulating in `float` fixes all of it.

`(int32_t)averageU` cuts off the mean the same way, adding up to a full count of DC offset to every sample.

### Zero-crossing direction is not checked

`findZCIndex()` returns the first crossing it finds, regardless of whether the signal is rising or falling through its mean. If voltage happens to cross rising and current crosses falling, the index difference between them is not a phase difference at all. Because each buffer starts at an arbitrary point in the cycle, the first current crossing can belong to the previous half-cycle, and the more reactive the load, the more often the answer comes out backwards: at a 90° shift it is a coin toss. The sampling is also coarse, about 18° per sample at 50 Hz, so a mildly reactive load reads as resistive. Matching crossing direction and averaging across several crossings would fix the first problem.

### DC power is never computed

Every calculation starts by subtracting the buffer's average. For AC that removes the front end's offset, which is correct. For a DC load it removes the signal itself, so RMS, P, S and Q all come out near zero. The DC level only appears as the raw `Avg U` / `Avg I` ADC averages, which are never scaled to volts or amps, and `Avg I` still includes the ACS712's mid-rail offset.

### One voltage input, no AC/DC mode

Voltage is read from `ADC0` only, and the firmware has no AC or DC mode, so the divider or the op-amp front end has to be connected by hand for the load being measured.

### Calibration is hard-coded

`SCALE_U` and `SCALE_I` are constants I worked out by measurement for this board's specific divider and sensor. Moving them to EEPROM behind a calibration routine would make this an instrument rather than something that only works on one board.

---

## Repository notes

The firmware in `src/` is the original working code with comments and two named constants added; the logic is unchanged.
