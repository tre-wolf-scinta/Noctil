# \# Noctil

## \## System Architecture \& Technical Design

### 

### \### Document Purpose:

### This document defines the technical architecture, feature set, and design rationale for Noctil, a headless embedded navigation system based on BLE beacons. The intent is to describe a production-grade system that could be extended into a market-ready product with additional investment.

### 

### \### 1. System Overview



Noctil is a headless Raspberry Pi 5 based embedded system that continuously scans for BLE beacon advertisements and converts them into navigational cues for blind and visually impaired users. The system supports multi-slot beacon signaling, real-time telemetry, audio guidance, and a local web-based configuration and monitoring interface.
The system is designed to operate continuously as a system service and to function without any attached display, keyboard, or mouse.

### 

### \### 2. Design Goals

#### 

#### \#### Primary Goals

* Reliable BLE advertisement ingestion with low latency
* Robust handling of noisy RSSI signals
* Explicit support for beacon-generated events such as button presses and motion triggers
* Real-time observability for demonstration and debugging
* Accessibility-first output modes
* Clear separation between hardware ingestion, inference logic, and presentation layers

#### 

#### \#### Non-Goals

* Cloud dependency in the core system
* Reliance on proprietary mobile applications during normal operation
* Visual-only user interfaces

### 

### \### 3. hardware and Beacon Model

#### 

#### \#### 3.1 Target Platform

* Raspberry Pi 5
* Raspberry Pi OS Bookworm 64-bit
* Integrated Bluetooth controller

#### 

#### \#### 3.2 Beacon Hardware

* BlueCharm BC021 Pro iBeacon
* Five independent advertisement slots per beacon
* Button trigger support
* Motion trigger support
* Configurable advertising intervals and TX power

#### 

#### \#### 3.3 Beacon Signaling Contract

Each physical beacon is treated as a logical group composed of multiple independent signals.

#### 

#### \##### Signal Types

* Presence signal
* Button intent signal
* Motion activity signal



Each signal is broadcast using a distinct iBeacon identity via a dedicated slot. Vendor-specific UUID modification behavior is not relied upon as a primary signaling mechanism.

#### 

#### \##### Recommended Slot Usage:

* Slot 0: Presence signal
* Slot 1: Button intent signal
* Slot 2: Motion activity signal
* Slot 3: Maintenance or provisioning
* Slot 4: Reserved

### 

### \### 4. BLE Ingestion Layer

#### 

#### \#### 4.1 Scanning Strategy

Primary scanning is implemented using raw HCI sockets rather than the BlueZ D-Bus API.

##### 

##### \##### Rationale:

* Lower latency
* Full control over scan parameters
* Reduced dependency on user-space Bluetooth abstractions
* Demonstrates low-level embedded Linux proficiency



A secondary BlueZ D-Bus based scanner may be implemented as a compile-time optional fallback.

#### 

#### \#### 4.2  Responsibilities

* Open and configure HCI device
* Enable continuous LE scanning
* Parse LE Advertising Report events
* Timestamp all received frames
* Forward raw frames to the decoding pipeline without blocking

### 

### \### 5. Advertisement Decoding

#### 

#### \#### 5.1 iBeacon parsing

* UUID
* Major ID
* Minor ID
* Measured Power
* RSSI
* Advertisement timestamp



Decoded frames are normalized into a common internal representation independent of the scanning backend.

### 

### \### 6. Core Data Model

#### 

#### \#### 6.1 Beacon

Represents a physical beacon device

##### 

##### \##### Fields:

* Beacon ID
* Human-readable name
* Description
* Category
* Associated signals

#### 

#### \#### 6.2 Signal:

Represents a single broadcast identity

##### 

##### \##### Fields

* UUID
* Major ID
* Minor ID
* Signal type
* Slot hint
* Beacon association

##### 

##### \##### Signal Types

* Presence
* Button intent
* Motion activity

#### 

#### \#### 6.3 Observation

Represents a single received advertisement

##### 

##### \##### Fields

* Timestamp
* Signal ID
* RSSI
* Measured Power
* Source Backend



### \### 7. Tracking and Inference Pipeline

#### 

#### \#### 7.1 Signal Tracking

Each signal maintains a continuous tracking state

##### 

##### \##### Tracked Properties:

* Last seen timestamp
* Smoothed RSSI using EWMA
* RSSI variance estimate
* Packet rate
* Confidence score

#### 

