#include "Wire.h"
#include "SSD1306.h"
//#include "SH1106Wire.h"
//#include "SH1106.h"
//#include "SSD1306brzo.h"
#include "images.h"
//#include "arduino.h"
#include <ESP8266WiFi.h>
#include <espnow.h>
//#include "DateTime.h"
//#include "DateTimeString.h"
#include "SafeString.h"


#include "SafeStringReader.h"

// Include the UI lib
#include "OLEDDisplayUi.h"
#include "OneButton.h"
#include <CircularBuffer.h>
#include <EEPROM.h>


uint8_t broadcastAddress[] = {0xe8, 0xdb, 0x84, 0xaf, 0xa3, 0x04};


uint8_t keyEncrypt[] = {21, 33, 65, 77, 19, 29, 40, 22, 88, 96, 20, 21, 33, 65, 77, 19};

//e8:db:84:af:a3:04
//48:55:19:00:eb:00



//#include "Melody.h"
//#include "Musician.h"

#define PIN_BUZZER 14
#define BTN 2


//time_t now();//makeTime(byte sec, byte min, byte hour, byte day, byte month, int year );


//#include "painlessMesh.h"
//#include <Arduino_JSON.h>

// MESH Details
#define   MESH_PREFIX     "BZEMESH" //name for your MESH
#define   MESH_PASSWORD   "BZEM3Sh" //password for your MESH
#define   MESH_PORT       8888 //default port

/*
  //The constructor depends on the architecture you use
  #ifdef ESP_PLATFORM
  #define CHANNEL 9
  Musician musician(PIN_BUZZER, CHANNEL);
  #else
  Musician musician(PIN_BUZZER);
  #endif
*/
CircularBuffer<float, 46> atemp;
CircularBuffer<float, 46> avolt;



int buzzerPin = 14;
int multimode = 0;
int animring = 0;

float voltwarn = 11;
float tempwarn = 95;
float adjtemp = 0;
float adjvolt = 0;
float oldvoltwarn = 0;
float oldtempwarn = 0;
float oldadjtemp = 0;
float oldadjvolt = 0;
double timeranim = 0;

double timersec = 0;


unsigned long timerwarn = 0;

bool blinkwarn = false;

// specify the buzzer's pin and the standby voltage level

int buzzon = 0;

int screen;

#define BUZZ 14   //ok as output


// 0 : view
// 1 : set value
int mode = 0;

int menu = 0;
int countmenu = 4;



//-- get from serial data
float temp = 0;
float volt = 0;

//Air conditioner parameter
float tempcurr = 0;
float tempset = 0;
int fanset = 0;
int freeze = 0;
int initserial = 0;
int idleserial = 0;
int defog = 0;
int mode1 = 0;
int mode2 = 0;
int compstat = 0;

//GPS Parameter
long lat = 0;
long lon = 0;
double speed = 0;
double odo = 0;
double resetodo = 0;
int alt = 0;
char times[9];
char dates[11];

int satused = 0;
int satview = 0;
int gpsstat = 0;

//GSM
int signal = 0;
int gprs = 0;
int bat = 0;



bool buzzer = false, buzzeron = false;

//Melody melody(" (cgc*)***---");
//Melody melody("c>>> d>> e>f g< a<< b<<< c*<<<<", 240);


typedef struct struct_message {
  double Lat;
  double Long;
  double odometer;
  int alt;
  float gps_speed;
  byte num_sat;
  byte satinview;
  char dates[11];
  char times[9];

  //  char node[15];
  //  char a[32];
  //  int b;
  //  float c;
  //  String d;
  //  bool e;
} struct_message;


typedef struct struct_data {
  float coollant;
  float volt;
  int fanst;
  float temp;
  float tempset;
  int acstat;


  //  char node[15];
  //  char a[32];
  //  int b;
  //  float c;
  //  String d;
  //  bool e;
} struct_data;

// Create a struct_message called myData
struct_message myData;

struct_data rcvData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  strcpy(dates, myData.dates);
  strcpy(times, myData.times);
  //dates = myData.dates;
  //times = myData.times;
  satused = myData.num_sat;
  satview = myData.satinview;
  odo = myData.odometer / 1000;
  speed = myData.gps_speed;
  alt = myData.alt;



  Serial.print("Bytes received: ");
  Serial.println(len);


   Serial.print("Lat:");
   Serial.println(myData.Lat);

 Serial.print("Lon:");
   Serial.println(myData.Long);   

 Serial.print("odo:");
   Serial.println(myData.odometer);   

 Serial.print("alt:");
   Serial.println(myData.alt);

 Serial.print("speed:");
   Serial.println(myData.gps_speed);   

 Serial.print("nunm_sat:");
   Serial.println(myData.num_sat);

 Serial.print("sat in View:");
   Serial.println(myData.satinview);      

 Serial.print("Date:");
   Serial.println(myData.dates);   

 Serial.print("Time:");
   Serial.println(myData.times);   
   
  ///Serial.print("Dates: ");
  //Serial.println(myData.dates);

  //Serial.print("times: ");
  //Serial.println(myData.times);

  /*
    Serial.print("Node: ");
    Serial.println(myData.node);
    Serial.print("Char: ");
    Serial.println(myData.a);
    Serial.print("Int: ");
    Serial.println(myData.b);
    Serial.print("Float: ");
    Serial.println(myData.c);
    Serial.print("String: ");
    Serial.println(myData.d);
    Serial.print("Bool: ");
    Serial.println(myData.e);
    Serial.println();
  */
}



unsigned int addrs = 0;

float getfloat(unsigned int addr) {
  float config; //float to be read back in to
  unsigned int address = addr;
  for (byte i = 0; i < sizeof(config); i++) {
    reinterpret_cast<byte*>(&config)[i] = EEPROM.read(address + i);
    addrs++;
  }

  //addrs = address + 1;
  return addrs + 1;
}


unsigned int putfloat(unsigned int addr, float val) {
  float config = val;
  unsigned int address = addr;
  unsigned int adr = addr;
  for (byte i = 0; i < sizeof(config); i++) {
    EEPROM.write(address + i, reinterpret_cast<byte*>(&config)[i]);
    adr++;
  }
  return adr + 1;
}




byte  writeFloat(byte pos, float val) {
  byte addr = 0;
  byte neg = 0;
  int vals = 0;

  if ( val < 0)
    neg = 1;

  EEPROM.write(pos, neg );

  if (val < 0 )
    vals = val * 10 * -1.0;
  else
    vals = val * 10;

  EEPROM.write(pos + 1, vals);

  addr = pos + 1 + sizeof(vals);

  return addr;
}


float  readFloat(byte pos) {
  byte addr = 0;
  byte neg = 0;
  int vals = 0;
  float values = 0;

  neg = EEPROM.read(pos);

  vals = EEPROM.read(pos + 1);

  if ( neg == 1) {
    values = vals * -1.0 * .1;
  } else {
    values = vals * .1;
  }
  addrs = pos + 1 + sizeof(vals);
  return values;
}



