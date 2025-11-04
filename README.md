## Alertify — Women Safety Wearable

> Lightweight two-part emergency alert system using an ESP32-based wearable (sender) and a receiver base-station that can notify via SMS/call using a SIM900 module and sound/LED alerts.

This repository contains sample firmware for:
- `sender_esp32c3/sender_code.ino` — wearable device (ESP32-C3) that sends an nRF24L01 packet and triggers an IFTTT webhook over WiFi when the SOS button is long-pressed.
- `receiver_base_station/receiver_code.ino` — base station that listens for nRF24L01 packets, flashes an alert pin, sends an SMS, and places a call using a SIM900 GSM module.

## Highlights
- Ultra-low-latency RF (nRF24L01) for local alerts.
- WiFi webhook integration (IFTTT) from the wearable as a secondary/remote alert path.
- GSM SMS & call fallback from the receiver for direct phone alerts.

## Quick start

1. Clone this repo (you already have it):

   git clone https://github.com/DEEPIKA2827/Alertify-Women-Safety-Wearable

2. Open either `sender_esp32c3/sender_code.ino` or `receiver_base_station/receiver_code.ino` in the Arduino IDE or PlatformIO.

3. Edit configuration placeholders before flashing:
   - In `sender_code.ino` set your WiFi SSID/password and replace `YOUR_IFTTT_KEY` in the `webhook` string.
   - In `receiver_code.ino` replace `+91XXXXXXXXXX` with the phone number you want the receiver to SMS/call.

4. Install required libraries in the Arduino IDE Library Manager (or add to PlatformIO):
   - RF24 (and nRF24L01 driver)
   - WiFi (part of ESP32 core)
   - HTTPClient (part of ESP32 core)

5. Select the correct board and upload:
   - Sender: choose your ESP32-C3 board (Tools → Board → ESP32C3 or your dev board) and upload `sender_code.ino`.
   - Receiver: choose the ESP32 board you use for the base (or compatible board) and upload `receiver_code.ino`.

## Wiring / Hardware notes

General nRF24L01 wiring (connect to 3.3V, not 5V):

- nRF24L01 VCC -> 3.3V
- GND -> GND
- MOSI -> MCU MOSI (hardware SPI)
- MISO -> MCU MISO (hardware SPI)
- SCK  -> MCU SCK (hardware SPI)
- CSN  -> pin defined as CSN_PIN in the sketch (see below)
- CE   -> pin defined as CE_PIN in the sketch (see below)

Pay attention to the pin defines in each sketch (these are the pins used in code):

- Sender (`sender_code.ino`):
  - CE_PIN = 9
  - CSN_PIN = 10
  - Button: GPIO 0 (buttonPin = 0)

- Receiver (`receiver_code.ino`):
  - CE_PIN = 4
  - CSN_PIN = 5
  - ALERT_PIN = 2 (LED / buzzer)
  - SIM900 UART uses pins 16 (RX) and 17 (TX) via `sim900.begin(9600, SERIAL_8N1, 16, 17);`

Notes:
- CE/CSN may be any available digital pins — they are not tied to the SPI peripheral. The MOSI/MISO/SCK lines must be wired to the MCU hardware SPI pins.
- nRF24L01 modules draw bursts of current; use a stable 3.3V regulator or bypass capacitors if you see instability.

## How it works (high level)

- Sender: a long button press (>2s) triggers `sendSOS()` which:
  1. Sends a small RF packet (struct Packet) via nRF24L01 to the receiver.
  2. Attempts an HTTP POST to a configured IFTTT webhook (if WiFi is available) with a JSON payload.

- Receiver: when an RF packet arrives the base station:
  1. Flashes the alert output (LED/buzzer) briefly.
  2. Sends an SMS via SIM900 with a quick alert message.
  3. Places a call to the configured number for immediate attention.

## Serial output examples

- Sender (when sending):
  - "nRF24 initialized!"
  - "SOS via nRF24 sent!"
  - "SOS via WiFi HTTP code: 200"

- Receiver (on SOS):
  - "📩 SOS Received!"
  - "✅ SMS Sent."
  - "📞 Call ended."

## Troubleshooting

- nRF24L01 not initializing: check CE/CSN wiring, power (3.3V), and try lowering PA level in code.
- WiFi failing on sender: confirm SSID/password and that the ESP32 board has the correct WiFi drivers installed.
- SIM900 issues: check SIM card activation, antenna, baud rate (9600), and GSM network availability. Use serial monitor to read AT responses.

## Security & privacy

- The webhook key and phone number are sensitive — do not commit production keys to public repos.
- For production use consider encrypting or storing tokens outside the sketch and avoid hardcoding secrets.

## Next improvements (suggestions)

- Add a `.env` or config header to keep secrets out of version history.
- Add OTA flashing for the sender so you can update firmware remotely.
- Add battery monitoring and low-power sleep mode on the wearable.
- Improve location reporting (use GPS or WiFi geolocation) and include it in messages.

## License

This project has no license file yet. If you want to open-source it, consider adding an OSI-approved license such as MIT or Apache-2.0.

---
If you want, I can also:
- add a tailored `.gitignore` (I will add one now),
- create a `LICENSE` (MIT) and
- improve the README with wiring diagrams or photos you provide.

Questions or changes you'd like in the README? Tell me and I'll update it and push the changes.
# Alertify-Women-Safety-Wearable
⚡ Alertify – A smart wearable that never stays silent in danger! Dual RF + Wi-Fi SOS alert system built on ESP32-C3.
