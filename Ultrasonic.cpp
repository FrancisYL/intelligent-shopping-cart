#include "Cart.h"

#define VELOCITY_SOUND_PER_MICROSECOND 340.0 * 0.000001

/* This constructor returns an instance of ultrasonic sensor */
Ultrasonic::Ultrasonic(int trig, int echo) {
	pinMode(trig, OUTPUT);
	pinMode(echo, INPUT);
	_trig = trig;
	_echo = echo;
}

/* This function returns the distance between a detected obstacle in meter */
float Ultrasonic::detect() {
  // Send the ultrasonic signal for 10us
	digitalWrite(_trig, HIGH);
	delay(0.01);
	digitalWrite(_trig, LOW);

  // Record the start and end time of receiving the reflected signal
	while(digitalRead(_echo) == LOW);
  unsigned long startTime = micros();
  while(digitalRead(_echo) == HIGH);
  unsigned long endTime = micros();

  // Calculate the distance with the detected object
	return (endTime - startTime) * VELOCITY_SOUND_PER_MICROSECOND / 2.0;
}

