#include <Charliplexing.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

#define Mode_Helix       0
#define Mode_Wormhole    1
#define Mode_Pong        2 
#define Mode_Clock       3
#define Mode_Temperature 4
#define Mode_Random      5
#define Mode_Bounce      6
#define Mode_CrossFade   7

#define NumberofCyclingModes 8 // the moves above will be cycled through
#define Mode_Smile 9
#define Mode_Test 10

#define Start_Mode Mode_Smile

#define Mode_Random_Delay 2500 // how long to run this mode before switching to the next
#define Mode_Pong_Delay 15000
#define Mode_Pong_Speed 70 // how fast the ball moves - delay in ms between frames
#define Mode_Clock_Delay 5000
#define Mode_Temperature_Delay 5000
#define Mode_Smile_Delay 2000
#define Mode_Bounce_Delay 10000
#define Mode_Bounce_Speed 20// how fast the ball moves - delay in ms between frames
#define Mode_Bounce_Gravity +0.045
#define Mode_Bounce_Dispersion 0.8
#define Mode_Helix_Delay 15000
#define Mode_Helix_Speed 50//delay in ms between frames
#define Mode_Helix_Period 10
#define Mode_Helix_RotateSpeed 0.2
#define Mode_Helix_Spacing 2
#define Mode_Helix_NucleotideShade 1
#define Mode_Helix_BackShade 2
#define Mode_CrossFade_Speed 75
#define Mode_Wormhole_Delay 15000
#define Mode_Wormhole_Speed 2//delay in ms between frames
#define Mode_WormholeCenterXAmplitude 3
#define Mode_WormholeCenterYAmplitude 3
#define Mode_WormholeCentreSpeed 0.3
#define Mode_WormholeZoomSpeed 0.2
#define Mode_WormholeZoomAmplitude 1.2
#define Mode_WormholeZoomDefault 3.5 // more is further away


#define Pin_SpeakJetSerial 14 // A0 (A1 is for receive (not used)
#define Pin_Temperature A3  
#define Pin_LightLevel A2
#define LightLevel_Dawn 1000 // out of 1024
#define LightLevel_Sunset 200 // out of 1024
#define EEPROM_MAX 1024
#define LEDrows 14
#define LEDcols 9
#define LEDFullBrightness 7
#define LEDMediumBrightness 4
/*
 I2C pins for RTC = A5 to RTC Pin 6 and A4 to RTC Pin 5
 */

SoftwareSerial SerialSpeakjet =  SoftwareSerial(15, Pin_SpeakJetSerial);
RTC_DS1307 RTC;
DateTime now;
int Temperature;
boolean Night=0;
boolean ShowSecondIndicator=0;
int Mode=Start_Mode;
int fader=255;
//int temp=0;
unsigned long  modetimer=0; // in milliseconds, capable of timing up to 50 days or 7 weeks before rolling over.
unsigned long  stopwatch=0; // in milliseconds
boolean NewMode=1;
byte pongballX=0;signed int pongdirX=1;
byte pongballY=0;signed int pongdirY=1;
byte pongpaddle1=0; byte pongpaddle2=0;
byte pongpaddle1target=0; byte pongpaddle2target=0;
float bounceballX; float bounceballY; float bouncespeedX; float bouncespeedY;
float helixangle;
byte CrossFadeIndex;
float WormholeShadeIndex;
float WormholeCenterX;
float WormholeCenterY;
float WormholeCentreAngle;
float WormholeZoom;
float WormholeZoomAngle;



byte screenbuffer [LEDrows][LEDcols];

