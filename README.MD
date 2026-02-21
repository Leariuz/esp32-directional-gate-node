# ESP32 Bidirectional Zone Tracking Node

## Overview

This project implements a bidirectional infrared gate sensor using an
ESP32. The system detects directional movement across a boundary and
maintains a real-time zone occupancy count.

The node functions as a modular boundary event device suitable for
warehouse flow monitoring, dock tracking, and industrial boundary
sensing applications.

------------------------------------------------------------------------

## System Architecture

                        +---------------------------+
                        |        Zone A             |
                        |    (Tracked Occupancy)    |
                        +-------------^-------------+
                                      |
                                      |  B2A = Enter
                                      |  A2B = Exit
                                      |
            Beam A             Beam B
         +----------+       +----------+
         | IR RX/TX |       | IR RX/TX |
         +-----+----+       +----+-----+
               |                  |
               | GPIO 4           | GPIO 16
               |                  |
         +----------------------------------+
         |            ESP32 Node            |
         |----------------------------------|
         |  Beam Filtering (Debounce)       |
         |  Direction State Machine (FSM)   |
         |  Zone Occupancy Logic            |
         |  JSON Event Emission             |
         +----------------------------------+
                        |
                        | Serial (NDJSON)
                        | Future: MQTT / BLE
                        v
                +------------------+
                | UI / Backend     |
                | Dashboard        |
                +------------------+

------------------------------------------------------------------------

## Objectives

-   Detect beam interruption events
-   Determine direction of movement (A→B or B→A)
-   Eliminate multi-trigger chatter
-   Emit structured JSON event output
-   Maintain zone occupancy state
-   Provide UI-ready event stream architecture

------------------------------------------------------------------------

## Hardware Requirements

-   ESP32 Dev Board (ESP-WROOM-32 compatible)
-   Two IR break-beam sensor modules
-   Jumper wires
-   USB power and programming cable

------------------------------------------------------------------------

## Wiring

### Beam A

-   VCC → VIN (5V) or 3V3 (module dependent)
-   GND → GND
-   OUT → GPIO 4

### Beam B

-   VCC → VIN (5V) or 3V3
-   GND → GND
-   OUT → GPIO 16

Both beams must be aligned and intact during boot. All grounds must
share a common reference.

------------------------------------------------------------------------

## Direction Detection

### Beam Sequence Logic

  Beam Sequence   Direction
  --------------- -----------
  A → B           A2B
  B → A           B2A

### Zone Mapping

Current configuration:

-   B2A → Enter Zone A\
-   A2B → Exit Zone A

------------------------------------------------------------------------

## Output Format

All output is newline-delimited JSON (NDJSON).

### Pass Event

``` json
{"v":1,"node":"door_01","type":"pass","dir":"B2A","ms":128100,"seq":59}
```

### Zone Update

``` json
{"v":1,"node":"door_01","type":"zone","zone":"A","occ":5,"ms":128105,"seq":59}
```

------------------------------------------------------------------------

## Tuning Parameters

``` cpp
constexpr uint32_t DEBOUNCE_MS = 25;
constexpr uint32_t SEQ_TIMEOUT_MS = 900;
constexpr uint32_t RESTORE_STABLE_MS = 120;
```

Adjust based on beam spacing, object speed, and environmental
conditions.

------------------------------------------------------------------------

## Behavioral Guarantees

-   Single event per full crossing
-   No chatter-induced double triggers
-   No repeated triggers while object remains in beam
-   Deterministic direction detection
-   Occupancy value never drops below zero

------------------------------------------------------------------------

## Limitations

-   Occupancy resets on reboot (non-persistent state)
-   No object identity tracking (RFID/BLE not implemented)
-   Assumes all traffic passes through the monitored gate

------------------------------------------------------------------------

## Future Extensions

-   WiFi MQTT transport
-   BLE-based object identification
-   Persistent occupancy storage (NVS)
-   Multi-node synchronization
-   OTA firmware updates
-   Dashboard integration

------------------------------------------------------------------------

## License

MIT License
