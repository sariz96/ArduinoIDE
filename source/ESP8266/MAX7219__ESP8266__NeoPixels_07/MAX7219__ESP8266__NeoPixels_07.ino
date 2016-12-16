// -- MAX7219 Driver  Implementation
// -- http://www.esp8266-projects.com/2015/06/arduino-ide-max7219-8-digit-display.html

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// ESP8266 - ESP-201 : IO5
// Nodemcu - D1 : GPI05
int NeoPixel_PIN = 5;
// How many NeoPixels are attached to the Arduino?
int NeoPixel_NUMPIXELS = 8;
// Button for user action - Nodemcu - D2 : GPI04
int Button_PIN = 4;

int INTENSITYMIN = 0; // minimum brightness, valid range [0,15]
int INTENSITYMAX = 1; // maximum brightness, valid range [0,15]

int DIN_PIN = 13;      // data in pin   -- D7
int CS_PIN = 12;       // load (CS) pin -- D6
int CLK_PIN = 14;      // clock pin     -- D5
int dly = 50;          // delay in us
int offset=5;          // input offset

int LED_pin = 2; // GPIO2
int iCounter = 0;
// 0 - készenlét
// 1 - számlálás előre
// 2 - számlálás vissza
//
int iMode = 0;

unsigned long lMilisLastPushedButton = 0;
unsigned long lMilisPushedButtonDelay = 500;

// MAX7219 registers
byte MAXREG_DECODEMODE = 0x09;
byte MAXREG_INTENSITY  = 0x0a;
byte MAXREG_SCANLIMIT  = 0x0b;
byte MAXREG_SHUTDOWN   = 0x0c;
byte MAXREG_DISPTEST   = 0x0f;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NeoPixel_NUMPIXELS, NeoPixel_PIN, NEO_GRB + NEO_KHZ800);
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


void putByte(byte data)
{
  byte i = 8;
  byte mask;
  while (i > 0)
  {
    mask = 0x01 << (i - 1);        // apply bitmask
    digitalWrite( CLK_PIN, LOW);   // CLK
    delayMicroseconds(dly);
    if (data & mask)               // select bit
    {
      digitalWrite(DIN_PIN, HIGH); // send 1
      delayMicroseconds(dly);
    }
    else
    {
      digitalWrite(DIN_PIN, LOW);  // send 0
      delayMicroseconds(dly);
    }
    digitalWrite(CLK_PIN, HIGH);   // CLK
    delayMicroseconds(dly);
    --i;                           // move to next bit
  }
}

void setRegistry(byte reg, byte value)
{
  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(dly);
  putByte(reg);   // specify register
  putByte(value); // send data

  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(dly);  
  digitalWrite(CS_PIN, HIGH);
}

void print_LED_int(int iVal, int w)
{
  int strl;
  int n = 0;
  String stringVal = "";     //data on buff is copied to this string
  char charVal;
  
  stringVal = String(iVal, DEC);
  strl = stringVal.length();  
  for(int i=0; i<strl; i++)
  {
    charVal = stringVal[i];
    n = charVal - 48;
    setRegistry(strl-i, n);
  }  
}

void print_LED(float fVal, int w, int p)
{
  int d = 1;
  int ch = 1;
  int n = 0;
  int nr_size = 0;
  char charVal[11];               //temporarily holds data from vals 
  String stringVal = "";     //data on buff is copied to this string
  
  //dtostrf(fVal, w, p, charVal);  //4 is mininum width, 3 is precision; 
                           //NOT WORKING FOR Values SMALLER THAT 0.01 !!
  // stringVal = charVal;
  // created a new function below for converting properly a pozitive xxxx.xxx float to string


  //stringVal=ftos(fVal,0);
  stringVal = String((int)fVal, DEC);
  
  int strl = stringVal.length()-1;  
  for (int i=0;i<strl+1;i++)
  { charVal[i]=stringVal[i]; }
  
    //Serial.print("Length: ");Serial.println(strl); //display string 
    //Serial.println(stringVal);
  //convert charVal[] to LED Display string
  for(int i=0;i<strl+1;i++)
  {
    if ((charVal[i] == '.') && (d==1))
    {
    stringVal=charVal[i]; 
    n = 0;
    n = (n * 10) + (charVal[i-1] - 48);
    setRegistry(strl-i+1, 128+n);
    d = 0;    
    }
    else  {           
        stringVal=charVal[i]; 
        //Serial.print("d: ");Serial.print(d); //display string
        //Serial.print("  - Increment: ");Serial.print(i); //display string
        //Serial.print(" - INT: ");Serial.println(charVal[i]); //display string
        n=0;
        n = (n * 10) + (charVal[i] - 48);
        int pos = i;
        if (d==0) { pos = i-1; } 
        setRegistry(strl-pos,n);
    }  
  }
}

 String ftos(float fVal, int prec)
{ 
  int mlt=10;
  String snr;  
  String dp;
  int iprt,dprt;
  
  iprt = int(fVal);

   // Round fVal for proper prec printing - correctly so that print(1.999, 2) prints as "2.00"
   double rnd = 0.5;
   for(uint8_t i = 0; i < prec; ++i)
        rnd /= 10.0;
        mlt *= 100;
    fVal += rnd;
   
 // Check and count "0"'s proper after ZERO (0.00xx) number display 
  dprt = 1000*(fVal-iprt);  
  if (dprt < 10)
  {
    dp = "00" + String(dprt);
  }else 
        if (dprt < 100)
        {
          dp = "0" + String(dprt);
        }else {dp = dprt;}

  snr = String(iprt) +"."+String(dp);  

  //Serial.println(""); 
  //Serial.print("MLT: ");Serial.println(mlt);   
  //Serial.println(""); 
  //Serial.print("DEC Part: ");Serial.println(dprt);   
  //Serial.println(""); 
  //Serial.print("Int Part: ");Serial.println(iprt);              
  //Serial.print(" . ");  
  //Serial.print("DP: "); Serial.print(dp); 

  return snr;
}