char Setup[] = {31, 20, 127, 21, 114, 22, 88, 23, 5,0};
char HelloMyNameisClonos[] = {183, 7, 159, 146, 164, 2, 140, 155, 141, 154, 140, 8, 129, 167, 195, 148, 164, 142, 135, 187,0};//};
char Pistol[] = {253,0}; // Pistol
char BigBen[]= {21,100,22, 82, 152, 22, 65, 152, 22, 73, 152, 22, 49, 152, 2, 22, 49, 152, 22, 73, 152, 22, 82, 152, 22, 65, 152,0};
char Beep[] = {223,3,0};
char TheTimeIs[] ={8, 169, 8, 128, 8, 191, 157, 8, 140, 8, 129, 167,1,0};
char TheTemperatureIs[] ={8, 169, 8, 128, 8,        191, 131, 7, 140, 7, 198, 148, 128, 182, 129, 148,    8, 129,8,167,1,0};
char Degrees[] = {15, 174, 129, 14, 178, 8, 148, 8, 128, 167,0};
char One[] = {147,14, 136, 8, 141,1,0};
char Two[] = {8, 191, 162,0};
char Three[] = {8, 190, 148, 8, 128,0};
char Four[] = {186, 7, 137, 153,0};
char Five[] = {186, 157, 166,0};
char Six[] = {8, 187, 129, 14, 194, 7, 187,0};
char Seven[] = {8, 187, 7, 131, 166, 131, 141,0};
char Eight[] = {154, 4, 191,0};
char Nine[] = {141, 14, 157, 141,0};
char Ten[] = {191, 131, 131, 141,0};
char Eleven[] = {7, 129, 145, 131, 166, 7, 131, 141,0};
char Twelve[] = {8, 191, 7, 147, 131, 145, 166,0};
char Thirteen[] = {8, 190, 7, 151, 191, 128, 141,0};
char Fourteen[] = {186, 7, 153, 7, 191, 128, 141,0};
char Fifteen[] = {186, 129, 186, 191, 128, 141,0};
char Sixteen[] = {8, 187, 7, 129, 14, 194, 187, 7, 191, 128, 141,0};
char Seventeen[] = {8, 187, 7, 129, 14, 194, 187, 7, 191, 128, 141,0};
char Eighteen[] = {154, 4, 191, 128, 141,0};
char Nineteen[] = {141, 7, 15, 155, 141, 191, 128, 8, 141,0};
char Twenty[] = {8, 7, 191, 7, 147, 131, 141, 7, 191, 128,0};
char Goodnight[] = {2,8, 179, 138, 138, 177, 141, 155, 191,0};
char Hello[] = {183, 7, 159, 146, 164,0};
char oClock [] = {8,137,8, 195, 7, 146, 8, 136, 197,0};
char setupok[] = {2,187, 130, 191, 133, 199, 2, 137, 194,8, 154,0};

//char Smile2[] = {255,0,0,36,102,0,0,129,195,102,60,0,0,0};
//char Smile2[] = {0,0,0,36,102,0,0,129,195,102,60,0,0,0};

