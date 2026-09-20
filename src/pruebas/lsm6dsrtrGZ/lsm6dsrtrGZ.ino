#include <SPI.h>
#include <LSM6DSRSensor.h>

#define SPI_SCK  18
#define SPI_MISO 19
#define SPI_MOSI 23
#define SPI_CS   5

SPIClass dev_spi(VSPI);
LSM6DSRSensor AccGyr(&dev_spi, SPI_CS);

// Variables para el cálculo del ángulo absoluto en Z
float anguloZ = 0.0;
unsigned long tiempoAnterior = 0;
float sesgoZ = 0.0; // Calibración inicial

void setup() {
  Serial.begin(115200);
  while (!Serial);

  dev_spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);
  if (AccGyr.begin() != 0) {
    Serial.println("¡Error con LSM6DSR!");
    while (1);
  }

  AccGyr.Enable_G(); 
  AccGyr.Set_G_FS(4000);        // Escala máxima a 4000 dps
  AccGyr.Set_G_ODR(1666.0f);     // Muestreo ultra rápido a 1666 Hz

  // --- Proceso de Calibración ---
  Serial.println("Calibrando... Deja el sensor completamente quieto.");
  delay(2000);
  int muestras = 500;
  int32_t gyro_raw[3];
  long sumaZ = 0;
  
  for(int i = 0; i < muestras; i++) {
    AccGyr.Get_G_Axes(gyro_raw);
    sumaZ += gyro_raw[2];
    delay(2);
  }
  sesgoZ = (sumaZ / (float)muestras) / 1000.0; // Sesgo en dps
  Serial.print("Calibración terminada. Sesgo Z: ");
  Serial.println(sesgoZ, 4);
  
  tiempoAnterior = micros();
}

void loop() {
  int32_t gyro_raw[3];
  AccGyr.Get_G_Axes(gyro_raw);

  // Calcular tiempo transcurrido en segundos (dt)
  unsigned long tiempoActual = micros();
  float dt = (tiempoActual - tiempoAnterior) / 1000000.0;
  tiempoAnterior = tiempoActual;

  // 1. Velocidad angular pura en dps (restando el sesgo de calibración)
  float velocidadZ_dps = (gyro_raw[2] / 1000.0) - sesgoZ;

  // Filtro de umbral mínimo: ignora el ruido eléctrico si el giro es menor a 2°/s
  if (abs(velocidadZ_dps) < 2.0) {
    velocidadZ_dps = 0.0;
  }

  // 2. Integración matemática para obtener el ángulo acumulado
  anguloZ += velocidadZ_dps * dt;

  // Imprimir los dos resultados críticos para el eje Z
  Serial.print("Velocidad_Z: ");
  Serial.print(velocidadZ_dps, 2);
  Serial.print(" dps\t | \tAngulo_Z: ");
  Serial.print(anguloZ, 2);
  Serial.println(" grados");

  delay(5); // Ciclo rápido de ejecución (5ms)
}
