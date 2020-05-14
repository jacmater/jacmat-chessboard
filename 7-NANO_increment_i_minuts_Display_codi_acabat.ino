
//NOTA IMPORTANT!!! per poder fer anar el Display he hagut d'esborrar del fitxer:
// /home/marc/Arduino/libraries/LED_Backpack_Library/Adafruit_LEDBackpack.cpp
// totes los void que em donaven conflicte que són en definitiva les que afectaven
// al codi per la matriu de leds ja que es una llibreira compartida tant per una matriu
// LED com pel display. En concret son 3 les voids esborrades:

// void Adafruit_8x16matrix::drawPixel(int16_t x, int16_t y, uint16_t color)
// void Adafruit_8x8matrix::drawPixel(int16_t x, int16_t y, uint16_t color)
// void Adafruit_BicolorMatrix::drawPixel(int16_t x, int16_t y, uint16_t color)

/*Proba per escriure un numero en una posicio o varis numseros
 * en varies posicions
 */
 
#include <Wire.h> 
#include "Adafruit_LEDBackpack.h" //controla una matriu LED i un Disp 7 seg.
#include "Adafruit_GFX.h"

//Display de 7 segments
Adafruit_7segment matrix = Adafruit_7segment();


//faig servir les mateixes varibales que pel micro
int btnSegMin=3; //boto de segons minuts
int btnOk=4;     //botó OK
int ahores=0;
int aminuts=0;
int asegons=0;
int segTotB=0;
int segTotN=0;
int btnBlanques=5;   //PIN 5 del NANO
int senyalMicro=12;  //PIN D12 del NANO
int ledB=A2;
int ledN=A3;
int buzzer=A6;
bool auxSenyal=0;    //senyal que li diu al micro que ja pot començar
                     //a descommptar el temps

int increment=1;
int minuts=1;
int desenesMinuts=1;



void setup() {
   
  Serial.begin(9600);
   //poso display negres sense iluminar
 
   
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
   /////////////////////////////////////////delay(5500); //temps en que es mostra la caratula amb el MICRO
   
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
   
   //sel.leccio minuts
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
      //resto una unitat als minuts i a desenesMinuts perquè m¡ha fet
      //l'ultim increment!!
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

/**************   AQUÍ COMENÇA EL CODI DEL RELLOTGE   ********/
/*************************************************************/

void loop() 
  {
  
  //primer espero la senyal del MICRO que em diu si el taulell esta apunt per
  //començar la partida, és la senyal 
  
  
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
  
  if(digitalRead(btnBlanques)==LOW) //si esta clicat el pulsador de les negres descompta temps blanques
    {
    digitalWrite(ledB,HIGH);
    digitalWrite(ledN,LOW);
    segTotN=segTotN+increment; // 1er augmenta comptador negres amb l'increment
    escriuDisplay(segTotN,0x74);
    while(digitalRead(btnBlanques)==LOW)
      {
      segTotB--; //2on disminueix el temps de les blanques
      if(segTotB==0)
        {
        finalPartida(0x70); //els blancs han esgotat el temps
        }        
      else  //segueix descomptant temps blanques
        {
        escriuDisplay(segTotB,0x70);
        if(digitalRead(btnOk)==HIGH)  //fara una pausa al joc, es queda tot quiet
          {
          pausa();
          }
        delay(1000); //es el segon unitat de temps
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
        finalPartida(0x74); //els negres han esgotat el temps
        }        
      else  //segueix descomptant temps blanques
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


//aquesta acció la faré servir sempre per escriure als displays
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


void pausa()
  {
  int pausa=1;
    delay(1500);
    while(pausa==1) //quan torni a clicar ok surto del bucle i de la pausa
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
    //no fer res fins que no es cliqui el boto de reset 
      
    }     
  }
/*  matrix.writeDigitNum(0, 7, false); //write number 7 in position 0 &dot due to TRUE
  matrix.setBrightness(3);
  matrix.writeDigitNum(1, 8, false);
  matrix.drawColon(true);
  matrix.writeDigitNum(3, 8, false);
  matrix.writeDigitNum(4, 8, false);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.writeDisplay();
  delay(500);
 */