void getEEPROM()
{
  byte pos = 0;
  byte addr = 0;

  tempwarn = EEPROM.read(pos);

  addr = sizeof(tempwarn);
  pos += addr;
  voltwarn = EEPROM.read(pos);

  addr = sizeof(voltwarn);
  pos += addr;

  adjtemp = readFloat(pos);

  adjvolt =  readFloat(addrs);



  /*

    adjtemp = EEPROM.read(pos);

    addr = sizeof(adjtemp);
    pos += addr;
  */

  //adjvolt = getfloat(addrs);

  //adjvolt = EEPROM.read(pos);

  addr = sizeof(adjvolt);
  pos += addr;

}


void clearparam() {

  //-- get from serial data
  temp = 0;
  volt = 0;

  //Air conditioner parameter
  tempcurr = 0;
  tempset = 0;
  fanset = 0;
  freeze = 0;
  initserial = 0;
  idleserial = 2;
  defog = 0;
  mode1 = 0;
  mode2 = 0;
  compstat = 0;

  //GPS Parameter
  lat = 0;
  lon = 0;
  //memset(&times, 0, sizeof(times));
  strcpy(times, "");
  strcpy(dates, "");
  speed = 0;
  //times[0]="/;
  //dates = "";
  satused = 0;
  satview = 0;
  gpsstat = 0;
  odo = 0;

  //GSM
  signal = 0;
  gprs = 0;
  bat = 0;
}

void putEEPROM()
{

  byte pos = 0;
  byte addr = 0;

  EEPROM.write(pos, tempwarn);

  addr = sizeof(tempwarn);
  pos += addr;
  EEPROM.write(pos, voltwarn);


  addr = sizeof(voltwarn);
  pos += addr;

  pos = writeFloat(pos, adjtemp);

  pos = writeFloat(pos, adjvolt);

  //adr =  putfloat(pos,adjtemp);


  //addr =  putfloat(adr,adjtemp);



  /*
    EEPROM.write(pos, adjtemp);

    addr = sizeof(adjtemp);
    pos += addr;
    EEPROM.write(pos, adjvolt);
  */
  //addr = sizeof(adjspeed);
  //pos += addr;

  //Serial.println("");
  if (EEPROM.commit()) {
    Serial.println("Data successfully committed");
  } else {
    Serial.println("ERROR! Data commit failed");
  }
}

void getSaved()
{

  getEEPROM();

  int flag = 0;


  // Serial.print("temps="+String(temp,1)+",volt="+String(volt,1)+",adjtemp="+String(adjtemp,1)+",adjvolt="+String(adjvolt,1));
  //adjhour = ((isnan(adjhour)|| adjhour > 24 || adjhour < 24 )?0:adjhour);
  if (isnan(tempwarn) || tempwarn > 100 || tempwarn < -1) {
    tempwarn = 95;
    flag = 1;
  }

  if (isnan(voltwarn) || voltwarn > 16 || voltwarn < 6) {
    voltwarn = 11;
    flag = 1;
  }



  if (isnan(adjtemp) || adjtemp < -100 || adjtemp > 100 ) {
    //Serial.print("temps="+String(temp,1)+",volt="+String(volt,1)+",adjtemp="+String(adjtemp,1)+",adjvolt="+String(adjvolt,1));

    adjtemp = 5;
    flag = 1;
  }

  if (isnan(adjvolt) || adjvolt < -100 || adjvolt > 20 ) {
    adjvolt = 0;
    flag = 1;
  }

  //
  //adjwarn = 0;

  //flag = 1;
  //adjtemp = -5;
  if (flag == 1) {
    saveConfig();
  }
}

void saveConfig()
{

  putEEPROM();
}

void playBuzzer()
{
  //buzzeron = true;
  if (buzzeron)
  {
    if (buzzer)
    {
      buzzer = false;
      //tone(BUZZ, 1200); // Send 1KHz sound signal...
      tone(BUZZ, 1500);
      //digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      //delay(1000);        // ...for 1 sec
      // noTone(BUZZ);     // Stop sound...
    }
    else
    {
      buzzer = true;
      noTone(BUZZ); // Stop sound...
      // tone(BUZZ, 500); // Send 1KHz sound signal...
      //
    }
  } else {
    noTone(BUZZ); // Stop sound...
    //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
}


/*
  void playMelody()
  {
  //buzzeron = true;
  //musician.refresh();

  if (buzzeron)
  {
    if (buzzer)
    {
      if (!musician.isPlaying())
      {
        buzzer = false;
        musician.play();
        //delay(1000);        // ...for 1 sec
        // noTone(BUZZ);     // Stop sound...
        //musician.playSync();
      }
    }
    else
    {
      buzzer = true;
      // musician.stop();
      // tone(BUZZ, 500); // Send 1KHz sound signal...
      //analogWrite(BUZZ,7000);
    }
  }
  else
  {
    //musician.stop();
  }
  }
*/


//OneButton bselect(14, true);

OneButton bmode(BTN, true);

// Define pin I2C for display
#define SDA1 4
#define SCL1 5

#define SDA2 13
#define SCL2 12


// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, SDA1, SCL1, GEOMETRY_128_32);
//SH1106Wire  display(0x3c, SDA1, SCL1);

//SSD1306Brzo display(0x3c, SDA1, SCL1);  // ADDRESS, SDA, SCL


//SH1106Wire  display2(0x3c, SDA2, SCL2);
SSD1306Wire  display2(0x3c, SDA2, SCL2, GEOMETRY_128_32);


OLEDDisplayUi ui ( &display2 );
OLEDDisplayUi ui2 ( &display );
OLEDDisplayUi ui3 ( &display );
OLEDDisplayUi ui4 ( &display );

//UI 1
void clockOverlay(OLEDDisplay * display, OLEDDisplayUiState * state) {

}


void singleDisplay(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui.disableIndicator();

  // Graph
  display->drawRect(x + 80, y + 0, 45, 32);
  for (byte j = 0; j < atemp.size(); j++) {
    if (j > 0)
      display->drawLine(x + 80 + j - 1, y + 32 - atemp[j - 1] , x + 80 + j, y + 32 - atemp[j] );

    display->drawVerticalLine(x + 80 + j, y + 32 - atemp[j], atemp[j]);

  }


  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 0, y + 0, "Coolant Temp:");

  //display.drawXbm(0, 0, 32, 29, tempicon);

  //display2->setFont(ArialMT_Plain_10);
  //display2.drawString(0, 0, "Battery Voltage :");

  display->setFont(ArialMT_Plain_24);



  if (( temp + adjtemp) >= tempwarn && initserial > 0) {
    display->drawString(x + 0, y + 10, String(temp + adjtemp, 1) + "");
    if (blinkwarn)
      display->drawXbm(x + 55, y + 12, 23 , 20, warning_logo);
    // display->invertDisplay();
    //else
    //  display->normalDisplay();

    //display2->fillRect(x +60 , y + 0, 36, 32);
    // display->drawXbm(x + 55, y + 8, 23 , 20, warning_logo);
    // display2->normalDisplay();
  } else {
    display->drawString(x + 0, y + 10, String((temp + adjtemp), 1) + " c");
    // display->normalDisplay();
  }

  //display2.setFont(ArialMT_Plain_24);
  //display2.drawString(0, 10, String(volt,1)+" v");

}


