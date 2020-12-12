#include <Entropy.h>

#include <Adafruit_Thermal.h>
#include <stdio.h>

#include "SoftwareSerial.h"
#include "logo.h"

#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled TX on printer

#define NUM_COPIES 2
#define NUM_LINES 10
#define NUMS_PER_LINE 5
#define NUM_LENGTH 5
#define OUTPUT_LENGTH (NUM_LENGTH * (NUMS_PER_LINE + 1))

unsigned int numbers[NUM_LINES * NUMS_PER_LINE] = {0};
char output[OUTPUT_LENGTH] = {0};

unsigned int trigger = 0;

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

void printPads() {
  unsigned int line = 0;
  unsigned int copy = 0;
  unsigned int num = 0;
  
  printer.reset();

  printer.setSize('M');

  for (line = 0; line < NUM_LINES; line++) {
    for (num = 0; num < NUMS_PER_LINE; num++) {
      numbers[(line * NUMS_PER_LINE) + num] = Entropy.random(100000);
      if (0 == line && 0 == num) {        
        printer.justify('L');
        printer.printBitmap(logo_width, logo_height, logo);
        printer.justify('C');
        printer.setSize('M');
        snprintf(output, OUTPUT_LENGTH, "OTP %05u", numbers[0]);
        printer.println(output);
        printer.feed(1);
        printer.setSize('S');
      }
    }
    snprintf(output, OUTPUT_LENGTH, "%05u %05u %05u %05u %05u", 
        numbers[(line * NUMS_PER_LINE) + 0],
        numbers[(line * NUMS_PER_LINE) + 1],
        numbers[(line * NUMS_PER_LINE) + 2],
        numbers[(line * NUMS_PER_LINE) + 3],
        numbers[(line * NUMS_PER_LINE) + 4]);
    printer.println(output);
    printer.feed(1);
  }
  printer.feed(1);
  
  printer.setSize('M');        // Set type size, accepts 'S', 'M', 'L'
  printer.println(F("DESTROY AFTER USE"));  
  printer.feed(4);  

  for (copy = 1; copy < NUM_COPIES; copy++) {
    snprintf(output, OUTPUT_LENGTH, "OTP %05u", numbers[0]);    
    printer.justify('L');
    printer.printBitmap(logo_width, logo_height, logo);
    printer.justify('C');
    printer.setSize('M');
    printer.println(output);
    printer.feed(1);
    printer.setSize('S');
    for (line = 0; line < NUM_LINES; line++) {
      snprintf(output, OUTPUT_LENGTH, "%05u %05u %05u %05u %05u", 
          numbers[(line * NUMS_PER_LINE) + 0],
          numbers[(line * NUMS_PER_LINE) + 1],
          numbers[(line * NUMS_PER_LINE) + 2],
          numbers[(line * NUMS_PER_LINE) + 3],
          numbers[(line * NUMS_PER_LINE) + 4]);
      printer.println(output);
      printer.feed(1);
    }    
    printer.setSize('M');        // Set type size, accepts 'S', 'M', 'L'
    printer.println(F("DESTROY AFTER USE"));  
    printer.feed(4);  
  }
  
  printer.reset();

  memset(numbers, 0 , NUM_LINES * NUMS_PER_LINE);
}

void setup() {  
  // This line is for compatibility with the Adafruit IotP project pack,
  // which uses pin 7 as a spare grounding point.  You only need this if
  // wired up the same way (w/3-pin header into pins 5/6/7):
  pinMode(7, OUTPUT); digitalWrite(7, LOW);
  
  mySerial.begin(19200);  // Initialize SoftwareSerial
  printer.begin();        // Init printer (same regardless of serial type)  
  printer.reset();

  Entropy.initialize();
}

void loop() {
  // If we release the button
  if (100 >= analogRead(A0) && 0 == trigger) {
    trigger = 1;
  // If we press the button
  } else if (100 < analogRead(A0) && 1 == trigger) {
    trigger = 0;
    printPads();
  }

  delay(10);
}
