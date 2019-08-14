#include "fish_new.h"
#include "MCUFRIEND_kbv.h"

#define FLOWSENSORPIN SCL

MCUFRIEND_kbv tft;

#define LOWFLASH (defined(__AVR_ATmega328P__) && defined(MCUFRIEND_KBV_H_))

// COLORS 
#define WATER   0x9162
#define SAND    0x2C9A
#define GREEN   0x25A7 ^ 0xFFFF
#define GROSS   0x2A5C
#define DEEP    0x1351 ^ 0xFFFF

// FLOW RATE DEFS
// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is called once a millisecond, looks for any pulses from the sensor!

//volatile float current_water;

SIGNAL(TIMER0_COMPA_vect) {
  uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }
  
  if (x == HIGH) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}

float checkLiters() {
    // ~0.28 per container
    // with error use 0.26 for each state transition
    //used for testing graphics:
    //pulses = pulses+2;
    float liters = pulses;
    liters /= 7.5;
    //money conversion
    liters /= 60.0;
    Serial.print(liters); Serial.println(" Liters");
    return liters;
}

void setup() {
//SETUP FLOW SENSOR
   Serial.begin(9600);
   pinMode(FLOWSENSORPIN, INPUT);
   digitalWrite(FLOWSENSORPIN, HIGH);
   lastflowpinstate = digitalRead(FLOWSENSORPIN);
   useInterrupt(true);

//SETUP LCD SCREEN
  uint16_t ID = tft.readID();
  tft.begin(ID);
  //make landscape
  tft.setRotation(3);
  //water and sand
  tft.fillScreen(WATER);
  tft.fillRect(0,270,480,200,SAND);
//setup text
  tft.setCursor(300, 270);
  tft.setTextColor(0x0000, SAND);
  tft.setTextSize(4);
  
  //labelled corners for adding visuals
  tft.drawChar(0,0,'0',0x0000,0xFFFF,2);
  tft.drawChar(470,0,'4',0x0000,0xFFFF,2);
  tft.drawChar(0,305,'3',0x0000,0xFFFF,2);
  tft.drawChar(470,300,'+',0x0000,0xFFFF,2);

  //test for visuals
  //void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
  // fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
  //tft.fillCircle(320, 270, 30, 0x2A5C); //gross green
    tft.fillCircle(470, 270, 30, 0x25A7 ^ 0xFFFF);
    tft.fillCircle(490, 240, 30, 0x25A7 ^ 0xFFFF);
    tft.fillCircle(470, 210, 30, 0x25A7 ^ 0xFFFF);
    tft.fillCircle(490, 170, 30, 0x25A7 ^ 0xFFFF);
    tft.fillCircle(0, 40, 30, 0x25A7 ^ 0xFFFF);
    tft.fillCircle(20, 10, 30, 0x25A7 ^ 0xFFFF);
    tft.fillCircle(30, 30, 30, 0x25A7 ^ 0xFFFF);
    tft.fillCircle(0, 50, 30, 0x25A7 ^ 0xFFFF);

}
void loop() {
  //screen size : 320 x 480
    int x, y;
    int w;
    //GREAT ZONE
    //////////////////////////////////////////
    if (checkLiters() < 0.88)
    {
     //snail
    tft.fillCircle(150, 270, 30, 0xFC10 ^ 0xFFFF);
    tft.drawCircle(150, 270, 20, 0xD228 ^ 0xFFFF);
    tft.drawCircle(150, 270, 10, 0xD228 ^ 0xFFFF);
    tft.fillCircle(180, 280, 10, 0xFC10 ^ 0xFFFF);
    tft.drawLine(180,280,200,250,0xFC10 ^ 0xFFFF);
    tft.drawLine(180,280,200,260,0xFC10 ^ 0xFFFF);
    for(w=0;w<500;w++) //second fish must go off screen
    {
      float current_liters = checkLiters();
      //PRINT MONEY ////////////////////////////////
      tft.setCursor(300, 270);
      tft.print("-$");
      tft.print(current_liters,2);
      //DRAIN OCEAN ////////////////////////////////
      tft.fillRect(0,0,480,0+current_liters*200,DEEP);
      if (current_liters > 0.88)
        break;
      tft.drawRGBBitmap(w, 100+(0.4)*w+(-3*(w%2)), fish2, 88, 56);
      tft.fillRect(w+45, 95+(0.25)*w+(-3*(w%2)), 5,5, WATER);
      //second fish
      tft.drawRGBBitmap(w-100, 150, fish2, 88, 56);
      //tft.drawCircle(w+88, 90+(0.5)*w+(-3*(w%2)), random(5,10), 0x0000);
      
      //fish speed
      delay(70); // smaller dely -> faster fish
      tft.fillRect(w+78, 80, 21,21, WATER);
    }
    }
    /*
    //NORMAL ZONE
    //////////////////////////////////////////
    }else if (checkLiters() < 0.52)
    {
    for(w=0;w<480;w++)
    {
      float current_liters = checkLiters();
      //PRINT MONEY ////////////////////////////////
      tft.setCursor(300, 270);
      tft.print("-$");
      tft.print(current_liters,2);
      //DRAIN OCEAN ////////////////////////////////
      tft.fillRect(0,0,480,0+current_liters*200,DEEP);
      if (current_liters > 0.52)
        break;
      tft.drawRGBBitmap(w, 100, fish2, 88, 56);
      tft.drawCircle(w+88, 90, random(5,10), 0x0000);
      
      //fish speed
      delay(90);
      tft.fillRect(w+78, 80, 21,21, WATER);
    }
    //BAD ZONE
    ////////////////////////////////////////////
    }else if (checkLiters() < 0.88)
    {
    for(w=0;w<480;w++)
    {
       //dead snail
    tft.fillCircle(150, 270, 30, WATER);
    tft.fillCircle(180, 280, 10, WATER);
      //shitty plants
      tft.fillCircle(470, 270, 30, GROSS);
      tft.fillCircle(490, 240, 30, GROSS);
      tft.fillCircle(470, 210, 30, GROSS);
      tft.fillCircle(490, 170, 30, GROSS);
      tft.fillCircle(0, 40, 30, GROSS);
      tft.fillCircle(20, 10, 30, GROSS);
      tft.fillCircle(30, 30, 30, GROSS);
      tft.fillCircle(0, 50, 30, GROSS);
      float current_liters = checkLiters();
      //PRINT MONEY ////////////////////////////////
      tft.setCursor(300, 270);
      tft.print("-$");
      tft.print(current_liters,2);
      //DRAIN OCEAN ////////////////////////////////
      tft.fillRect(0,0,480,0+current_liters*200,DEEP);
      if (current_liters > 0.88)
        break;
      tft.drawRGBBitmap(w, 100, fish2, 88, 56);
      //bubble
      tft.drawCircle(w+88, 90, random(5,10), 0x0000);
      
      //fish speed
      delay(100);
      //refill bubble
      tft.fillRect(w+78, 80, 21,21, WATER);
    }
    */
    else{
    //setup text
    tft.setCursor(500, 20);
    tft.setTextColor(0x0000, DEEP);
    tft.setTextSize(6);
    tft.println("  GAME OVER");
    }

    tft.fillRect(0,270,480,200,SAND);
}
