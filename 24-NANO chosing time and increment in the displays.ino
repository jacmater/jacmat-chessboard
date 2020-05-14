
//I had to remove 3 voids in the ...Arduino/libraries/LED_Backpack_Library/Adafruit_LEDBackpack.cpp
//in order to run the displays:

// void Adafruit_8x16matrix::drawPixel(int16_t x, int16_t y, uint16_t color)
// void Adafruit_8x8matrix::drawPixel(int16_t x, int16_t y, uint16_t color)
// void Adafruit_BicolorMatrix::drawPixel(int16_t x, int16_t y, uint16_t color)

#include <Wire.h> 
#include "Adafruit_LEDBackpack.h" 
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

int btnSegMin=3; 
int btnOk=4;     
int ahores=0;
int aminuts=0;
int asegons=0;
int segTotB=0;
int segTotN=0;
int btnBlanques=5;   
int senyalMicro=12;  
int ledB=A2;
int ledN=A3;
int buzzer=A6;
bool auxSenyal=0;   
int increment=1;
int minuts=1;
int desenesMinuts=1;


void setup() {
   
  Serial.begin(9600);
   
   matrix.begin(0x74);  //7segment I2C adress for Black
   matrix.writeDigitNum(0, -1, false);
   matrix.writeDigitNum(1, -1, false);
   matrix.drawColon(false);
   matrix.writeDigitNum(3, -1, false);
   matrix.writeDigitNum(4, -1, false);
   matrix.writeDisplay();
   delay(250);
   
   matrix.begin(0x70);  //7segment I2C adress for white
   matrix.setBrightness(3);
   pinMode(btnSegMin,INPUT);
   pinMode(btnOk,INPUT);
   pinMode(senyalMicro,INPUT);
   pinMode(ledB,OUTPUT);
   pinMode(ledN,OUTPUT);
   pinMode(buzzer,OUTPUT);
   delay(250);
   /////////////////////////////////////////delay(5500); //Micro header time
   
   matrix.writeDigitNum(0, -1, false);
   matrix.writeDigitNum(1, -1, false);
   matrix.drawColon(false);
   matrix.writeDigitNum(3, -1, false);
   matrix.writeDigitNum(4, 0, false);
   matrix.writeDisplay();
   delay(250);
   
   //****** INCREMENT *******
   while(digitalRead(btnOk)==LOW)
      {
      if(digitalRead(btnSegMin)==HIGH)
        {
        matrix.writeDigitNum(4,increment, false);
        matrix.writeDisplay();
        if(increment>9)
          {
          increment=0;
          matrix.writeDigitNum(3,desenesMinuts, false);
          matrix.writeDigitNum(4,0, false);
          matrix.writeDisplay();
          desenesMinuts++;
         
          if(desenesMinuts>=6)
          {
           increment=0;
           desenesMinuts=0;
          }
          }
        increment++;
        delay(250);
        } 
      }
    
   
    if(desenesMinuts>1){increment=(desenesMinuts-1)*10+(increment-1);}
    else{ increment=increment-1;}
    Serial.print("1er increment: ");
    Serial.println(increment);
    Serial.print("1er desenesMinuts: ");
    Serial.println(desenesMinuts);
     
     //és l'increment final que necessito pel cas que hi hagi desenes de segons
    desenesMinuts=1;   //reinicialitzo la variable per a les desenes
    
    //poso tot a zero
    matrix.writeDigitNum(0, 0, false); //write number 7 in position 0 &dot due to TRUE
    matrix.writeDigitNum(1, 0, false);
    matrix.drawColon(true);
    matrix.writeDigitNum(3, 0, false);
    matrix.writeDigitNum(4, 0, false);
    matrix.writeDisplay();
    delay(250);
   
   //minutes select
   while(digitalRead(btnOk)==LOW)
      {
      if(digitalRead(btnSegMin)==HIGH)
        {
        matrix.writeDigitNum(1, minuts, false);
        matrix.writeDisplay();
        if(minuts>9)
          {
          minuts=0;
          matrix.writeDigitNum(0,desenesMinuts, false);
          matrix.writeDigitNum(1,0, false);
          matrix.writeDisplay();
          desenesMinuts++;
         
          if(desenesMinuts>=6)
          {
           minuts=0;
           desenesMinuts=0;
          }
          }
        minuts++;
        delay(250);
        } 
      }
      
      minuts--;
      desenesMinuts--;
      segTotB=(minuts+(desenesMinuts)*10)*60;
///      segTotB=10;
      segTotN=(minuts+(desenesMinuts)*10)*60;
///      segTotN=10;
      
      Serial.print("minuts:");
      Serial.println(minuts);
      Serial.print("desenesMinuts:");
      Serial.println(desenesMinuts);
      Serial.print("segons totals: ");
      Serial.println(segTotB);
      
      escriuDisplay(segTotN,0x74);
      delay(250);
}

