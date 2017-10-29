
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

void xloop() {
  
  
  /*
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
  */
  
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
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

/*
Pixels numbered anticlockwise from "6" position

    /---/---/---/
      25 200 25
    
    24 * 5 = 120 "logical" positions
*/

void addRGB(int pos, byte rgb, byte val) {
  const int cDiv = 10;
  const int cMax = 24*cDiv;
  if(pos < 0) pos += cMax;
  if(pos >= cMax) pos -= cMax;
  pos/=cDiv;
  uint32_t col = strip.getPixelColor(pos);
  int c;
  switch(rgb)
  {
    case 0://R
        c = (byte)(col>>16);
        c += val;
        if(c > 255) c=255;
        col &= (uint32_t)0x00FFFF;
        col |= (c<<16);
        break;
    case 1://G
        c = (byte)(col>>8);
        c += val;
        if(c > 255) c=255;
        col &= (uint32_t)0xFF00FF;
        col |= (c<<8);
        break;
    case 2://B
        c = (byte)(col);
        c += val;
        if(c > 255) c=255;
        col &= (uint32_t)0xFFFF00;
        col |= c;
        break;
  }
  strip.setPixelColor(pos, col);
}
void showPixel(float pos, byte RGB) 
{
  int p = 240 - (pos * 10.0);
  addRGB(p-4, RGB, 1);
  addRGB(p-3, RGB, 2);
  addRGB(p-2, RGB, 5);
  addRGB(p-1, RGB, 10);
  addRGB(p,   RGB, 200);
  addRGB(p+1, RGB, 10);
  addRGB(p+2, RGB, 5);
  addRGB(p+3, RGB, 2);
  addRGB(p+4, RGB, 1);
  
  /*
  int index = ((int)(35.0 - pos)) % 24;
  uint32_t col = strip.getPixelColor(index);
  col |= r<<16;
  col |= g<<8;
  col |= b;
  strip.setPixelColor(index, col);
  */
}


void showTime(int h, int m, int s10)
{
  strip.clear();
  showPixel(2*h%12, 0);
  showPixel(m*24.0/60.0, 2);
  showPixel(s10*2.4/60.0, 1);
//  strip.setPixelColor(1, strip.Color(255,0,0));
//  strip.setPixelColor(23 - dt.minute()*24.0/60.0, strip.Color(0,255,0));
//  strip.setPixelColor(23 - dt.second()*24.0/60.0, strip.Color(0,0,255));
  strip.show();    
}

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

byte q = 0;
byte blink = 0;
byte sec = 255;
int s10=0;
void yloop () {
    DateTime now = rtc.now();  
    if(sec != now.second()) {
      sec = now.second();
      s10 = 10*sec;      
    }
    else {
      ++s10;
    }
    showTime(now.hour(),now.minute(),s10);
//  strip.clear();
//    showPixel(q,255,0,0);
//    if (++q>=24)q=0;
//  strip.show();    
    digitalWrite(P_LED, blink);
    blink = !blink;

    /*
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    Serial.println();*/
    delay(100);
}

typedef struct {
    byte r;
    byte g;
    byte b;
} POINT;

POINT points[24];

void clearPoints() {
  memset(points, 0, sizeof(points));
}


/*

*/
void updateStrip() {
  int l;
  int p;
  strip.clear();
  for(l=0; l<12; ++l) {
    p = 11-l;
    strip.setPixelColor(p, strip.Color(points[l].g, points[l].r, points[l].b));
  }
  for(; l<24; ++l) {
    p = 35-l;
    strip.setPixelColor(p, strip.Color(points[l].g, points[l].r, points[l].b));
  }
  strip.show();
}

int l2p(int l) {
  if(l<12) {
    return 11-l;
  }
  else {
    return 35-l;
  }
}

#define MAX_BRI 128
void point(float p) {
  
  /*
  for(int i=0; i<23; ++i) {
    int x = MAX_BRI - abs(MAX_BRI * (p-i))
    if(x > 0) {
      points[i].b = x;
    }
  }
  */
  
  
  
  points[0].r = 255;
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

unsigned int nextSec10 = 0;
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
    /*
    i=mm;
    if(i<0) i+= 24;
    strip.setPixelColor(l2p(i), 0);
    if(++i>23) i-=24;
    mm = i;
    if(++i>23) i-=24;
    strip.setPixelColor(l2p(i), 0);

    i=hh*2;
    if(i<0) i+= 24;
    strip.setPixelColor(l2p(i), 0);
    if(++i>23) i-=24;
    hh = i;
    if(++i>23) i-=24;
    strip.setPixelColor(l2p(i), 0);
*/
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
