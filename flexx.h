#include <ArduinoJson.h>
// ASCI Char/guage detection normalized (to 100%)
// Table is probably wrong data
int strainTable[27][6] =
{
  {10, 90, 90, 90, 90, 90},
  {97, 30, 50, 55, 60, 80},
  {98, 90, 90, 90, 90, 70},
  {99, 66, 70, 70, 77, 8},
  {100, 40, 54, 55, 86, 75},
  {101, 48, 53, 59, 53, 66},
  //{102, 90, 90, 90, 62, 83},
  {103, 45, 54, 63, 90, 78},
  {104, 43, 58, 90, 90, 71},
  {105, 90, 66, 66, 64, 69},
  {106, 90, 66, 66, 61, 80},
  //{107, 38, 60, 90, 90, 72},
  {108, 42, 64, 61, 90, 90},
  // {109, 52, 57, 64, 64, 68},
  //{110, 54, 70, 65, 62, 69},
  {111, 69, 68, 68, 67, 83},
  //{112, 42, 59, 81, 90, 90},
  //{113, 43, 58, 58, 73, 89}
  //{114, 45, 59, 90, 90, 71},
  //{115, 49, 55, 58, 57, 67}
  //{116, 46, 55, 64, 60, 76},
  //{117, 45, 58, 90, 90, 75},
  //{118, 48, 59, 90, 90, 70},
  //{119, 59, 90, 90, 90, 71},
  //{120, 43, 54, 57, 75, 70},
  {121, 90, 58, 66, 58, 87},
  // {122, 45, 57, 63, 90, 73}
};

// Historical measurements
// 10 measurememts to detect when system is stable
// as soon as more than 5 readings have distance < 1
// we can consider it stable
int strainHistory[10][5];
int iHist = 0; // running counter for history

// Helper functions
float vDistance(int u[5], int v[6]) {
  // Calculates vector dictance between 2 vectors length 5
  // the v vector has ASCI code as 1st element
  int dist = 0;
  int dist1 = 0;
  for (int i = 0; i < 5; i++) {
    if (v[i + 1] > u[i])
    {
      dist += (v[i + 1] - u[i]); //*(u[i]-v[i+1]);
    }
    else
    {
      dist += ((u[i] - v[i + 1]));
    }

  }
  return dist;
}


class Glove {
    float flexADC[5];
    int sensorMin[5];
    int sensorMax[5];
    static int letter[6];


    // Glove strain guages
    // DEFAULT PINS pink = 1,ring = 2,middle=3,index=4,thumb=5
    byte strainPins[5] = {3, 4, 5, 6, 7};
    const char* fingers[5] = {"pink", "ring", "middle", "index", "thumb"}; // fingers named sequence // IMU specifics (or nested class)

    // Char/word Detection timers (letter, word, sentence)
    int detTimers[3] = {100, 350, 1000}; //???
    // Params to manage the sign/typing speed
    String lastChar = "";       // last typed char
    unsigned long lastCharTime; // time of last characted
    int typeRate = 700;         // nr of millis between letters Also add MIN rate
    // To Do's
    int wordPause = 1000;       // word pause assumes end of word (or is there a sign for that??)
    int sentencePause = 2000;     // After 2 secs of nothing system assumes and of sentence and will send a trigger to your Text-To-Speech
    int repeatCount = 0;         // makes sure that if you want to repeat the same letter you just hold it longer than a normal one

    // System state & detection variables
    bool stableState = 0;         // hand is in stable state for detection
    int stableMs = 200;           // amount of MS the system must be in stable state to switch stable state
    int strainState[5];  // array with strain guage values
    int accelState[6];            // state of the accelerometer
  public:
    // Class constructors
    Glove() {
      setup();
    }
    //
    // Class Methods / functions
    void setup() {
      // Init / calibrate MPU

      // Init /calibrate glove strain

    }

    float vDistance(int u[5], int v[6]) {
      // Calculates vector dictance between 2 vectors length 5
      // the v vector has ASCI code as 1st element
      int dist = 0;
      int dist1 = 0;
      for (int i = 0; i < 5; i++) {
        if (v[i + 1] > u[i])
        {
          dist += (v[i + 1] - u[i]); //*(u[i]-v[i+1]);
        }
        else
        {
          dist += ((u[i] - v[i + 1]));
        }

      }
      return dist;
    }



