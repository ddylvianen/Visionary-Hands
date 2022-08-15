#include <Arduino.h>
#line 1 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\The_ALS_Glove_starter_code.ino"
#include <esp_now.h>
#include <WiFi.h>

#include "flexsensor.h"
#include "flexx.h"
#include "mpu.h"
#include "MPU9250.h"
#include "wire.h"
#include "sign.h"

//an flex sensor object with the flex sensor pins for every finger
flex fl;

// Create a struct_message to hold incoming sensor readings
struct_message incomingtReadings;
struct_message letters;

int status;
char Read_letter = '0';
unsigned long privtime = 0, times = 15000;

#line 22 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\The_ALS_Glove_starter_code.ino"
void setup();
#line 142 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\The_ALS_Glove_starter_code.ino"
void loop();
#line 262 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\The_ALS_Glove_starter_code.ino"
void printletter(char read_letter);
#line 271 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\The_ALS_Glove_starter_code.ino"
void calibrateflex();
#line 22 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\The_ALS_Glove_starter_code.ino"
void setup() 
{
  Serial.begin(115200); //how much space you want to use
  fl.pinmode(Flex_pin1, Flex_pin2, Flex_pin3, Flex_pin4, Flex_pin5);

  //imu cor
  Wire.begin(11, 18); // left glove
 // Wire.begin(18, 20); // right glove
  while(!Serial) {}
//  start communication with IMU 
  status = IMU.begin();
  if (status < 0) {
  Serial.println("IMU initialization unsuccessful");
    
  Serial.print("Status: ");
  Serial.println(status);
  while(1) {}
  }
  
  set_range();

  // callibrating the sensors for adaptivity with different bends
  //calibrateflex();
//##################################################################################################################################################################################################################################################################################
  sensorMax1 = analogRead(Flex_pin1);
  sensorMax2 = analogRead(Flex_pin2);
  sensorMax3 = analogRead(Flex_pin3);
  sensorMax4 = analogRead(Flex_pin4);
  sensorMax5 = analogRead(Flex_pin5);
  privtime = millis();
  
  while(millis() < privtime + times)
  {
    if(digitalRead(7)==HIGH)
      {
        float flexADC1 = analogRead(Flex_pin1);
        float flexADC2 = analogRead(Flex_pin2);
        float flexADC3 = analogRead(Flex_pin3);
        float flexADC4 = analogRead(Flex_pin4);
        float flexADC5 = analogRead(Flex_pin5);
      
        if(flexADC1<sensorMin1)
        {
          sensorMin1=flexADC1;
        }
        if(flexADC1>sensorMax1)
        {
          sensorMax1=flexADC1;
        }
        if(flexADC2<sensorMin2)
        {
          sensorMin2=flexADC2;
        }
        if(flexADC2>sensorMax2)
        {
          sensorMax2=flexADC2;
        }
      
        if(flexADC3<sensorMin3)
        {
          sensorMin3=flexADC3;
        }
        if(flexADC3>sensorMax3)
        {
          sensorMax4=flexADC4;
        }
        if(flexADC4<sensorMin4)
        {
          sensorMin4=flexADC4;
        }
        if(flexADC4>sensorMax4)
        {
          sensorMax4=flexADC4;
        }
        if(flexADC5<sensorMin5)
        {
          sensorMin5=flexADC5;
        }
        if(flexADC5>sensorMax5)
        {
          sensorMax5=flexADC5;
        }
      
      }
  }

//##################################################################################################################################################################################################################################################################################



  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback functionthat will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  //####################################################################################################################
  fl.printsetup();
  delay(1000);
}

void loop() 
{

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &letters, sizeof(letters));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

 // Serial.printf("ring%i%\n", analogRead(Flex_pin2));
 //reading the mpu values
 IMU.readSensor();
 readmpu();
 // print_mpu();
 //read, map and constrain the flex values
 float Flexgrade1 = readconstrainmapflex(Flex_pin1, flexADC1, sensorMin1, sensorMax1);
 float Flexgrade2 = readconstrainmapflex(Flex_pin2, flexADC2, sensorMin2, sensorMax2);
 float Flexgrade3 = readconstrainmapflex(Flex_pin3, flexADC3, sensorMin3, sensorMax3);
 float Flexgrade4 = readconstrainmapflex(Flex_pin4, flexADC4, sensorMin4, sensorMax4);
 float Flexgrade5 = readconstrainmapflex(Flex_pin5, flexADC5, sensorMin5, sensorMax5);

 //print the flex values
 // printflex(Flexgrade1, Flexgrade2, Flexgrade3, Flexgrade4, Flexgrade5);

