#include <Wire.h> // For i2c Communication
#include <Adafruit_GFX.h> // Graphics Effects Library
#include <Adafruit_SSD1306.h> // SSD1306 Display Library

#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 32 // OLED display height
#define OLED_RESET -1 // Reset pin -1 if sharing Arduino reset pin
#define SCREEN_ADDRESS 0x3C // set i2c Adress
#define PmSensor_INPUT 2 // Particulate matter Sensor INPUT pin
#define PmSensor_OUTPUT A0 // Particulate matter Sensor OUTPUT pin
#define VOCsSensor A1 // VOCs Sensor OUTPUT pin

//Set parameter value
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET);

int i; // For for loop
int PM[10]; // Variables that store 10 PM values
long PM_SUM; // The sum of 10 PM values
float PM_OUTPUT; // Final OUTPUT values
int VOCs[10]; // Variables that store 10 VOCs values
long VOCs_SUM; // The sum of 10 VOCs values
float VOCs_OUTPUT; // Final OUTPUT values

void setup()
{
  pinMode(A5,INPUT_PULLUP); // for oled
  pinMode(A4,INPUT_PULLUP); // for oled
  pinMode(VOCsSensor,INPUT_PULLUP); // for VOCs sensor
  pinMode(PmSensor_OUTPUT,INPUT_PULLUP); // for PM sensor output
  pinMode(PmSensor_INPUT,OUTPUT); // for PM sensor input
  Serial.begin(9600); // Start serial communication at 9600 baud rate
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.setRotation(2); // Rotate in the right direction
  display.ssd1306_command(0xA0); // Flip display For AR screen
  display.setTextSize(1); // Set text size
  display.setTextColor(SSD1306_WHITE); // Set text color
  display.display(); // Displaying these on the screen
  delay(100); // A little delay
}

void loop()
{
  PM_SUM=0; // Reset the sum of PM values to 0
  VOCs_SUM=0; // Reset the sum of VOCs values to 0
  for(i=0;i<9;i++) // Repeat 9 times (i= 0~8 )
  {
    PM[i]=PM[i+1]; // Pulling the PM value forward one space
    PM_SUM+=PM[i]; // Add value to the PM SUM
    
    VOCs[i]=VOCs[i+1]; // Pulling the VOCs value forward one space
    VOCs_SUM+=VOCs[i]; // Add value to the VOCs SUM
  }
  digitalWrite(PmSensor_INPUT,HIGH); // Turn on the PM sensor's LED
  delay(280); // A little delay
  PM[9]=analogRead(PmSensor_OUTPUT); // Read the PM value and save it
  delay(20); // A little delay
  digitalWrite(PmSensor_INPUT,LOW); // Turn off the PM sensor's LED
  delay(10); // A little delay
  
  PM_SUM+=PM[9]; // Add the read value to PM SUM
  PM_SUM/=10; // We added 10 values, so we have to divide it by 10
  // The μg/m^3 value depending on the voltage.
  PM_OUTPUT=(0.17*PM_SUM*(5.0/1023)-0.1)*1000;

  VOCs[9]=analogRead(VOCsSensor); //Read the VOCs value and save it
  VOCs_SUM+=VOCs[9]; // Add the read value to VOCs SUM
  VOCs_SUM/=10; // We added 10 values, so we have to divide it by 10
  // The mg/m^3 value depending on the voltage.
  VOCs_OUTPUT=pow(10,-0.867+(1.274*VOCs_SUM*(5.0/1023)));
  
  Serial.print("PM: "); 
  Serial.print(PM_OUTPUT); // Serial print Final PM value
  Serial.print("  VOCs: ");
  Serial.println(VOCs_OUTPUT); // Serial print Final VOCs value

  display.clearDisplay(); // Clear the screen

  display.setCursor(0,0); // Set cursor to x=0,y=0
  display.print("  PM:");
  display.print(PM_OUTPUT); // Print Final PM value to display 
  display.setCursor(0,16); // Set cursor to x=0,y=16
  display.print("VOCs:");
  display.print(VOCs_OUTPUT); // Print Final VOCs value to display
  display.display(); // Displaying these on the screen
  
  delay(100); // A little delay to prevent too many operations
}