void doubleDisplay(OLEDDisplay * display, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui.disableIndicator();

  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 0, y + 0, "Coolant:");
  display->setFont(ArialMT_Plain_16);


  if (temp + adjtemp >= tempwarn && initserial > 0) {
    if (blinkwarn) {
      display->drawXbm(x + 32, y + 10, 23 , 20, warning_logo);
      display->drawString(x + 0, y + 14, String(temp + adjtemp, 1) + "");
    } else {
      display->drawString(x + 0, y + 14, String(temp + adjtemp, 1) + " c");
    }
  } else {
    display->drawString(x + 0, y + 14, String(temp + adjtemp, 1) + " c");
  }


  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 70, y + 0, "Battery:");
  display->setFont(ArialMT_Plain_16);

  if (volt + adjvolt <= voltwarn && initserial > 0) {
    if (blinkwarn) {
      display->drawXbm(x + 105, y + 10, 23 , 20, warning_logo);
      display->drawString(x + 70, y + 14, String(volt + adjvolt, 1) + "");
    } else {
      display->drawString(x + 70, y + 14, String(volt + adjvolt, 1) + " v");
    }

  } else {
    display->drawString(x + 70, y + 14, String(volt + adjvolt, 1) + " v");
  }
}


// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { singleDisplay, doubleDisplay };
// how many frames are there?
int frameCount = 2;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { clockOverlay };
int overlaysCount = 1;





// UI 2

void clockOverlay2(OLEDDisplay * display2, OLEDDisplayUiState * state) {

}

void singleDisplay2(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui2.disableIndicator();



  display2->setTextAlignment(TEXT_ALIGN_LEFT);


  // Graph
  display2->drawRect(x + 80, y + 0, 45, 32);
  for (byte j = 0; j < avolt.size(); j++) {
    if (j > 0)
      display2->drawLine(x + 80 + j - 1, y + 32 - avolt[j - 1] , x + 80 + j, y + 32 - avolt[j] );

    display2->drawVerticalLine(x + 80 + j, y + 32 - avolt[j], avolt[j]);

  }
  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 0, y + 0, "Battery Voltage:");

  //display.drawXbm(0, 0, 32, 29, tempicon);

  //display2->setFont(ArialMT_Plain_10);
  //display2.drawString(0, 0, "Battery Voltage :");

  display2->setFont(ArialMT_Plain_24);



  if (volt + adjvolt <= voltwarn && initserial > 0) {
    display2->drawString(x + 0, y + 10, String(volt + adjvolt, 1) + "");

    if (blinkwarn)
      display2->drawXbm(x + 55, y + 12, 23 , 20, warning_logo);

    //   display2->invertDisplay();
    // else
    //   display2->normalDisplay();

    //display2->fillRect(x +60 , y + 0, 36, 32);
    // display2->drawXbm(x + 55, y + 12, 23 , 20, warning_logo);
    // display2->normalDisplay();
  } else {
    display2->drawString(x + 0, y + 10, String(volt + adjvolt, 1) + " v");
    // display2->normalDisplay();
  }


  //display2.setFont(ArialMT_Plain_24);
  //display2.drawString(0, 10, String(volt+adjvolt,1)+" v");

}


void doubleDisplay2(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui2.disableIndicator();


  display2->setTextAlignment(TEXT_ALIGN_LEFT);

  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 0, y + 0, "Cabin Temp :");
  display2->setFont(ArialMT_Plain_16);
  display2->drawString(x + 0, y + 14, String(tempcurr, 1) + " c");

  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 70, y + 0, "Temp Set:");
  display2->setFont(ArialMT_Plain_16);
  display2->drawString(x + 70, y + 14, String(tempset, 1) + " c");
}

void doubleDisplay21(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui2.disableIndicator();

  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 0, y + 0, "A/C Status : ");
  display2->setFont(ArialMT_Plain_16);
  display2->drawString(x + 0, y + 14, (freeze == 1 ? "ON" : ( compstat == 1 ? "Idle" : "OFF" ) ));


  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 70, y + 0, "Fan Speed:");
  display2->setFont(ArialMT_Plain_16);
  display2->drawString(x + 70, y + 14, String(fanset) + "");

}

FrameCallback frames2[] = { singleDisplay2, doubleDisplay2, doubleDisplay21 };

int frameCount2 = 3;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays2[] = { clockOverlay2 };
int overlaysCount2 = 1;




// UI 3 - Setting -

void clockOverlay3(OLEDDisplay * display2, OLEDDisplayUiState * state) {

}

bool increase = false;
bool decrease = false;
unsigned long timericon = 0;


void singleDisplay3(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  display2->setTextAlignment(TEXT_ALIGN_LEFT);
  ui3.disableIndicator();
  display2->drawXbm(x + 0, y + 0, 21, 25, icon_setup);
  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 22, y + 0, "Coolant");
  display2->drawLine(x + 23, y + 15, x + 75, y + 15);

  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 24, y + 15, "Warning");
  display2->setFont(Dialog_plain_14);

  //if ( mode == 0)
  //  display2->drawString(x+ 0 + 80,y+12,String(tempwarn, 0));
  //else
  display2->drawString(x + 0 + 84, y + 7, String(tempwarn, 0));


  if (mode == 1) {
    display2->drawRect(x + 78, y + 3, 50, 26 );
    if (increase ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 8, 9, 6, up_press);
    }

    if (millis() - timericon > 300) {
      increase = false;
      display2->drawXbm(x + 115, y + 8, 9, 6, up_release);
    }


    if (decrease ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 18, 9, 6, down_press);
    }

    if (millis() - timericon > 200) {
      decrease = false;
      display2->drawXbm(x + 115, y + 18, 9, 6, down_release);
    }
  }
}


void doubleDisplay3(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui3.disableIndicator();
  display2->drawXbm(x + 0, y + 0, 21, 25, icon_setup);
  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 22, y + 0, "Battery");
  display2->drawLine(x + 23, y + 15, x + 75, y + 15);

  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 24, y + 15, "Warning");
  display2->setFont(Dialog_plain_14);

  //if ( mode == 0)
  //  display2->drawString(x+ 0 + 80,y+12,String(voltwarn, 0));
  //else
  display2->drawString(x + 0 + 84, y + 7, String(voltwarn, 0));


  if (mode == 1) {
    display2->drawRect(x + 78, y + 3, 50, 26 );
    if (increase ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 8, 9, 6, up_press);
    }

    if (millis() - timericon > 300) {
      increase = false;
      display2->drawXbm(x + 115, y + 8, 9, 6, up_release);
    }


    if (decrease ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 18, 9, 6, down_press);
    }

    if (millis() - timericon > 200) {
      decrease = false;
      display2->drawXbm(x + 115, y + 18, 9, 6, down_release);
    }
  }
}


