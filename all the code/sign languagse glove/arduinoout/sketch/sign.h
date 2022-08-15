#line 1 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\sign.h"
#include <esp_now.h>
#include <WiFi.h>



// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0x34, 0xB4, 0x72, 0x6A, 0x54, 0xC6};

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
 

 
