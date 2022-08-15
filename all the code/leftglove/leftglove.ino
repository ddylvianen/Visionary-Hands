
#include <esp_now.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>

#include <WebSocketsServer.h>
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "SPIFFS.h"

#include "flexsensor.h"
#include "flexx.h"
#include "mpu.h"
#include "MPU9250.h"
//#include "Wire.h"
#include <Wire.h>
#include "sign.h"


//an flex sensor object with the flex sensor pins for every finger
flex fl;



int status;
char Read_letter = '0';
unsigned long privtime = 0, times = 15000;


const char* wifi_network_ssid = "telew_f2a";
const char* wifi_network_password =  "aa003234";
 
const char *soft_ap_ssid = "SlosAP";
const char *soft_ap_password = "87654321";


const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;


WebSocketsServer webSocket = WebSocketsServer(81);
uint8_t cam_num;
bool connected = false;


AsyncWebServer server(80);
AsyncEventSource events("/events");
char msg_buf[10];
int led_state = 120;
int arm_ground = 0;


// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {
 
  // Figure out the type of WebSocket event
  switch(type) {
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);// Serial.printf("[%u] Disconnected!\n", num);
      break;
 
    // New client has connected
    case WStype_CONNECTED:
      {
        cam_num = client_num;
        connected = true;
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;
 
    // Handle text messages from client
    case WStype_TEXT:
      {
        String string_payload = (char *)payload;
              
      // Print out raw message
      Serial.printf("[%u] Received text: %s\n", client_num, payload);

      break;
      }
    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}


// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.ccs", "text/css");
}


void onJSRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/js/script.js", "text/css");
}
 
// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                  "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
} 

void rightflex()
{
  
  float rFlexgrade1 = myData.rFlexgrade1;
  float rFlexgrade2 = myData.rFlexgrade2;
  float rFlexgrade3 = myData.rFlexgrade3;
  float rFlexgrade4 = myData.rFlexgrade4;
  float rFlexgrade5 = myData.rFlexgrade5;
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(10);


   WiFi.mode(WIFI_MODE_APSTA);
 
  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  WiFi.begin(wifi_network_ssid, wifi_network_password);
 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.print("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());
 
  Serial.print("ESP32 IP on the WiFi network: ");
  Serial.println(WiFi.localIP());
  
   // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);
 
  // On HTTP request for style sheet, provide style.css
  server.on("/css/style.css", HTTP_GET, onCSSRequest); 

  // On HTTP request for style sheet, provide style.css
  server.on("/js/script.js", HTTP_GET, onJSRequest);

  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);
   
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

    fl.pinmode(Flex_pin1, Flex_pin2, Flex_pin3, Flex_pin4, Flex_pin5);

  //imu cor
  Wire.begin(11, 18); // left glove

  while(!Serial) {}
  //start communication with IMU 
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

// Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet

   if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);

  //####################################################################################################################
  fl.printsetup();
  delay(1000);

  webSocket.broadcastTXT("HELLO WORLD"); 
 
}

// Params to manage the sign/typing speed
String lastChar = "";       // last typed char
unsigned long lastCharTime; // time of last characted
int typeRate = 700;         // nr of millis between letters Also add MIN rate
// To Do's
int wordPause = 1000;       // word pause assumes end of word (or is there a sign for that??)
int sentencePause = 2000;     // After 2 secs of nothing system assumes and of sentence and will send a trigger to your Text-To-Speech

int repeatCount=0;           // makes sure that if you want to repeat the same letter you just hold it longer than a normal one