/*************************   LOOP   **************************/
/*************************************************************/

void loop() 
  {
  while(!auxSenyal)
    {
    if(digitalRead(senyalMicro)==LOW) {auxSenyal=0;}
    else
      {
      auxSenyal=1;
      descompteTemps();  
      }
    }
  }


void descompteTemps()
 {
  iniciDte:
  
  if(segTotN==600 ||segTotB==600)
    {tone(buzzer,500);
     delay(250);
     noTone(buzzer);}
  
  if(digitalRead(btnBlanques)==LOW) 
    {
    digitalWrite(ledB,HIGH);
    digitalWrite(ledN,LOW);
    segTotN=segTotN+increment; 
    escriuDisplay(segTotN,0x74);
    while(digitalRead(btnBlanques)==LOW)
      {
      segTotB--; 
      if(segTotB==0)
        {
        finalPartida(0x70); //end of game
        }        
      else  
        {
        escriuDisplay(segTotB,0x70);
        if(digitalRead(btnOk)==HIGH)  //Pause
          {
          pausa();
          }
        delay(1000); //this is the unit se second
        }
      }
    }
   else
    {
    digitalWrite(ledN,HIGH);
    digitalWrite(ledB,LOW);
    segTotB=segTotB+increment;
    escriuDisplay(segTotB,0x70);
    while(digitalRead(btnBlanques)==HIGH)
      {
      segTotN--;
      if(segTotN==0)
        {
        finalPartida(0x74); 
        }        
      else  
        {
        escriuDisplay(segTotN,0x74);
        if(digitalRead(btnOk)==HIGH)  //fara una pausa al joc, es queda tot quiet
          {
          pausa();
          }
        delay(1000); //es el segon unitat de temps
        }
      }
    }
 goto iniciDte;
 }  //void


//display writing
void escriuDisplay(int segons,byte adress)
  {
  matrix.begin(adress);  //la del display que toqui
  matrix.setBrightness(3);
  matrix.writeDigitNum(0, segons/60/10, false);                  //desenes dels minuts
  matrix.writeDigitNum(1, segons/60-(segons/60/10)*10, false);  //unitats dels minuts
  matrix.drawColon(true);
  matrix.writeDigitNum(3, segons%60/10, false);                  //desenes dels segons
  matrix.writeDigitNum(4, segons%60-(segons%60/10)*10, false);  //unitats dels segons
  matrix.writeDisplay();
  }

//void pause
void pausa()
  {
  int pausa=1;
    delay(1500);
    while(pausa==1) 
      {
      if(digitalRead(btnOk)==HIGH)
        {
        pausa=0;
        }
      }
  }

void finalPartida(byte adress)
  {
  //fes anar el Buzzerrrrrrrr
  matrix.begin(adress);
  matrix.setBrightness(3);
  matrix.writeDigitNum(0, -1, false);                  
  matrix.writeDigitNum(1, 15, false); // F
  matrix.drawColon(false);
  matrix.writeDigitNum(3, 1, false);  // I           
  matrix.writeDigitNum(4, -1, false); 
  matrix.writeDisplay(); 
  while(1)
    {
    //do nothing 
      
    }     
  }