#### \#### 7.2 Proximity classification

Proximity is classified into discrete states rather than attempting absolute distance estimation.

##### 

##### \##### States:

* Immediate
* Near
* Far
* Lost



State transitions apply hysteresis and minimum dwell times to prevent flapping.

#### 

#### \#### 7.3 Event detection

Button and motion signals are treated as short-lived event sources.

##### 

##### \##### Event detection logic

* Burst detection within a sliding time window
* Minimum packet count threshold
* Receiver-side debounce and cooldown enforcement



Detected events are emitted as high-level navigation events.

#### 

\### 8. Navigation Engine 

#### \#### 8.1 Responsibilities

* Maintain current navigation target
* Interpret proximity and trend information
* Generate guidance cues
* Enforce rate limits on output

#### 

#### \#### 8.2 Navigation states

* Idle
* Acquiring
* Navigating
* Arrived
* Lost



Transitions are driven by confidence, proximity stability, and user commands.

### 

### \### 9. Audio output subsystem

#### 

#### \#### 9.1 Output modes

##### 

##### \##### Mode 1: Text-to-speech

* Announces target selection
* Announces proximity state changes
* Announces arrival and loss conditions

##### 

##### \##### Mode 2: Tone-only guidance

* Pitch increases with proximity
* Pulse rate increases with confidence
* Distinct arrival tone



Modes can be toggled at runtime without restarting navigation.

#### 

#### \#### 9.2 TTS backend

* Offline TTS as default
* Pluggable backend architecture
* No cloud dependency in baseline operation

### 

### \### 10. Web interface

#### 

#### \#### 10.1 Access model

* Web interface accessible to any device on the local network
* Authentication required for configuration actions
* Read-only access permitted for demonstration if configured

#### 

#### \#### 10.2 Web API

##### \##### REST endpoints:

* GET /api/beacons
* GET /api/beacons/{id}
* PUT /api/beacons/{id}
* POST /api/navigation/start
* POST /api/navigation/stop
* GET /api/system/status

##### 

##### \##### WebSocket endpoint:

* /ws/telemetry
  Telemetry stream includes:

  * Per-beacon RSSI
  * Proximity state
  * Confidence
  * Event notifications
  * System metrics

#### 

#### \#### 10.3 Web UI features

* Beacon discovery and naming
* Beacon metadata editing
* Live telemetry visualization
* Receiver health dashboard
* Navigation control panel
* Audio mode toggle

### 

### \### 11. Persistence

#### 

#### \#### 11.1 Storage backend

* SQLite database stored locally on the device

#### 

#### \#### 11.2 Core tables

* beacon
* signal
* event
* calibration\_profile
* settings



All writes are asynchronous to prevent interference with BLE scanning.

### 

### \### 12. Observability and diagnostics

#### 

#### \#### 12.1 Logging

* Structured JSON logs
* Severity levels
* Timestamped entries

#### 

#### \#### 12.2 Metrics

* Advertisements per second
* Per-signal packet rate
* Queue depth
* Dropped frame count
* CPU and memory usage

#### 

#### \#### 12.3 Trace replay

* Raw advertisement capture to file
* Offline replay for testing and tuning

### 

### \### 13. Deployment

* Single daemon managed by systemd
* Automatic restart on failure
* Configuration loaded at startup and hot-reloaded where safe
* Headless operation only

### 

### \### 14. Security considerations

* No default credentials
* Minimal exposed ports
* Local authentication for configuration access
* Optional TLS for web interface

### 

### \### 15. Extensibility

The architecture supports:

* Additional beacon types
* Alternative inference models
* Cloud synchronization layers
* Multi-device mesh deployments
* Integration with mobile clients

### 

### \### 16. Implementation phases

#### 

#### \#### Phase 1:

* HCI scanning
* iBeacon decoding
* Observation logging

#### 

#### \#### Phase 2:

* Tracking pipeline
* Event detection
* Basic navigation state machine

#### 

#### \#### Phase 3:

* Web API
* WebSocket telemetry
* Beacon registry persistence

#### 

#### \#### Phase 4:

* Audio output modes
* Navigation guidance logic

#### 

#### \#### Phase 5:

* Observability
* Trace replay
* Test coverage
* System hardening

### 

### \## 17. Professional positioning

This project demonstrates:

* Low-level Linux Bluetooth programming
* Real-time data processing
* Embedded system design
* Accessibility-focused engineering
* Production-grade observability and reliability practices
