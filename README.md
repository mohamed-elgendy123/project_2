# Project 2 — Traffic Light Robot

A state-machine and bitwise-driven smart traffic light controller implemented in pure C99 for embedded systems practice.

---

## Overview

This project simulates a real-world adaptive intersection traffic light. The controller automatically alters signal timings based on real-time traffic volume, handles day/night operating cycles, tracks a running history of states, and relies on bit-level manipulation to maintain hardware-efficient state registers.

---

## Key Features

* **Finite State Machine (FSM):** Strictly alternates states through `GREEN` → `YELLOW` → `RED` → `GREEN` with invariant validation[cite: 1].
* **Bitwise State Register:** Tracks control states using single-bit flags (`NIGHT`, `BUSY`, and `BLINK_ON`) via dedicated macros (`SET_BIT`, `CLR_BIT`, `TOGGLE_BIT`, `READ_BIT`)[cite: 1].
* **Adaptive Congestion Control:** Green duration dynamically increases from 5 to 7 ticks when waiting queues exceed 6 cars (`QUEUE_BUSY`)[cite: 1].
* **Vehicle Throughput Logic:** Automatically dispatches up to 2 queued vehicles per tick during active green signals[cite: 1].
* **Night Caution Mode:** Switches off primary sequences and blinks yellow caution signals; returns safely to full-cycle Red upon daytime recovery[cite: 1].
* **Sliding Event Logger:** Maintains an in-memory 20-tick history array showing sequential state letters left-to-right (`G`, `Y`, `R`, `y`)[cite: 1].

---

## Technical Specifications

| Parameter | Specification |
| :--- | :--- |
| **Language Standard** | ISO C99 (`-std=c99`)[cite: 1] |
| **Compiler Flags** | Zero warnings with `-Wall -Wextra`[cite: 1] |
| **Standard Libraries** | `<stdio.h>`, `<stdint.h>`, `<string.h>` only[cite: 1] |
| **Timing Defaults** | Green: 5s (Normal) / 7s (Busy) \| Yellow: 2s \| Red: 4s[cite: 1] |
| **Memory Constraint** | Zero dynamic allocation, maximum function length < 40 lines[cite: 1] |

---

## Build & Run

Compile the program using `gcc`:

```bash
# Compile
gcc -std=c99 -Wall -Wextra -o app main.c

# Run
./app