void doubleDisplay33(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui3.disableIndicator();
  display2->drawXbm(x + 0, y + 0, 21, 25, icon_setup);
  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 22, y + 0, "Adjust.");
  display2->drawLine(x + 23, y + 15, x + 75, y + 15);

  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 24, y + 15, "Coolant");
  display2->setFont(Dialog_plain_14);

  //if ( mode == 0)
  //  display2->drawString(x+ 0 + 80,y+12,String(adjtemp, 1));
  //else
  display2->drawString(x + 0 + 84, y + 7, String((temp + adjtemp), 1));


  if (mode == 1) {
    display2->drawRect(x + 78, y + 3, 50, 26 );
    if (increase ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 8, 9, 6, up_press);
    }

    if (millis() - timericon > 300) {
      increase = false;
      display2->drawXbm(x + 115, y + 8, 9, 6, up_release);
    }


    if (decrease ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 18, 9, 6, down_press);
    }

    if (millis() - timericon > 200) {
      decrease = false;
      display2->drawXbm(x + 115, y + 18, 9, 6, down_release);
    }
  }
}


void doubleDisplay34(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui3.disableIndicator();
  display2->drawXbm(x + 0, y + 0, 21, 25, icon_setup);
  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 22, y + 0, "Adjust.");
  display2->drawLine(x + 23, y + 15, x + 75, y + 15);

  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 24, y + 15, "Battery");
  display2->setFont(Dialog_plain_14);

  //if ( mode == 0)
  //  display2->drawString(x+ 0 + 80,y+12,String(adjvolt, 0));
  //else
  display2->drawString(x + 0 + 84, y + 7, String((volt + adjvolt), 1));


  if (mode == 1) {
    display2->drawRect(x + 78, y + 3, 50, 26 );
    if (increase ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 8, 9, 6, up_press);
    }

    if (millis() - timericon > 300) {
      increase = false;
      display2->drawXbm(x + 115, y + 8, 9, 6, up_release);
    }


    if (decrease ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 18, 9, 6, down_press);
    }

    if (millis() - timericon > 200) {
      decrease = false;
      display2->drawXbm(x + 115, y + 18, 9, 6, down_release);
    }
  }
}



void doubleDisplay35(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui3.disableIndicator();
  display2->drawXbm(x + 0, y + 0, 21, 25, icon_setup);
  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 22, y + 0, "Restart");
  display2->drawLine(x + 23, y + 15, x + 75, y + 15);

  display2->setFont(Dialog_plain_12);
  display2->drawString(x + 24, y + 15, "Device");
  display2->setFont(Dialog_plain_14);

  //if ( mode == 0)
  //  display2->drawString(x+ 0 + 80,y+12,String(adjvolt, 0));
  //else
 // display2->drawString(x + 0 + 84, y + 7, String((volt + adjvolt), 1));


  if (mode == 1) {
     mode = 0;
     ESP.restart();
     /*
    display2->drawRect(x + 78, y + 3, 50, 26 );
    if (increase ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 8, 9, 6, up_press);
    }

    if (millis() - timericon > 300) {
      increase = false;
      display2->drawXbm(x + 115, y + 8, 9, 6, up_release);
    }


    if (decrease ) {
      if ( timericon + 600 < millis() )
        timericon = millis();
      display2->drawXbm(x + 115, y + 18, 9, 6, down_press);
    }

    if (millis() - timericon > 200) {
      decrease = false;
      display2->drawXbm(x + 115, y + 18, 9, 6, down_release);
    }
    */
  }
}

FrameCallback frames3[] = { singleDisplay3, doubleDisplay3, doubleDisplay33, doubleDisplay34,doubleDisplay35 };

int frameCount3 = 5;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays3[] = { clockOverlay3 };
int overlaysCount3 = 1;





// UI 4

void clockOverlay4(OLEDDisplay * display2, OLEDDisplayUiState * state) {

}

byte secs = 0;

void arrsubstr(char * source, char * dest, int len) {
  //char tim[5];
  memcpy(&dest, &source, len);
  dest[len] = '\0';
}

createSafeString(msgStr, 30);
createSafeString(temps, 30);

void singleDisplay41(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui4.disableIndicator();
  display2->setTextAlignment(TEXT_ALIGN_LEFT);

  //display2->setFont(Dialog_plain_12);
  //display2->setFont(ArialMT_Plain_10);
  //display2->drawString(x + 64, y + 0, "Time:");
  //display2->setTextAlignment(TEXT_ALIGN_CENTER);
  //display2->drawString(x + 64, y + 0, "Time");
  //display2->drawLine(x + 3, y + 10, x + 125, y + 15);

  //display2->setFont(ArialMT_Plain_24);

  char hm[6];
  char sec[2];
  char dm[5];
  char ym[8];


  if (millis() - timersec > 1000) {
    timersec = millis();

    if (secs == 0)
      secs = 1;
    else
      secs = 0;
  }

  //arrsubstr(times,hm,5);
  //memcpy(hm,&times,5);
  hm[0] = times[0];
  hm[1] = times[1];

  if (secs == 0)
    hm[2] = times[2];
  else
    hm[2] = ' ';

  hm[3] = times[3];
  hm[4] = times[4];
  hm[5] = '\0';

  sec[0] = times[6];
  sec[1] = times[7];
  sec[2] = '\0';

  dm[0] = dates[8];
  dm[1] = dates[9];
  dm[2] = '/';
  dm[3] = dates[5];
  dm[4] = dates[6];
  dm[5] = '\0';
  //2022-03-01

  //memcpy(ym,&dates,4);
  ym[0] = dates[0];
  ym[1] = ' ';
  ym[2] = dates[1];
  ym[3] = ' ';
  ym[4] = dates[2];
  ym[5] = ' ';
  ym[6] = dates[3];
  ym[7] = '\0';

  ui4.disableIndicator();

  //cSFA(msgStr, times);
  //SF(temps, "");

  //msgStr.substring(1,5);

  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 0, y + 0, "");
  display2->setFont(ArialMT_Plain_16);

  display2->setFont(ArialMT_Plain_24);
  display2->drawString(x + 3, y + 5, hm);


  //display2->setFont(ArialMT_Plain_10);
  //display2->drawString(x + 70, y + 0, "");
  display2->setFont(ArialMT_Plain_16);
  display2->drawString(x + 70, y + 1, dm);


  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 75, y + 20, ym);

  //display2->setFont(ArialMT_Plain_10);
  //display2->drawString(x + 90, y + 14, sec);


  // display2->setFont(ArialMT_Plain_24);
  // display2->setTextAlignment(TEXT_ALIGN_LEFT);
  // display2->drawString(x + 0, y + 10, hm);

  //display2->setFont(Dialog_plain_12);
  // display2->setTextAlignment(TEXT_ALIGN_RIGHT);
  // display2->drawString(x + 90, y + 10, dm);

  //display2.setFont(ArialMT_Plain_24);
  //display2.drawString(0, 10, String(volt+adjvolt,1)+" v");
}

