#include "flexx.h"
#include "vibrator.h"
//#include IMU // include IMU code as part of the glove for determining motion & pose
#include "ESPAsyncWebServer.h"
//#include <Arduino_JSON.h>
#include "time.h"
//#include <ArduinoJson.h>
#include <WebSocketsServer.h>
#include "soc/soc.h"           // disable brownout problems
#include "soc/rtc_cntl_reg.h"  // disable brownout problems
#include "SPIFFS.h"
#include "string.h"

#include <WiFi.h>

const char *soft_ap_ssid = "VisionAP";
const char *soft_ap_password = "12345678";

const char *wifi_network_ssid = "telew_f2a";
const char *wifi_network_password = "aa003234";


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

//StaticJsonBuffer<200> jsonBuffer;

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch (type) {
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);  // Serial.printf("[%u] Disconnected!\n", num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        cam_num = client_num;
        connected = true;
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());

        String text = "Welcome to the visionnary hands web app!";
        StaticJsonDocument<200> db;
        db["letter"] = text;
        String datab = db.as<String>();
        webSocket.broadcastTXT(datab);
                StaticJsonDocument<200> d;
        d["letter"] = "NAN";
        datab = d.as<String>();
        webSocket.broadcastTXT(datab);
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
  Serial.println("[" + remote_ip.toString() + "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// instantiate glove class
Glove leftGlove;

vibrator vi;
void setup() {
  // put your setup code here, to run once:
  analogReadResolution(12);

  vi.set_up();
  WiFi.mode(WIFI_MODE_APSTA);

  WiFi.softAP(soft_ap_ssid, soft_ap_password);
//  WiFi.begin(wifi_network_ssid, wifi_network_password);
//   while (WiFi.status() != WL_CONNECTED) {
//    Serial.print('.');
//    delay(1000);
//  }

  Serial.begin(115200);
  Serial.println("Visionary Hands Loaded");
  leftGlove.calibrateflex();
  Serial.println("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("ESP32 IP on the WiFi network: ");
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // put your setup code here, to run once:
  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  server.serveStatic("/", SPIFFS, "/");

  events.onConnect([](AsyncEventSourceClient *client) {
    if (client->lastId()) {
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
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("loop");
  wait(1000);

  // update Glove values
  leftGlove.update();

  // TO DO!!handle flow here? glove changed + stable
  //if (leftGlove.state=true){
  //leftGlove.printFlex();

  String data = leftGlove.scanChar();

  int j = sendJson(data);
  
  wait(1000);
}

void wait(int i) {
  long mil1 = millis() + i;

  while (millis() < mil1) {
    webSocket.loop();
    leftGlove.update();
  }
}

bool loop__sound() {
  long mil1 = millis() + 3000;

  while (millis() < mil1) {
    webSocket.loop();
    leftGlove.update();
    float sound = vi.loop_sound();
    if (sound > 3000) {
      return true;
    }
  }
}

int sendJson(String datab) {
  webSocket.broadcastTXT(datab);
  return 1;
}
