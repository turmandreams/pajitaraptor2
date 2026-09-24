#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// Definimos el pin GPIO 5
const uint16_t RECV_PIN = 5; 

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn(); // Arranca el receptor
  Serial.println("Receptor IR inicializado en GPIO 5");
}

void loop() {
  if (irrecv.decode(&results)) {
    // Imprime el código recibido en formato hexadecimal
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume(); // Recibe el siguiente valor
  }
  delay(100);
}
