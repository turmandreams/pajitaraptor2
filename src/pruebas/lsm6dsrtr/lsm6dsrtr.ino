#include <SPI.h>
#include <LSM6DSRSensor.h>

// Definir los pines SPI para el ESP32 (Pines VSPI por defecto)
#define SPI_SCK  18
#define SPI_MISO 19
#define SPI_MOSI 23
#define SPI_CS   5

// Crear la interfaz SPI utilizando la clase nativa de Arduino para ESP32
SPIClass dev_spi(VSPI);

// Inicializar el sensor LSM6DSR pasándole la interfaz SPI y el pin CS
LSM6DSRSensor AccGyr(&dev_spi, SPI_CS);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Esperar a que se abra el monitor serie
  }

  Serial.println("Inicializando LSM6DSR por SPI...");

  // Inicializar el bus SPI con los pines correctos del ESP32
  dev_spi.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);

  // Arrancar el sensor mediante SPI
  if (AccGyr.begin() != 0) {
    Serial.println("¡Error al inicializar el LSM6DSR por SPI! Verifica el cableado.");
    while (1);
  }

  // Activar los componentes de hardware internos
  AccGyr.Enable_X(); // Activa el acelerómetro
  AccGyr.Enable_G(); // Activa el giroscopio

  Serial.println("LSM6DSR inicializado con éxito por SPI.");
}

void loop() {
  // Arreglos de 3 elementos para almacenar los datos de los ejes (X, Y, Z)
  int32_t accelerometer[3];
  int32_t gyroscope[3];

  // Leer valores del sensor mediante el bus SPI
  AccGyr.Get_X_Axes(accelerometer);
  AccGyr.Get_G_Axes(gyroscope);

  // Mostrar los datos del Acelerómetro correctamente indexados
 
  Serial.print(accelerometer[0]);
  Serial.print(",");
  Serial.print(accelerometer[1]);
  Serial.print(",");
  Serial.print(accelerometer[2]);

  Serial.print(",");
  Serial.print(gyroscope[0]);
  Serial.print(",");
  Serial.print(gyroscope[1]);
  Serial.print(",");
  Serial.println(gyroscope[2]);

  delay(10); // Muestreo cada 200 milisegundos
}
