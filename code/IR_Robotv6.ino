#include <Wire.h>
#include <Adafruit_INA219.h> // for current sensor needed we were building the circuit to ensure the H-Bridge is not fried (TB66FNG123) 
#include <Adafruit_NeoPixel.h> // for the adafruit neopixel that glows different colors for different directions of the motor
#include "Adafruit_NECremote.h" // adafruit's NEC remote controller
// Connect a 38KHz remote control sensor to the pin below
#define IRpin         11
 
Adafruit_NECremote remote(IRpin); // initialize the IR remote.

#define PIN 8 // for the neopixel workings.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_INA219 ina219; // current sensor; used during the time of callibration only.

const int enablepin = 9;  // for the TB66FNG123 H-bridge's 1st enable pin.
const int enablepin2 = 10; // for the TB66FNG123 H-bridge's 2nd enable pin.
const int inp1 = 3;//first of the 4 inputs to the H-Bridge.
const int inp2 = 4;//second of the 4 inputs to the H-Bridge. These two for the first motor.
const int inp3 = 5;//third  of the 4 inputs to the H-Bridge.
const int inp4 = 6;//fourth of the 4 inputs to the H-Bridge. These two for the second motor

int speedVal = 50; // starting speed.
const int stbypin= 12; // to feed to STBY pin of the H-bridge.

void setup(){
  ina219.begin(); // initialize the current sensor.
 // various pinmodes.
  pinMode(enablepin, OUTPUT);
  pinMode(inp1, OUTPUT);
  pinMode(inp2,OUTPUT);
  Serial.begin(115200); 
  pinMode(enablepin2, OUTPUT);
  pinMode(inp3, OUTPUT);
  pinMode(inp4,OUTPUT);
  pinMode(stbypin, OUTPUT);
  // initialize the PWM pins for the H-bridge.
  analogWrite(enablepin, speedVal);
  analogWrite(enablepin2, speedVal);
  
  strip.begin(); // initialize neopixel
  strip.setBrightness(100); //adjust brightness here for the neopixel
  strip.show(); // Initialize all pixels to 'off'
  digitalWrite(stbypin, HIGH); //stby needs to be high
  Serial.begin(115200); // this is very high, and the current sensor needs it. could bring it back to 9600 if needed.
  
  Serial.println("Ready to decode IR!");

}

int code =1; //IR code for "Stop".
int lastcode = 1; // you need "Last Code" to maintain prior state: (a) when nothing is pressed after polling for 2seconds (b) vol+ and vol- are pressed.

void forward(){
colorWipe(strip.Color(0, 255, 0), 10);
digitalWrite(inp1,HIGH);
digitalWrite (inp2,LOW );
digitalWrite(inp3,LOW  );
digitalWrite (inp4,HIGH);
 
}
void stopMove()
{
colorWipe(strip.Color(255, 255, 255), 10);
digitalWrite(inp1, LOW);
digitalWrite (inp2, LOW);
digitalWrite(inp3, LOW );
digitalWrite (inp4,LOW );
 
}

void backward(){
colorWipe(strip.Color(255, 0, 0), 10);
 digitalWrite(inp1, LOW);
digitalWrite (inp2, HIGH);
digitalWrite(inp3, HIGH );
digitalWrite (inp4,LOW );
 
}


void Right(){
colorWipe(strip.Color(0, 0, 255), 10);
digitalWrite(inp1,LOW);
digitalWrite (inp2,LOW );
digitalWrite(inp3,LOW  );
digitalWrite (inp4,HIGH);
 
}

void left(){
colorWipe(strip.Color(255, 255, 0), 10);
digitalWrite(inp1,HIGH);
digitalWrite (inp2,LOW );
digitalWrite(inp3,LOW  );
digitalWrite (inp4,LOW);
 

}

 
void runcurr() // for current sensor.
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void loop()
{

  if (code >= 0)
  {
    Serial.print("speed = "); Serial.print(speedVal); Serial.print("Received code # ");
    Serial.println(code, DEC);

    if (code == 5)     {    lastcode = code; Serial.print  ("Moving Forward...with ");  Serial.println(speedVal);  forward(); Serial.println(); }
    else if (code ==13)  {   lastcode = code; Serial.print  ("Moving Back...with ");Serial.println(speedVal);  backward(); Serial.println();}
    else if (code == 8) { lastcode = code;  Serial.print  ("Moving Left..with ."); Serial.println(speedVal); left();Serial.println();}
    else if (code == 10) { lastcode = code; Serial.print  ("Moving Right..."); Serial.println(speedVal);  Right(); Serial.println();}
    else if (code== 1) { lastcode = code; Serial.println ("Stopping to move...");stopMove();}
    else if (code == 0) 
    { 
     Serial.println ("Decreased the speed by 10"); speedVal =speedVal-10; if (speedVal<20) {speedVal =20;}; 
     Serial.print ("current SpeedVal.."); Serial.println(speedVal); analogWrite(enablepin,speedVal);analogWrite(enablepin2,speedVal);
     code = lastcode;//continue what you were doing.
     Serial.print ("Continuing with ");Serial.println(code); Serial.println();
    }
   else if (code == 2) 
   { 
    Serial.println ("Increased the speed by 10"); speedVal =speedVal+10; if (speedVal>255) {speedVal =255;}; 
    Serial.print ("current SpeedVal.."); Serial.println(speedVal); analogWrite(enablepin,speedVal);analogWrite(enablepin2,speedVal);
    code = lastcode;//continue what you were doing.
    Serial.print ("Continuing with ");Serial.println(code); Serial.println();
   }
  }  

 else if (code == -1) {code = lastcode; Serial.print  ("Nothing received -- so staying with current status "); Serial.println(code); Serial.println(); }

 

   code = remote.listen(5);

}