void loop() 
{
  webSocket.loop();

 //reading the mpu values
 IMU.readSensor();
 readmpu();
 print_mpu();

 float Flexgrade1 = readconstrainmapflex(Flex_pin1, flexADC1, sensorMin1, sensorMax1);
 float Flexgrade2 = readconstrainmapflex(Flex_pin2, flexADC2, sensorMin2, sensorMax2);
 float Flexgrade3 = readconstrainmapflex(Flex_pin3, flexADC3, sensorMin3, sensorMax3);
 float Flexgrade4 = readconstrainmapflex(Flex_pin4, flexADC4, sensorMin4, sensorMax4);
 float Flexgrade5 = readconstrainmapflex(Flex_pin5, flexADC5, sensorMin5, sensorMax5);

 
 printflex(Flexgrade1, Flexgrade2, Flexgrade3, Flexgrade4, Flexgrade5);

/*char* rcvChar = readFlex(); // read the flex and store value
// NOTE: you need a end or word indicator, end of line indicator that calls text to speech
  if(millis()-lastCharTime > sentencePause && lastChar != "'\n'"){
     webSocket.broadcastTXT("\n");
     lastCharTime = millis();
  }
  if(millis()-lastCharTime > wordPause && lastChar != " "){
     webSocket.broadcastTXT(" ");
     lastCharTime = millis();
     lastChar = " ";
  }
  if (millis()-lastCharTime > typeRate){ // handle false letters
    if ( lastChar != rcvChar || repeatCount == 3){ 
      webSocket.broadcastTXT(rcvChar);
      lastCharTime = millis();
      repeatCount = 0;
    } else {
      lastCharTime = millis();
      repeatCount++;
    }
  }
  */
char* priv_letter = "";
 printletter(readFlex());
}


void printletter(char read_letter)
{
  if (read_letter != priv_letter)
  {
    Serial.println(read_letter);
    priv_letter = read_letter;
  }
}

