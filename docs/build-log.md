# Build Log

This is the unfiltered version of how this project actually came together — including the parts that didn't work the first time. Kept here rather than in the README because it's long, but it's arguably the most useful part of this repo if you're trying to learn from it rather than just clone it.

---

## Power architecture — attempt 1: 4×AA NiMH pack

**Setup:** 4× rechargeable NiMH AA cells (6V nominal) feeding a DFRobot 2×15A motor driver directly, Adafruit Verter regulating 5.2V for logic.

**Symptom:** Motors worked on the bench with no load, but stalled or clicked without turning once any real resistance was applied (e.g. driving on carpet).

**Diagnosis:** TT motor stall current is ~1.1A each — 2.2A combined peak. NiMH cells have relatively high internal resistance, so pack voltage sagged well below the motor driver's 4.8V minimum the instant real current was demanded.

**Also discovered along the way:** the motor driver's screw terminals need to be fully tightened — a loose connection caused one motor to intermittently drop out, which looked like a wiring fault but was just a loose screw.

---

## Power architecture — attempt 2: single-cell Li-Ion + 5V boost

**Reasoning:** NiMH sag was the root cause, so switch to LiPo/Li-Ion chemistry which handles current spikes much better, and keep the motor driver's minimum voltage requirement in mind.

**Setup:** 3.7V Li-Ion cell boosted to 5V via a boost converter, feeding the same DFRobot motor driver.

**Symptom:** Motors didn't spin at all — not even weakly.

**Diagnosis:** Tested with a lab-style power bank capable of supplying a controlled 5V at 2–3A — still no motor movement. This confirmed the problem was voltage, not available current. 5V was simply too close to the motor driver's stated 4.8V minimum once any real-world drop was accounted for, and evidently the DFRobot driver needed more headroom in practice than the datasheet minimum suggested.

**Root cause:** Chose a motor driver sized for much larger motors/current (2×15A) than this project needed, which meant its practical operating envelope didn't suit a low-voltage, low-current setup.

---

## Power architecture — attempt 3 (final): 3.7V direct + smaller motor driver

**Change:** Switched to a DollaTek TB6612FNG driver, rated for 2.5–13.5V motor supply and 2.7–5.5V logic supply — both ranges comfortably cover a bare 3.7V Li-Ion cell.

**Result:** Motors ran directly off the 3.7V cell with no boost conversion needed on the motor side. Verter retained purely for logic-side regulation (Metro M0, HUZZAH, motor driver VCC).

**Trade-off accepted:** TB6612FNG is rated 1.2A continuous per channel against a 1.1A motor stall current — a thin margin. Acceptable for normal driving; sustained stall (e.g. wedged against furniture) should be avoided.

---

## Battery selection and charging

Went through several options before landing on a repurposed 3.7V 525mAh Nintendo Switch Joy-Con battery (Hisewen HAC-006):

- **7.4V 2S LiPo** — rejected. No affordable module exists that combines USB-C charging *and* proper per-cell balance charging. Non-balancing USB-C boards are common but risk long-term cell drift on a 2-cell pack.
- **Better-quality NiMH (Eneloop Pro)** — a genuinely valid alternative that was ultimately not pursued once the smaller motor driver made single-cell Li-Ion viable, but worth considering as a simpler, safer option if avoiding LiPo entirely.
- **Single-cell Li-Ion (final choice)** — no balance charging needed since there's only one cell. TP4056 USB-C charging board handles it directly.

**Known compromise:** the TP4056 board's default charge current (1000mA) is roughly 2C for a 525mAh cell — higher than the ideal 0.5–1C. The battery's built-in protection circuit provides a safety backstop (it will cut charging if it gets too hot), but the correct fix is replacing the board's SMD programming resistor to bring charge current down to ~500mA. Not yet done on this build — flagged here for anyone replicating this.

---

## Communication protocol

Settled on a single-byte command protocol between the HUZZAH and Metro M0 (`0xFF` = forward, `0x01` = backward, `0xAA` = right, `0x55` = left, `0x00` = stop) sent continuously over hardware UART, rather than one message per button press. This was a deliberate choice: continuous transmission acts as an implicit heartbeat, so a 500ms timeout on the Metro M0 can detect a dropped WiFi connection and stop the motors automatically, rather than the robot driving indefinitely on the last command received.

Rejected alternatives: I2C and SoftwareSerial were both considered to avoid the HUZZAH's hardware TX pin conflicting with USB traffic during development. Both were dropped — I2C because the ESP8266's WiFi interrupts make it an unreliable I2C master, and SoftwareSerial for the same underlying interrupt-timing reason. The simpler fix was powering the HUZZAH from a standalone supply (not USB) during real operation, and accepting the USB conflict only during development/flashing.

---

## Chassis — Fusion 360 design vs. plywood reality

Fully worked through a two-tier chassis design in Fusion 360: measured every
component (motors, motor driver, battery, breadboard, Arduino, ESP8266),
derived the internal width/length from the widest and longest components on
each tier, added wall thickness and clearance as parameters, and used a
mirror operation to build the four walls symmetrically. Chassis footprint
ended up at 84×167mm with 3mm walls and a 20mm internal tier height.

That design was never actually printed. The final build is a single sheet
of **9mm plywood**, cut to size as a flat base plate with no walls, no
two-tier structure, and none of the CAD dimensions carried over. The
Fusion file is kept in `cad/fusion-360-exploration/` purely as a record of
the design reasoning — component-driven layout, tolerance thinking, and
learning parametric CAD from scratch — not as a build reference for anyone
replicating this project.

If replicating: the plywood base needs to be large enough to fit the
motor driver, battery, breadboard, Metro M0, and HUZZAH with reasonable
spacing between them — refer to the component dimensions in
`hardware/bill-of-materials.md` rather than the Fusion file's dimensions.

---

## Hardware mishaps

- **USB-A vs USB-C cable:** the TP4056 board didn't respond to a USB-C to USB-C cable from a modern PD power bank — needed a USB-A to USB-C cable instead. Cheap USB-C charging boards frequently lack PD negotiation support.
- **Cold solder joints:** early motor driver soldering attempts failed because solder was applied to the iron rather than to the (pre-heated) joint. Tinning both the wire and the pad separately before joining fixed this.
- **Hot glue on PCBs:** almost mounted boards directly with a hot glue gun (~195°C) — would likely have damaged components. Switched to gluing chassis-side only, with the board resting on the glue frame rather than glue touching the board.