void singleDisplay42(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui4.disableIndicator();



  ui4.disableIndicator();

  if (speed < 3)
    speed = 0;


  display2->setTextAlignment(TEXT_ALIGN_CENTER);
  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 20, y + 0, "");
  display2->setFont(ArialMT_Plain_16);
  //speed = 199;

  display2->setFont(ArialMT_Plain_24);
  display2->drawString(x + 25, y + 5, String(speed, 0));

  display2->setTextAlignment(TEXT_ALIGN_LEFT);
  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 70, y + 1, "TRIP:");
  //display2->setFont(ArialMT_Plain_10);
  //display2->drawString(x + 70, y + 0, "trip:");


  display2->drawRect(x + 70, y + 13, 69, 20);
  //odo = 9000;

  display2->setTextAlignment(TEXT_ALIGN_RIGHT);



  display2->setFont(Roboto_6);
  display2->drawString(x + 60, y + 20, "km/h");

  display2->setFont(Roboto_8);
  display2->drawString(x + 110, y + 3, "(km)");

  display2->setFont(ArialMT_Plain_16);
  display2->drawString(x + 128, y + 15, String((odo - resetodo), 1));

  /*
    display2->setFont(ArialMT_Plain_10);
    display2->drawString(x + 0, y + 0, "Speed:");
    display2->setFont(ArialMT_Plain_16);

    display2->drawString(x + 0, y + 14, String(speed, 0));

    display2->setFont(ArialMT_Plain_10);
    display2->drawString(x + 60, y + 0, "Trip:");
    display2->setFont(ArialMT_Plain_16);


    display2->drawString(x + 63, y + 14, String(odo));
  */


}


void singleDisplay43(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui4.disableIndicator();


  display2->setTextAlignment(TEXT_ALIGN_LEFT);
  ui4.disableIndicator();

  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 0, y + 0, "Sat used:");
  display2->setFont(ArialMT_Plain_16);

  display2->drawString(x + 0, y + 14, String(satview) + " / " + String(satused));

  display2->setFont(ArialMT_Plain_10);
  display2->drawString(x + 70, y + 0, "Altitude:");
  display2->setFont(ArialMT_Plain_16);

  display2->setTextAlignment(TEXT_ALIGN_RIGHT);
  display2->drawString(x + 100, y + 14, String(alt));

  display2->setFont(Roboto_8);
  display2->drawString(x + 112, y + 21, " m");

}

void singleDisplay40(OLEDDisplay * display2, OLEDDisplayUiState * state, int16_t x, int16_t y) {
  ui4.disableIndicator();


  display2->setFont(Dialog_plain_12);
  display2->setTextAlignment(TEXT_ALIGN_LEFT);
  display2->drawString(x + 30, y + 0, "A/C Control");
  display2->drawLine(x + 3, y + 15, x + 125, y + 15);

  display2->setTextAlignment(TEXT_ALIGN_CENTER);
  display2->setFont(Dialog_plain_12);

  if (mode == 1)
    display2->drawRect(x + 1, y + 15, 40, 17 );

  display2->setTextAlignment(TEXT_ALIGN_LEFT);
  display2->drawString(x + 4, y + 17, "A/C");

  if (compstat == 1 || freeze == 1 ) {

    if (animring == 0)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_0);
    else if (animring == 1)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_1);
    else if (animring == 2)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_2);
    else if (animring == 3)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_3);
    else if (animring == 4)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_4);
    else if (animring == 5)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_5);
    else if (animring == 6)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_6);
    else if (animring == 7)
      display2->drawXbm(x + 25, y + 17, 15, 15, ring_7);

    /*
        if (animring == 0)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame0);
        else if (animring == 1)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame1);
        else if (animring == 2)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame2);
        else if (animring == 3)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame3);
        else if (animring == 4)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame4);
        else if (animring == 5)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame5);
        else if (animring == 6)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame6);
        else if (animring == 7)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame7);
        else if (animring == 8)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame8);
        else if (animring == 9)
          display2->drawFastImage(x + 25, y + 17, 15, 15, frame9);
    */
    //display2->drawXbm(x + 5, y + 17, 33, 15, ac_on);

  } else
    display2->drawXbm(x + 25, y + 17, 15, 15, ring_x);

  //display2->drawString(x + 64, y + 16, (compstat == 1 ? "A/C: ON"  : "A/C: OFF"));


  if (mode == 2)
    display2->drawRect(x + 42, y + 15, 45, 17 );
  display2->setTextAlignment(TEXT_ALIGN_CENTER);
  display2->drawXbm(x + 45, y + 17, 9, 15, temp_icon);
  display2->drawString(x + 69, y + 17, String(tempset, 1));


  if (mode == 3)
    display2->drawRect(x + 90, y + 15, 37, 17 );
  display2->setTextAlignment(TEXT_ALIGN_CENTER);

  if (fanset == 0)
    display2->drawXbm(x + 93, y + 20, 9, 9, fan_off);
  else
    display2->drawXbm(x + 93, y + 16, 9, 15, fan_icon);
  //display2->drawString(x + 110, y + 17, String(fanset));


  //fan bar
  int addx = 0;
  int heights = 3;
  int startx = 106;
  for (byte j = 0; j < 4 ; j++) {
    if (j + 1 <= fanset) {
      display2->drawVerticalLine(x + startx + j + addx, y + 30 - ((j + 1)*heights), (j + 1)*heights);
      display2->drawVerticalLine(x + (startx + 1) + j + addx, y + 30 - ((j + 1)*heights), (j + 1)*heights);
      display2->drawVerticalLine(x + (startx + 2) + j + addx, y + 30 - ((j + 1)*heights), (j + 1)*heights);
      addx += 4;
      //addx++;
      //addx++
    } else {
      display2->drawVerticalLine(x + (startx) + j + addx, y + 30 - 1, 1);
      display2->drawVerticalLine(x + (startx + 1) + j + addx, y + 30 - 1, 1);
      display2->drawVerticalLine(x + (startx + 2) + j + addx, y + 30 - 1, 1);
      addx += 4;
      //addx++;
    }

  }
  if (millis() - timeranim > 150 && freeze == 1) {
    timeranim = millis();
    animring++;
    if (animring > 7)
      animring = 0;
  }
}


FrameCallback frames4[] = { singleDisplay40, singleDisplay41, singleDisplay42, singleDisplay43};

int frameCount4 = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays4[] = { clockOverlay4 };
int overlaysCount4 = 1;



// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  // Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  }
  else {
    Serial.println("Delivery fail");
  }
}





