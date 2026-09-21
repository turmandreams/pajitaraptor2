// Variables globales para almacenar los promedios finales
int s = 0;
int v = 0;
int p = 0;
int i_pid = 0; // Cambiado el nombre para evitar conflicto con el bucle for
int d = 0;

void leepotenciometros(){
    // Variables locales de tipo long para acumular de forma segura las 100 muestras
    unsigned long sumS = 0;
    unsigned long sumV = 0;
    unsigned long sumP = 0;
    unsigned long sumI = 0;
    unsigned long sumD = 0;

    // Realizar las 100 lecturas
    for(int k = 0; k < 100; k++){
        sumS += (255-analogRead(35));
        sumV += (255-analogRead(32));
        sumP += (255-analogRead(33));
        sumI += (255-analogRead(26));
        sumD += (255-analogRead(27));
    }

    // Calcular el promedio y guardarlo en las variables globales
    s = sumS / 100;
    v = sumV / 100;
    p = sumP / 100;
    i_pid = sumI / 100;
    d = sumD / 100;

    // Mostrar resultados en el Monitor Serie
    Serial.print("Succion : "); Serial.println(s);
    Serial.print("Velocidad : "); Serial.println(v);
    Serial.print("P : "); Serial.println(p);
    Serial.print("I : "); Serial.println(i_pid);
    Serial.print("D : "); Serial.println(d);
    Serial.println("");
}
    
void setup(){
    Serial.begin(115200);

    // Configura la resolución a 8 bits (valores de 0 a 255)
    // Nota: Asegúrate de que tu placa (ej. ESP32) soporte esta función.
    analogReadResolution(8); 
}

void loop(){
    leepotenciometros(); 
    delay(2000); // Espera 2 segundos entre lecturas
}
