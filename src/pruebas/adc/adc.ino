float posicion=0.0;

int pines[]={36,39,34,35,32,33,25,26,27,14,12,13,15,2,0,4};


void setup(){

    Serial.begin(115200);

    analogReadResolution(8);
    

    for(int i=0;i<16;i++){
      pinMode(pines[i],INPUT);      
    }
   
}

void loop(){
    
    String dato="";

    float sensores[16];
    float suma = 0;
    float sumaPesos = 0;

    int val=0;

    for(int i=0;i<16;i++){
      
        val=255-analogRead(pines[i]);dato+=val; sensores[i]=(float)val;
        if(i!=15){ dato+=","; }
    }

    
    Serial.println(dato);
    
    int peso=-7.5;

    int num=0;
    for (int i = 0; i < 11; i++) {        
        suma += sensores[i];
        sumaPesos += sensores[i] * peso; 
        peso++;
        if(sensores[i]>20){ num++;}
        
    }

    float posicion2 = sumaPesos/suma;
    if(num!=0){  posicion=posicion2;  }
         
    
   // Serial.println(posicion);
    
    delay(50);
  
}
