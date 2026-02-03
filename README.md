\# Noctil: Embedded Environmental Awareness System



\*\*Current Status:\*\* \*Active Development (v2.0 Refactor)\*

\*Migrating core legacy logic to C++20 Modules and Coroutines for improved compile times and non-blocking sensor concurrency.\*



---



\## Overview

Noctil is a real-time, offline navigation assistant for the visually impaired. It utilizes a Raspberry Pi-based embedded system to scan for Bluetooth Low Energy (BLE) beacons and provides immediate, latency-sensitive spatial audio feedback to the user.



Unlike cloud-based solutions, Noctil prioritizes \*\*local processing\*\* to ensure sub-100ms response times for safety-critical navigation cues.



\## Key Technical Features

\* \*\*Modern C++ (C++20):\*\* heavily utilizes Concepts for hardware abstraction and Coroutines for asynchronous BLE scanning loops.

\* \*\*Event-Driven Architecture:\*\* Custom `epoll`-based event loop (migrating from `select`) to handle multiple sensor inputs without thread blocking.

\* \*\*Low-Level Linux Audio:\*\* Direct ALSA integration for low-latency audio buffer management (removing PulseAudio overhead).

\* \*\*Hardware Abstraction:\*\* Decoupled sensor logic allowing simulation of hardware environments for CI/CD testing.



\## Tech Stack

\* \*\*Language:\*\* C++20, Python (Tooling)

\* \*\*Platform:\*\* Linux (Raspberry Pi OS / Debian), Docker

\* \*\*Build System:\*\* CMake, VCPKG

\* \*\*Libraries:\*\* BlueZ (BLE), ALSA (Audio), Espeak-NG (TTS), gRPC (Config Interface)



\## Architecture (v2.0)

The system is designed as a modular monolith to minimize context switching overhead while maintaining separation of concerns:



1\.  \*\*Core Engine:\*\* Manages the main Event Loop and State Machine.

2\.  \*\*HAL (Hardware Abstraction Layer):\*\* Interfaces with BlueZ and ALSA.

3\.  \*\*Spatial Processor:\*\* Calculates distance/direction based on RSSI signal strength.

4\.  \*\*Audio Subsystem:\*\* Generates adaptive tone mapping and speech synthesis.



\## Build Instructions

\*Prerequisites: CMake 3.20+, GCC 11+\*



mkdir build \&\& cd build

cmake ..

make -j4



Author: Tre Wolf Scinta



