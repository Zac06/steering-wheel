#include <Wire.h>

#define I2C_SLAVE_ADDR 0x08
#define POT_PIN A0

volatile uint16_t potValue = 0;

void setup() {
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
}

void loop() {
  // Simple averaging to smooth out any residual noise
  uint32_t sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += analogRead(POT_PIN);
    delay(1);
  }
  potValue = sum / 8; // 0-1023

  Serial.println(potValue);
  delay(20);
}

void requestEvent() {
  byte data[2];
  data[0] = (potValue >> 8) & 0xFF; // high byte
  data[1] = potValue & 0xFF;        // low byte
  Wire.write(data, 2);
}