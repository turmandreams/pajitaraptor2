#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 25
#define SLAVE_ADDR 0x08

volatile float receivedFloat = 0.0f;
volatile bool newData = false;

int contador=-1;

int tiempo=0;

void receiveEvent(int bytesCount) {
  
  // Comprobamos que hayan llegado exactamente 4 bytes (tamaño de un float)
  if (bytesCount == sizeof(float)) {
    uint8_t buffer[sizeof(float)];
    
    for (int i = 0; i < sizeof(float); i++) {
      buffer[i] = Wire.read();
    }
    
    // Copiamos los bytes directamente a la variable float en memoria
    memcpy((void*)&receivedFloat, buffer, sizeof(float));
    newData = true;

    contador++;
    
  } else {
    // Si llegan bytes extras, vaciamos el bus
    while (Wire.available()) Wire.read();
  }

  
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SLAVE_ADDR, SDA_PIN, SCL_PIN, 1000000);
  Wire.onReceive(receiveEvent);
}

void loop() {
  
  if (newData) {
    //Serial.println(receivedFloat, 2);
    if(contador==0){  tiempo=millis(); }
    else if(contador==100){
        contador=-1;
        int valor=millis()-tiempo;
        Serial.print("Ha tardado : ");
        Serial.println(valor);
        Serial.print("Posicion : ");
        Serial.println(receivedFloat, 2);
    }
      
    newData = false;
   // Serial.print("Float recibido: ");
    
  }
  
}
