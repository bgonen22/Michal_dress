#include <FastLED.h>

#define LED_PIN_1     6

#define START_SKIP_LEDS    0
#define NUM_LEDS    200
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

int ledPerGroup = 1;
int groups[] = {32,37,43, 51, 33}; //TODO Should be 60+
int groupsSize = 15;

CRGB leds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

// Gradient palette "bhw1_04_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_04.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_04_gp ) {
    0, 229,227,  1,
   15, 227,101,  3,
  142,  40,  1, 80,
  198,  17,  1, 79,
  255,   0,  0, 45};



  // Gradient palette "bhw1_28_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_28.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_28_gp ) {
    0,  75,  1,221,
   30, 252, 73,255,
   48, 169,  0,242,
  119,   0,149,242,
  170,  43,  0,242,
  206, 252, 73,255,
  232,  78, 12,214,
  255,   0,149,242};

// Gradient palette "bhw2_turq_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_turq.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw2_turq_gp ) {
    0,   1, 33, 95,
   38,   1,107, 37,
   76,  42,255, 45,
  127, 255,255, 45,
  178,  42,255, 45,
  216,   1,107, 37,
  255,   1, 33, 95};


DEFINE_GRADIENT_PALETTE( bhw2_38_gp ) {
    0, 104,205,212,
   28,  32,184,203,
   61,  39, 39,168,
   96,  46,  1,144,
  130,   8,  1, 22,
  163,  17,  3, 28,
  196,  24,  4, 31,
  255, 213,  9, 89};

// Gradient palette "bhw3_61_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw3/tn/bhw3_61.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

//DEFINE_GRADIENT_PALETTE( bhw3_61_gp ) {
//    0,  14,  1, 27,
//   48,  17,  1, 88,
//  104,   1, 88,156,
//  160,   1, 54, 42,
//  219,   9,235, 52,
//  255, 139,235,233};



// Gradient palette "bhw1_15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.
//
//DEFINE_GRADIENT_PALETTE( bhw1_15_gp ) {
//    0,   1,  8, 87,
//   71,  23,195,130,
//  122, 186,248,233,
//  168,  23,195,130,
//  255,   1,  8, 87};


// Gradient palette "fruit_03_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ma/fruit/tn/fruit_03.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.

//DEFINE_GRADIENT_PALETTE( fruit_03_gp ) {
//    
//0,   2, 25,  3,
//0,   2, 25,  3,
//  10, 184, 22, 29,
//  10, 184, 22, 29,
//  20,   2, 25,  3,
//20,   2, 25,  3,
//30, 184, 22, 29,
//  30, 184, 22, 29,
//40,   2, 25,  3,
//40,   2, 25,  3,
//  50, 184, 22, 29,
//  50, 184, 22, 29,
//  150,   2, 25,  3,
//150,   2, 25,  3,
//180, 184, 22, 29,
//  180, 184, 22, 29,
//  220,   2, 25,  3,
//220,   2, 25,  3,
//255, 184, 22, 29,
//  255, 184, 22, 29,
//};


// Gradient palette "spring_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/h5/tn/spring.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 64 bytes of program space.

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



// Gradient palette "bhw2_xc_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw2/tn/bhw2_xc.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw2_xc_gp ) {
    0,   4,  2,  9,
   58,  16,  0, 47,
  122,  24,  0, 16,
  158, 144,  9,  1,
  183, 179, 45,  1,
  219, 220,114,  2,
  255, 234,237,  1};


// Gradient palette "rainbow_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/rc/tn/rainbow.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 48 bytes of program space.

DEFINE_GRADIENT_PALETTE( rainbow_gp ) {
    0, 126,  1,142,
   25, 171,  1, 26,
   48, 224,  9,  1,
   71, 237,138,  1,
   94,  52,173,  1,
  117,   1,201,  1,
  140,   1,211, 54,
  163,   1,124,168,
  186,   1,  8,149,
  209,  12,  1,151,
  232,  12,  1,151,
  255, 171,  1,190};

// Gradient palette "GeeK07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/heine/tn/GeeK07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 2320 bytes of program space.

DEFINE_GRADIENT_PALETTE( GeeK07_gp ) {
    0, 255,255,255,
    0, 255,255,255,
    10,   0,  0,  0,
    10,   0,  0,  0,
    20, 255,255,255,
    20, 255,255,255,
    30,   0,  0,  0,
    30,   0,  0,  0,
    40, 255,255,255,
    40, 255,255,255,
    50,   0,  0,  0,
    50,   0,  0,  0,
    150, 255,255,255,
    150, 255,255,255,
    180,   0,  0,  0,
    180,   0,  0,  0,
    220, 255,255,255,
    220, 255,255,255,
    255,   0,  0,  0,
    255,   0,  0,  0
   };
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
    
    //FillLEDsFromPaletteColors(startIndex);
    
    //Make sure we only call this every X steps - less then loop

    //create_new_blink_interval = random(MIN_BLINQ_INTERVAL, MAX_BLINQ_INTERVAL);
    //create_new_blink_interval = MIN_BLINQ_INTERVAL;
    if(millis() - lastRefreshTime >= create_new_blink_interval)
    {
        Serial.println("Creating a new one");
        lastRefreshTime += create_new_blink_interval;
        selectRandomLed();
////      
////      //FillLedsRandomByGroups(ledPerGroup, groups, groupsSize);
    }