// bitmap for each character. 0 encoded as 255 and terminating NULL
prog_uchar fontZero[] PROGMEM  = {14,10,10,10,140,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontOne[] PROGMEM  = {4,6,4,4,14,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontTwo[] PROGMEM  = {14,8,4,2,14,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontThree[] PROGMEM  = {14,8,12,8,14,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontFour[] PROGMEM  = {8,12,10,14,8,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontFive[] PROGMEM  = {14,2,14,8,14,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontSix[] PROGMEM  = {14,2,14,1255,14,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontSeven[] PROGMEM  = {14,8,8,8,8,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontEight[] PROGMEM  = {14,10,14,10,14,255,255,255,255,255,255,255,255,255,0};
prog_uchar fontNine[] PROGMEM  = {14,10,14,8,14,255,255,255,255,255,255,255,255,255,0};
prog_uchar Smile[]    PROGMEM =  {255,255,255,36,102,255,255,129,195,102,60,255,255,255,0};
 
PROGMEM const unsigned char *bitmap_table[] = {
  fontZero,
  fontOne,
  fontTwo,
  fontThree,
  fontFour,
  fontFive,
  fontSix,
  fontSeven,
  fontEight,
  fontNine,
  Smile
};
char buffer[LEDrows];

void setup(){
  //Setup LED array
  LedSign::Init(GRAYSCALE);  // Initializes the screen
  //Setup Speakjet
  pinMode(Pin_SpeakJetSerial,OUTPUT);
  SerialSpeakjet.begin(9600);
  //delay(1000);
  //SerialSpeakjet.print(HelloMyNameisClonos);//delay(3000);
  // Setup temperature and light sensors
  pinMode(Pin_Temperature,INPUT);pinMode(Pin_LightLevel,INPUT);
  analogReference(INTERNAL); // set volt ref to 1.1v
  // Setup RTC
  Serial.begin(57600);
  
  // Setup RTC via I2C interface
  /*
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__),16); // Set RTC date to match compile time and turn on 1HZ output for LED (bit 4 on the control register)
  if (RTC.isrunning()) {
    SerialSpeakjet.print(setupok);
  }
   */
   Night = IsNight();
   now=DateTime(__DATE__,__TIME__);
   //now = DateTime(__DATE__, "19:45");
  //SerialSpeakjet.print(BigBen);SerialSpeakjet.print(Setup);
  modetimer=millis(); // reset timer;
  //SerialSpeakjet.print(setupok);

  //EEPROM.read(0);
}



void loop(){
  
  Serial.println(analogRead(Pin_LightLevel));
  
  if (IsNight()&&!Night) { // Light has turned off off
    Night=1;
    //NextMode(Mode_Smile);
    SerialSpeakjet.print(Goodnight);
  } else
  if (!IsNight()&&Night) { // Light has turned on
    Night =0;
    //NextMode(Mode_Smile);
    SerialSpeakjet.print(Hello);
  }
  
  
  /* // Clock Cycle
  now = RTC.now();
  if (now.second()==0) { //on the minute
    Serial.print(now.hour());Serial.print(":");Serial.println(now.minute()); //Output time to serial port via USB
    //SerialSpeakjet.print(Beep);
    //SayTime();SayTemperature();
    delay(1000);
      if (now.minute()==00) { // on the hour
        SayTime();
        delay(1000);
        SayTemperature();
      }  
  }
  */
  if (Mode==Mode_Test) {
    /*SerialSpeakjet.print(setupok);
    delay(3000);
           for (int row=0;row<LEDrows;row++) {
         for (int col=0;col<LEDcols;col++) {
           LedSign::Set(row,col,random(2));
         }
       }
   */
   //Serial.println(analogRead(Pin_LightLevel));
  }
  
  if (Mode==Mode_Wormhole) {
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      stopwatch=millis(); //reset timer
      LedSign::Clear();
      NewMode=0; // next time skip setup for this mode, it's already done.
      WormholeShadeIndex=0;
      WormholeCenterX=4;WormholeCenterY=7;WormholeCentreAngle=0;WormholeZoomAngle=0;
      WormholeZoom=0;
    }
    if(millis()-modetimer>Mode_Wormhole_Delay) { // switch to next mode after a fixed delay
      NextMode((Mode+1)%NumberofCyclingModes);
      LedSign::Clear();
    }
    if (millis()-stopwatch>Mode_Wormhole_Speed) { // time for next frame
      stopwatch=millis(); //reset timer
      //LedSign::Clear();
       for (int row=0;row<LEDrows;row++) {
         for (int col=0;col<LEDcols;col++) {
           int distance=((int)(WormholeShadeIndex+sqrt(pow(row-(WormholeCenterY+Mode_WormholeCenterYAmplitude*sin(WormholeCentreAngle)),2)+pow(col-(WormholeCenterX+Mode_WormholeCenterXAmplitude*cos(WormholeCentreAngle)),2))*WormholeZoom))%15;
           LedSign::Set(row,col,distance<=7?distance:7-(distance-7));
         }
       }
      WormholeShadeIndex+=0.2;
      WormholeCentreAngle+=Mode_WormholeCentreSpeed;
      WormholeZoomAngle+=Mode_WormholeZoomSpeed;
      WormholeZoom=Mode_WormholeZoomDefault+Mode_WormholeZoomAmplitude*sin(WormholeZoomAngle);
    }
  } // Wormhole
  
  if (Mode==Mode_CrossFade) {
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      stopwatch=millis(); //reset timer
      LedSign::Clear();
      NewMode=0; // next time skip setup for this mode, it's already done.
      CrossFadeIndex=0;
    }
    if (millis()-stopwatch>Mode_CrossFade_Speed) {
      
      stopwatch=millis(); //reset timer
      delay(1); // make sure only one loop iteration happens each millisecond clock reset
      if (CrossFadeIndex++==14) {
        NextMode((Mode+1)%NumberofCyclingModes);
        LedSign::Clear();
      } else
       for (int row=0;row<LEDrows;row++) {
         for (int col=0;col<LEDcols;col++) {
           LedSign::Set(row,col,CrossFadeIndex<=7?CrossFadeIndex:7-(CrossFadeIndex-7));
         }
       }

    }
  } // Crossfade
    
  if (Mode==Mode_Helix) {
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      stopwatch=millis(); //reset timer
      LedSign::Clear();
      NewMode=0; // next time skip setup for this mode, it's already done.
      helixangle=0.0;
    }
    if(millis()-modetimer>Mode_Helix_Delay) { // switch to next mode after a fixed delay
      NextMode((Mode+1)%NumberofCyclingModes);
      LedSign::Clear();
    }
    if (millis()-stopwatch>Mode_Helix_Speed) {
      stopwatch=millis(); //reset timer
      delay(1); // make sure only one loop iteration happens each millisecond clock reset
      helixangle+=Mode_Helix_RotateSpeed;
      for (int row=0;row<LEDrows;row++) {
        char LED1 = (int)((sin(helixangle+(float)row/(float)LEDrows*6.28)/2.0+0.5)*LEDcols*0.9+0.5);
        char LED2 = (int)((sin(Mode_Helix_Spacing+helixangle+(float)row/(float)LEDrows*6.28)/2.0+0.5)*LEDcols*0.9+0.5);
        
        char Brightness1;char Brightness2;
        if (fmod(helixangle+(float)row/(float)LEDrows*6.28+1.5,6.28)<3.14) {Brightness1=LEDFullBrightness;} else {Brightness1=Mode_Helix_BackShade;}
        if (fmod(Mode_Helix_Spacing+helixangle+(float)row/(float)LEDrows*6.28+1.5,6.28)<3.14) {Brightness2=LEDFullBrightness;} else {Brightness2=Mode_Helix_BackShade;}
        for (int col=0;col<LEDcols;col++) {
          if ((((col<LED2)&&(col>LED1))||((col<LED1)&&(col>LED2)))&&(((int)(row))%3==0)) { // nucleotide
            LedSign::Set(row,col,Mode_Helix_NucleotideShade);// not at brightest
          } else LedSign::Set(row,col,LED1==col?Brightness1:LED2==col?Brightness2:0);
        }       
      }
    }
  } // Mode_Helix
  
  if (Mode==Mode_Bounce) {
    //Serial.println("smile mode");
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      stopwatch=millis(); //reset timer
      LedSign::Clear();
      NewMode=0; // next time skip setup for this mode, it's already done.
      bounceballX=3.0;
      bounceballY=1.0;
      bouncespeedX=0.5;
      bouncespeedY=0.0;
    }
    if(millis()-modetimer>Mode_Bounce_Delay) {
      NextMode((Mode+1)%NumberofCyclingModes);
      LedSign::Clear();
    }
    if (millis()-stopwatch>Mode_Bounce_Speed) {
      stopwatch=millis(); //reset timer
      delay(1); // make sure only one loop iteration happens each millisecond clock reset
      LedSign::Clear();
      bounceballX+=bouncespeedX;
      bounceballY+=bouncespeedY;
      bouncespeedY+=Mode_Bounce_Gravity;
      if (bounceballX>LEDcols-1) {bounceballX=(LEDcols-1)-(bounceballX-(LEDcols-1));bouncespeedX*=-Mode_Bounce_Dispersion;}
      if (bounceballY>LEDrows-1) {bounceballY=(LEDrows-1)-(bounceballY-(LEDrows-1));bouncespeedY*=-Mode_Bounce_Dispersion;if(bouncespeedY*(bouncespeedY)<0.2){bouncespeedY=-0.9;bouncespeedX=0.5;}}
      if (bounceballX<0) {bounceballX=-bounceballX;bouncespeedX*=-Mode_Bounce_Dispersion;}
      if (bounceballY<0) {bounceballY=-bounceballY;bouncespeedY*=-Mode_Bounce_Dispersion;}
//      Serial.print(bounceballY,4);Serial.print(",");Serial.print((int)(bounceballY+0.5));Serial.print(",");Serial.println(bouncespeedY,4);
      LedSign::Set((int)(bounceballY+0.5),(int)(bounceballX+0.5),LEDFullBrightness);
    }
  } // Mode_Bounce
  
  if (Mode==Mode_Smile) {
    //Serial.println("smile mode");
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      //LedSign::Clear();
      NewMode=0; // next time skip setup for this mode, it's already done.
      loadFont(10);
      LEDDrawArray(buffer,0,0);
    }
    if(millis()-modetimer>Mode_Smile_Delay) {
       NextMode(0); // start mode cycling
    }
  } // Mode_Smile

  if (Mode==Mode_Random) {
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      NewMode=0; // next time skip setup for this mode, it's already done.
    }
    if(millis()-modetimer>Mode_Random_Delay) {
      NextMode((Mode+1)%NumberofCyclingModes);
    }
    for (int rows=0;rows<LEDrows;rows++) {
     for (int col=0;col<LEDcols;col++) {
      LedSign::Set(rows,col,random(8));
      }
    }
  } // Mode_Random
  if (Mode==Mode_Temperature) {
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      stopwatch=millis(); //reset timer
      NewMode=0; // next time skip setup for this mode, it's already done.
    }
    if(millis()-modetimer>Mode_Temperature_Delay) {
      NextMode((Mode+1)%NumberofCyclingModes);
    }
    if (millis()-stopwatch>500) { // update every half second
       stopwatch=millis(); //reset timer
      Temperature = analogRead(Pin_Temperature)/9.30; //9.3 = 1024/1.1, analog pin is out of 1024 values, with a volt reference of 1.1v. sensor output is celsius in 10x mV
      ShowTemperature();
    }
   
  } // Mode_Temperature
  if (Mode==Mode_Clock) {
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      stopwatch=millis(); //reset timer
      NewMode=0; // next time skip setup for this mode, it's already done.
      ShowTime();
    }
    if(millis()-modetimer>Mode_Clock_Delay) {
      NextMode((Mode+1)%NumberofCyclingModes);
    }
    if (millis()-stopwatch>500) { // flash every second
       stopwatch=millis(); //reset timer
      ShowSecondIndicator=1-ShowSecondIndicator; // flip the flashing second indicator
      ShowTime();
      LedSign::Set(6,4,ShowSecondIndicator*LEDFullBrightness);
    }
   
  } // Mode_Clock
 
  if (Mode==Mode_Pong) {
    if (NewMode) { // setup routine for this mode
      modetimer=millis(); //reset timer
      LedSign::Clear();
      NewMode=0; // next time skip setup for this mode, it's already done.
      pongballX=0;pongdirX=1;
      pongballY=0;pongdirY=1;
      pongpaddle1=0; pongpaddle2=0;
      pongpaddle1target=pongballX;
    }

    if ((millis()%Mode_Pong_Speed)==0) {
      delay(1); // make sure only one loop iteration happens each millisecond clock reset
      LedSign::Clear();
      if ((pongballX==0)||(pongballX==8 )) {pongdirX=-pongdirX;} // bounce against wall if at the boundary
      if ((pongballY==0)||(pongballY==13)) {pongdirY=-pongdirY;} //201=high beeep;200=lower beep; 227=very high very short tap
      if (pongballY==0) { // set new target for paddle 1 to where ball will be
        pongpaddle1target=pongdirX==-1?(pongballX<=2?pongballX+6:10-pongballX):(pongballX<=6?6-pongballX:pongballX-6); // formula for predicting where ball will land next
      }
      if (pongballY==13) { // set new target for paddle 2
        pongpaddle2target=pongdirX==-1?(pongballX<=2?pongballX+6:10-pongballX):(pongballX<=6?6-pongballX:pongballX-6);
      }
      
      pongballX=(pongballX+pongdirX)%LEDcols;
      pongballY=(pongballY+pongdirY)%LEDrows;
      LedSign::Set(pongballY,pongballX,LEDMediumBrightness); // draw ball at new location
      
      // move paddles:
      if (pongpaddle1<pongpaddle1target) {pongpaddle1++;} // move paddle towards its target
      if (pongpaddle1>pongpaddle1target) {pongpaddle1--;}
      if (millis()/Mode_Pong_Speed%4==0) { // player 2 is fat and slow
        if (pongpaddle2<pongpaddle2target) {pongpaddle2++;}
        if (pongpaddle2>pongpaddle2target) {pongpaddle2--;}
      }
      // draw paddles:
      LedSign::Set(0,pongpaddle1,LEDFullBrightness);
      if (pongpaddle1<8) {LedSign::Set(0,pongpaddle1+1,LEDFullBrightness);} else{LedSign::Set(0,pongpaddle1-1,LEDFullBrightness);} // print extra paddle pixel
      LedSign::Set(13,pongpaddle2,LEDFullBrightness);
      if (pongpaddle2<8) {LedSign::Set(13,pongpaddle2+1,LEDFullBrightness);} else{LedSign::Set(13,pongpaddle2-1,LEDFullBrightness);} // print extra paddle pixel~

    }
    if(millis()-modetimer>Mode_Pong_Delay) {
      NextMode((Mode+1)%NumberofCyclingModes);
      LedSign::Clear();
    }
  } // Mode_Pong 
    
};

