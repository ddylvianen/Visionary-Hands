// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    float rFlexgrade1;
    float rFlexgrade2;
    float rFlexgrade3;
    float rFlexgrade4;
    float rFlexgrade5;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  float rFlexgrade1 = myData.rFlexgrade1;
  float rFlexgrade2 = myData.rFlexgrade2;
  float rFlexgrade3 = myData.rFlexgrade3;
  float rFlexgrade4 = myData.rFlexgrade4;
  float rFlexgrade5 = myData.rFlexgrade5;
}