createSafeStringReader(sfReader, 25, "#\r\n");

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  //Serial.println("Start");
  getSaved();

  //  pinMode(LED_BUILTIN, OUTPUT);

  //musician.setMelody(&melody);
  //musician.setBreath(1);            //milliseconds of silence at the end of notes
  //musician.setLoudnessLimit(0, 16); //Depending on your hardware
  //musician.setMelody(&melody);

  //musician.play();
  //musician.playSync(); //The instruction is blocked until the melody is finished.
  buzzeron = true;
  playBuzzer();
  //playMelody();
  /*
    bselect.attachClick(clickselect);
    bselect.attachDoubleClick(doubleclickselect);
    bselect.attachLongPressStart(longPressStartselect);
    bselect.attachLongPressStop(longPressStopselect);
    bselect.attachDuringLongPress(longPressselect);
  */

  bmode.attachClick(clickmode);
  bmode.attachDoubleClick(doubleclickmode);
  bmode.attachLongPressStart(longPressStartmode);
  bmode.attachLongPressStop(longPressStopmode);
  bmode.attachDuringLongPress(longPressmode);

  //pinMode(BUTTON_MODE, INPUT_PULLDOWN);
  //  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  // Initialising the UI will init the display too.
  display.init();
  display2.init();

  // This will make sure that multiple instances of a display driver
  // running on different ports will work together transparently
  display.setI2cAutoInit(true);
  display2.setI2cAutoInit(true);

  //display.flipScreenVertically();
  //display.setFont(ArialMT_Plain_10);
  //display.setTextAlignment(TEXT_ALIGN_LEFT);

  //display2.flipScreenVertically();
  display2.setFont(ArialMT_Plain_10);
  display2.setTextAlignment(TEXT_ALIGN_LEFT);


  //UI 1
  ui.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);
  //display.setI2cAutoInit(true);
  // Initialising the UI will init the display too.
  ui.init();
  //display.flipScreenVertically();


  //UI 2
  ui2.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui2.setActiveSymbol(activeSymbol);
  ui2.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui2.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui2.setIndicatorDirection(LEFT_RIGHT);
  // ui2.setIndicatorDirection(SLIDE_DOWN);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui2.setFrameAnimation(SLIDE_DOWN);

  // Add frames
  ui2.setFrames(frames2, frameCount2);

  // Add overlays
  ui2.setOverlays(overlays2, overlaysCount2);
  //display2.setI2cAutoInit(true);
  // Initialising the UI will init the display too.
  ui2.init();
  //ui2.disableAutoTransition();
  ui.disableAutoTransition();
  ui2.disableAutoTransition();



  //UI 3
  ui3.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui3.setActiveSymbol(activeSymbol);
  ui3.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui3.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui3.setIndicatorDirection(LEFT_RIGHT);
  // ui2.setIndicatorDirection(SLIDE_DOWN);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui3.setFrameAnimation(SLIDE_UP);

  // Add frames
  ui3.setFrames(frames3, frameCount3);

  // Add overlays
  ui3.setOverlays(overlays3, overlaysCount3);
  //display2.setI2cAutoInit(true);
  // Initialising the UI will init the display too.
  ui3.init();
  //ui2.disableAutoTransition();
  ui3.disableAutoTransition();




  //UI 4
  ui4.setTargetFPS(60);

  // Customize the active and inactive symbol
  ui4.setActiveSymbol(activeSymbol);
  ui4.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui4.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui4.setIndicatorDirection(LEFT_RIGHT);
  // ui2.setIndicatorDirection(SLIDE_DOWN);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui4.setFrameAnimation(SLIDE_UP);

  // Add frames
  ui4.setFrames(frames4, frameCount4);

  // Add overlays
  ui4.setOverlays(overlays4, overlaysCount4);
  //display2.setI2cAutoInit(true);
  // Initialising the UI will init the display too.
  ui4.init();
  //ui2.disableAutoTransition();
  ui4.disableAutoTransition();



  //display2.flipScreenVertically();
  pinMode(BUZZ, OUTPUT); // Set buzzer - pin 5 as an output

  display.flipScreenVertically();
  display2.flipScreenVertically();

  display.clear();
  display.drawXbm(25, 0, 76, 32, chery_logo);
  display.display();

  display2.clear();
  display2.drawXbm(0, 0, 128, 32, chery_text);
  display2.display();

  delay(2000);


  //main screen
  screen  = 1;

  sfReader.connect(Serial); // where SafeStringReader will read from

  //espnow receiver

  WiFi.setOutputPower(2);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  esp_now_register_send_cb(OnDataSent);


  // Register peer
  //  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, keyEncrypt, 16);

  esp_now_register_recv_cb(OnDataRecv);

}

String buff = "";
long timerupdate = 0;

String getStringPartByNr(SafeString &data, char separator, int index) {
  int stringData = 0;        //variable to count data part nr
  String dataPart = "";      //variable to hole the return text

  for (int i = 0; i < data.length() - 1; i++) { //Walk through the text one letter at a time
    if (data[i] == separator) {
      //Count the number of times separator character appears in the text
      stringData++;
    } else if (stringData == index) {
      //get the text when separator is the rignt one
      dataPart.concat(data[i]);
    } else if (stringData > index) {
      //return text and stop if the next separator appears - to save CPU-time
      return dataPart;
      break;
    }
  }
  //return text if this is the last part
  return dataPart;
}


long string_to_long (String number)
{
  number = number + ' ';
  char buf[number.length()];
  //Serial.println(number);
  number.toCharArray(buf, number.length());
  long result = atol(buf);
  //Serial.println(result);
  return result;
}

int updates = 0;



void processCommand(SafeString &cmd) {
  //String coms = cmd
  cmd.replace("#", "");
  String com = getStringPartByNr(cmd, ':', 0);
  //String values = getStringPartByNr(cmd, ':', 1 );

  int i = 0;
  float f = 0.0;


  //  values.replace("#", "");

  //Serial.print("#command ");
  //Serial.println(com);

  if ( com == "COOLANT") {
    //com.concat(":");
    cmd.replace("COOLANT:", "");
    //temp =
    cmd.toFloat(temp);
  } else if (com == "BAT") {
    //com.concat(":");
    cmd.replace("BAT:", "");
    //volt =
    cmd.toFloat(volt);
  } else if (com == "FAN") {
    //com.concat(":");
    cmd.replace("FAN:", "");
    if (updates == 1) return;

    if (updates == 0) {
      //fanset =
      cmd.toInt(fanset);
    } else {
      cmd.toInt(i);
      //if ( i == fanset && millis() - timerupdate > 2000) {
      //updates = 0;
      //}
    }
  } else if (com == "TEMP") {
    //com.concat(":");
    cmd.replace("TEMP:", "");
    //tempcurr =
    cmd.toFloat(tempcurr);
  } else if (com == "TEMPSET") {
    //com.concat(":");
    cmd.replace("TEMPSET:", "");

    if (updates == 1) return ;

    if (updates == 0) {
      cmd.toFloat(tempset);
    } else {
      cmd.toFloat(f);
      //if ( f == tempset && millis() - timerupdate > 2000) {
      //  updates = 0;
      // }
    }
  } else if (com == "FREEZE") {
    //com.concat(":");
    cmd.replace("FREEZE:", "");
    //freeze =
    cmd.toInt(freeze);
  } else if (com == "COMPSTAT") {
    //com.concat(":");
    cmd.replace("COMPSTAT:", "");
    //values = cmd.to

    //values =  cmd;

    if (updates == 0) {

      if (cmd == "1")
        compstat = 1;//
      else
        compstat = 0;
      //values.toInt();

      // Serial.print("#sudah update=");
      //Serial.print(compstat);
      // Serial.println("#");

    } else {
      //   Serial.print("jadi update=");
      // Serial.print(cmd);
      // Serial.println("#");
      cmd.toInt(i);
      //if (i == compstat && millis() - timerupdate > 2000 ) {
      //  updates = 0 ;
      //}
    }
  } else if (com == "DEFOG") {
    //com.concat(":");
    cmd.replace("DEFOG:", "");
    //defog =
    cmd.toInt(defog);
  } else if (com == "MODE1") {
    //com.concat(":");
    cmd.replace("MODE1:", "");
    //mode1 =
    cmd.toInt(mode1);
  } else if (com == "MODE2") {
    // com.concat(":");
    cmd.replace("MODE2:", "");
    //mode2 =
    cmd.toInt(mode2);
  } else if (com == "DATE") {
    //if (values.length() == 10)
    // dates = values;
  } else if (com == "TIME") {
    //if (values.length() == 8)
    // times = values;
  } else if (com == "SPD") {
    //speed = values;// string_to_long(values);
  } else if (com == "ODO") {
    //odo = values;//.toFloat();
  } else if (com = "SAT") {
    //satused = values;//.toInt();
    //if (satused < 0)
    // satused = 0;

  }

}

