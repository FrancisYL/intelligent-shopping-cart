#include "Cart.h"

extern Encoder *encoderObject;  // The motors class collaborate with 

// Public Functions
/* This function return an instance of two motors, the right and the left motor */
Motors::Motors(int ctrl_r_1, int ctrl_r_2, int ctrl_l_1, int ctrl_l_2) {
  pinMode(ctrl_r_1, OUTPUT);
  pinMode(ctrl_r_2, OUTPUT);
  pinMode(ctrl_l_1, OUTPUT);
  pinMode(ctrl_l_2, OUTPUT);
  _ctrl_r_1 = ctrl_r_1;
  _ctrl_r_2 = ctrl_r_2;
  _ctrl_l_1 = ctrl_l_1;
  _ctrl_l_2 = ctrl_l_2;
}

/* Drive both motors, speed > 0 to move forward, else move backward */
void Motors::drive(int speed) {
  if (speed > 0) {
    rightFwd((byte)(abs(speed)));
    leftRev((byte)(abs(speed)));
  }
  else {
    rightRev((byte)(abs(speed)));
    leftFwd((byte)(abs(speed)));
  }
}

/* Drive both motors with a duration of time */
void Motors::drive(int speed, int duration) {
  if (speed > 0) {
    rightFwd((byte)(abs(speed)));
    leftRev((byte)(abs(speed)));
  }
  else {
    rightRev((byte)(abs(speed)));
    leftFwd((byte)(abs(speed)));
  }
  delay(duration);
  rightStop();
  leftStop();
}

/* Drive both motors in the reverse direction, speed > 0 to anticlockwise, else to clockwise*/
void Motors::pivot(int speed) {
  if (speed > 0) {
    rightFwd((byte)(abs(speed)));
    leftFwd((byte)(abs(speed)));
  }
  else {
    rightRev((byte)(abs(speed)));
    leftRev((byte)(abs(speed)));
  }
}

/* Drive both motors in the reverse direction with a duration of time */
void Motors::pivot(int speed, int duration) {
  if (speed > 0) {
    rightFwd((byte)(abs(speed)));
    leftFwd((byte)(abs(speed)));
  }
  else {
    rightRev((byte)(abs(speed)));
    leftRev((byte)(abs(speed)));
  }
  delay(duration);
  rightStop();
  leftStop();
}

/* Stop both motors */
void Motors::stop() {
  rightStop();
  leftStop();
}

/* Brake both motors */
void Motors::brake() {
  rightBrake();
  leftBrake();
}

/* Only drive the right motor */
void Motors::rightMotor(int speed) {
  if (speed > 0) {
    rightFwd((byte)(abs(speed)));
  }
  else {
    rightRev((byte)(abs(speed)));
  }
}

/* Drive the right motor with a duration of time */
void Motors::rightMotor(int speed, int duration) {
  if (speed > 0) {
    rightFwd((byte)(abs(speed)));
  }
  else {
    rightRev((byte)(abs(speed)));
  }
  delay(duration);
  rightStop();
}

/* Stop the right motor */
void Motors::rightStop() {
  digitalWrite(_ctrl_r_1, LOW);
  digitalWrite(_ctrl_r_2, LOW);
}

/* Brake the right motor immediately */
void Motors::rightBrake() {
  digitalWrite(_ctrl_r_1, HIGH);
  digitalWrite(_ctrl_r_2, HIGH);
}

/* Only drive the left motor */
void Motors::leftMotor(int speed) {
  if (speed > 0) {
    leftRev((byte)(abs(speed)));
  }
  else {
    leftFwd((byte)(abs(speed)));
  }
}

/* Drive the left motor with a duration of time */
void Motors::leftMotor(int speed, int duration) {
  if (speed > 0) {
    leftRev((byte)(abs(speed)));
  }
  else {
    leftFwd((byte)(abs(speed)));
  }
  delay(duration);
  leftStop();
}

/* Stop the left motor */
void Motors::leftStop() {
  digitalWrite(_ctrl_l_1, LOW);
  digitalWrite(_ctrl_l_2, LOW);
}

/* Brake the left motor immediately */
void Motors::leftBrake() {
  digitalWrite(_ctrl_l_1, HIGH);
  digitalWrite(_ctrl_l_2, HIGH);
}

/* Private Functions */
/* Righ motor rotates forward */
void Motors::rightFwd(byte speed) {
  analogWrite(_ctrl_r_1, 0);
  analogWrite(_ctrl_r_2, speed);
  // If we have an encoder in the system, we want to make sure that it counts
  //  in the right direction when ticks occur.
  if (encoderObject != 0) {
    encoderObject->rDir = 1;
  }
}

/* Right motor rotates backward */
void Motors::rightRev(byte speed) {
  analogWrite(_ctrl_r_1, speed);
  analogWrite(_ctrl_r_2, 0);
  // If we have an encoder in the system, we want to make sure that it counts
  //  in the right direction when ticks occur.
  if (encoderObject != 0) {
    encoderObject->rDir = -1;
  }
}

/* Left motor rotates forward */
void Motors::leftFwd(byte speed) {
  analogWrite(_ctrl_l_1, 0);
  analogWrite(_ctrl_l_2, speed);
  // If we have an encoder in the system, we want to make sure that it counts
  //  in the right direction when ticks occur.
  if (encoderObject != 0) {
    encoderObject->lDir = -1;
  }
}

/* Left motor rotates backward */
void Motors::leftRev(byte speed) {
  analogWrite(_ctrl_l_1, speed);
  analogWrite(_ctrl_l_2, 0);
  // If we have an encoder in the system, we want to make sure that it counts
  //  in the right direction when ticks occur.
  if (encoderObject != 0) {
    encoderObject->lDir = 1;
  }
}

