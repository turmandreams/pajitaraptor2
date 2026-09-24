#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 26
#define SLAVE_ADDR 0x08

const int pinled=2;

const int pinAIN1 = 4;
const int pinAIN2 = 16;
const int pinPWMA = 17;


const int pinBIN1 = 15;
const int pinBIN2 = 12; 
const int pinPWMB = 27;  


const int pinsuccion = 13;  

const int frecuencia = 20000; 
const int resolucion = 8;    

// Asignación de canales de hardware obligatorios en v2.x
const int canalMotorA = 0;
const int canalMotorB = 1;


// ---------- Rangos máximos de cada constante ----------
// Kp: con error 750 y Kp=0.5 -> corrección 375 (satura sobrada)
const float KP_MAX = 0.5;
const float KI_MAX = 0.005;
const float KD_MAX = 5.0;

// ---------- Parámetros del robot ----------
int   VEL_BASE = 150;      // velocidad en recta (0..255)
const float INTEGRAL_MAX = 20000; // anti-windup

float Kp = 0, Ki = 0, Kd = 0;
float integral = 0;
float errorAnterior = 0;
unsigned long tAnterior = 0;
unsigned long tPots = 0;

const int canalSuccion = 2;

const int pinBateria = 36; 

// Factor para R1=220k y R2=100k -> (220+100)/100 = 3.2
const float factorDivisor = 3.2; 

// Voltaje de referencia real del ESP32 (típicamente 3.3V)
const float VREF = 3.3; 


volatile float receivedFloat = 0.0f;
volatile bool newData = false;

int contador=-1;

int tiempo=0;

int succion = 0;
int p_pid = 0;
int i_pid = 0; 
int d_pid = 0;

int tiemposensores=0;

boolean corre=false;




void medirbateria(){

 int valorADC = analogRead(pinBateria);
  
  // Convertir el valor de 8 bits (0-255) a voltaje en el pin (0 - 3.3V)
  float voltiosPin = (valorADC * VREF) / 255.0;
  
  // Calcular el voltaje real de la batería usando el divisor de tensión
  float voltajeBateria = voltiosPin * factorDivisor;
  
  // Calcular el porcentaje de carga (Batería 2S: 6.4V vacía a 8.4V llena)
  float porcentaje = ((voltajeBateria - 6.4) / (8.4 - 6.4)) * 100.0;
  
  // Limitar el porcentaje entre 0 y 100%
  if (porcentaje > 100) porcentaje = 100;
  if (porcentaje < 0) porcentaje = 0;

  // Imprimir los resultados
  Serial.print("Lectura ADC (8-bit): ");
  Serial.print(valorADC);
  Serial.print(" | Voltaje Batería: ");
  Serial.print(voltajeBateria, 2);
  Serial.print(" V | Carga: ");
  Serial.print(porcentaje, 0);
  Serial.println("%");

  
}

void leepotenciometros(){
  
    unsigned long sumS = 0;
    unsigned long sumV = 0;
    unsigned long sumP = 0;
    unsigned long sumI = 0;
    unsigned long sumD = 0;

    for(int k = 0; k < 100; k++){
        sumS += (255-analogRead(34));
        sumV += (255-analogRead(35));
        sumP += (255-analogRead(32));
        sumI += (255-analogRead(33));
        sumD += (255-analogRead(39));
    }

    succion = sumS / 100;
    VEL_BASE = sumV / 100;
    p_pid = sumP / 100;
    i_pid = sumI / 100;
    d_pid = sumD / 100;


    Kp = p_pid / 255.0 * KP_MAX;
    Ki = i_pid / 255.0 * KI_MAX;
    Kd = d_pid / 255.0 * KD_MAX;

    
    // Mostrar resultados en el Monitor Serie
    Serial.print("Succion : "); Serial.println(succion);
    Serial.print("Velocidad Base: "); Serial.println(VEL_BASE);
    Serial.print("P : "); Serial.println(p_pid);
    Serial.print("I : "); Serial.println(i_pid);
    Serial.print("D : "); Serial.println(d_pid);
    Serial.println("");
    
    
    //ponemos el pwm en el ventilador de succion
    if(succion<20){ succion=0;}
    succion=succion/5;
    
    ledcWrite(canalSuccion,succion);
    
}