String readFlex(){
  // return a character from flex sensors
 //read, map and constrain the flex values
 float Flexgrade1 = readconstrainmapflex(Flex_pin1, flexADC1, sensorMin1, sensorMax1);
 float Flexgrade2 = readconstrainmapflex(Flex_pin2, flexADC2, sensorMin2, sensorMax2);
 float Flexgrade3 = readconstrainmapflex(Flex_pin3, flexADC3, sensorMin3, sensorMax3);
 float Flexgrade4 = readconstrainmapflex(Flex_pin4, flexADC4, sensorMin4, sensorMax4);
 float Flexgrade5 = readconstrainmapflex(Flex_pin5, flexADC5, sensorMin5, sensorMax5);
 readmpu();
 //print the flex values
 // printflex(Flexgrade1, Flexgrade2, Flexgrade3, Flexgrade4, Flexgrade5);

Serial.println();
  
  if (((Flexgrade1 >= 25) && (Flexgrade1 <= 42)) && ((Flexgrade2 >= 40) && (Flexgrade2 <= 60)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 60)) && ((Flexgrade4 >= 35) && (Flexgrade4 <= 60)) && (Flexgrade5 == 90))
  {
    Serial.print("A");
    //webSocket.broadcastTXT("A");
    return "A";
  }
  
  else if  ((Flexgrade1 == 90) && (Flexgrade2 == 90) && (Flexgrade3 == 90) && (Flexgrade4 == 90) && (Flexgrade5 <= 70))
  {
    Serial.print("B");
    return "B";
  }
  else if  (((Flexgrade1 >= 47) && (Flexgrade1 <= 65)) && ((Flexgrade2 >= 40) && (Flexgrade2 <= 70)) && ((Flexgrade3 >= 47) && (Flexgrade3 <= 55)) && ((Flexgrade4 >= 40) && (Flexgrade4 <= 65)) && ((Flexgrade5 >= 56) && (Flexgrade5 <= 75)))
  {
    Serial.print("C");
   return "C";
   
  }
  else if (((Flexgrade1 >= 44)&& (Flexgrade1 <= 60)) && ((Flexgrade2 >= 45) && (Flexgrade2 <= 60)) && ((Flexgrade3 >= 45) && (Flexgrade3 <= 60)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && (Flexgrade5 <= 65))
  {
    Serial.print("D");
   return "D";
    //printletter('D');
  }
  else if (((Flexgrade1 >= 28) && (Flexgrade1 <= 35)) && ((Flexgrade2 >= 30) && (Flexgrade2 <= 43)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 54)) && ((Flexgrade4 >= 30) && (Flexgrade4 <= 45)) && ((Flexgrade5 >= 53) && (Flexgrade5 <= 55)))
  {
    Serial.print("E");
    return "E";
    //printletter('E');
  }
  else if (((Flexgrade1 >= 85) && (Flexgrade1 <= 90)) && ((Flexgrade2 >= 85) && (Flexgrade2 <= 90)) && ((Flexgrade3 >= 85) && (Flexgrade3 <= 90)) && ((Flexgrade4 >= 38) && (Flexgrade4 <= 50)) && ((Flexgrade5 >= 59) && (Flexgrade5 <= 75)))
  {
    Serial.print("F");
    //printletter('F');
    return "F";
  }


  //mpu
  else if (((Flexgrade1 >= 25) && (Flexgrade1 <= 32)) && ((Flexgrade2 >= 40) && (Flexgrade2 <= 55)) && ((Flexgrade3 >= 43) && (Flexgrade3 <= 58)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 80) && (Flexgrade5 <= 90)))
    {
    Serial.print("G");
    //printletter('G');
    return "G";
  }
  
  else if ((Flexgrade1 == 90) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 63)) && ((Flexgrade3 >= 50) && (Flexgrade3 <= 61)) && ((Flexgrade4 >= 40) && (Flexgrade4 <= 52)) && ((Flexgrade5 >= 48) && (Flexgrade5 <= 65)))
  {
    Serial.print("I");
    //printletter('I');
    return "I";
  }
  else if (((Flexgrade1 >= 33) && (Flexgrade1 <= 42)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 61)) && (Flexgrade3 >= 90) && (Flexgrade4 == 90) && ((Flexgrade5 >= 59) && (Flexgrade5 <= 65)))
  {
    Serial.print("K");
   // printletter('K');
   return "K";
  }
  else if (((Flexgrade1 >= 30) && (Flexgrade1 <= 42)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 63)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 55))&& (Flexgrade4 == 90) && (Flexgrade5 == 90))
  {
    Serial.print("L");
   // printletter('L');
   return "L";
  }
  else if (((Flexgrade1 >= 28) && (Flexgrade1 <= 50)) && ((Flexgrade2 == 90) && (Flexgrade3 == 90)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 50) && (Flexgrade5 >= 65 )) && ((accelZ >= 8) && (accelZ <= 10)))
  {
    Serial.print("M");
   // printletter('M');
    return "M";
  }

  //
  else if (((Flexgrade1 >= 36) && (Flexgrade1 <= 45)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 60)) && (Flexgrade3 == 90)&& ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 50) && (Flexgrade5 >= 70)))
  {
    Serial.print("N");
//    printletter('N');
    return "N";
  }
  else if  (((Flexgrade1 >= 45) && (Flexgrade1 <= 60)) && ((Flexgrade2 >= 50) && (Flexgrade2 <= 60)) && ((Flexgrade3 >= 40) && (Flexgrade3 <= 55)) && ((Flexgrade4 >= 30) && (Flexgrade4 <= 50)) && ((Flexgrade5 >= 40) && (Flexgrade5 <= 55)))
  {
    Serial.print("O");
    return "O";
    
  }

  else if  (((Flexgrade1 >= 70) && (Flexgrade1 <= 80)) && ((Flexgrade2 >= 68) && (Flexgrade2 <= 85)) && ((Flexgrade3 >= 55) && (Flexgrade3 <= 70)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && ((Flexgrade5 >= 54) && (Flexgrade5 <= 64)))
  {
    Serial.print("P");
   return "P";
  }
  /* if((rFlexgrade1 >= 80) && (rFlexgrade2 >= 80) && (rFlexgrade3 >= 80) && (rFlexgrade4 >= 80) && (rFlexgrade5 >= 80))
  {
    if ((Flexgrade1 >= 80) && (Flexgrade2 >= 80) && (Flexgrade3 >= 80) && (Flexgrade4 >= 80))
    {
      Serial.print("SunDay");
      return "SunDay";
    }
  }*/
  else if (((Flexgrade1 >= 44)&& (Flexgrade1 <= 60)) && ((Flexgrade2 >= 45) && (Flexgrade2 <= 60)) && ((Flexgrade3 >= 45) && (Flexgrade3 <= 60)) && ((Flexgrade4 >= 80) && (Flexgrade4 <= 90)) && (Flexgrade5 <= 65) && (accelz >= 1) && (accelx >= 2))
  {
    Serial.print("Dinsdag");
   return "Dinsdag";
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
