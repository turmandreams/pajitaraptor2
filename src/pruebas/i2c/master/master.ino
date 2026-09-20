#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define SLAVE_ADDR 0x08
#define I2C_SPEED 500000  // 1 MHz (Fast Mode+)

void setup() {
  Serial.begin(115200);
  
  // Inicializa I2C con pines específicos y frecuencia máxima
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(I2C_SPEED);
}

void loop() {
  uint8_t payload[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(payload, sizeof(payload));
  uint8_t error = Wire.endTransmission();

  if (error == 0) {
    // Para comprobar la transmisión correcta
  }

  delay(1); // Pausa corta entre envíos
  
}
