#include <AddicoreRFID.h>
#include <SPI.h>
#include <RtcDS1302.h>
#include <Ds1302.h>
#include <ThreeWire.h>
#include "pitches.h"

#define	uchar	unsigned char
#define	uint	unsigned int

uchar fifobytes;
uchar fifoValue;

AddicoreRFID myRFID;

const int chipSelectPin = 10;
const int NRSTPD = 5;
const int buzzerPin = 8;
ThreeWire myWire(7, 6, 4);
RtcDS1302<ThreeWire> Rtc(myWire);

#define MAX_LEN 16

bool isPlaying = false;
int noteIndex = 0;

int melody[] = {
  REST, NOTE_D4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_C5, 
  NOTE_A4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_F4, NOTE_GS4,
  NOTE_D4, 
  NOTE_D4,
  
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_B4,
  NOTE_G4,
  NOTE_AS4,
   
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_DS5, NOTE_D5,
  NOTE_CS5, NOTE_A4,
  NOTE_AS4, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_D4,
  NOTE_D5, 
  REST, NOTE_AS4,  
  
  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_AS4,
  NOTE_G4
};

int durations[] = {
  2, 4,
  4, 8, 4,
  2, 4,
  2, 
  2,
  4, 8, 4,
  2, 4,
  1, 
  4,
  
  4, 8, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1,
  4,
   
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1, 
  4, 4,  
  
  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1
};


RtcDateTime lastTime;
void setup() {                
   Serial.begin(9600);                        
 
  SPI.begin();
  
  pinMode(chipSelectPin,OUTPUT);           
  digitalWrite(chipSelectPin, LOW);
  pinMode(NRSTPD,OUTPUT);
  digitalWrite(NRSTPD, HIGH);

  pinMode(buzzerPin, OUTPUT);

  myRFID.AddicoreRFID_Init();

  //clock module
  Rtc.Begin();


  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    Rtc.SetDateTime(compiled);
  }

  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  lastTime = now;
}

void loop()
{
    uchar status;
    uchar str[MAX_LEN];
    str[1] = 0x4400;
    status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);	
    if (status == MI_OK)
    {
        Serial.println("RFID tag detected");
        toggleSong(); //toggle song state
        delay(500);
    }

    if (isPlaying) {
        playSong(); //play song
    }
    myRFID.AddicoreRFID_Halt();


  //Clock module
  RtcDateTime now = Rtc.GetDateTime();

  if (now != lastTime) {
    printDateTime(now);
    lastTime = now;
  }

       
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];
  snprintf_P(datestring, 
      countof(datestring),
      PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
      dt.Month(),
      dt.Day(),
      dt.Year(),
      dt.Hour(),
      dt.Minute(),
      dt.Second());
  Serial.println(datestring);
}

void toggleSong() {
    if (isPlaying) {
        noTone(buzzerPin); //stop  song
        isPlaying = false;
    } else {
        isPlaying = true;
    }
}

void playSong() {
    if (noteIndex < sizeof(melody) / sizeof(int)) {
        int duration = 1000 / durations[noteIndex];
        tone(buzzerPin, melody[noteIndex], duration);

        int pauseBetweenNotes = duration * 1.30;
        delay(pauseBetweenNotes);

        noTone(buzzerPin);
        noteIndex++;
    } else {
        noteIndex = 0;
        isPlaying = false;
    }
}
