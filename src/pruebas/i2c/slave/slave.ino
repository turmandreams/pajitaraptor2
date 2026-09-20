#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define SLAVE_ADDR 0x08

// Búfer para procesar datos fuera de la interrupción
volatile bool dataReady = false;
volatile uint8_t rxBuffer[32];
volatile size_t rxLen = 0;

void receiveEvent(int bytesCount) {
  rxLen = 0;
  while (Wire.available() && rxLen < sizeof(rxBuffer)) {
    rxBuffer[rxLen++] = Wire.read();
  }
  dataReady = true;
}

void setup() {
  Serial.begin(115200);

  // Inicializa en modo Esclavo
  Wire.begin(SLAVE_ADDR, SDA_PIN, SCL_PIN,500000);
  Wire.onReceive(receiveEvent);
}

void loop() {
  if (dataReady) {
    dataReady = false;
    for(int i=0;i<rxLen;i++){
        Serial.print(rxBuffer[i]);
        Serial.print(",");
    }
    Serial.println("");
    // Procesa los datos recibidos fuera de la función de interrupción
  }
}