    String scanChar() {
    
      //long int mil = millis + 2000;
      int index = 0;
      int dist = 1000;
      for (int i = 0; i < 24; i++) {

        // CHANGE i<14 if more chars
        // calculate vector distance against each character i table
        float vdist = vDistance(strainState, strainTable[i]);


        // If dist< threshold => print the detected chars
        if (vdist) { // only if pos detect
          Serial.write(strainTable[i][0]);
          Serial.print(" |- ");
          for (int j = 1; j < 6; j++) {
            Serial.print(strainState[j - 1]);
            Serial.print("/");
            Serial.print(strainTable[i][j]);
            Serial.print(" - ");



            //webSocket.broadcastTXT(letter);
            //sent++;
          }
        }
        if (dist > vdist)
        {
          index = i;
          dist = vdist;
          Serial.print("smaller");
        }
        Serial.print(vdist);
        Serial.println(" | ");
        //Serial.print(vdist <  ? " X " : "");


      }
    StaticJsonDocument<200> db;
    if (strainTable[index][0] == 10){
      db["letter"] = "NAN";
    }
    else{
   db["letter"] = String((char)strainTable[index][0]);
    }
   db["pink"] = strainState[1];
   db["ring"] = strainState[3];
    db["middle"] = strainState[3];
  db["pointer"] = strainState[4];
   db["thumb"] = strainState[5];
   db["distance"] = dist;
  
  String datab = db.as<String>();
  return datab;
    }
    // return array of chars found (by closest dist)

    void calibrateflex() {
      for (int i = 0; i < 5; i++)
      {
        sensorMax[i] = sampleRead(strainPins[i]);
        sensorMin[i] = 1;
        Serial.print("saved!");
      }
      int times = 5000;
      int privtime = millis();

      while (millis() < privtime + times) //looping for 15 sec(1500 mili sec)
      {
        for (int i = 0; i < 5; i++) {

          flexADC[i] = sampleRead(strainPins[i]);

          if (flexADC[i] < sensorMin[i])
          {
            sensorMin[i] = flexADC[i];
            Serial.print("small");
          }
          if (flexADC[i] > sensorMax[i])
          {
            sensorMax[i] = flexADC[i];
            Serial.print("BIg");
          }
        }


      }


    };


    void getFlex()
    {
      // prints the strain state per finger
      for ( int i = 0 ; i < 5 ; i++ ) {
        //strainState[i]=sampleRead(strainPins[i]);
        // Normalize to 90-1
        int tread = constrain(sampleRead(strainPins[i]), sensorMin[i], sensorMax[i]);
        strainState[i] = map(tread, sensorMin[i], sensorMax[i], 1, 90);
        // store strainState into strainHistory +
        strainHistory[iHist][i + 1] = strainState[i];
      }
      // calculate vactor distance to previous
      strainHistory[iHist][0] = vDistance(strainState, strainHistory[iHist == 0 ? 10 : iHist - 1]);
      iHist = (iHist + 1) % 10; // calculates i+1 and recycles above 10
      //Serial.println(strainHistory[iHist][0]);
    }

    void getState()
    {
      for (int i = 0; i < 10; i++) {
        if (true) { // only if pos detect
          Serial.print(strainHistory[i][0]);
          Serial.print(" | ");
          for (int j = 1; j < 6; j++) {
            Serial.print(strainHistory[i][j]);
            Serial.print(" - ");
          }
          Serial.print(" | ");
          Serial.println(strainHistory[i][0] < 1 ? " X " : "");

        }
      }
    }


    void printFlex()
    {
      // prints the strain state per finger
      for ( int i = 0 ; i < 5 ; i++ ) {
        Serial.print(fingers[i]);
        Serial.print("->");
        Serial.print(strainState[i]);
        Serial.print(" | ");
      }
      Serial.println("\n");
    }

    void update() {
      // scan sensors
      getFlex();


      // handle all Glove updates here (detect stable state and activate detection

      // handle all letter/word/sentence pause timers here

    }
    int sampleRead(byte aPin, int numReads = 20, int sampDelay = 1) {
      // function for sampling Analog inputs
      long senseSum = 0 ; // sum of sensor readings
      for ( int i = 0 ; i < numReads ; i++ ) {
        senseSum += analogRead(aPin) ;
        delay(sampDelay) ;
      }
      int flex = senseSum / numReads;
      return flex; // average sensor readings .
    }
};

int sampleRead(byte aPin, int numReads = 20, int sampDelay = 1) {
  // function for sampling Analog inputs
  long senseSum = 0 ; // sum of sensor readings
  for ( int i = 0 ; i < numReads ; i++ ) {
    senseSum += analogRead(aPin) ;
    delay(sampDelay) ;
  }
  int flex = senseSum / numReads;
  return flex; // average sensor readings .
}
