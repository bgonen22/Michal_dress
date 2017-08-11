#include <FastLED.h>

#define LED_PIN_1     6

#define START_SKIP_LEDS    0
#define NUM_LEDS    230
#define BRIGHTNESS  30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BUTTON_PIN 3

#define NUM_OF_ACTIVE_LEDS 6
int buttonState = 0;
uint8_t animationIndex = 1;

int UPDATES_PER_SECOND = 1000; //Speed of the app - Max is 1000

int animateSpeed = 450; //Num for each blinq animation
int INCRESE_BLINQ_SIZE = 5; //Blinq brightness increase
int INCRESE_BLINQ_GREEN_SIZE = 3; //Blinq brightness increase
int INCRESE_BLINQ_BLUE_SIZE = 2; //Blinq brightness increase

// Auto animate params
uint8_t autoAnimate = 0;
int lastAnimateSecond = 0;

// How much time should new sparks get created
static int MIN_BLINQ_INTERVAL = animateSpeed*3; //TODO: Right now we create 3 every time so
static int MAX_BLINQ_INTERVAL = 300;
//static int MIN_BLINQ_INTERVAL = 1000;
//static int MAX_BLINQ_INTERVAL = 2000;
int last_create_new_blinq_interval;
static unsigned long lastRefreshTime = 0;


CRGB leds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


DEFINE_GRADIENT_PALETTE( spring_gp ) {
    0, 255,  0,255,
   17, 255,  1,212,
   33, 255,  2,178,
   51, 255,  7,145,
   68, 255, 13,115,
   84, 255, 22, 92,
  102, 255, 33, 71,
  119, 255, 47, 52,
  135, 255, 62, 37,
  153, 255, 82, 25,
  170, 255,104, 15,
  186, 255,127,  9,
  204, 255,156,  4,
  221, 255,186,  1,
  237, 255,217,  1,
  255, 255,255,  0};

CRGBPalette16 DEFAULT_PALETTE = spring_gp;

// Gradient palette "bhw1_33_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_33.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_33_gp ) {
    0,   2,  1,  8,
   34,  79,  2,212,
  70, 110, 11,197,
  125,   2,  1,  8,
  150,   2,  1,  8,
  180,   2,  1,  8,
  200,   2,  1,  8,
  255,  79,  2,212};



void setup() {
    Serial.begin(9600);
    delay( 1000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN_1, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );

    currentPalette = DEFAULT_PALETTE;
    currentBlending = LINEARBLEND;

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    clearAll();
    Serial.println("Started");
    //selectRandomLed(); //TODO testing
}

int crazyMode = 0;
int create_new_blink_interval = MIN_BLINQ_INTERVAL; //TODO: always min
void loop()
{
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
       
    //Make sure we only call this every X steps - less then loop

    if(millis() - lastRefreshTime >= create_new_blink_interval)
    {
 //       Serial.println("Creating a new one");
        lastRefreshTime += create_new_blink_interval;
        selectRandomLed();
     }

    FastLED.show();
    //FastLED.delay(1000 / UPDATES_PER_SECOND);

  if(crazyMode == 0) {
       //Serial.println("moveThroughLedStates");
    moveThroughLedStates();
  }else {
    //Serial.println("FillLEDsFromPaletteColors");
    ChangePalettePeriodically();
    FillLEDsFromPaletteColors( startIndex);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  }

    //Read the button
    buttonState = digitalRead(BUTTON_PIN);
    // check if the pushbutton is pressed - change animation
    if (buttonState == LOW) {
      delay(500);
      clearAll();
      if(crazyMode == 0) {
        crazyMode = 1;
      }else {
        crazyMode = 0;
      }
    }

}


void clearAll() {
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}


byte selectedLeds[NUM_LEDS]; //has a 1 if led is selected. 0 otherwise
int ledStates[NUM_LEDS]; //holds the frame which the led is in - 1 - 50 or whatever the animation is
//TODO: check if those start at zero or need to be initialized


void selectRandomLed() {
  for (int i = 0;i<NUM_OF_ACTIVE_LEDS; ++i) {
    int ledToLightInGroup = random(0, NUM_LEDS);    
    selectedLeds[ledToLightInGroup] = 1;  
    //Serial.println(ledToLightInGroup);
  }    
}


//int halfWay = map((animateSpeed/2),0,animateSpeed,0,255);
void moveThroughLedStates() {
  int start = millis();
  //int animationPart = ((millis() / 256) % 256) //Count from 0 to 255
  for(int i=0;i<NUM_LEDS;i++) {
    if(selectedLeds[i] != 1) {
      continue;
    }
  //  Serial.print(i);
//    Serial.print(" ledStates ");
    //Serial.println(ledStates[i]);
    int animationPart = map(ledStates[i],0,animateSpeed,0,255); //map it to 256 space
    //Serial.print("animationPart");
    //Serial.println(animationPart);
    // Can load animation from pallete here.
    // Did it manually - Right now it is total of 20 animation
    if(ledStates[i] == 0) {
      //Serial.print("Starting a new state for ");
      //Serial.println(i);
      //Serial.print("start From ");
      //Serial.println(startFrom);
      leds[i] = CRGB::Black;
      //Serial.println("Starting");
    }else if(ledStates[i] < (animateSpeed/2)) {
      //Serial.println("Fading by half way ");
      //Serial.println(256-animationPart);
      leds[i] = CRGB(255,204,0).fadeLightBy( 255-animationPart );
      //Serial.println(255-animationPart);
      //leds[i]+= CRGB(INCRESE_BLINQ_SIZE,INCRESE_BLINQ_GREEN_SIZE, INCRESE_BLINQ_BLUE_SIZE);
      //leds[i] = CRGB(119, 255, 47).fadeLightBy( 256-ledStates[i] ); //Out of 256
      //leds[i].addToRGB(INCRESE_BLINQ_SIZE);
    //}else if(ledStates[i] > ((0.30)*animateSpeed/2)) {
      //leds[i]-= CRGB(INCRESE_BLINQ_SIZE,INCRESE_BLINQ_GREEN_SIZE, INCRESE_BLINQ_BLUE_SIZE);
      //leds[i].subtractFromRGB(INCRESE_BLINQ_SIZE);
    }else if(ledStates[i] < animateSpeed) { //The fading out should be longer the fading in
      //Serial.println("Fading by ");
      //Serial.println(animationPart);
      leds[i]= CRGB(255,204,0).fadeLightBy(animationPart); //Out of 256
      //Serial.println(animationPart);
      //Serial.print("Reducing blinq for ");
      //Serial.println(i);
      //leds[i].subtractFromRGB( INCRESE_BLINQ_SIZE);
      //leds[i]-= CRGB(INCRESE_BLINQ_SIZE,INCRESE_BLINQ_GREEN_SIZE, INCRESE_BLINQ_BLUE_SIZE);
    }

    ledStates[i]++;
    // Check if ended animation
    if(ledStates[i] >= animateSpeed) {
      //Serial.print("Ended animation for ");
      //Serial.println(i);
      leds[i] = CRGB::Black;
      selectedLeds[i] = 0;
      ledStates[i] = 0;
    }
  }
  //int final = millis();
  //Serial.print("move throgh states");
  //Serial.println(final-start);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    Serial.print (secondHand);
    Serial.print (" ");
    Serial.println (lastSecond);
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};