void NextMode(int nextmode) {Mode=nextmode;NewMode=1;}

void LEDDrawArray(char bitmap[], byte offsetrow, byte offsetcol){
  for (int rows=0;rows<LEDrows;rows++) {
    unsigned char line=bitmap[rows];
    Serial.println(line,DEC);
    if (line==255) {line=0;} // show 0s = empty line to get around the null termination problem.
    for (int col=0;col<LEDcols;col++) {
      if ((line) & (1<<(col))) {LedSign::Set(rows+offsetrow,(LEDcols-1)-(col+offsetcol),LEDFullBrightness);};
    }
  }
}

boolean IsNight() {
  if (Night) {
    if (analogRead(Pin_LightLevel)>LightLevel_Dawn) {return 0;} else {return 1;}
  } else {
    if (analogRead(Pin_LightLevel)<LightLevel_Sunset) {return 1;} else {return 0;}
  }
}

void SayTemperature() {
  SerialSpeakjet.print(TheTemperatureIs);
  if (Temperature==10) {SerialSpeakjet.print(Ten);}
  if (Temperature==11) {SerialSpeakjet.print(Eleven);}
  if (Temperature==12) {SerialSpeakjet.print(Twelve);}
  if (Temperature==13) {SerialSpeakjet.print(Thirteen);}
  if (Temperature==14) {SerialSpeakjet.print(Fourteen);}
  if (Temperature==15) {SerialSpeakjet.print(Fifteen);}
  if (Temperature==16) {SerialSpeakjet.print(Sixteen);}
  if (Temperature==17) {SerialSpeakjet.print(Seventeen);}
  if (Temperature==18) {SerialSpeakjet.print(Eighteen);}
  if (Temperature==19) {SerialSpeakjet.print(Nineteen);}
  if (Temperature==20) {SerialSpeakjet.print(Twenty);}
  if (Temperature==21) {SerialSpeakjet.print(Twenty);SerialSpeakjet.print(One);}
  if (Temperature==22) {SerialSpeakjet.print(Twenty);SerialSpeakjet.print(Two);}
  if (Temperature==23) {SerialSpeakjet.print(Twenty);SerialSpeakjet.print(Three);}
  if (Temperature==24) {SerialSpeakjet.print(Twenty);SerialSpeakjet.print(Four);}
  if (Temperature==25) {SerialSpeakjet.print(Twenty);SerialSpeakjet.print(Five);}
  SerialSpeakjet.print(Degrees);
  delay(2000);
}

