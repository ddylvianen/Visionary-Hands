#include <esp_now.h>
#include <WiFi.h>
#include "string.h"




// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x34, 0xB4, 0x72, 0x6A, 0x54, 0xC6};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    float rFlexgrade1;
    float rFlexgrade2;
    float rFlexgrade3;
    float rFlexgrade4;
    float rFlexgrade5;
    int raccelY;
    int raccelZ;
    int raccelX;
    int rgyroX;
    int rgyroY;
    int rgyroZ;
    
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
 
