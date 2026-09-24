#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


const uint16_t RECV_PIN = 5; 

IRrecv irrecv(RECV_PIN);
decode_results results;
const int pinled=19;

#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define SLAVE_ADDR 0x08
#define I2C_SPEED 1000000  // 1 MHz (Fast Mode+)

int pines[]={36,39,34,35,32,33,25,26,27,14,12,13,15,2,0,4};

float posicion=0.0;

boolean funcionando=false;

int tiempo=0;

int tiempoparpadeo=0;
boolean ledparpadeo=false;


void leesensores(){
    
    float sensores[16];
    float suma = 0;
    float sumaPesos = 0;

    int val=0;

   // String dato="";
    
    for(int i=0;i<16;i++){      
        val=255-analogRead(pines[i]);
      //  dato+=val;
        sensores[i]=(float)val;
     //   if(i!=15){ dato+=","; }
    }
    
  //  Serial.println(dato);

   
    int peso=-8.0;

    int num=0;
    for (int i = 0; i < 16; i++) {        
                
        if(sensores[i]>30){ 
          suma += sensores[i];
          sumaPesos += sensores[i] * peso;  
          num++;
        }
        peso++;        
    }

        
    float posicion2 = sumaPesos/suma;
    if(num!=0){  
        posicion=(posicion2*100); 
        posicion+=50.0;
    }      
    
}


void setup() {
  

  Serial.begin(115200);
  
  
  
  irrecv.enableIRIn(); 
  
  // Inicializa I2C con pines específicos y frecuencia máxima
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(I2C_SPEED);

  analogReadResolution(8);
    
  for(int i=0;i<16;i++){  pinMode(pines[i],INPUT);  }

  pinMode(pinled,OUTPUT);


    
}

void loop() {

  if (irrecv.decode(&results)) {
    irrecv.resume(); 
  
    if((millis()-tiempo)>300){    
        funcionando=!funcionando;
        if(funcionando){ digitalWrite(pinled,HIGH);}
        else{ digitalWrite(pinled,LOW);}
    }

    tiempo=millis();
    
  }

  if(funcionando){

      leesensores();
  
      Wire.beginTransmission(SLAVE_ADDR);
  
      Wire.write((uint8_t*)&posicion, sizeof(float));
  
      Wire.endTransmission();

      //delay(1);
      
  }else{

      if((millis()-tiempoparpadeo)>1000) {

          ledparpadeo=!ledparpadeo;
          digitalWrite(pinled,ledparpadeo);
          tiempoparpadeo=millis();
        
      }

    
  }
  
}
