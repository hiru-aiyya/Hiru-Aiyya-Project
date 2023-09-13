#define verbose_RS485 3

boolean initModBus(){
 mod.begin(4800);
 pinMode(RE, OUTPUT);
 pinMode(DE, OUTPUT);
 int test_count = 3;
 bool pass = 0;
 do{
  #if(verbose_RS485==1)
    Serial.println("star reading...");
  #endif
  if(readSensor()){
    pass = 1;
    #if(verbose_RS485==1)
       Serial.println("done 0");
    #endif
   }
  test_count--;
  delay(1000);
 }while((test_count>=0) && (pass==0));

 if(pass){
    #if(verbose_RS485==1)
       Serial.println("RS 485 working");
    #endif
   return true;
 }
 else{
    #if(verbose_RS485==1)
       Serial.println("RS 485 not working");
    #endif
   return false;
 }
}


boolean readSensor(){
  digitalWrite(DE,HIGH);
  digitalWrite(RE,HIGH);
  delay(100);
  if(mod.write(test,sizeof(test))==8){
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);

    //wait for data 
    int count =10;
    do{
      delay(1);
      count--;
      if(mod.available())
      {
        count=0;
      }
    }while(count>0);
    if(count==0){
      #if(verbose_RS485==2)
        Serial.println("no data received");
      #endif
      return false;

    }

    for(byte i=0;i<19;i++){
       readings[i] = mod.read();
    }
  }
  digitalWrite(DE,LOW);
  digitalWrite(RE,LOW);

  #if(verbose_RS485==2)
    Serial.println("received array - ");
    for(byte i=0;i<19;i++){
      Serial.print(readings[i],HEX);
    }
    Serial.println(".");
  #endif

  if((readings[0]==1)&&(readings[1]==3)&&(readings[2]==14)){
      return true;
    }
  else{
      return false;
    }
}

SoilData getAvgReadings_struct(){
  SoilData sData = {"","","","","","","",false};
  
  if(readSensor()){
        for(byte i=0; i<4; i++){
           delay(1000);
           if(readSensor()){
            avgReading[0]+=getTempe();
            avgReading[1]+=getMoisture();
            avgReading[2]+=getCon();
            avgReading[3]+=getPH();
            avgReading[4]+=getN();
            avgReading[5]+=getP();
            avgReading[6]+=getK(); 
           }
           else{
            #if(verbose_RS485==3)
              Serial.println("fail sensor while working");
            #endif
              return sData;
            }
         }

         sData.t=String((float(avgReading[0])/4),2);
         sData.m=String((float(avgReading[1])/4),1);
         sData.c=String((float(avgReading[2])/4),1);
         sData.ph=String((float(avgReading[3])/4),1);
         sData.n=String((float(avgReading[4])/4),1);
         sData.p=String((float(avgReading[5])/4),1);
         sData.k=String((float(avgReading[6])/4),1);
         //Serial.println(d);
         cleanAvgDataBuff();
         if(((avgReading[0])/4)<60){
            sData.w=true;
            return sData;
          }
         else{
           #if(verbose_RS485==3)
              Serial.println("data out of range");
            #endif
            return sData;
          }
    }

  else{
      #if(verbose_RS485==3)
        Serial.println("sensor failed at 1st reading");
      #endif
      return sData;
   }
}


String getAvgReadings(){
  
  if(readSensor()){
        for(byte i=0; i<4; i++){
           delay(1000);
           if(readSensor()){
           avgReading[0]+=getTempe();
           avgReading[1]+=getMoisture();
           avgReading[2]+=getCon();
           avgReading[3]+=getPH();
           avgReading[4]+=getN();
           avgReading[5]+=getP();
           avgReading[6]+=getK(); 
           }
           else{
             return "";
            }
         }

         String d =String((float(avgReading[0])/4),2)+","+String((float(avgReading[1])/4),1)+","+String((float(avgReading[2])/4),1)+","+String((float(avgReading[3])/4),1)+","+String((float(avgReading[4])/4),1)+","+String((float(avgReading[5])/4),1)+","+String((float(avgReading[6])/4),1);
         //Serial.println(d);
         cleanAvgDataBuff();
         if(((avgReading[0])/4)<60){
            return d;
          }
         else{
            return "";
          }
    }

  else{
      return "";
   }
}




void enableSoilSensor(){
   pinMode(HARDWARE_SW_1,OUTPUT);
   digitalWrite(HARDWARE_SW_1,HIGH);
   delay(100);
}

void disableSoilSensor(){
  digitalWrite(RE,HIGH);
  digitalWrite(DE,LOW);
 // pinMode(RE, INPUT);
 // pinMode(DE, INPUT);
  digitalWrite(HARDWARE_SW_1,LOW);
   //gpio_hold_en(GPIO_NUM_27);  
}





float getTempe(){
int result = (readings[5]*256) + readings[6];
float temp=float(result)/10;
//Serial.printf("Temp : %f \n",temp);
return temp;  
}

float getMoisture(){
int result = (readings[3]*256) + readings[4];
float m=float(result)/10;
return m;  
}


float getPH(){
int result = (readings[9]*256) + readings[10];
float ph=float(result)/10;
//Serial.printf("Temp : %f \n",temp);
return ph;  
}

float getCon(){
int result = (readings[7]*256) + readings[8];
return float(result); 
}

float getN(){
int result = (readings[11]*256) + readings[12];
float n=float(result)/10;  
return n;
}

float getP(){
int result = (readings[13]*256) + readings[14];
float p=float(result)/10;
return p; 
}

float getK(){
int result = (readings[15]*256) + readings[16];
float k=float(result)/10;
return k;
}


//Clean average sensor data array 
void cleanAvgDataBuff(){
   avgReading[0]=0.0;
   avgReading[1]=0.0;
   avgReading[2]=0.0;
   avgReading[3]=0.0;
   avgReading[4]=0.0;
   avgReading[5]=0.0;
   avgReading[6]=0.0;
}

void modbus_write(char a){
  digitalWrite(DE,HIGH);
    digitalWrite(RE,HIGH);
    delay(100);
    mod.write(a);
    digitalWrite(DE,LOW);
    digitalWrite(RE,LOW);
}

