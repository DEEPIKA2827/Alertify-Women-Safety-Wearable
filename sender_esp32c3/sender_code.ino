#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define CE_PIN   9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "NODE1";

const int buttonPin = 0;    // Change this if you choose another GPIO
unsigned long pressStart = 0;
bool pressed = false;
const unsigned long LONGPRESS_MS = 2000;

const char* ssid     = "DSATM";
const char* password = "";
String webhook       = "https://maker.ifttt.com/trigger/sos_alert/with/key/YOUR_IFTTT_KEY";

struct Packet {
  uint8_t  device_id;
  uint8_t  type;       // 1 = SOS
  uint16_t seq;
};
uint16_t seqnum = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);

  // WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());

  // nRF24
  if (!radio.begin()) {
    Serial.println("nRF24 init failed!");
    while (true) { delay(1000); }
  }
  radio.openWritingPipe((uint64_t)0xF0F0F0F0E1LL);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
  Serial.println("nRF24 initialized!");
}

void loop() {
  if (digitalRead(buttonPin) == LOW && !pressed) {
    pressed = true;
    pressStart = millis();
  }
  else if (digitalRead(buttonPin) == LOW && pressed) {
    if (millis() - pressStart > LONGPRESS_MS) {
      sendSOS();
      pressed = false;
    }
  }
  else if (digitalRead(buttonPin) == HIGH && pressed) {
    pressed = false;
  }

  delay(100);
}

void sendSOS() {
  Packet p;
  p.device_id = 1;
  p.type      = 1;
  p.seq       = seqnum++;

  bool ok = radio.write(&p, sizeof(p));
  Serial.println(ok ? "SOS via nRF24 sent!" : "SOS via nRF24 failed!");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(webhook);
    http.addHeader("Content-Type", "application/json");
    String body = "{\"value1\":\"SOS ALERT\",\"value2\":\"Device1\",\"value3\":\"Location via WiFi\"}";
    int code = http.POST(body);
    Serial.println("SOS via WiFi HTTP code: " + String(code));
    http.end();
  } else {
    Serial.println("WiFi not connected, cannot send WiFi SOS.");
  }
}