// velocidad: -255..255 (negativo = marcha atrás)
void motor(int canal, int in1, int in2, int velocidad) {
  velocidad = constrain(velocidad, -255, 255);
  if (velocidad >= 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  ledcWrite(canal,abs(velocidad));
}

void pidmotores(float error){

  unsigned long ahora = micros();
  float dt = (ahora - tAnterior) / 1000.0;   // en milisegundos
  tAnterior = ahora;
  if (dt <= 0) dt = 1;


  // --- PID ---
  integral += error * dt;
  integral = constrain(integral, -INTEGRAL_MAX, INTEGRAL_MAX);

  float derivada = (error - errorAnterior) / dt;
  errorAnterior = error;

  float correccion = Kp * error + Ki * integral + Kd * derivada;

  // --- Aplicar a motores ---
  int velIzq = VEL_BASE + correccion;
  int velDer = VEL_BASE - correccion;

  motor(canalMotorA,pinAIN2,pinAIN1, velIzq);
  motor(canalMotorB,pinBIN1,pinBIN2, velDer);
    
}

void receiveEvent(int bytesCount) {
  
  // Comprobamos que hayan llegado exactamente 4 bytes (tamaño de un float)
  if (bytesCount == sizeof(float)) {
  
    digitalWrite(pinled,HIGH);
    corre=true;

  
    uint8_t buffer[sizeof(float)];
    
    for (int i = 0; i < sizeof(float); i++) {
      buffer[i] = Wire.read();
    }
    
    // Copiamos los bytes directamente a la variable float en memoria
    memcpy((void*)&receivedFloat, buffer, sizeof(float));

    pidmotores(receivedFloat);

    tiemposensores=millis();

/*
    contador++;

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
*/
    
  } else {
    // Si llegan bytes extras, vaciamos el bus
    while (Wire.available()) Wire.read();
  }

  
}

void espera(int t){
  boolean estado=corre;
  for(int i=0;i<t;i++){
        if(corre){
            if((millis()-tiemposensores)>10){ 
              corre=false;
              digitalWrite(pinled,LOW);              
            }
        }
        
        if(estado!=corre){ break; }
        delay(1);
  }
  
}

void setup() {
  
  Serial.begin(115200);
 
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinPWMB, OUTPUT);

  
  ledcSetup(canalMotorA, frecuencia, resolucion); 
  ledcSetup(canalMotorB, frecuencia, resolucion); 
  ledcSetup(canalSuccion, frecuencia, resolucion); 
  
  
  ledcAttachPin(pinPWMA, canalMotorA); 
  ledcAttachPin(pinPWMB, canalMotorB); 

  ledcAttachPin(pinsuccion,canalSuccion); 
  
  pinMode(pinled,OUTPUT);
  digitalWrite(pinled,LOW);
    
  analogReadResolution(8); 

  delay(2000);
  leepotenciometros();
  delay(2000);
  
  Wire.begin(SLAVE_ADDR, SDA_PIN, SCL_PIN, 1000000);
  Wire.onReceive(receiveEvent);
  
}

void loop() {

  /*
  if(corre){

    ledcWrite(canalMotorA, velocidad); 
    ledcWrite(canalMotorB, velocidad); 

    //Hacia atras
    digitalWrite(pinAIN2, LOW);
    digitalWrite(pinAIN1, HIGH);
    digitalWrite(pinBIN1, LOW);
    digitalWrite(pinBIN2, HIGH);
    espera(2000); 

  }
*/
  espera(2);
  
  if(!corre){

    leepotenciometros();
    medirbateria();
    
    motor(canalMotorA,pinAIN2,pinAIN1,0);
    motor(canalMotorB,pinBIN1,pinBIN2,0);
    espera(100); 
    
  }
  
  
}
