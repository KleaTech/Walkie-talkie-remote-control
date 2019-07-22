#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*
 * CONFIGURATION
 */
#define VALUE_LIMIT     100                                                       //Threshold of the analog readout noise, values below this are ignored
#define SLEEP_AFTER    5000                                                       //In this period (in millisecs) the screen stays on and the output cannot be triggered. After this time the screen turns off and the program waits for the trigger signal
#define LOOP_INTERVAL    20                                                       //Wait this much millisecs after each iteration
#define MAX_COOLDOWN_RATE 3                                                       //The current max (average) indicator value decreases by this amount in each iteration. So it defines how fast the average indicator line is moving to the right

//Do not touch these
#define SCREEN_WIDTH    128                                                       //OLED screen width in pixels
#define SCREEN_HEIGHT    32                                                       //OLED screen height in pixels
#define OLED_RESET        4                                                       //Number of pin which resets the OLED (-1 for reset button)
#define ON                0                                                       //The relay board is ON when the signal is LOW
#define OFF               1                                                       //The relay board is OFF when the signal is HIGH
#define IDLE_VALUE SLEEP_AFTER / LOOP_INTERVAL

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);         //Initialize OLED display

void setup() {
  analogReference(INTERNAL);                                                      //Use INTERNAL reference if anitparallel diodes and a resistor is used before the sound source
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  togglePins(ON);                                                                 //Initially toggle output from ON to OFF
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);                                      //Initialize OLED display
}

void loop() {
  int value = analogRead(A0);                                                     //Analog readout from 0 to 1023

  logic(value);
  cycleVuMeter(value);

  delay(LOOP_INTERVAL);
}

void cycleVuMeter(int value) {
  static int maxvalue = 0;                                                        //This holds the value of the average indicator
  
  display.clearDisplay();

  display.drawLine(SCREEN_WIDTH - 1, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT, WHITE);  //Max value indicator
  display.fillRect(0, 0, value >> 3, SCREEN_HEIGHT, WHITE);                       //Current value indicator

  if (value > maxvalue) maxvalue = value;
  else maxvalue -= MAX_COOLDOWN_RATE;
  if (maxvalue < 0) maxvalue = 0;
  display.drawLine(maxvalue >> 3, 5, maxvalue >> 3, SCREEN_HEIGHT - 10, WHITE);   //Average value indicator

  display.display();                                                              //Nothing is drawn until this point
}

void togglePins(int previousState) {                                              //Changes relays on pins 8-11 from previousState to the opposite
  if (previousState) {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
  }
  else {
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
  }
}

/*
 * MAIN LOGIC BELOW
 */
void logic(int value) {
  static int previousState = ON;                                                  //Stores whether the relays were previously ON or OFF
  static int isScreenOn = true;                                                   //Stores whether the screen was previously on or off
  static int idle = IDLE_VALUE;                                                   //Stores the idle countdown timer
  
  if (value > VALUE_LIMIT) idle = IDLE_VALUE;                                     //If there is a value above the threshold resets the idle countdown
  if (--idle == 0) {                                                              //When there is no input
    if (isScreenOn) {                                                             //And the screen is on
      display.ssd1306_command(SSD1306_DISPLAYOFF);                                //Turn off the screen
      isScreenOn = false;
      previousState = previousState == OFF ? ON : OFF;                            //Toggle the relay state variable
    }
  }
  else {                                                                          //When there is input
    if (!isScreenOn) {                                                            //And the screen is off
      display.ssd1306_command(SSD1306_DISPLAYON);                                 //Turn on the screen
      isScreenOn = true;                                                          
      togglePins(previousState);                                                  //Change the relay state to the opposite
    }
  }
  if (idle < 1) idle = 1;
}
