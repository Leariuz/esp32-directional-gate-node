🚪 ESP32 Bidirectional Zone Tracking Node

A modular ESP32-based infrared gate sensor for directional movement detection and real-time zone occupancy tracking.

Designed as a reusable edge device for warehouse flow tracking, dock monitoring, and boundary-based inventory systems.

📦 What This Project Does

This system uses two IR break-beam sensors and an ESP32 to:

✅ Detect beam interruptions

✅ Determine direction of movement (A → B or B → A)

✅ Eliminate multi-trigger chatter

✅ Emit structured JSON events

✅ Maintain live zone occupancy count

The node acts as a boundary event sensor that can plug into dashboards, backend services, or IoT systems.

🧠 System Architecture

The design follows a clean, layered structure:

1️⃣ Beam Filtering Layer

GPIO input with INPUT_PULLUP

Software debounce timing

Stable-state validation

2️⃣ Direction Detection Layer

Finite State Machine (FSM):

Idle → AFirst → Locked
Idle → BFirst → Locked

Direction logic:

Beam Order	Direction
A → B	A2B
B → A	B2A

Protection mechanisms:

Debounce window

Sequence timeout

Restore-to-rearm lock state

Occupancy clamping (no negative values)

3️⃣ Event & Zone Layer

Structured JSON output

Event sequence counter

Zone occupancy tracking

🔌 Hardware Requirements

ESP32 Dev Board (ESP-WROOM-32 compatible)

2× IR Break-Beam Sensors

Jumper wires

USB cable

🧩 Wiring
Beam A

VCC → VIN (5V) or 3V3 (module dependent)

GND → GND

OUT → GPIO 4

Beam B

VCC → VIN (5V) or 3V3

GND → GND

OUT → GPIO 16

⚠ Important:

Both beams must be aligned and intact during boot.

All grounds must be common.

🔄 Direction Mapping

Current configuration:

B2A → Enter Zone A

A2B → Exit Zone A

Mapping can be modified depending on physical installation.

📡 Output Format (NDJSON)

Each event is printed as newline-delimited JSON.

▶ Pass Event
{"v":1,"node":"door_01","type":"pass","dir":"B2A","ms":128100,"seq":59}
Field	Description
v	Protocol version
node	Device identifier
type	Event type
dir	Direction
ms	Milliseconds since boot
seq	Monotonic event counter

Adjust based on:

Beam spacing

Movement speed

Environmental lighting

Object size

🛡 Behavior Guarantees

✔ One event per full crossing
✔ No chatter-induced double triggers
✔ No repeated triggers while object remains in beam
✔ Deterministic direction detection
✔ Occupancy never drops below zero

🏭 Example Use Cases

Warehouse dock monitoring

Staging area flow tracking

Forklift traffic counting

Entry/exit monitoring

Industrial automation prototypes

⚠ Limitations

Occupancy count resets on reboot (non-persistent)

No object identity tracking (RFID/BLE not included)

Assumes all traffic passes through monitored gate

🚀 Future Improvements

WiFi + MQTT transport

BLE tag association

Persistent occupancy storage (NVS)

Multi-node synchronization

OTA firmware updates

Real-time dashboard integration

📜 License

MIT