void zero_lcd()
{
 for (int i=1;i<9;i++)
 {
  setRegistry(i, 0);
    delayMicroseconds(100);  
 }
}

void blank_lcd()
{
 for (int i=1;i<9;i++)
 {
  setRegistry(i, 0x0f);
    delayMicroseconds(100);  
 }
}

void init_MAX7219()
{
  // select allocated I/O pins
  pinMode(DIN_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);

  // initialization of the MAX7219
  setRegistry(MAXREG_SCANLIMIT, 0x07);
    delayMicroseconds(dly);
  setRegistry(MAXREG_DECODEMODE, 0xFF);  // full decode mode BCD 7 Seg Display
    delayMicroseconds(dly);
  setRegistry(MAXREG_SHUTDOWN, 0x01);    // shutdown mode OFF
    delayMicroseconds(dly);
  setRegistry(MAXREG_DISPTEST, 0x00);    // no test
    delayMicroseconds(dly);
  setRegistry(MAXREG_INTENSITY, 0x02);  // 5/32   
    delayMicroseconds(dly);
  //zero_lcd();
  blank_lcd();
}

void colorPingPong(uint32_t c, uint32_t cycles, uint8_t wait)
{
  uint32_t c0 = strip.Color(3, 3, 3);
  
  for(uint32_t i = 0; i < cycles; i++)
  {
    for(uint16_t j = 0; j < strip.numPixels(); j++)
    {
      if(j > 0)
      {
        strip.setPixelColor(j-1, c0);
      }
      strip.setPixelColor(j, c);
      strip.show();
      delay(wait);
    }

    for(uint16_t k = strip.numPixels() - 1; k > 0 ; k--)
    {
      if(k < (strip.numPixels() - 1))
      {
        strip.setPixelColor(k+1, c0);
      }
      strip.setPixelColor(k, c);
      strip.show();
      delay(wait);
    }    
    
  }
}

void colorFill(uint32_t c)
{
  for(uint16_t j = 0; j < strip.numPixels(); j++)
  {
    strip.setPixelColor(j, c);
  }
  strip.show();
  delayMicroseconds(50);
}

// Gombot megnyomták
// 0: - nem
// 1: - igen
// ...
int Button_Pushed()
{
  int iRet = 0;
  unsigned long lMilisNow;
  
  if(digitalRead(Button_PIN) == LOW)
  {
    delayMicroseconds(50);
    if(digitalRead(Button_PIN) == LOW)
    {
      lMilisNow = millis();
      if((lMilisNow - lMilisLastPushedButton) >= lMilisPushedButtonDelay)
      {
        lMilisLastPushedButton = lMilisNow;
        iRet = 1;
      }
    }
  }
  return iRet;
}

void setup ()
{
  //Serial.begin(9600);
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, HIGH);

  pinMode(Button_PIN, INPUT);

  init_MAX7219();
  iCounter = 0;
  iMode = 0;

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  delay(2000);
}
 

void loop ()
{
  int iButtonx;
  
  iButtonx = Button_Pushed();

  if(iMode == 0)
  {
// -- készenlét --
    if(iButtonx == 1)
    {
      iMode = 1;
      iCounter = 0;
      zero_lcd();
      delay(80);
      blank_lcd();
      delay(20);      
    }
   delay(50);
  }
  else if(iMode == 1)
  {
// -- számlálás előre --
    
    if(iButtonx == 1)
    {
      iMode = 0;
      colorFill(strip.Color(0, 0, 0));
    }
    else
    {
      iCounter += 1;
    
      digitalWrite(LED_pin, LOW);
      
      print_LED_int(iCounter, 0);
    
      delay(100);
      digitalWrite(LED_pin, HIGH);
    
      if((iCounter % 4) == 0)
      {
        colorPingPong(strip.Color(255, 0, 0), 1, 50); // Red
      }
      else if((iCounter % 4) == 1)
      {
        colorPingPong(strip.Color(0, 255, 0), 1, 50); // Green
      }
      else if((iCounter % 4) == 2)
      {
        colorPingPong(strip.Color(0, 0, 255), 1, 50); // Blue
      }
      else if((iCounter % 4) == 3)
      {
        colorPingPong(strip.Color(255, 255, 0), 1, 50); // Yellow
      }
    }
   delay(200);
  }
 }


