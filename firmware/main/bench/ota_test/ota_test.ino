// Bootloader + OTA test for ESP32-S3 DevKitC-1 (Arduino core)
// - Prints which app partition the bootloader started, and the OTA state of each slot
// - Accepts firmware over Wi-Fi with ArduinoOTA (Arduino IDE "network port" / espota)
// Nothing else: no radar, audio, motors.
//
// Test: flash FW_VERSION "v1" over USB, then change it to "v2" and upload over the network.
// After reboot, "Running" should move app0 -> app1 and the version should read v2.

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>
#include <esp_system.h>

#include "secrets.h"   // WIFI_SSID, WIFI_PASS, OTA_PASSWORD (copy secrets.h.example)

// ---------- Config ----------
constexpr char     FW_VERSION[]       = "v1";              // change before each OTA upload
constexpr char     HOSTNAME[]         = "robot-ota-test";  // shows up as the network port
constexpr uint32_t WIFI_TIMEOUT_MS    = 20000;             // restart if Wi-Fi not up by then
constexpr uint32_t HEARTBEAT_MS       = 5000;

// ---------- Boot info ----------
const char* resetReasonStr(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_POWERON:   return "power-on";
    case ESP_RST_EXT:       return "external pin";
    case ESP_RST_SW:        return "software (esp_restart)";   // expected after OTA
    case ESP_RST_PANIC:     return "panic";
    case ESP_RST_INT_WDT:   return "interrupt watchdog";
    case ESP_RST_TASK_WDT:  return "task watchdog";
    case ESP_RST_WDT:       return "other watchdog";
    case ESP_RST_DEEPSLEEP: return "deep sleep";
    case ESP_RST_BROWNOUT:  return "brownout";
    default:                return "other/unknown";
  }
}

const char* otaStateStr(esp_ota_img_states_t s) {
  switch (s) {
    case ESP_OTA_IMG_NEW:            return "NEW";
    case ESP_OTA_IMG_PENDING_VERIFY: return "PENDING_VERIFY";
    case ESP_OTA_IMG_VALID:          return "VALID";
    case ESP_OTA_IMG_INVALID:        return "INVALID";
    case ESP_OTA_IMG_ABORTED:        return "ABORTED";
    default:                         return "UNDEFINED";
  }
}

void printPartition(const char* tag, const esp_partition_t* p) {
  if (p == nullptr) {
    Serial.printf("  %-9s: (none)\n", tag);
    return;
  }
  esp_ota_img_states_t st;
  const char* state = (esp_ota_get_state_partition(p, &st) == ESP_OK) ? otaStateStr(st) : "-";
  Serial.printf("  %-9s: %-6s @0x%06lX  %5lu KB  state=%s\n", tag, p->label,
                (unsigned long)p->address, (unsigned long)(p->size / 1024), state);
}

void printBootInfo() {
  const esp_partition_t* running = esp_ota_get_running_partition();
  const esp_partition_t* boot    = esp_ota_get_boot_partition();
  const esp_partition_t* next    = esp_ota_get_next_update_partition(nullptr);

  Serial.println();
  Serial.println("===== Boot info =====");
  Serial.printf("  FW version: %s (built %s %s)\n", FW_VERSION, __DATE__, __TIME__);
  Serial.printf("  Reset     : %s\n", resetReasonStr(esp_reset_reason()));
  printPartition("Running", running);
  printPartition("Boot", boot);
  printPartition("Next OTA", next);
  if (running != boot) {
    Serial.println("  NOTE: running != boot (boot slot was changed, or that image failed to start)");
  }

  Serial.println("  App partitions:");
  // esp_partition_next() frees the iterator when it returns nullptr, so no release call here
  for (esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP,
                                                        ESP_PARTITION_SUBTYPE_ANY, nullptr);
       it != nullptr; it = esp_partition_next(it)) {
    printPartition("", esp_partition_get(it));
  }
  Serial.println("=====================");
}

// ---------- Wi-Fi + OTA ----------
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.printf("Wi-Fi: connecting to \"%s\"", WIFI_SSID);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > WIFI_TIMEOUT_MS) {
      Serial.println("\nWi-Fi: timeout, restarting");
      delay(100);
      ESP.restart();
    }
    delay(500);
    Serial.print('.');
  }
  Serial.printf("\nWi-Fi: connected, IP %s, RSSI %d dBm\n",
                WiFi.localIP().toString().c_str(), WiFi.RSSI());
}

void setupOTA() {
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    Serial.printf("OTA: start (%s)\n",
                  ArduinoOTA.getCommand() == U_FLASH ? "firmware" : "filesystem");
  });
  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
    static int lastTenth = -1;               // print every 10 %
    int pct = (int)(done * 100UL / total);
    if (pct / 10 != lastTenth) {
      lastTenth = pct / 10;
      Serial.printf("OTA: %3d%%\n", pct);
    }
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA: done, rebooting into the new slot");
  });
  ArduinoOTA.onError([](ota_error_t e) {
    const char* why = (e == OTA_AUTH_ERROR)    ? "auth failed"
                    : (e == OTA_BEGIN_ERROR)   ? "begin failed (partition too small / no OTA slot?)"
                    : (e == OTA_CONNECT_ERROR) ? "connect failed"
                    : (e == OTA_RECEIVE_ERROR) ? "receive failed"
                    : (e == OTA_END_ERROR)     ? "end failed (image check)"
                    : "unknown";
    Serial.printf("OTA: error %u: %s — still running the old firmware\n", (unsigned)e, why);
  });

  ArduinoOTA.begin();
  Serial.printf("OTA: ready, host \"%s\"\n", HOSTNAME);
}

// ---------- Main ----------
void setup() {
  Serial.begin(115200);
  delay(500);

  printBootInfo();
  connectWiFi();
  setupOTA();
}

void loop() {
  ArduinoOTA.handle();

  static uint32_t lastBeat = 0;
  if (millis() - lastBeat >= HEARTBEAT_MS) {
    lastBeat = millis();
    Serial.printf("alive: %s on %s, IP %s\n", FW_VERSION,
                  esp_ota_get_running_partition()->label,
                  WiFi.localIP().toString().c_str());
  }
}
