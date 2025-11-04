#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <HardwareSerial.h>

#define CE_PIN 4
#define CSN_PIN 5
#define ALERT_PIN 2  // LED or buzzer output

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "NODE1";

HardwareSerial sim900(2);  // Use UART2 -> GPIO16 (RX), GPIO17 (TX)

struct Packet {
  uint8_t device_id;
  uint8_t type;
  uint16_t seq;
};

const char phoneNumber[] = "+91XXXXXXXXXX"; // Replace with your phone number

void setup() {
  Serial.begin(115200);
  pinMode(ALERT_PIN, OUTPUT);
  digitalWrite(ALERT_PIN, LOW);

  // Initialize nRF24L01
  if (!radio.begin()) {
    Serial.println("nRF24 initialization failed!");
    while (true) { delay(1000); }
  }
  radio.openReadingPipe(1, (uint64_t)0xF0F0F0F0E1LL);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  Serial.println("Receiver ready, waiting for SOS...");

  // Initialize SIM900A
  sim900.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Initializing SIM900A...");
  delay(2000);
  sim900.println("AT");
  waitForResponse();
  sim900.println("AT+CMGF=1");
  waitForResponse();
}

void loop() {
  if (radio.available()) {
    Packet p;
    radio.read(&p, sizeof(p));

    Serial.println("📩 SOS Received!");
    Serial.print("Device ID: "); Serial.println(p.device_id);
    Serial.print("Seq #: "); Serial.println(p.seq);

    digitalWrite(ALERT_PIN, HIGH);
    delay(500);
    digitalWrite(ALERT_PIN, LOW);

    sendSMS(phoneNumber, "🚨 SOS ALERT from Device 1! Please check immediately.");
    makeCall(phoneNumber);
  }
}

void sendSMS(const char* number, const char* message) {
  Serial.println("Sending SMS...");
  sim900.println("AT+CMGF=1");
  delay(500);
  sim900.print("AT+CMGS=\"");
  sim900.print(number);
  sim900.println("\"");
  delay(500);
  sim900.print(message);
  delay(500);
  sim900.write(26);  // Ctrl+Z
  delay(3000);
  Serial.println("✅ SMS Sent.");
}

void makeCall(const char* number) {
  Serial.println("Dialing call...");
  sim900.print("ATD");
  sim900.print(number);
  sim900.println(";");
  delay(15000); // Wait 15 seconds before hanging up
  sim900.println("ATH");
  Serial.println("📞 Call ended.");
}

void waitForResponse() {
  unsigned long start = millis();
  while (millis() - start < 2000) {
    if (sim900.available()) {
      String response = sim900.readString();
      Serial.println(response);
      break;
    }
  }
}