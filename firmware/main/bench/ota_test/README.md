# ota_test — bootloader + OTA bench sketch

Standalone Arduino-ESP32 sketch for an ESP32-S3 DevKitC-1. It does two things only:

1. At boot, prints which app slot the bootloader started (`Running`), which slot is marked to
   boot (`Boot`), where the next OTA will go (`Next OTA`), the OTA state of every app slot,
   and the reset reason.
2. Accepts new firmware over Wi-Fi with ArduinoOTA.

No radar, audio, or motor code.

## How it fits together

```
flash:  [bootloader] [partition table] [otadata] [app0] [app1] ...
                                          │
            bootloader reads otadata ─────┘──▶ starts app0 or app1
OTA:    running app writes new image into the *other* slot ──▶ checks it ──▶ updates otadata ──▶ reboot
```

If the upload is cut off or the image check fails, otadata is not changed, so the bootloader
keeps starting the old slot.

## Setup

1. Copy `secrets.h.example` to `secrets.h` in this folder and fill in Wi-Fi and OTA password.
   `secrets.h` is git-ignored.
2. Arduino IDE with the ESP32 core, board **ESP32S3 Dev Module**.
3. **Flash Size** must match your module (DevKitC-1-N8 = 8 MB). **Partition Scheme** must have
   two app slots (e.g. "Default 4MB with spiffs" or "8M with spiffs"). "Huge APP" has no
   OTA slot and the OTA will fail with `begin failed`.
4. Serial monitor at 115200. If you use the native USB port instead of the UART port, turn on
   **USB CDC On Boot**.

Use a phone hotspot or home router. eduroam (WPA2-Enterprise) won't work with
`WiFi.begin(ssid, pass)`, and campus networks often block the mDNS/UDP traffic OTA needs.
The laptop must be on the same network.

## Test procedure

| # | Step | Expected |
| --- | --- | --- |
| 1 | `FW_VERSION = "v1"`, upload over **USB** | Boot info shows `Running: app0`, `Next OTA: app1` |
| 2 | Change to `"v2"`, pick the network port `robot-ota-test`, upload | `OTA: 0% … 100%`, reboot, reset = software, `Running: app1`, version v2 |
| 3 | Change to `"v3"`, upload over the network again | Back to `Running: app0`, version v3 |
| 4 | Start an OTA upload and turn off the hotspot (or unplug the board) halfway | Board comes back on the **previous** version and slot |
| 5 | Wrong OTA password in the IDE | `auth failed`, nothing changes |
| 6 | Upload over USB again | Back to `app0` (the USB upload also rewrites otadata) |

Record results in `test/main/`.

## Pitfalls

- The network port can take ~10 s to appear in the IDE after boot (mDNS). If it never shows up,
  check the hotspot and laptop firewall (espota needs UDP 3232 to the board and a TCP
  connection back to the laptop).
- Each new firmware must still contain the OTA code, or you lose OTA until the next USB flash.
- App rollback (`PENDING_VERIFY` state) is not part of this test. It depends on how the
  bootloader is built; revisit once the firmware framework is decided (`docs/decisions.md`).
