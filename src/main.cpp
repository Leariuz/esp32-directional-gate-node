#include <Arduino.h>

// ---------------- CONFIG ----------------
constexpr int PIN_A = 4;
constexpr int PIN_B = 16;

constexpr uint32_t DEBOUNCE_MS = 25;
constexpr uint32_t SEQ_TIMEOUT_MS = 900;
constexpr uint32_t RESTORE_STABLE_MS = 120;

const char* NODE_ID = "door_01";
// ----------------------------------------

// -------- Beam State Structure ----------
struct Beam {
  int pin;
  int intactLevel;
  int brokenLevel;

  int stableState;
  int lastRead;
  uint32_t lastChangeMs;

  void begin() {
    pinMode(pin, INPUT_PULLUP);
    delay(10);
    intactLevel = digitalRead(pin);
    brokenLevel = (intactLevel == HIGH) ? LOW : HIGH;
    stableState = intactLevel;
    lastRead = stableState;
    lastChangeMs = millis();
  }

  bool update() {
    int r = digitalRead(pin);

    if (r != lastRead) {
      lastRead = r;
      lastChangeMs = millis();
    }

    if ((millis() - lastChangeMs) >= DEBOUNCE_MS &&
        stableState != lastRead) {
      stableState = lastRead;
      return true;  // state changed
    }

    return false;
  }

  bool isBroken() const { return stableState == brokenLevel; }
  bool isIntact() const { return stableState == intactLevel; }
};
// ----------------------------------------

Beam beamA{PIN_A};
Beam beamB{PIN_B};

// -------- Direction State Machine -------
enum class State { Idle, AFirst, BFirst, Locked };

State state = State::Idle;
uint32_t tFirst = 0;
uint32_t intactSince = 0;
uint32_t eventSeq = 0;

void emitEvent(const char* dir) {
  eventSeq++;

  Serial.print("{\"v\":1,\"node\":\"");
  Serial.print(NODE_ID);
  Serial.print("\",\"type\":\"pass\",\"dir\":\"");
  Serial.print(dir);
  Serial.print("\",\"ms\":");
  Serial.print(millis());
  Serial.print(",\"seq\":");
  Serial.print(eventSeq);
  Serial.println("}");
}

void setup() {
  Serial.begin(115200);
  delay(200);

  beamA.begin();
  beamB.begin();

  Serial.println("{\"v\":1,\"type\":\"status\",\"msg\":\"node_online\"}");
}

void loop() {

  bool aChanged = beamA.update();
  bool bChanged = beamB.update();

  // Timeout partial sequence
  if (state != State::Idle && state != State::Locked &&
      millis() - tFirst > SEQ_TIMEOUT_MS) {
    state = State::Idle;
  }

  switch (state) {

    case State::Idle:
      if (beamA.isBroken() && !beamB.isBroken()) {
        state = State::AFirst;
        tFirst = millis();
      }
      else if (beamB.isBroken() && !beamA.isBroken()) {
        state = State::BFirst;
        tFirst = millis();
      }
      break;

    case State::AFirst:
      if (beamB.isBroken()) {
        emitEvent("A2B");
        state = State::Locked;
        intactSince = 0;
      }
      break;

    case State::BFirst:
      if (beamA.isBroken()) {
        emitEvent("B2A");
        state = State::Locked;
        intactSince = 0;
      }
      break;

    case State::Locked:
      // Require both beams intact before re-arming
      if (beamA.isIntact() && beamB.isIntact()) {
        if (intactSince == 0)
          intactSince = millis();

        if (millis() - intactSince >= RESTORE_STABLE_MS) {
          state = State::Idle;
          intactSince = 0;
        }
      } else {
        intactSince = 0;
      }
      break;
  }
}