//    
    
    FastLED.show();
    //FastLED.delay(1000 / UPDATES_PER_SECOND);

  if(crazyMode == 0) {
    moveThroughLedStates();
  }else {
    FillLEDsFromPaletteColors(startIndex); 
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

int REMOVE_ME_COUNTER = 0;
void selectRandomLed() {
  //int ledToLightInGroup = REMOVE_ME_COUNTER++;
  for (int i = 0;i<NUM_OF_ACTIVE_LEDS; ++i) {
    int ledToLightInGroup = random(0, NUM_LEDS);    
    selectedLeds[ledToLightInGroup] = 1;      
  }    
}
//void FillLedsRandomByGroups(int numLedToLightOnEachGroup, int groups[], int groupSize) {
//  int ledToLightInGroup = random(0, NUM_LEDS);
//  selectedLeds[ledToLightInGroup] = 1;
//  int currentGroupStart = 0;
//  for (int i=0; i<groupSize;i++) {
//    //Serial.print("group ");
//    //Serial.println(i);
//    int groupSize = groups[i];
//    Serial.print("groupsize ");
//    Serial.println(groupSize);
//      int selectedGroup = random(0,groupSize-1);
//      for(int i=0;i<selectedGroup;i++){
//        currentGroupStart+=groups[i]; //Need to sum all the leds to this group
//      }
//      int ledToLightInGroup = random(currentGroupStart, currentGroupStart + groupSize);
//      Serial.print("ledtoLight ");
//      Serial.println(ledToLightInGroup);
//      selectedLeds[ledToLightInGroup] = 1;
//    
//    currentGroupStart+=groupSize;
//  }
//}

CRGB warm_orange = CRGB(255,204,0);
//int halfWay = map((animateSpeed/2),0,animateSpeed,0,255);
void moveThroughLedStates() {
  int start = millis();
  //int animationPart = ((millis() / 256) % 256) //Count from 0 to 255
  for(int i=0;i<NUM_LEDS;i++) {
    if(selectedLeds[i] != 1) {
      continue;
    }
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
//      Serial.print("Ended animation for ");
//      Serial.println(i);
      leds[i] = CRGB::Black;
      selectedLeds[i] = 0;
    }
  }
  //int final = millis();
  //Serial.print("move throgh states");
  //Serial.println(final-start);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = START_SKIP_LEDS; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void FillLEDsFromPaletteColorsAndShow( uint8_t colorIndex=0)
{
    uint8_t brightness = 255;
    
    for( int i = START_SKIP_LEDS; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
        FastLED.show();
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
  switch(animationIndex){
    case 1:
      currentPalette = bhw2_turq_gp;
      break;
//    case 2:
//     currentPalette = bhw1_28_gp;
//     FastLED.setBrightness(5);
//     break;
//    case 3:
//     currentPalette = bhw2_turq_gp;
//     FastLED.setBrightness(20);
//     break;
//    case 4:
//     currentPalette = bhw2_38_gp;
//     UPDATES_PER_SECOND = 102;
//     FastLED.setBrightness(30);
//     break;
//    case 5:
//     currentPalette = bhw2_38_gp;
//     UPDATES_PER_SECOND = 10;
//     FastLED.setBrightness(5);
//     break;
//    case 6:
//     currentPalette = spring_gp;
//     UPDATES_PER_SECOND = 50;
//     FastLED.setBrightness(10);
//     break;
//    case 7:
//     currentPalette = bhw3_61_gp;
//     UPDATES_PER_SECOND = 200;
//     FastLED.setBrightness(20);
//     break;
//    case 8:
//     currentPalette = bhw1_15_gp;
//     UPDATES_PER_SECOND = 102;
//     FastLED.setBrightness(30);
     default:     
     //Go back to start
     animationIndex = 0;
     break;
  }
  currentBlending = LINEARBLEND;
  FillLEDsFromPaletteColorsAndShow();
}


//1 blackwhite, 2blue purple, 3 green blue ?, 4 ,5, 6torquiz, 7?geeen pink,8flame, 9? ,10rainbow
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
//void SetupBlackAndWhiteStripedPalette()
//{
//    // 'black out' all 16 palette entries...
//    fill_solid( currentPalette, 16, CRGB::Black);
//    // and set every fourth one to white.
//    currentPalette[0] = CRGB::White;
//    currentPalette[4] = CRGB::White;
//    currentPalette[8] = CRGB::White;
//    currentPalette[12] = CRGB::White;
//    
//}

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



// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
