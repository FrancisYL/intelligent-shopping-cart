#include <avr/interrupt.h>
#include "Cart.h"

// We need to track what the prior state of our pins for various PCINTS was;
//  this varies by interrupt. These values are initialized to the "all high"
//  state; we don't want any low-to-high transitions at beginning of code
//  execution to be caught.
volatile byte lastPC0PinState = 0x0E;  // For pins 9, 10, 11, PB1-3
volatile byte lastPC1PinState = 0x3F;  // For pins A0-A5/14-19, PC0-5
volatile byte lastPC2PinState = 0x08;  // For pin 3, PD3

// We need some way to exclude short transients on the encoder inputs; we'll do
//  that by capturing the most recent rise time with micros() and ignoring
//  falling edges that happen within 20us of a rise.
volatile unsigned long lastRRise = 0;
volatile unsigned long lastLRise = 0;
volatile unsigned long lastBumpRise = 0;
#define ENC_HIGH_DELAY 50
#define  WHISKER_HIGH_DELAY   0

byte PBMask = 0;
byte PCMask = 0;
byte PDMask = 0;

volatile byte pinFunction[10];     // Store the currently assigned function
                                   //  of the PCINT associated with each pin
                                   //  in this array. Array indices are of
                                   //  the type "PCINT_pinname".
                                   
extern void (*whiskerAction[10])(void); // Declared in RedBotBumper.cpp

extern Encoder *encoderObject;   // Declared in RedBotEncoder.cpp

                                       
// The RedBot uses pin change interrupts for detecting wheel encoder ticks and
//  wire bumper contacts. The sources for these are normally high, so we want to
//  look for falling edges and take an action when we see one.
ISR(PCINT0_vect)
{
  // The first thing we want to do is determine which interrupt(s) we're 
  //  servicing, and what those interrupts are associated with. We can cheat, a
  //  bit, because we know which pins we care about: for PCINT0, it's only
  //  bits 1, 2, and 3, which are pins 9, 10, and 11 in Arduino-land, or pins
  //  PB1, PB2, and PB3.
  // Since all the pins are in Port B, we can check for a low-to-high transition
  //  by masking out the pins on Port B we don't care about and returning if they
  //  are all high.
  byte PBTemp = PINB & PBMask;  // Capture the state of the pins-of-interest now,
                                //  before they have a chance to change.
  
  PC0Handler(PBTemp);
}

void PC0Handler(byte PBTemp)
{
  // Okay, now we have to figure out what changed, and if the change was a
  //  high-to-low or a low-to-high transition.
        
  // Was it pin 9, AKA PB1?
  if ((lastPC0PinState & 0x02) && !(PBTemp & 0x02))  // a falling edge
  {
    pinFunctionHandler(PCINT_9);
  }
  
  else if (!(lastPC0PinState & 0x02) && (PBTemp & 0x02)) // a rising edge
  {
    if (pinFunction[PCINT_9] == LENCODER) lastLRise = micros();
    if (pinFunction[PCINT_9] == RENCODER) lastRRise = micros();
  }
  // Was it pin 10, AKA PB2?
  if ((lastPC0PinState & 0x04) && !(PBTemp & 0x04)) // a falling edge
  {
    pinFunctionHandler(PCINT_10);
  }
  else if (!(lastPC0PinState & 0x04) && (PBTemp & 0x04)) // a rising edge
  {
    if (pinFunction[PCINT_10] == LENCODER) lastLRise = micros();
    if (pinFunction[PCINT_10] == RENCODER) lastRRise = micros();
  }
    // Was it pin 11, AKA PB3?
  if ((lastPC0PinState & 0x08) && !(PBTemp & 0x08)) // a falling edge
  {
    pinFunctionHandler(PCINT_11);
  }
  
  else if (!(lastPC0PinState & 0x04) && (PBTemp & 0x04)) // a rising edge
  {
    if (pinFunction[PCINT_11] == LENCODER) lastLRise = micros();
    if (pinFunction[PCINT_11] == RENCODER) lastRRise = micros();
  }
  
  lastPC0PinState = PBTemp;
}

ISR(PCINT1_vect)
{

  // The first thing we want to do is determine which interrupt(s) we're 
  //  servicing, and what those interrupts are associated with. We can cheat, a
  //  bit, because we know which pins we care about: for PCINT1, it's only
  //  bits 0-5, PC0-PC5, or for Arduino, A0-A5/14-19.
  // Since all the pins are in Port C, we can check for a low-to-high transition
  //  by masking out the pins on Port C we don't care about and returning if they
  //  are all high.
  
  byte PCTemp = PINC & PCMask;  // Capture the state of the pins-of-interest now,
                               //  before they have a chance to change.
                       
  PC1Handler(PCTemp);
}

