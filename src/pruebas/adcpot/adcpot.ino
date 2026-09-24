
int succion = 0;
int velocidad = 0;
int p_pid = 0;
int i_pid = 0; 
int d_pid = 0;


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
    velocidad = sumV / 100;
    p_pid = sumP / 100;
    i_pid = sumI / 100;
    d_pid = sumD / 100;

    // Mostrar resultados en el Monitor Serie
    Serial.print("Succion : "); Serial.println(succion);
    Serial.print("Velocidad : "); Serial.println(velocidad);
    Serial.print("P : "); Serial.println(p_pid);
    Serial.print("I : "); Serial.println(i_pid);
    Serial.print("D : "); Serial.println(d_pid);
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
