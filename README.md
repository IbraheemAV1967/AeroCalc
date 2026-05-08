# ✈️ AeroCalc — Takeoff Performance Calculator

> A physics-based C++ console application that determines whether an aircraft can safely take off from a given runway under specific environmental conditions.

---

## 📖 Overview

AeroCalc simulates the **takeoff ground roll** of an aircraft — the distance needed to accelerate from rest to liftoff speed. It uses closed-form analytical equations from aerospace engineering textbooks (primarily Marchman's *Aerodynamics and Aircraft Performance*) to compute:

- ✅ **Takeoff ground roll distance** (metres)
- ✅ **Takeoff time** (seconds)
- ✅ **Go / No-Go clearance** based on available runway length
- ✅ **Safety margin** or runway deficit

The program manages a persistent **fleet database** and **airport database**, saved locally as plain-text files between sessions.

---

## 🖥️ Demo

```
---------------------- Welcome to AeroCalc , Your Takeoff Clearance Assistant ----------------------
====================================================================================================
Loaded 3 aircraft from database.
Loaded 2 airport(s) from database.
1. Manage Aircrafts
2. Manage Airports
3. Run Simulation
4. Exit
Enter your Choice (1-4):
```

```
=========== TAKEOFF SIMULATION RESULTS ===========
Aircraft:  Cessna172 (GA)
Airport:   KHI | Surface: Concrete/Asphalt_Dry
----------------------------------------------------
  Total Mass:                1156.27 kg
  Total Weight (W):          11343 N
  Static Thrust (T0):        2668.93 N
  Thrust Model:              Constant
  Thrust-to-Weight (T0/W):   0.235293
  Air Density (rho):         1.22498 kg/m^3
  Rolling Friction (mu):     0.02
----------------------------------------------------
  Stall Speed (Vstall):      26.7587 m/s  (96.3315 km/h)
  Takeoff Speed (VTO):       32.1105 m/s  (115.598 km/h)
  Wind:                      None
----------------------------------------------------
  A (accel constant):        2.11202 m/s^2
  B (drag constant):         0.000301534 m^-1
----------------------------------------------------
  TAKEOFF GROUND RUN:        264.049 m
  TAKEOFF TIME:              16.0234 sec
  RUNWAY AVAILABLE:          3000 m
----------------------------------------------------
  >> TAKEOFF POSSIBLE
  >> Safety Margin: 2735.95 m (91.1984%)
====================================================
```

---

## 🔬 Physics Model

The simulation is grounded in Newton's Second Law applied to an aircraft during its ground roll:

```
m·(dV/dt) = T − D − μ(W − L)
```

### Drag Polar
Aerodynamic forces use the **parabolic drag polar**:
```
C_D = C_D0 + K·C_L²
```

### Optimal Ground-Run Lift Coefficient
```
C_Lg = μ / (2K)        (minimises ground roll distance)
```

### Thrust Models

| Model | Equation | Suitable For |
|---|---|---|
| **Constant** | T = T₀ | Jets, afterburners, propellers |
| **Variable** | T = T₀ − a·V² | Turbofans (thrust drops with speed) |

### Analytical Solution (Marchman §7.1 & §7.5)

```
A = g·(T₀/W − μ)
B = (g/W)·[½ρS·(C_Dg − μ·C_Lg) + a]

Ground roll:  S = (1/2B)·ln[(A − B·Vw²)/(A − B·Vto²)] − Vw·t
Time:         t = (1/√AB)·[atanh(Vto·√(B/A)) − atanh(Vw·√(B/A))]
```

> Setting `a = 0` gives the Constant Thrust model. Setting `Vw = 0` gives the no-wind case. Both handled by the same unified formula.

### Air Density
Uses the **standard atmosphere pressure** model with actual ambient temperature:
```
P = P_SL·(1 − 2.2558×10⁻⁵·h)^5.2561
ρ = P / (R_air · T)
```

---

## ⚙️ Features

- **Two thrust models** — Constant and Variable (T = T₀ − aV²), with automatic coefficient calculation
- **Wind support** — headwind, tailwind, and no-wind handled by a single unified equation
- **Temperature-corrected air density** — using ISA pressure model + user-provided ambient temperature
- **6 runway surfaces** — Concrete/Asphalt, Hard Turf, Short Grass, Long Grass, Soft Ground, Custom
- **Persistent database** — aircraft and airport data saved to `.txt` files and auto-loaded on startup
- **Input validation** — every prompt is guarded; the program will never crash on bad input
- **Error detection** — catches impossible configurations (insufficient thrust, negative net force at VTO, extreme wind speeds)
- **Safety margin display** — tells you exactly how much runway you have to spare (or how much you're short)

---

## 🚀 Getting Started

### Prerequisites
- A C++17-compatible compiler (g++, MSVC, Clang)
- On Windows: [MinGW-w64](https://www.mingw-w64.org/) or MSVC via Visual Studio

### Build

```bash
g++ -o AeroCalc.exe Code/AeroCalc.cpp -std=c++17
```

### Run

```bash
./AeroCalc.exe
```

> The database files (`aircraft_db.txt` and `airport_db.txt`) are created automatically in the same directory as the executable on first save.

---


## 🧭 Usage Guide

### Main Menu
```
1. Manage Aircrafts     → Add aircraft or view fleet
2. Manage Airports      → Add airport or view saved airports
3. Run Simulation       → Select aircraft + airport, enter conditions, get result
4. Exit
```

### Adding an Aircraft

You'll be prompted for:
| Field | Example |
|---|---|
| Name | `Cessna172` |
| Empty mass (kg) | `1156.27` |
| Static thrust T₀ (N) | `2668.93` |
| Wing area S (m²) | `16.1651` |
| Zero-lift drag C_D0 | `0.037` |
| Induced drag factor K | `0.056` |
| Aircraft type | GA / Fighter / Airliner / UAV / Custom |
| Thrust model | Constant or Variable |

For **Variable thrust**, you can enter the lapse coefficient `a` directly, or let the program calculate it from thrust at a known speed.

### Adding an Airport
| Field | Example |
|---|---|
| Runway code | `KHI` |
| Runway length (m) | `3200` |
| Altitude (m) | `22` |
| Surface type | Concrete / Turf / Soft Ground / Custom |

### Running a Simulation
Select an aircraft and airport from your saved lists, then provide:
- Payload mass (kg)
- Fuel mass (kg)
- Ambient temperature (°C)
- Wind speed (m/s) — positive = headwind, negative = tailwind

---

## ✅ Validated Against Real Aircraft

| Aircraft | Type | Published S_TO | AeroCalc | Error |
|---|---|---|---|---|
| Cessna 172S | GA Piston | ~270 m | 264 m | ~2.2% |
| F-16C Fighting Falcon | Fighter Jet | ~457 m | 431 m | ~5.7% |
| Boeing 737-800 | Turbofan Airliner | ~2090 m | 2201 m | ~5.3% |
| Airbus A320 | Turbofan Airliner | ~2100 m | 2088 m | ~0.6% |
| Gulfstream G550 | Business Jet | ~1707 m | 1830 m | ~7.2% |

> Typical accuracy: **±5–10%** against published ground roll figures. Variance is expected — published values include pilot technique, flap settings, and ground effect, which are not modelled here.

---

## ⚠️ Limitations

- Calculates **ground roll only** — does not include rotation, transition, or climb-out
- No **ground effect** modelling (reduced induced drag near runway surface)
- Aerodynamic coefficients (C_D0, K) must be sourced by the user from public data
- Not intended for operational flight planning — **educational/academic use only**

---

## 🛠️ Built With

- **C++17** — Standard Library only (`<iostream>`, `<cmath>`, `<vector>`, `<string>`, `<fstream>`, `<limits>`)
- No external dependencies

---

## 📚 References

1. Marchman, J.F. III, *Aerodynamics and Aircraft Performance*, Virginia Tech, 2004 — primary equations (Ch. 6–7)
2. Anderson, J.D., *Introduction to Flight*, 8th Ed., McGraw-Hill, 2015
3. FAA, *Pilot's Handbook of Aeronautical Knowledge*, Ch. 11, 2016
4. ISA Standard Atmosphere model (ICAO Doc 7488)

---

## 👤 Author

Developed as an academic C++ project for the **Department of Aerospace Engineering, SMME, NUST, Islamabad**.

---

## 📄 License

This project is open-source and available under the [MIT License](LICENSE).
