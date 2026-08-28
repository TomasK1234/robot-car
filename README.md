# WiFi-Controlled Robot Car

A two-wheeled robot car driven entirely from a phone browser over its own local WiFi network — no app, no internet connection required. Built from scratch as a first hardware project, with zero prior electronics experience going in.

!\[Robot car](https://raw.githubusercontent.com/TomasK1234/robot-car/docs/images/robot-hero.jpg)

## Features

* Drive forward, backward, left, and right from any phone or laptop browser
* Robot creates its own WiFi access point — connect and go, no router needed
* Press-and-hold controls with automatic stop on connection loss (500ms safety timeout)
* Simple plywood chassis, two-microcontroller architecture: one board dedicated to WiFi/web serving, one dedicated to real-time motor control

## How it works

```
Phone browser  →  WiFi  →  Feather HUZZAH (web server)
                              │
                              │ UART (single-byte command protocol)
                              ▼
                           Metro M0 Express
                              │
                              │ PWM + direction signals
                              ▼
                         TB6612FNG motor driver
                              │
                              ▼
                          2× TT DC motors
```

The HUZZAH hosts a small web page with directional buttons and handles all WiFi/HTTP traffic. It continuously streams a single command byte to the Metro M0 over a dedicated hardware UART. The Metro M0 is solely responsible for real-time motor timing — this split exists because the ESP8266's WiFi stack introduces unpredictable timing interrupts that make it unreliable for driving PWM directly.

See [`docs/build-log.md`](docs/build-log.md) for the full reasoning behind this architecture, including two failed alternatives.

## Hardware

Full parts list and approximate cost: [`hardware/bill-of-materials.md`](hardware/bill-of-materials.md)

Wiring diagram: [`hardware/wiring-diagram.svg`](hardware/wiring-diagram.svg)

Core components:

* Adafruit Metro M0 Express — motor control logic
* Adafruit Feather HUZZAH (ESP8266) — WiFi + web server
* DollaTek TB6612FNG dual motor driver
* 2× TT DC gear motors
* Adafruit Verter (buck-boost) — regulated logic power
* 3.7V Li-Ion cell + TP4056 USB-C charging board
* 9mm plywood base plate chassis

## Firmware

* [`firmware/metro\\\_m0/`](firmware/metro_m0) — motor control, command decoding, timeout safety
* [`firmware/huzzah/`](firmware/huzzah) — WiFi access point, web server, control interface

Both are Arduino sketches. See inline comments for pin mappings — these must match your own wiring if you're replicating this build.

## Chassis

The final chassis is a simple 9mm plywood base plate — components are mounted directly to it, no walls or enclosure. This was a late change from the original plan: an earlier two-tier chassis was fully designed in Fusion 360 first, including parametric sizing driven by every component's measured footprint, but it was never built as designed. That exploration is kept in [`cad/fusion-360-exploration/`](cad/fusion-360-exploration) as a record of the design process — **it does not reflect the final physical build**. See the build log for why.

## Build log

This project went through two full power-architecture rewrites and a fair amount of trial and error — the AA battery pack that couldn't supply peak motor current, a single-cell + boost converter approach that didn't provide enough voltage headroom, and the eventual move to a 3.7V Li-Ion cell with a smaller motor driver. The full story, including the reasoning at each step, is documented in [`docs/build-log.md`](docs/build-log.md).

## License

MIT — see [`LICENSE`](LICENSE). Free to use, modify, and build on.

