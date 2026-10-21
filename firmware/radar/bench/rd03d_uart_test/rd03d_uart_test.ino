// RD-03D 24GHz radar test for ESP32-S3 DevKitC-1 (Arduino core)
// Wiring: RD-03D TX -> GPIO2 (ESP RX), RD-03D RX -> GPIO1 (ESP TX), 5V, GND
// Output: prints x / y / speed / distance / angle for each valid target over USB serial

#include <Arduino.h>
#include <math.h>

// ---------- Config ----------
constexpr int      RADAR_RX_PIN   = 2;
constexpr int      RADAR_TX_PIN   = 1;
constexpr uint32_t RADAR_BAUD     = 256000;   // RD-03D default, NOT 115200
constexpr bool     USE_MULTI_TARGET = false;  // power-on default is single-target
constexpr bool     DEBUG_HEX      = false;    // true = also dump raw frame bytes

// ---------- Frame format ----------
// AA FF 03 00 | target1 (8B) | target2 (8B) | target3 (8B) | 55 CC
constexpr uint8_t HEADER[4]   = {0xAA, 0xFF, 0x03, 0x00};
constexpr uint8_t TAIL[2]     = {0x55, 0xCC};
constexpr size_t  PAYLOAD_LEN = 24;
constexpr int     MAX_TARGETS = 3;

struct Target {
  bool     valid;
  int16_t  x_mm;
  int16_t  y_mm;
  int16_t  speed_cms;
  uint16_t res_mm;
  float    dist_mm;
  float    angle_deg;   // 0 = straight ahead, + = right (sign follows x)
};

// ---------- Parser state machine ----------
enum class State { FIND_HEADER, READ_PAYLOAD, READ_TAIL };

static State   state = State::FIND_HEADER;
static uint8_t idx   = 0;
static uint8_t payload[PAYLOAD_LEN];

// Feed one byte. Returns true when a complete, tail-checked frame is in payload[].
bool feedByte(uint8_t b) {
  switch (state) {
    case State::FIND_HEADER:
      if (b == HEADER[idx]) {
        if (++idx == sizeof(HEADER)) { state = State::READ_PAYLOAD; idx = 0; }
      } else {
        // Mismatch: this byte might itself be the start of a new header
        idx = (b == HEADER[0]) ? 1 : 0;
      }
      break;

    case State::READ_PAYLOAD:
      payload[idx++] = b;
      if (idx == PAYLOAD_LEN) { state = State::READ_TAIL; idx = 0; }
      break;

    case State::READ_TAIL:
      if (b == TAIL[idx]) {
        if (++idx == sizeof(TAIL)) { state = State::FIND_HEADER; idx = 0; return true; }
      } else {
        // Bad tail: drop frame and resync
        state = State::FIND_HEADER;
        idx = (b == HEADER[0]) ? 1 : 0;
      }
      break;
  }
  return false;
}

// RD-03D sign rule: bit15 = 1 -> positive, bit15 = 0 -> negative. Magnitude = low 15 bits.
int16_t decodeSigned(uint16_t raw) {
  int16_t mag = raw & 0x7FFF;
  return (raw & 0x8000) ? mag : -mag;
}

uint16_t readLE16(const uint8_t* p) {
  return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

void parseTargets(const uint8_t* p, Target* t) {
  for (int i = 0; i < MAX_TARGETS; i++) {
    const uint8_t* q = p + i * 8;
    uint16_t rx = readLE16(q + 0);
    uint16_t ry = readLE16(q + 2);
    uint16_t rs = readLE16(q + 4);
    uint16_t rr = readLE16(q + 6);

    t[i].valid = !(rx == 0 && ry == 0 && rs == 0 && rr == 0);
    if (!t[i].valid) continue;

    t[i].x_mm      = decodeSigned(rx);
    t[i].y_mm      = decodeSigned(ry);
    t[i].speed_cms = decodeSigned(rs);
    t[i].res_mm    = rr;

    float x = t[i].x_mm, y = t[i].y_mm;
    t[i].dist_mm   = sqrtf(x * x + y * y);
    t[i].angle_deg = atan2f(x, y) * 180.0f / PI;
  }
}

// ---------- Optional: switch to multi-target mode ----------
void sendCmd(const uint8_t* cmd, size_t len) {
  Serial1.write(cmd, len);
  Serial1.flush();
  delay(100);   // give the radar time to ACK (ACK frames are skipped by the parser)
}

void enableMultiTarget() {
  const uint8_t enterCfg[] = {0xFD,0xFC,0xFB,0xFA, 0x04,0x00, 0xFF,0x00, 0x01,0x00, 0x04,0x03,0x02,0x01};
  const uint8_t multi[]    = {0xFD,0xFC,0xFB,0xFA, 0x02,0x00, 0x90,0x00,            0x04,0x03,0x02,0x01};
  const uint8_t exitCfg[]  = {0xFD,0xFC,0xFB,0xFA, 0x02,0x00, 0xFE,0x00,            0x04,0x03,0x02,0x01};
  sendCmd(enterCfg, sizeof(enterCfg));
  sendCmd(multi,    sizeof(multi));
  sendCmd(exitCfg,  sizeof(exitCfg));
  while (Serial1.available()) Serial1.read();   // clear ACK bytes
}

// ---------- Main ----------
static uint32_t lastFrameMs = 0;
static uint32_t frameCount  = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial1.setRxBufferSize(1024);   // must be BEFORE begin()
  Serial1.begin(RADAR_BAUD, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);

  delay(200);
  if (USE_MULTI_TARGET) enableMultiTarget();

  Serial.println("RD-03D test started");
  lastFrameMs = millis();
}

void loop() {
  while (Serial1.available()) {
    if (!feedByte((uint8_t)Serial1.read())) continue;

    frameCount++;
    lastFrameMs = millis();

    if (DEBUG_HEX) {
      Serial.print("RAW: ");
      for (size_t i = 0; i < PAYLOAD_LEN; i++) Serial.printf("%02X ", payload[i]);
      Serial.println();
    }

    Target t[MAX_TARGETS];
    parseTargets(payload, t);

    bool any = false;
    for (int i = 0; i < MAX_TARGETS; i++) {
      if (!t[i].valid) continue;
      any = true;
      Serial.printf("#%lu T%d  x=%6d mm  y=%6d mm  v=%5d cm/s  d=%7.1f mm  ang=%6.1f deg\n",
                    (unsigned long)frameCount, i + 1,
                    t[i].x_mm, t[i].y_mm, t[i].speed_cms,
                    t[i].dist_mm, t[i].angle_deg);
    }
    if (!any) Serial.printf("#%lu no target\n", (unsigned long)frameCount);
  }

  // No frames for 1 s -> likely wiring / baud / power problem
  if (millis() - lastFrameMs > 1000) {
    Serial.println("WARN: no valid frame in 1 s (check TX/RX swap, 256000 baud, 5V power)");
    lastFrameMs = millis();
  }
}
