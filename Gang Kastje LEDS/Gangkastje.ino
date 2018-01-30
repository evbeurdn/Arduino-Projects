#include <ArduinoJson.h>

#include <WiFiRestClient.h>

#include <ESP8266WiFi.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 12

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);

// Global variables
#define ESP8266_LED 5
#define ESP8266_BTN1 15
#define ESP8266_BTN2 13
#define ESP8266_BTN3 14
#define ESP8266_BTN4 4

// Wireless Network Configuration
const char WIFI_SSID[] = "DiBurdo";
const char WIFI_PSK[] = "0135704777";

// Hue Configuration
const char HUE_IP[] = "192.168.178.19";
const  int HUE_PORT = 80;
WiFiRestClient hue(HUE_IP,HUE_PORT);


const char LIGHTS_ON[] = "{\"on\":true}";
const char LIGHTS_OFF[] = "{\"on\":false}";
const char EFFECT_COLORLOOP[] = "{\"effect\":\"colorloop\"}";
const char EFFECT_NONE[] = "{\"effect\":\"none\"}";

// App Configuration
bool btn1 = false;
bool btn2 = false;
bool btn3 = false;
bool btn4 = false;
bool party_on = false;
int duration = 0;
int button_press = 0;

void blinkLED(int dlay, int count = 1)
{
  for (int c=0; c < count; c++) {
    delay(dlay);
    digitalWrite(ESP8266_LED, HIGH);
    delay(dlay);
    digitalWrite(ESP8266_LED, LOW);
  }
}

void setup() 
{
  Serial.begin(115200);
//  pinMode(ESP8266_LED, OUTPUT);
//  pinMode(ESP8266_BTN1, INPUT_PULLUP);
//  pinMode(ESP8266_BTN2, INPUT_PULLUP);
//  pinMode(ESP8266_BTN3, INPUT_PULLUP);
//  pinMode(ESP8266_BTN4, INPUT_PULLUP);
Serial.print("Getting connected\n");
  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);

  // Initiate connection with SSID and PSK
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  // Blink LED while we wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) blinkLED(100);
  if (WiFi.status() != WL_CONNECTED) while(1) blinkLED(1000);

Serial.print("Connected\n");
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() 
{
 int StatusCode;
 String json = "";
 StaticJsonBuffer<800> jsonBuffer;
 Serial.print("Hue get: \n");
 StatusCode = hue.get("/api/QPUUK2uDxGsvunEXLJ5U-WeN6ea3bFuLP5weH9TE/lights/12", &json);
 //Serial.print(StatusCode,DEC);
 //Serial.print(Response);
 JsonObject& root = jsonBuffer.parseObject( (char*)&json[0]);
if (!root.success()) {
  // Parsing failed :-(
  Serial.println("Parse fail");
}

 
Serial.println("Parsed okay");
Serial.println(root["state"]["effect"].as<char*>());
bool on = root["state"]["on"];
if (on) {
  Serial.println("ON");
} else
{
  Serial.println("OFF");
}
int brightness = root["state"]["bri"];
Serial.println(brightness, DEC);

int ct = root["state"]["ct"];
Serial.println(ct, DEC);

float CIE_x = root["state"]["xy"][0];
float CIE_y = root["state"]["xy"][1];
Serial.println(CIE_x, DEC);
Serial.println(CIE_y, DEC);

// Convert xy to RGB

uint32_t colour = xy2RGB(CIE_x, CIE_y, ((float)brightness)/255.0f);
Serial.println(colour, HEX);

//waiting for input

while (Serial.available() == 0);

int val = Serial.parseInt(); //read int or parseFloat for ..float...
Serial.print("Wheelvalue: ");
Serial.println(val);



strip.setPixelColor(0, Wheel(val));
strip.setPixelColor(1, Wheel(val+1));
strip.setPixelColor(2, Wheel(val+2));
strip.setPixelColor(3, Wheel(val+3));
strip.setPixelColor(4, Wheel(val+4));
strip.setPixelColor(5, Wheel(val+5));
strip.setPixelColor(6, Wheel(val+6));
strip.setPixelColor(7, Wheel(val+7));
strip.setPixelColor(8, Wheel(val+8));
strip.setPixelColor(9, Wheel(val+9));
strip.setPixelColor(10, Wheel(val+10));
strip.setPixelColor(11, Wheel(val+11));
strip.setPixelColor(12, Wheel(val+12));
strip.setPixelColor(13, Wheel(val+13));
strip.setPixelColor(14, Wheel(val+14));
strip.setPixelColor(15, Wheel(val+15));

strip.show();
delay(1000);
 

 // rainbowCycle(20);
 // theaterChaseRainbow(50);  

 
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


// Input a value 0 to 255 to get a color value255.
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


// From HUE API documentation, convert the received xy value to an RGB value.
// Code derived from https://developers.meethue.com/documentation/color-conversions-rgb-xy
uint32_t xy2RGB(float x, float y, float brightness) {
uint32_t calculatedRGB;

float z = 1.0f - x - y;
float Y = brightness; // The given brightness value
float X = (Y / y) * x;
float Z = (Y / y) * z;
Serial.println(z);
Serial.println(Y);
Serial.println(X);
Serial.println(Z);

// Convert to RGB using Wide RGB D65 conversion

/*float r =  X * 1.656492f - Y * 0.354851f - Z * 0.255038f;
float g = -X * 0.707196f + Y * 1.655397f + Z * 0.036152f;
float b =  X * 0.051713f - Y * 0.121364f + Z * 1.011530f;
*/
float r =  X * 3.1338561f - Y * 1.6168667f - Z * 0.4906146f;
float g = -X * 0.9787684f + Y * 1.9161415f + Z * 0.0334540f;
float b =  X * 0.0719453f - Y * 0.2289914f + Z * 1.4052427f;


// Apply reverse gamma correction
r = r <= 0.0031308f ? 12.92f * r : (1.0f + 0.055f) * pow(r, (1.0f / 2.4f)) - 0.055f;
g = g <= 0.0031308f ? 12.92f * g : (1.0f + 0.055f) * pow(g, (1.0f / 2.4f)) - 0.055f;
b = b <= 0.0031308f ? 12.92f * b : (1.0f + 0.055f) * pow(b, (1.0f / 2.4f)) - 0.055f;

if (r>1.0) r= 1.0;
if (g>1.0) g= 1.0;
if (b>1.0) b= 1.0;

Serial.println(r);
Serial.println(g);
Serial.println(b);


calculatedRGB = strip.Color(r*255,g*255, b*255);

return calculatedRGB;
 
}





