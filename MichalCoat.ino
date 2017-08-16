#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PINL 6
#define PINR 5
#define RATIO 10 // how many rainbow for each colorWipe
#define DELAY 20 // delay for each colorWipe

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(70, PINL, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(25, PINR, NEO_GRB + NEO_KHZ800);



// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
   //This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  //#if defined (__AVR_ATtiny85__)
    //if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  //#endif
  // End of trinket special code

  strip.begin();
  strip.show();
 strip2.begin();
  strip2.show();  
  
}

int rainbow_t = 0;

void loop() {  
  draw(strip2.Color(150, 0, 0)); // Red  
  draw(strip2.Color(0, 150, 0)); // Green  
  draw(strip2.Color(0, 0, 150)); // Blue  

}


void draw (uint32_t c) {
  for(int i=0; i<strip2.numPixels(); i++) {
    colorWipe(c,i);
    for (int t= 0; t<RATIO; ++t) {
      rainbow_t = (rainbow_t + 1)%(256*5);
      rainbowCycle(rainbow_t);
    }
    delay(DELAY);
    
  }
  
 
}
void colorWipe(uint32_t c,int i) {   
    strip2.setPixelColor(i, c);
    strip2.show();
   
}
void rainbowCycle(int t) {
  uint16_t i;
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + t) & 255));
  }
  strip.show();  
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3)*0.3, 0, 0.3*(WheelPos * 3));

  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, 0.3 * (WheelPos * 3), 0.3 * (255 - WheelPos * 3));
  }
  
  WheelPos -= 170;
  return strip.Color( 0.3 * (WheelPos * 3), 0.3 * ( 255 - WheelPos * 3), 0);
  }


 
