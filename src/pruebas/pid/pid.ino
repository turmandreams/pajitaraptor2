#include <Arduino.h>

// ---------- Pines TB6612 (ajusta a tu montaje) ----------
#define PWMA  25
#define AIN1  26
#define AIN2  27
#define PWMB  14
#define BIN1  12
#define BIN2  13
#define STBY  33

// ---------- Potenciómetros (usa pines ADC1: 32-39) ----------
#define POT_KP 34
#define POT_KI 35
#define POT_KD 32

// ---------- PWM ----------
#define PWM_FREQ 20000   // 20 kHz: fuera del rango audible
#define PWM_RES  8       // 8 bits -> 0..255

// ---------- Rangos máximos de cada constante ----------
// Kp: con error 750 y Kp=0.5 -> corrección 375 (satura sobrada)
const float KP_MAX = 0.5;
const float KI_MAX = 0.005;
const float KD_MAX = 5.0;

// ---------- Parámetros del robot ----------
const int   VEL_BASE = 150;      // velocidad en recta (0..255)
const float INTEGRAL_MAX = 20000; // anti-windup

float Kp = 0, Ki = 0, Kd = 0;
float integral = 0;
float errorAnterior = 0;
unsigned long tAnterior = 0;
unsigned long tPots = 0;

// ---- Sustituye por la lectura real de tu sensor ----
float leerSensor() {
  // Devuelve -750.0 .. 750.0 (0 = línea centrada)
  return 0.0;
}

// ESP32 tiene ADC de 12 bits (0..4095); >>4 lo deja en 0..255
int leerPot(int pin) {
  return analogRead(pin) >> 4;
}

void actualizarConstantes() {
  Kp = leerPot(POT_KP) / 255.0 * KP_MAX;
  Ki = leerPot(POT_KI) / 255.0 * KI_MAX;
  Kd = leerPot(POT_KD) / 255.0 * KD_MAX;
}

// velocidad: -255..255 (negativo = marcha atrás)
void motor(int pwmPin, int in1, int in2, int velocidad) {
  velocidad = constrain(velocidad, -255, 255);
  if (velocidad >= 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  ledcWrite(pwmPin, abs(velocidad));
}

void setup() {
  Serial.begin(115200);

  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  // API del core ESP32 v3.x
  ledcAttach(PWMA, PWM_FREQ, PWM_RES);
  ledcAttach(PWMB, PWM_FREQ, PWM_RES);

  actualizarConstantes();
  tAnterior = micros();
}

void loop() {
  // Leer potenciómetros solo cada 100 ms (no hace falta más)
  if (millis() - tPots > 100) {
    tPots = millis();
    actualizarConstantes();
  }

  unsigned long ahora = micros();
  float dt = (ahora - tAnterior) / 1000.0;   // en milisegundos
  tAnterior = ahora;
  if (dt <= 0) dt = 1;

  float error = leerSensor();

  // --- PID ---
  integral += error * dt;
  integral = constrain(integral, -INTEGRAL_MAX, INTEGRAL_MAX);

  float derivada = (error - errorAnterior) / dt;
  errorAnterior = error;

  float correccion = Kp * error + Ki * integral + Kd * derivada;

  // --- Aplicar a motores ---
  int velIzq = VEL_BASE + correccion;
  int velDer = VEL_BASE - correccion;

  motor(PWMA, AIN1, AIN2, velIzq);
  motor(PWMB, BIN1, BIN2, velDer);

  // Depuración (coméntalo en carrera: Serial ralentiza el bucle)
  static unsigned long tDebug = 0;
  if (millis() - tDebug > 200) {
    tDebug = millis();
    Serial.printf("Kp=%.3f Ki=%.4f Kd=%.2f  err=%.1f  corr=%.1f\n",
                  Kp, Ki, Kd, error, correccion);
  }
}