void SayTime() {
  SerialSpeakjet.print(TheTimeIs);
  if (now.hour()%12==1) {SerialSpeakjet.print(One);}
  if (now.hour()%12==2) {SerialSpeakjet.print(Two);}
  if (now.hour()%12==3) {SerialSpeakjet.print(Three);}
  if (now.hour()%12==4) {SerialSpeakjet.print(Four);}
  if (now.hour()%12==5) {SerialSpeakjet.print(Five);}
  if (now.hour()%12==6) {SerialSpeakjet.print(Six);}
  if (now.hour()%12==7) {SerialSpeakjet.print(Seven);}
  if (now.hour()%12==8) {SerialSpeakjet.print(Eight);}
  if (now.hour()%12==9) {SerialSpeakjet.print(Nine);}
  if (now.hour()%12==10) {SerialSpeakjet.print(Ten);}
  if (now.hour()%12==11) {SerialSpeakjet.print(Eleven);}
  if (now.hour()%12==0) {SerialSpeakjet.print(Twelve);}
  SerialSpeakjet.print(oClock);
}

void ShowTime() {
  LedSign::Clear();
  if (now.hour()%12<10) { // show only 1 digit
    loadFont(now.hour()%12);
    LEDDrawArray(buffer,0,2);
  } else { //time is 10 or 11
    loadFont(now.hour()/10);
    LEDDrawArray(buffer,0,0);
    loadFont(now.hour()%10);
    LEDDrawArray(buffer,0,4);
  }
  loadFont(now.minute()/10);
  LEDDrawArray(buffer,8,0);
  loadFont(now.minute()%10);
  LEDDrawArray(buffer,8,4);
}

