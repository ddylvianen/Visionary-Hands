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

int status;
char Read_letter = '0';
unsigned long privtime = 0, times = 15000;

 
void setup() {
 
  
  // Init Serial Monitor
  Serial.begin(115200);

fl.pinmode(Flex_pin1, Flex_pin2, Flex_pin3, Flex_pin4, Flex_pin5);

  //imu cor
  
  Wire.begin(18, 20); // right glove
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


//############################################################################################################################################
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
}
 
void loop() 
{
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
 float Flexgrade5 = analogRead(Flex_pin5);

 //print the flex values
 // printflex(Flexgrade1, Flexgrade2, Flexgrade3, Flexgrade4, Flexgrade5);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(200);


}

void readflex()
{
  myData.rFlexgrade1 = readconstrainmapflex(Flex_pin1, flexADC1, sensorMin1, sensorMax1);
  myData.rFlexgrade2 = readconstrainmapflex(Flex_pin2, flexADC2, sensorMin2, sensorMax2);
  myData.rFlexgrade3 = readconstrainmapflex(Flex_pin3, flexADC3, sensorMin3, sensorMax3);
  myData.rFlexgrade4 = readconstrainmapflex(Flex_pin4, flexADC4, sensorMin4, sensorMax4);
  myData.rFlexgrade5 = analogRead(Flex_pin5);
}

void readmpu()
{
  myData.raccelY = IMU.getAccelY_mss();
  myData.raccelZ = IMU.getAccelZ_mss();
  myData.raccelX = IMU.getAccelX_mss();
  myData.rgyroX = IMU.getGyroX_rads();
  myData.rgyroY = IMU.getGyroY_rads();
  myData.rgyroZ = IMU.getGyroZ_rads();

}
