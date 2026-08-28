# Bill of Materials

| Component | Qty | Notes | Approx. cost (GBP) |
|---|---|---|---|
| Adafruit Metro M0 Express | 1 | Motor control logic | — |
| Adafruit Feather HUZZAH (ESP8266) | 1 | WiFi + web server | — |
| DollaTek TB6612FNG dual motor driver | 1 | 2.5–13.5V motor supply, 2.7–5.5V logic | ~£3–5 |
| TT DC gear motor | 2 | ~71×20×22mm, 6V rated, runs fine at 3–4V | — |
| Adafruit Verter | 1 | Buck-boost, regulates logic supply to 5.2V | — |
| 3.7V Li-Ion cell (Hisewen HAC-006, 525mAh) | 1 | Repurposed Nintendo Switch Joy-Con battery | ~£5–8 |
| TP4056 USB-C charging board | 1 | With OUT+/OUT- protected output | ~£1–2 |
| Adafruit Perma-Proto half-size breadboard | 1 | Power rail junction point | — |
| Dupont jumper wires (M-M, M-F) | assorted | | ~£3–5 for a mixed kit |
| 9mm plywood sheet | 1 | Chassis base plate, cut to size | ~£3–8 depending on offcut size |
| TT motor wheels, 65mm | 2 | If not already owned | ~£3–5 |
| Multimeter | 1 | Essential for verifying voltages before powering on | ~£11–13 |
| Slide switch | 1 | Main power cutoff, wired on positive rail | ~£1 |

**Not used in final build (originally sourced, later replaced):**
- DFRobot DC Motor Driver 2×15A Lite — oversized for this project's current/voltage needs, see build log
- 4×AA NiMH battery holder + cells — replaced by single Li-Ion cell, see build log

**Optional / extension parts owned but not yet integrated:**
- micro:bit — candidate for a status display extension
- Adafruit FeatherWing — not required for core functionality

## Tools required

- Soldering iron + solder
- Wire strippers
- Small screwdriver set
- Computer with Arduino IDE (ESP8266 and SAMD/Adafruit board packages installed)
- Fusion 360 (student/personal license) for chassis CAD
