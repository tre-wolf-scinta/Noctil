# \# Noctil

## \## Design Patterns



### \### Document Purpose:

This document describes the software design patterns used in Noctil, including each pattern’s responsibilities, rationale, advantages, and where it applies in the codebase.



## \##Interface Segregation \& Dependency Inversion

### 

### \###Responsibilities

* Define narrow, role-specific interfaces for external dependencies
* Decouple core logic from platform, IO, and framework concerns

### 

### \###Rationale

Noctil integrates hardware scanning, persistence, audio output, and network services. Decoupling these dependencies prevents the domain logic from being tightly coupled to a specific Bluetooth backend, database, or audio engine.



### \###Advantages

* Improves testability through mocks and fakes
* Allows multiple implementations of the same capability (HCI vs BlueZ, TTS backends)
* Reduces rebuild impact and keeps compilation boundaries clean

### 

### \###Codebase usage:

* src/noctil/ports/

  * ble\_scanner.h
  * storage.h
  * audio.h
  * telemetry.h

* Core consumers in src/noctil/domain/ depend only on ports.



## \##Ports \& Adapters Architecture

### 

### \###Responsibilities

* Separate the core application logic from external systems
* Treat hardware, storage and UI delivery as replaceable adapters

### 

### \###Rationale

Embedded products require long-lived maintainability across changing hardware, OS versions, and feature requirements. This architecture ensures the domain remains stable while adapters evolve.

### 

### \###Advantages

* Enables offline development and unit testing of domain logic
* Supports multiple runtime environments and future product variants
* Improves long-term maintainability and onboarding

### 

### \###Codebase Usage:

* Domain: src/noctil/domain/
* Ports: src/noctil/ports/
* Adapters: src/noctil/adapters/
* ble\_hci/
* ble\_bluez\_dbus/
* storage\_sqlite/
* audio\_tts/
* audio\_tones/
* http\_server/



## \##Strategy Pattern

### 

### \###Responsibilities

* Encapsulate interchangeable algorithms behind a stable interface
* Enable runtime or configuration-based selection of behavior

### 

### \###Rationale

BLE signal processing and audio guidance require tuning and experimentation. Strategies allow algorithm changes without altering the rest of the system.

### 

### \###Advantages

* Simplifies experimentation and calibration
* Keeps algorithms isolated, testable, and replaceable
* Avoids hardcoding policy decisions into core pipelines

### 

### \###Codebase Usage:

* Signal smoothing strategies in src/noctil/domain/tracking/
* EWMA smoothing
* Optional median filter or more advanced estimators
* Event detection strategies in src/noctil/domain/events/
* Burst thresholds
* Cooldown policy
* Guidance strategies in src/noctil/domain/navigation/
* TTS guidance policy
* Tone mapping policy

## 

## \##State Machine Pattern

### 

### \###Responsibilities

* Model system behavior as explicit states and transitions
* Ensure outputs depend on stable states rather than transient noise

### 

### \###Rationale

Noctil must resist RSSI noise, prevent flapping, and produce predictable output. State machines provide determinism and clarity.



### \###Advantages

* Improves correctness under noisy input
* Makes behavior auditable and testable
* Provides a clear framework for rate limiting and hysteresis



### \###Codebase Usage:

* Navigation state machine in src/noctil/domain/navigation/
* States: Idle, Acquiring, Navigating, Arrived, Lost
* Per-signal proximity tracking state in src/noctil/domain/tracking/
* States: Far, Near, Immediate, Lost



## \##Observer \& Publisher-Subscriber Pattern

### 

### \###Responsibilities

* Broadcast internal events and telemetry to multiple consumers
* Decouple producers from the number and type of subscribers

### 

### \###Rationale

Noctil needs to publish telemetry to logs, a web UI, metrics, and potentially future remote consumers. A publish-subscribe model prevents tight coupling.

### 

### \###Advantages

* Enables fan-out without introducing backpressure into the scan path
* Supports optional components without changing core logic
* Improves testability by allowing telemetry sinks to be replaced

### 

### \###Codebase Usage:

* Telemetry publication in src/noctil/ports/telemetry.h
* WebSocket stream adapter in src/noctil/adapters/http\_server/
* Logging and metrics sinks in adapter layer



\##Command Pattern


\###Responsibilities

* Represent control actions as explicit command objects.
  - Centralize validation, logging, and execution of user-initiated actions.



\###Rationale
The web UI triggers actions that must be validated and traced, such as starting navigation, stopping navigation, or changing audio mode.


\###Advantages

* Produces consistent audit logs for system actions.
  - Simplifies unit testing of control paths.
  - Prevents business rules from leaking into HTTP handlers.



\###Codebase usage

* Control commands in src/noctil/domain/ or src/noctil/app/:
  - StartNavigation
  - StopNavigation
  - SetAudioMode
  - UpdateBeaconMetadata
  - HTTP handlers call command executors rather than domain internals directly.



\## RAII and Deterministic Resource Management


\###Responsibilities

* Ensure resources are acquired and released safely and predictably.
  - Make shutdown behavior correct even under exceptions or early returns.



\###Rationale
Embedded systems interact with file descriptors, sockets, threads, timers, and OS resources. Leaks and partial shutdown behavior are unacceptable in long-running daemons.


\###Advantages

* Prevents resource leaks and undefined shutdown states.
  - Encourages exception-safe and maintainable code.
  - Simplifies ownership reasoning and reduces lifecycle bugs.



\###Codebase usage

* HCI socket wrappers in src/noctil/adapters/ble\_hci/
  - Thread ownership in src/noctil/app/ and adapter components
  - File and database handle wrappers in src/noctil/adapters/storage\_sqlite/



\## Bounded Queue and Backpressure Pattern


\###Responsibilities

* Enforce a maximum buffering capacity between producer and consumer stages.
  - Provide explicit handling of overload conditions.



\###Rationale
BLE scanning must not block under load. Downstream delays must not cause unbounded memory growth or latency collapse.


\###Advantages

* Prevents memory blow-ups and tail-latency spikes.
  - Preserves real-time behavior under stress.
  - Makes overload visible through counters and logs.



\###Codebase usage

* Scan ingestion queue between BLE capture and decoding stages
  - Pipeline queues between decoding, tracking, and publishing stages
  - Metrics counters for dropped frames and queue depth



\##Clean Shutdown and Cooperative Cancellation


\###Responsibilities

* Support coordinated shutdown across threads and subsystems.
  - Prevent deadlocks and partial termination.



\###Rationale
Noctil runs as a system service and must handle restarts and upgrades cleanly. Threads must stop reliably without forcing abrupt termination.


\###Advantages

* Improves reliability and deployability.
* Reduces corruption risk for persistence layers.
  Supports test harnesses that start and stop the system repeatedly.



\###Codebase usage

* std::jthread and std::stop\_token for worker threads
  Cooperative cancellation in pipeline workers, web server, and audio subsystems
  Unified shutdown sequence in src/noctil/app/



\##Where these patterns converge
The patterns are intended to reinforce each other:


* Ports and Adapters isolates the domain.
  Interfaces and strategies enable swapping algorithms and backends.
  State machines and bounded queues enforce deterministic behavior.
  Observer and commands keep UI and telemetry decoupled.
  RAII and cooperative cancellation ensure long-running stability.



This combination is aligned with embedded systems engineering best practices focused on correctness, maintainability, and operational reliability.

