// Library Imports
//=========================================
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Servo.h>



// Define pins for display
//=========================================
#define TFT_MISO 8
#define TFT_DC 9
#define TFT_CS 10
#define TFT_MOSI 11
#define TFT_RST 12
#define TFT_CLK 13

// Define colours for display
//=========================================
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF


Servo motor;

long duration, cm, inches;
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;

int rangeReturn = 0;
int threshold = 15;
int pos = 90;
int colourState = 0;
int save = 0;
int posDraw = 0;
int num = -1;
int newNum = 2;
int sweepRate = 5;
int selector = 1;

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);


void setup() {
  selectorButton(4);
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  int count = 0;
    while (num == -1){
      int thresholdRead = analogRead(A0);
      threshold = map(thresholdRead, 0, 1023, 5, 350);
      tft.setCursor(10, 50);
      tft.setTextColor(BLACK);
      tft.setTextColor(GREEN);
      tft.setTextSize(2);
      tft.println("Set Range:");
      tft.setCursor(10, 90);
      tft.println(threshold);
      tft.setCursor(50, 90);
      tft.println("cm");
      delay(300);
      tft.setCursor(10, 90);
      tft.setTextColor(BLACK);
      tft.println(threshold);
      
       if (digitalRead(4) == 0){
          tft.setTextColor(GREEN);
          tft.setCursor(10, 90);
          tft.println(threshold);
          num++;
          delay(250);
          tft.fillScreen(ILI9341_BLACK);
          
       }
    }
    
    while (num == 0){
    
    tft.setCursor(10, 50);
    tft.setTextColor(GREEN);
    tft.setTextSize(2);
    tft.println("Pick a mode:");
    
    if ((digitalRead(4) == 0) && (selector % 2 != 0)){
      delay(250);
      if (digitalRead(4) == 0){
        textFlash(3, 30, 80, "Sonar");
        num = 2;
      } else {
        selector++;
      }
    } else if ((digitalRead(4) == 0) && (selector % 2 == 0)) {
      delay(250);
      if (digitalRead(4) == 0){
        textFlash(3, 30, 110, "Range Finder");
        num = 1;
      } else {
        selector++;
      }
    }
    
    if (selector % 2 == 0){
      tft.setTextSize(3);
      tft.setCursor(30,80);
      tft.setTextColor(GREEN);
      tft.print("Sonar");
      tft.setCursor(30,110);
      tft.setTextColor(RED);
      tft.print("Range Finder");
    } else {
      tft.setTextSize(3);
      tft.setCursor(30,80);
      tft.setTextColor(RED);
      tft.println("Sonar");
      tft.setCursor(30,110);
      tft.setTextColor(GREEN);
      tft.println("Range Finder");
    }
    }

 
    if(num == 1){
      newNum = 1;
      motor.attach(5);
      motor.write(pos);
      tft.begin();
      tft.setRotation(3);
      tft.fillScreen(ILI9341_BLACK);
      int width = tft.width();
      int height = 240;
      for (int i = 1; i <= 5; i++) {
        tft.drawCircle(width/2, height, (i*(height/5)), GREEN);
        }
      
      tft.fillTriangle(0, -1, width, -1, width/2, height, BLACK);
      tft.drawTriangle(0, -1, width, -1, width/2, height, GREEN);
    } else if (num == 2) {
      newNum = 2;
      pos = 5;
      motor.attach(5);
      tft.begin();
      tft.setRotation(3);
      tft.fillScreen(ILI9341_BLACK);
      int width = tft.width();
      int height = 240;
      for (int i = 1; i <= 5; i++) {
      tft.drawCircle(width/2, height, (i*(height/5)), GREEN);
    }
    }

}


void loop() {
if (newNum == 1){
    unsigned long currentMillis = millis();
  
    if (currentMillis - previousMillis >= 500){
        previousMillis = currentMillis;
        tft.fillScreen(ILI9341_BLACK);
        rangeReturn = ultraSonic(3,2);
        if (rangeReturn >= threshold){
          rangeReturn = threshold;
        }     
      }    
  
     if ((currentMillis - previousMillis1 >= 200) && (colourState == 0)){
          previousMillis1 = currentMillis;
          colourState = 1;
          posDraw = map(rangeReturn, 0, threshold, 240, 0);
          tft.fillCircle(320/2, posDraw, 10, GREEN);
      } else if ((currentMillis - previousMillis1 >= 200) && (colourState == 1)){
          previousMillis1 = currentMillis;
          colourState = 0;
          tft.fillCircle(320/2, posDraw, 10, BLACK);
      } 
     
 } else if (newNum == 2){
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= 200){

    previousMillis = currentMillis;
    
    if ((pos <= 0) || (pos >= 180)) {
          sweepRate *= -1;
      }
      motor.write(pos);
      
      
    rangeReturn = ultraSonic(3,2);
    if (rangeReturn >= threshold){
      rangeReturn = threshold;
    }

    int width = 320;
    int height = 240;
    float t = (pos*PI)/180;
    float drawPos = map(rangeReturn, 1, threshold, 0 , 240);

    float xClear  = 160 + (height * cos(t));
    float yClear = height + (height * (sin(t)*-1));
    

    float xDraw = width/2 + (drawPos * cos(t));
    float yDraw = height + (drawPos * (sin(t)*-1));


   
//
    tft.drawLine(320/2, 240, xClear, yClear, BLACK);
    tft.drawLine(xClear, yClear, xDraw, yDraw, GREEN);
    tft.drawLine( xDraw, yDraw,320/2, 240, BLACK);
    Serial.println(xDraw);Serial.println(yDraw);
    pos+=sweepRate; 
  }
 
 }
}

// Functions
//=========================================

void selectorButton(int pin) {
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
 }

void textFlash(int textSize, int cursorOne, int cursorTwo, String displayText){
  tft.setCursor(cursorOne,cursorTwo);
  tft.setTextSize(textSize);
  tft.setTextColor(BLACK);
  tft.print(displayText);
  delay(200);
  tft.setCursor(cursorOne,cursorTwo);
  tft.setTextColor(WHITE);
  tft.print(displayText);
  delay(200);
  }

int ultraSonic(int trigPin, int echoPin){
    pinMode(trigPin, OUTPUT);
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
   
    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);
   
    // Convert the time into a distance
    cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
    inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
   
    int cmCon = cm;

    return cmCon;
}
