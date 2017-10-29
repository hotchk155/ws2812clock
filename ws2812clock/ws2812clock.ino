
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;


#define P_LED 8
#define P_SWITCH 10

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 9

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
delay(200);

  pinMode(P_LED, OUTPUT);
  pinMode(P_SWITCH, INPUT_PULLUP);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  Serial.begin(9600);
  Serial.println("Begin");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
     rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
//     rtc.adjust(DateTime(2017, 10, 14, 21, 00, 00));
  
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


int l2p(int l) {
  if(l<12) {
    return 11-l;
  }
  else {
    return 35-l;
  }
}


void wheel(byte p) {
  for(int i=0; i<24; ++i) {
    uint32_t col = Wheel(p);
    byte g = col>>16;
    byte r = col>>8;
    byte b = col;
    g/=8;
    r/=8;
    b/=8;
    col = ((uint32_t)g) << 16;
    col |= ((uint32_t)r) << 8;
    col |= ((uint32_t)b);
    strip.setPixelColor(i, col);
    p++;
  }
}



/*

//DUMMY GET TIME
int g_hh = 0;
int g_mm = 0;
int g_ss = 0;
int g_s10 = 0;
unsigned long nextTick = 0;
// hh is 0-11
void getTime(int *hh, int *mm, int *ss, int *s10) {
  unsigned long ms = millis();
  if(nextTick < ms || ms < (nextTick - 1000)) {
    if(++g_s10 >= 10) {
      g_s10 = 0;
      if(++g_ss >= 60) {
        g_ss = 0;
        if(++g_mm >= 60) {
          g_mm = 0;
          if(++g_hh >= 12) {
            g_hh = 0;
          }
        }
      }
    }
    nextTick = ms + 100;
  }
  *hh = g_hh;
  *mm = g_mm;
  *ss = g_ss;
  *s10 = g_s10;
}
void setTime(int hh, int mm, int ss) {
  g_hh = hh;
  g_mm = mm;
  g_ss = ss;
  g_s10 = 0;
}
*/


//RTC GET TIME
unsigned long nextTick = 0;
// hh is 0-11
int g_sec = 0;
int g_s10 = 0;
void getTime(int *hh, int *mm, int *ss, int *s10) {
  
  unsigned long ms = millis();
  DateTime now = rtc.now();  
  if(nextTick < ms || ms < (nextTick - 1000)) {
    
      if(g_sec != now.second()) {
        g_sec = now.second();
        g_s10 = 0;
      }
      else if(g_s10 < 10) {
        ++g_s10;
      }    
      nextTick = ms + 100;
  }
      *hh = now.hour() % 12;
      *mm = now.minute();
      *ss = g_sec;      
      *s10 = g_s10;      
}
void setTime(int hh, int mm, int ss) {
  rtc.adjust(DateTime(2014, 1, 1, hh, mm, ss));
  nextTick = 0; 
}
void incTime() {
  int hh, mm, ss, s10;
  getTime(&hh,&mm, &ss, &s10);
  if(++mm >= 60) {
    mm = 0;
    if(++hh >= 12) {
      hh = 0;
    }
  }
  setTime(hh,mm,0);
}


byte qq = 0;
byte lastms = 0;
int auto_repeat = 0;
void loop () {
    byte ms_tick = 0;
    if(lastms != (byte)millis()) {
      lastms = (byte)millis();
      ms_tick = 1;
    }
    int hh,mm,ss,s10,i;
    getTime(&hh,&mm,&ss,&s10);  
    digitalWrite(P_LED,s10<1);
    strip.clear();
    wheel((mm*3600 + mm*600 + ss * 10 + s10));
    
    int hh_pos = hh * 2 + mm/30;
    int mm_pos = mm * 24.0/60.0;
    
    strip.setPixelColor(l2p(mm_pos), strip.Color(200,255,0));
    strip.setPixelColor(l2p(hh_pos), strip.Color(200,200,200));
  
    strip.show();  
    if(digitalRead(P_SWITCH)) {
      auto_repeat = 0;
    }
    else if(!auto_repeat) {
      auto_repeat = 1;
      incTime();
    }
    else if(ms_tick) {
      if(++auto_repeat > 1000) {
        auto_repeat = 990;
        incTime();
      }
    }
}