/*
  void listenSerial() {

  String datas = "";

  while (Serial.available()) {
    char character = Serial.read();
    buff.concat(character);
    if (character == '#') { // if end of message received
      datas = buff;
      buff = "";
      initserial++;
      if (initserial > 150) {
        initserial = 1;
        idleserial = 0;

      }
      //Serial.print(buff); //display message and
      processCommand(datas);
      //buff = ""; //clear buffer
      //Serial.println();
    }
  }

*/

void clickselect() {
  //Serial.println("click");
}
void doubleclickselect() {
  //Serial.println("doubleclick");
}
void longPressStartselect() {
  //Serial.println("LongPress Start");
}

void longPressStopselect() {
  // Serial.println("Longpress Stop");

}
void longPressselect() {
  //Serial.println("During long Press");
}

int submenu = 0;
int subscreen = 0;


void clickmode() {
  //Serial.println("click");
  display.normalDisplay();
  display2.normalDisplay();

  if (screen == 1) {
    if (multimode == 0) {
      multimode = 1;
      ui2.nextFrame();
      ui2.enableAutoTransition();
      ui.transitionToFrame(1);
    } else {
      multimode = 0;
      ui2.disableAutoTransition();
      ui2.transitionToFrame(0);
      ui.transitionToFrame(0);
    }
  } else if ( screen == 2) {
    if (mode == 0) {
      menu++;
      if ( menu < 1 )
        menu = countmenu;
      else if (menu > countmenu)
        menu = 1;

      ui3.nextFrame();
    } else if (mode == 1) {
      if (menu == 1) {
        tempwarn++;
        increase = true;
      } else if (menu == 2) {
        voltwarn += 1;
        increase = true;
      } else if (menu == 3) {
        adjtemp = adjtemp + 0.1;
        increase = true;
      } else if (menu == 4) {
        adjvolt = adjvolt + 0.1;
        increase = true;
      }
    }

  } else if (screen == 3) {
    if (submenu == 1) {

      updates = 1;
      if (mode == 1) {
        updates = 1;
        if (compstat == 0) {
          compstat = 1;
        } else {
          compstat = 0;
        }
        sendDataMonitor();
        //timerupdate = millis();
      } else if (mode == 2) {
        updates = 1;
        tempset += 0.5;

        //timerupdate = millis();
        sendDataMonitor();
      } else if (mode == 3) {
        updates = 1;
        fanset++;
        if (fanset > 4)
          fanset = 1;
        //timerupdate = millis();
        sendDataMonitor();
      }
    }
  }
}

void doubleclickmode() {

  if (screen == 2) {
    if (mode == 0) {
      mode = 1;
    } else if (mode == 1) {
      if (menu == 1) {
        tempwarn--;
        decrease = true;
      } else if (menu == 2) {
        voltwarn -= 1;
        decrease = true;
      }
      else if (menu == 3) {
        adjtemp = adjtemp - 0.1;
        decrease = true;
      } else if (menu == 4) {
        adjvolt = adjvolt - 0.1;
        decrease = true;
      }
    }
  } else if (screen == 1) {
    screen = 3;
    submenu = 1;
    multimode = 1;
    ui.transitionToFrame(1);

    mode = 1;
  } else if (screen == 3) {
    if (mode == 1) {
      if (submenu >= 1) {
        submenu++;
      }
      //subscreen = 1;
      ui4.nextFrame();

      if ( submenu > 4) {
        //ui4.transitionToFrame(0);
        screen = 1;
        screen = 1;
        multimode = 0;
        ui2.disableAutoTransition();
        ui2.transitionToFrame(0);
        ui.transitionToFrame(0);
      }
    } else {
      if (submenu == 1) {
        if (mode == 2) {
          updates = 1;
          tempset -= 0.5;
          if (tempset < 14)
            tempset = 14;
          //timerupdate = millis();
        } else if (mode == 3) {
          updates = 1;
          fanset--;
          if (fanset < 0)
            fanset = 0;

          if (fanset == 0)
            compstat = 0;
          //timerupdate = millis();
          // sendDataMonitor();
        }
        sendDataMonitor();
      }
    }
  } else if (screen == 4) {
    screen = 5;
  } else if (screen == 5) {
    screen = 1;
    multimode = 0;
    ui2.disableAutoTransition();
    ui2.transitionToFrame(0);
    ui.transitionToFrame(0);

  }


  // Serial.println("doubleclick");
}


void longPressStartmode() {
  display.normalDisplay();
  display2.normalDisplay();

  if (screen == 1) {
    if ( multimode == 0)
      ui.transitionToFrame(1);
    screen = 2;
    oldtempwarn = tempwarn;
    oldvoltwarn = voltwarn;
    oldadjtemp = adjtemp;
    oldadjvolt = adjvolt;
    menu = 1;
    ui3.transitionToFrame(0);
  } else if ( screen == 2) {
    if (mode == 0) {
      screen = 1;
      if (oldtempwarn != tempwarn || oldvoltwarn != voltwarn || oldadjtemp != adjtemp || oldadjvolt != adjvolt)
        saveConfig();

      if ( multimode == 0)
        ui.transitionToFrame(0);

    } else if (mode == 1) {
      mode = 0;

    }
  } else if (screen == 3) {
    if (submenu == 1) {
      mode++;

      if (mode > 3)
        mode = 1;

    } else if (submenu == 3) {
      resetodo = odo;
    }




  }
  //Serial.println("LongPress Start");
}

void longPressStopmode() {
  //Serial.println("Longpress Stop Mode");
}
void longPressmode() {


  // Serial.println("During long Press");
}


unsigned long timercollect = 0;

void collectDataGraph() {

  if (millis() - timercollect > 500) {

    timercollect = millis();

    if (atemp.size() >= 45)
      atemp.shift();

    float at = 0;
    at = (temp + adjtemp - 60) / (100 - 60) * 25;
    if (at < 0)
      at = 0;
    atemp.push(at);


    if (avolt.size() >= 45)
      avolt.shift();

    at = 0;
    at = (volt + adjvolt - 11) / (15 - 11) * 25;
    if (at < 0)
      at = 0;
    avolt.push(at);
  }
}

