#include "flexsensor.h"

   //pink
  int Flex_pin1 = 1;
  float flexADC1 = 0; 
  int sensorMin1 = 0; 
  int sensorMax1 = 1023;
  
  //ring
  int Flex_pin2 = 2;
  float flexADC2 = 0; 
  int sensorMin2 = 0; 
  int sensorMax2 = 1023;
  
  //middle
  int Flex_pin3 = 3;
  float flexADC3 = 0; 
  int sensorMin3 = 0; 
  int sensorMax3 = 1023;
  
  //pointer
  int Flex_pin4 = 4;
  float flexADC4 = 0; 
  int sensorMin4 = 0; 
  int sensorMax4 = 1023;
  
  //thumb
  int Flex_pin5 = 5;
  float flexADC5 = 0; 
  int sensorMin5 = 0; 
  int sensorMax5 = 1023;

  char priv_letter = '0';

float readconstrainmapflex(int Flex_pin, float flexADC, int sensorMin, int sensorMax)
{
  flexADC = analogRead(Flex_pin);
  
  flexADC = constrain(flexADC,sensorMin, sensorMax);

  float Flexgrade = map(flexADC, sensorMin, sensorMax, 0, 90);
  return Flexgrade;
}
float readconstrainmapflex1(int Flex_pin, float flexADC, int sensorMin, long sensorMax)
{
  flexADC = analogRead(Flex_pin);
  
  flexADC = constrain(flexADC, sensorMin, sensorMax);

  float Flexgrade = map(flexADC, sensorMin, sensorMax, 0, 90);
  return Flexgrade;
}
void printflex(float Flexgrade1, float Flexgrade2, float Flexgrade3, float Flexgrade4, float Flexgrade5)
{
  Serial.printf("pink%f%", Flexgrade1);
  Serial.printf("ring%f%", Flexgrade2);
  Serial.printf("middle%f%", Flexgrade3);
  Serial.printf("point%f%", Flexgrade4);
  Serial.printf("thumb%f%", Flexgrade5);
  Serial.println("\n");
}


// char findletter(float Flexgrade1, float Flexgrade2, float Flexgrade3, float Flexgrade4, float Flexgrade5)
