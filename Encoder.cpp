#include "Cart.h"

Encoder *encoderObject = 0; // Create a local pointer to an instance of this
                          //  class, so we can edit the counts with other
                          //  library functions.

/* Public Functions */
/* This function return an instance of encoder, the right and the left encoder */
Encoder::Encoder(int lPin, int rPin) {
  pinMode(lPin, INPUT_PULLUP);
  pinMode(rPin, INPUT_PULLUP);
  setPinChangeInterrupt(lPin, LENCODER);
  setPinChangeInterrupt(rPin, RENCODER);
  lCounts = 0;
  rCounts = 0;
  encoderObject = this; // We want a local pointer to the class member that is
                        // instantiated in the sketch, so we can manipulate its
                        // private members with other classes.
  lDir = 1;		          // default direction to forward -- used for encoder counting
  rDir = 1;		          // default direction to forward -- used for encoder counting
}

/* Public function to clear the encoder counts */
void Encoder::clearEnc(WHEEL wheel) {
  switch(wheel) {
    case LEFT:
      lCounts = 0;
      break;
    case RIGHT:
      rCounts = 0;
      break;
    case BOTH:
      lCounts = 0;
      rCounts = 0;
      break;
  }
}

/* Public function to read the encoder counts for a given wheel */
long Encoder::getTicks(WHEEL wheel) {
  switch(wheel) {
    case LEFT:
      return lCounts;
    case RIGHT:
      return rCounts;
    case BOTH:
      return lCounts + rCounts;
  }
  return 0;
}

/* Private Functions */
/* This private function changes the counter when a tick happens. The direction
   is set by the functions that set the motor direction */
void Encoder::wheelTick(WHEEL wheel) {
  switch(wheel) {
    case LEFT:
      lCounts+= (long)lDir;
      break;
    case RIGHT:
      rCounts+= (long)rDir;
      break;
    case BOTH:
      break;
  }
}

