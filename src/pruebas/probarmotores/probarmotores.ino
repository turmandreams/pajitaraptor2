// Pines de control del Motor A
const int pinAIN1 = 4;
const int pinAIN2 = 16;
const int pinPWMA = 17;

// Pines de control del Motor B (Pines seguros)
const int pinBIN1 = 15;
const int pinBIN2 = 12; 
const int pinPWMB = 27;  

// Configuración PWM clásica para el ESP32
const int frecuencia = 20000; 
const int resolucion = 8;    

// Asignación de canales de hardware obligatorios en v2.x
const int canalMotorA = 0;
const int canalMotorB = 1;

void setup() {
  // Configurar pines de dirección como salidas
  pinMode(pinAIN1, OUTPUT);
  pinMode(pinAIN2, OUTPUT);
  pinMode(pinBIN1, OUTPUT);
  pinMode(pinBIN2, OUTPUT);
  pinMode(pinPWMA, OUTPUT);
  pinMode(pinPWMB, OUTPUT);

  // SINTAXIS CLÁSICA (ESP32 core v2.x)
  // 1. Configurar la frecuencia y resolución en cada canal interno
  ledcSetup(canalMotorA, frecuencia, resolucion); 
  ledcSetup(canalMotorB, frecuencia, resolucion); 
  
  // 2. Asociar el pin físico al canal configurado
  ledcAttachPin(pinPWMA, canalMotorA); 
  ledcAttachPin(pinPWMB, canalMotorB); 
}

void loop() {
  // IMPORTANTE: En la versión clásica se le envía el valor al CANAL, no al pin.
  ledcWrite(canalMotorA, 50); 
  ledcWrite(canalMotorB, 50); 
  
  // Sentido de giro 1 durante 2 segundos
  digitalWrite(pinAIN2, LOW);
  digitalWrite(pinAIN1, HIGH);
  digitalWrite(pinBIN1, LOW);
  digitalWrite(pinBIN2, HIGH);
  delay(2000); 
  
  // Sentido de giro inverso durante 2 segundos
  digitalWrite(pinAIN1, LOW);
  digitalWrite(pinAIN2, HIGH);
  digitalWrite(pinBIN2, LOW);
  digitalWrite(pinBIN1, HIGH);
  delay(2000); 

  // Parada/Pausa durante 2 segundos
  digitalWrite(pinAIN1, LOW);
  digitalWrite(pinAIN2, LOW);
  digitalWrite(pinBIN2, LOW);
  digitalWrite(pinBIN1, LOW);
  delay(2000); 
}