void ShowTemperature() {
  LedSign::Clear();
  loadFont(Temperature/10);
  LEDDrawArray(buffer,0,-1);
  loadFont(Temperature%10);
  LEDDrawArray(buffer,0,3);
  LedSign::Set(0,0,1); // degree symbol
}

void loadFont(byte fontindex) {
  strcpy_P(buffer, (char*)pgm_read_word(&(bitmap_table[fontindex]))); // Load font into buffer from progmem
}

void EEPROMClearMemory() {
  for (int addr=0;addr<EEPROM_MAX;addr++) {EEPROM.write(addr,0);} //overwrite all memory with 0s
}

void EEPROMWriteLong(int p_address, long p_value) // Long = 4 bytes
{
  byte Byte1 = ((p_value >> 0) & 0xFF);
  byte Byte2 = ((p_value >> 8) & 0xFF);
  byte Byte3 = ((p_value >> 16) & 0xFF);
  byte Byte4 = ((p_value >> 24) & 0xFF);

  EEPROM.write(p_address, Byte1);
  EEPROM.write(p_address + 1, Byte2);
  EEPROM.write(p_address + 2, Byte3);
  EEPROM.write(p_address + 3, Byte4);
}

long EEPROMReadLong(int p_address)
{
  byte Byte1 = EEPROM.read(p_address);
  byte Byte2 = EEPROM.read(p_address + 1);
  byte Byte3 = EEPROM.read(p_address + 2);
  byte Byte4 = EEPROM.read(p_address + 3);

  long firstTwoBytes = ((Byte1 << 0) & 0xFF) + ((Byte2 << 8) & 0xFF00);
  long secondTwoBytes = (((Byte3 << 0) & 0xFF) + ((Byte4 << 8) & 0xFF00));
  secondTwoBytes *= 65536; // multiply by 2 to power 16 - bit shift 24 to the left

  return (firstTwoBytes + secondTwoBytes);
}