unsigned long timers = 0;
unsigned long timerb = 0;

void getAlarm() {
  int flag = 0;

  if (initserial > 0) {

    if (temp + adjtemp >= tempwarn ) {
      flag = 1;
    }

    if (volt + adjvolt <= voltwarn) {
      flag = 1;
    }
  }
  if (flag == 1)
    buzzeron = true;
  else
    buzzeron = false;

}



bool dataMonitor = false;
double timersend = 0;




void sendToMonitor(String com, String values) {

  //Serial.print("BT Status");
  //Serial.println(bt.available());
  //if (bt.available()){

  Serial.print(com);
  Serial.print(values);
  Serial.print("#");
  // }
}

bool checkSum(SafeString &msg) {
  size_t idxStar = msg.indexOf('*');
  // could do these checks also
  // BUT SafeString will just return empty sfCheckSumHex and so fail to hexToLong conversion below
  //  if (idxStar == msg.length()) {
  //    return false; // missing * //this also checks for empty string
  //  }
  //  // check for 2 chars
  //  if (((msg.length()-1) - idxStar) != 2) { // msg.length() -1 is the last idx of the msg
  //    return false; // too few or to many chars after *
  //  }
  cSF(sfCheckSumHex, 2);
  msg.substring(sfCheckSumHex, idxStar + 1); // next 2 chars SafeString will complain and return empty substring if more than 2 chars or idxStar+1 out of range
  long sum = 0;
  if (!sfCheckSumHex.hexToLong(sum)) {
    return false; // not a valid hex number
  }
}


void sendDataMonitor() {
  if (!dataMonitor ) {
    //Serial.println("Send data BT");
    dataMonitor = true;
    sendToMonitor("UPDATE:", String(updates));
    sendToMonitor("FAN:", String(fanset));
    //sendToBT(tfan, String(fanSet));
    //sendToMonitor("TEMP:", String(tempcurr, 1));
    //sendToBT(ttemp, String(tempCurr, 1));
    sendToMonitor("TEMPSET:", String(tempset, 1));
    sendToMonitor("COMPSTAT:", String(compstat));
    //sendToBT(ttempset, String(tempSet, 1));
    //sendToMonitor("FREEZE:", String(freeze));
    //sendToMonitor("DEFOG:", String(defogstat));
    //sendToMonitor("MODE1:", String(mode1));
    //sendToMonitor("MODE2:", String(mode2));
    //sendToMonitor("COOLANT:", String(temp, 1));

    //sendToMonitor("BAT:", String(bat, 1));

    dataMonitor = false;
    updates = 0;

  }
}



unsigned long lastTime = 0;
unsigned long timerDelay = 1000;  // send readings timer



void loop() {
  //  bselect.tick();
  bmode.tick();


  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    //strcpy(myData.Node, "MID Display");
/*
    rcvData.coollant = (temp + adjtemp);
    rcvData.volt = (volt + adjvolt);
    rcvData.fanst = fanset;
    rcvData.tempset = tempset;
    rcvData.acstat = compstat;
    rcvData.temp = tempcurr;
*/



  rcvData.coollant = 70.20;
    rcvData.volt = 12.48;
    rcvData.fanst = 1;
    rcvData.tempset = 18.21;
    rcvData.acstat = 1;
    rcvData.temp = 27.18;  
    /*

          Serial.print("collant: ");
        Serial.println(rcvData.coollant);
        Serial.print("FAN: ");
        Serial.println( rcvData.fanst);
        Serial.print("BAT: ");
        Serial.println(rcvData.volt);
        Serial.print("rcvData.tempset");
        Serial.println( rcvData.tempset );
        Serial.print("AC");
        Serial.println(  rcvData.acstat );
        Serial.println();
    */

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &rcvData, sizeof(rcvData));

    lastTime = millis();
  }


  if (millis() - timerwarn > 1000) {

    blinkwarn = !blinkwarn;
    timerwarn = millis();
    idleserial++;
  }

  if ( idleserial >= 7) {
    if (initserial < 5 && initserial > 0 ) {
      clearparam();
      initserial = 1;
    }
    idleserial = 0;
  }


  if (sfReader.read()) {

    //if (!checkSum(sfReader)) { // is the check sum OK

    // Serial.println(sfReader);
    processCommand(sfReader);
    // }


  }

  //listenSerial();





  //float vvolt = 0;
  //float vtemp = 0;

  //vvolt = volt + adjvolt;
  //vtemp = temp + adjtemp;

  //  temp = vtemp;
  //  volt = vvolt;

  // Serial.println("temp="+String(temp,1)+",volt="+String(volt,1)+",adjtemp="+String(adjtemp,1)+",adjvolt="+String(adjvolt,1));




  // UI 1 Display
  int remainingTimeBudget = ui.update();
  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);

  }

  if ( screen == 1 ) {
    //UI 2 display
    int remainingTimeBudget2 = ui2.update();
    if (remainingTimeBudget2 > 0) {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      delay(remainingTimeBudget2);

    }
  } else if (screen == 2) {
    //settings
    //UI 2 display
    int remainingTimeBudget3 = ui3.update();
    if (remainingTimeBudget3 > 0) {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      delay(remainingTimeBudget3);

    }
  } else if (screen == 3) {
    //settings
    //UI 3 display
    int remainingTimeBudget4 = ui4.update();
    if (remainingTimeBudget4 > 0) {
      // You can do some work here
      // Don't do stuff if you are below your
      // time budget.
      delay(remainingTimeBudget4);

    }
  }


  if (millis() - timers > 1000) {
    timers = millis();
    //Serial.print("Free Memory:");
    //Serial.println(ESP.getFreeHeap());
    //Serial.println("temp="+String(temp,1)+",volt="+String(volt,1)+",adjtemp="+String(adjtemp,1)+",adjvolt="+String(adjvolt,1));
    //Serial.println("ST,+000099.3  g");
  }


  //musician.refresh();

  if (millis() - timerb > 300)
  {

    playBuzzer();
    //musician.playSync(); //The instruction is blocked until the melody is finished.
    //playMelody();
    timerb = millis();
  }

  getAlarm();
  collectDataGraph();


  //if (millis() - timersend > 500 && updates==1) {
  //  timersend = millis();
  //   sendDataMonitor();
  //}

  /*
    //display.clear();
    display2.clear();

    //display.drawRect(80, 0, 45, 32);



    //display.setFont(ArialMT_Plain_10);
    //display.drawString(0, 0, "Coolant Temp:");

    //display.drawXbm(0, 0, 32, 29, tempicon);

    display2.setFont(ArialMT_Plain_10);
    display2.drawString(0, 0, "Battery Voltage :");

    //display.setFont(ArialMT_Plain_24);
    //display.drawString(0, 10, String(temp,1)+" C");

    display2.setFont(ArialMT_Plain_24);
    display2.drawString(0, 10, String(volt,1)+" V");

    //display.display();
    display2.display();

  */
}