Serial.println();
  
  if (((Flexgrade1 >= 25) && (Flexgrade1 <= 42)) && ((Flexgrade2 >= 40) && (Flexgrade2 <= 60)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 60)) && ((Flexgrade4 >= 35) && (Flexgrade4 <= 60)) && (Flexgrade5 == 90))
  {
    Serial.print("A");
  }
  
  else if  ((Flexgrade1 == 90) && (Flexgrade2 == 90) && (Flexgrade3 == 90) && (Flexgrade4 == 90) && (Flexgrade5 <= 70))
  {
    Serial.print("B");
  }
  else if  (((Flexgrade1 >= 47) && (Flexgrade1 <= 65)) && ((Flexgrade2 >= 40) && (Flexgrade2 <= 70)) && ((Flexgrade3 >= 47) && (Flexgrade3 <= 55)) && ((Flexgrade4 >= 40) && (Flexgrade4 <= 65)) && ((Flexgrade5 >= 56) && (Flexgrade5 <= 75)))
  {
    Serial.print("C");
   
  }
  else if (((Flexgrade1 >= 44)&& (Flexgrade1 <= 60)) && ((Flexgrade2 >= 45) && (Flexgrade2 <= 60)) && ((Flexgrade3 >= 45) && (Flexgrade3 <= 60)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && (Flexgrade5 <= 65))
  {
    Serial.print("D");
    //printletter('D');
  }
  else if (((Flexgrade1 >= 28) && (Flexgrade1 <= 35)) && ((Flexgrade2 >= 30) && (Flexgrade2 <= 43)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 54)) && ((Flexgrade4 >= 30) && (Flexgrade4 <= 45)) && ((Flexgrade5 >= 53) && (Flexgrade5 <= 55)))
  {
    Serial.print("E");
    //printletter('E');
  }
  else if (((Flexgrade1 >= 85) && (Flexgrade1 <= 90)) && ((Flexgrade2 >= 85) && (Flexgrade2 <= 90)) && ((Flexgrade3 >= 85) && (Flexgrade3 <= 90)) && ((Flexgrade4 >= 38) && (Flexgrade4 <= 50)) && ((Flexgrade5 >= 59) && (Flexgrade5 <= 75)))
  {
    Serial.print("F");
    //printletter('F');
  }


  //mpu
  else if (((Flexgrade1 >= 25) && (Flexgrade1 <= 32)) && ((Flexgrade2 >= 40) && (Flexgrade2 <= 55)) && ((Flexgrade3 >= 43) && (Flexgrade3 <= 58)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 80) && (Flexgrade5 <= 90)))
    {
    Serial.print("G");
    //printletter('G');
  }
  else if ((Flexgrade1 == 90) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 63)) && ((Flexgrade3 >= 50) && (Flexgrade3 <= 61)) && ((Flexgrade4 >= 40) && (Flexgrade4 <= 52)) && ((Flexgrade5 >= 48) && (Flexgrade5 <= 65)))
  {
    Serial.print("I");
    //printletter('I');
  }
  else if (((Flexgrade1 >= 33) && (Flexgrade1 <= 42)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 61)) && (Flexgrade3 >= 90) && (Flexgrade4 == 90) && ((Flexgrade5 >= 59) && (Flexgrade5 <= 65)))
  {
    Serial.print("K");
   // printletter('K');
  }
  else if (((Flexgrade1 >= 30) && (Flexgrade1 <= 42)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 63)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 55))&& (Flexgrade4 == 90) && (Flexgrade5 == 90))
  {
    Serial.print("L");
   // printletter('L');
  }
  else if (((Flexgrade1 >= 28) && (Flexgrade1 <= 50)) && ((Flexgrade2 == 90) && (Flexgrade3 == 90)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 50) && (Flexgrade5 >= 65 )))
  {
    Serial.print("M");
   // printletter('M');
  }



  //
  else if (((Flexgrade1 >= 36) && (Flexgrade1 <= 45)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 60)) && (Flexgrade3 == 90)&& ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 50) && (Flexgrade5 >= 70)))
  {
    Serial.print("N");
    printletter('N');
  }
  else if  (((Flexgrade1 >= 45) && (Flexgrade1 <= 60)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 60)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 55)) && ((Flexgrade4 >= 30) && (Flexgrade4 <= 50)) && ((Flexgrade5 >= 40) && (Flexgrade5 <= 55)))
  {
    Serial.print("O");
    
  }

  else if  (((Flexgrade1 >= 70) && (Flexgrade1 <= 80)) && ((Flexgrade2 >= 68) && (Flexgrade2 <= 85)) && ((Flexgrade3 >= 55) && (Flexgrade3 <= 70)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 54) && (Flexgrade5 <= 64)))
  {
    Serial.print("P");
   // printletter('P');
  }
  
 delay(200);
// printletter(Read_letter);



Serial.println(letters.*Letter[]);

 // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &letters, sizeof(letters));
}


void printletter(char read_letter)
{
  if (read_letter != priv_letter)
  {
    Serial.println(read_letter);
    priv_letter = read_letter;
  }
}

void calibrateflex()
{
  sensorMax1 = analogRead(Flex_pin1);
  sensorMax2 = analogRead(Flex_pin2);
  sensorMax3 = analogRead(Flex_pin3);
  sensorMax4 = analogRead(Flex_pin4);
  sensorMax5 = analogRead(Flex_pin5);
  privtime = millis();
  
  while(millis() < privtime + times)
  {
    if(digitalRead(7)==HIGH)
      {
        float flexADC1 = analogRead(Flex_pin1);
        float flexADC2 = analogRead(Flex_pin2);
        float flexADC3 = analogRead(Flex_pin3);
        float flexADC4 = analogRead(Flex_pin4);
        float flexADC5 = analogRead(Flex_pin5);
      
        if(flexADC1<sensorMin1)
        {
          sensorMin1=flexADC1;
        }
        if(flexADC1>sensorMax1)
        {
          sensorMax1=flexADC1;
        }
      
        if(flexADC2<sensorMin2)
        {
          sensorMin2=flexADC2;
        }
        if(flexADC2>sensorMax2)
        {
          sensorMax2=flexADC2;
        }
      
        if(flexADC3<sensorMin3)
        {
          sensorMin3=flexADC3;
        }
        if(flexADC3>sensorMax3)
        {
          sensorMax4=flexADC4;
        }
        if(flexADC4<sensorMin4)
        {
          sensorMin4=flexADC4;
        }
        if(flexADC4>sensorMax4)
        {
          sensorMax4=flexADC4;
        }
        if(flexADC5<sensorMin5)
        {
          sensorMin5=flexADC5;
        }
        if(flexADC5>sensorMax5)
        {
          sensorMax5=flexADC5;
        }
      
      }
  }
}

