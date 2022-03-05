#include <SoftwareSerial.h>
#include <Wire.h>

const int AirValue = 444;
const int WaterValue = 192;
int moistureValue1 = 0;
int moisturePercent1 = 0;
int moistureValue2 = 0;
int moisturePercent2 = 0;
int soilmoisturepercent = 0;

int periodSeconds = 6;

SoftwareSerial server(2, 3);

#define RE 8
#define DE 7

const byte nitro[] = {0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c};
const byte phos[] = {0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0};

byte values[11];
SoftwareSerial mod(5, 6);

void setup() {
  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps
  server.begin(9600);

  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);

  // put RS-485 into receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  delay(500);
  delay(3000);
}
void loop() {
  moistureValue1 = analogRead(A0);
  moisturePercent1 = map(moistureValue1, AirValue, WaterValue, 0, 100);
  moisturePercent1 = constrain(moisturePercent1, 0, 100);

  moistureValue2 = analogRead(A1);
  moisturePercent2 = map(moistureValue2, AirValue, WaterValue, 0, 100);
  moisturePercent2 = constrain(moisturePercent2, 0, 100);

  soilmoisturepercent = (moisturePercent1 + moisturePercent2) / 2;

  byte val1, val2, val3;
  val1 = nitrogen();
  delay(500);

  val2 = phosphorous();
  delay(500);

  val3 = potassium();
  
  delay(3000);

  server.print("http://plantcare.cloud/writeRecords?moisture=");
  server.print(soilmoisturepercent);
  server.print("&nitrogen=");
  server.print(val1);
  server.print("&phosphorous=");
  server.print(val2);
  server.print("&potassium=");
  server.println(val3);

  Serial.print("http://plantcare.cloud/writeRecords?moisture=");
  Serial.print(soilmoisturepercent);
  Serial.print("&nitrogen=");
  Serial.print(val1);
  Serial.print("&phosphorous=");
  Serial.print(val2);
  Serial.print("&potassium=");
  Serial.println(val3);

  delay(periodSeconds * 1000);
}

byte nitrogen() {
  // clear the receive buffer
  mod.flush();

  // switch RS-485 to transmit mode
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);

  // write out the message
  for (uint8_t i = 0; i < sizeof(nitro); i++ ) mod.write( nitro[i] );

  // wait for the transmission to complete
  mod.flush();
  
  // switching RS485 to receive mode
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);

  // delay to allow response bytes to be received!
  delay(200);

  // read in the received bytes
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    Serial.print(values[i], HEX);
    Serial.print(' ');
  }
  return values[4];
}

byte phosphorous() {
  mod.flush();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(phos); i++ ) mod.write( phos[i] );
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
// delay to allow response bytes to be received!
  delay(200);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    Serial.print(values[i], HEX);
    Serial.print(' ');
  }
  return values[4];
}

byte potassium() {
  mod.flush();
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(1);
  for (uint8_t i = 0; i < sizeof(pota); i++ ) mod.write( pota[i] );
  mod.flush();
  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
// delay to allow response bytes to be received!
  delay(200);
  for (byte i = 0; i < 7; i++) {
    values[i] = mod.read();
    Serial.print(values[i], HEX);
    Serial.print(' ');
  }
  return values[4];
}