void PC1Handler(byte PCTemp)
{
  // Okay, now we have to figure out what changed, and if the change was a
  //  high-to-low or a low-to-high transition. All these if() statements check
  //  for a high-to-low transition; we want to ignore the low-to-highs.
  
  // Was it pin A0/14, AKA PC0?
  if ((lastPC1PinState & 0x01) && !(PCTemp & 0x01))
  {
    pinFunctionHandler(PCINT_A0);
  }
  else if (!(lastPC1PinState & 0x01) && (PCTemp & 0x01))
  {
    if (pinFunction[PCINT_A0] == LENCODER) lastLRise = millis();
    if (pinFunction[PCINT_A0] == RENCODER) lastRRise = millis();
  }
  // Was it pin A1/15, AKA PC1?
  if ((lastPC1PinState & 0x02) && !(PCTemp & 0x02))
  {
    pinFunctionHandler(PCINT_A1);
  }
  else if (!(lastPC1PinState & 0x02) && (PCTemp & 0x02))
  {
    if (pinFunction[PCINT_A1] == LENCODER) lastLRise = millis();
    if (pinFunction[PCINT_A1] == RENCODER) lastRRise = millis();
  }
  // Was it pin A2/16, AKA PC2?
  if ((lastPC1PinState & 0x04) && !(PCTemp & 0x04))
  {
    pinFunctionHandler(PCINT_A2);
  }
  else if (!(lastPC1PinState & 0x04) && (PCTemp & 0x04))
  {
    if (pinFunction[PCINT_A2] == LENCODER) lastLRise = millis();
    if (pinFunction[PCINT_A2] == RENCODER) lastRRise = millis();
  }
  // Was it pin A3/17, AKA PC3?
  if ((lastPC1PinState & 0x08) && !(PCTemp & 0x08))
  {
    pinFunctionHandler(PCINT_A3);
  }
  else if (!(lastPC1PinState & 0x08) && (PCTemp & 0x08))
  {
    if (pinFunction[PCINT_A3] == LENCODER) lastLRise = millis();
    if (pinFunction[PCINT_A3] == RENCODER) lastRRise = millis();
  }
  // Was it pin A4/18, AKA PC4?
  if ((lastPC1PinState & 0x10) && !(PCTemp & 0x10))
  {
    pinFunctionHandler(PCINT_A4);
  }
  else if (!(lastPC1PinState & 0x10) && (PCTemp & 0x10))
  {
    if (pinFunction[PCINT_A4] == LENCODER) lastLRise = millis();
    if (pinFunction[PCINT_A4] == RENCODER) lastRRise = millis();
  }
  // Was it pin A5/19, AKA PC5?
  if ((lastPC1PinState & 0x20) && !(PCTemp & 0x20))
  {
    pinFunctionHandler(PCINT_A5);
  }
  else if (!(lastPC1PinState & 0x20) && (PCTemp & 0x20))
  {
    if (pinFunction[PCINT_A5] == LENCODER) lastLRise = millis();
    if (pinFunction[PCINT_A5] == RENCODER) lastRRise = millis();
  }
  lastPC1PinState = PCTemp;
}
ISR(PCINT2_vect)
{

  // The first thing we want to do is determine which interrupt(s) we're 
  //  servicing, and what those interrupts are associated with. We can cheat, a
  //  bit, because we know which pins we care about: for PCINT2, it's only
  //  bit 3, PD3 or pin 3 in Arduino-speke.
  // First, check if that pin is high. If so, we don't need to know any more.
  
  byte PDTemp = PIND & PDMask;// Capture the state of the pin-of-interest now,
                               //  before they have a chance to change.
  PC2Handler(PDTemp);                   
}

void PC2Handler(byte PDTemp)
{
  // Okay, now we know that at least one of our pin-of-interest is low. Which one
  //  has GONE low since the last time we called this function?
  
  // Was it pin 3, AKA PD3?
  if ((lastPC2PinState & 0x08) && !(PDTemp & 0x08))
  {
    pinFunctionHandler(PCINT_3);
  }
  else if (!(lastPC2PinState & 0x08) && (PDTemp & 0x08))
  {
    if (pinFunction[PCINT_3] == LENCODER) lastLRise = millis();
    if (pinFunction[PCINT_3] == RENCODER) lastRRise = millis();
  }
    
  lastPC2PinState = PDTemp;
}

void pinFunctionHandler(byte pinIndex)
{
  switch(pinFunction[pinIndex])
  {
    case LENCODER:
    if (lastLRise + ENC_HIGH_DELAY < micros()) encoderObject->wheelTick(LEFT);
    //encoderObject->wheelTick(LEFT);
      break;
    case RENCODER:
      if (lastRRise + ENC_HIGH_DELAY < micros()) encoderObject->wheelTick(RIGHT);
    //encoderObject->wheelTick(RIGHT);
      break;
  }
}

void setPinChangeInterrupt(int pin, byte role)
{
  switch(pin)
  {
    // Start with the analog pins, and provide a means for either the analog
    //  name or the digital name to enter that case.
    case A0:    // PCINT 8: PCMSK1, bit 0, PC0
      PCMSK1 |= 0x01;  // Enable the pin change interrupt for this pin.
      PCICR |= 0x02;   // Enable pin change interrupts for this group.
      pinFunction[PCINT_A0] = role; // Set the role for this pin- ENCODER,
                                    //  whisker, serial, etc.
      PCMask |= 0x01;  // Add this pin to our mask bits for Port C.
      break;
    case A1:    // PCINT 9: PCMSK1, bit 1, PC1
      PCMSK1 |= 0x02;  // Enable the pin change interrupt for this pin.
      PCICR |= 0x02;   // Enable pin change interrupts for this group.
      pinFunction[PCINT_A1] = role; // Set the role for this pin- ENCODER,
                                    //  whisker, serial, etc.
      PCMask |= 0x02;  // Add this pin to our mask bits for Port C.
      break;
  }
}

