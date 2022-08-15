#include <esp_now.h>
#include <WiFi.h>



// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    float Flexgrade1;
    float Flexgrade2;
    float Flexgrade3;
    float Flexgrade4;
    float Flexgrade5;
    char *Letter[];
} struct_message;



// Create a struct_message to hold incoming sensor readings
struct_message incomingleftReadings;
struct_message letters;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingleftReadings, incomingData, sizeof(incomingleftReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
}
 

 
