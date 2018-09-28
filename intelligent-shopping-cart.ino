#include "Cart.h"

#define SPEED          130
#define UNIT_TIME      50
#define UNIT_DEGREE    3.0
#define UNIT_DISTANCE  2.0
#define COUNTSPERDEG   1.25
#define COUNTSPERCM    9.40
#define SAFE_DISTANCE  0.08
#define MIDDLEX        160
#define MIDDLEHEIGHT   90
#define CHANGE_CART_TIME 2000
#define CHECKOUTCC     140  // Checkout color code number

Pixy pixy;
Motors motors = Motors(CTRL_R_1, CTRL_R_2, CTRL_L_1, CTRL_L_2);
Encoder encoder = Encoder(LPIN, RPIN);
Ultrasonic sonic = Ultrasonic(TRIG, ECHO);
XBee xbee = XBee();

float xCur = 0.0;
float yCur = 0.0;
float orient = 0.0;
float xGoal = 0.0;
float yGoal = 0.0;
int followCC = 0;

void setup() {
  pinMode(FOLLOWBUTTON, INPUT_PULLUP);
  pinMode(CHECKOUTBUTTON, INPUT_PULLUP);
  Serial.begin(9600);
  pixy.init();
  set_cc();
}

void loop() {

  
  // Press the Follow Button to start auto-following function
  if (digitalRead(FOLLOWBUTTON) == LOW) {
    delay(1000);
    follow_mode();
  }

  // Wait to receive the location information
  if (Serial.read() == '<') {
    find_mode();
  }
}

/* Set the following color code value */
void set_cc() {
  bool findCC = false;

  // Set the first color code seen by the pixy to be the object to follow
  while (findCC != true) {
    int blocks = pixy.getBlocks();
    if (blocks) {
      followCC = pixy.blocks[0].signature;
      Serial.println(followCC);
      findCC = true;
    }
    delay(1);
  }

  // Initialize the starting coordinates
  float distance = sonic.detect() * 100.0;
  if (followCC == 19) {
    orient = -180;
    xCur = 61;
    yCur = distance;
  }
  else if (followCC == 11) {
    orient = -180;
    xCur = 15;
    yCur = distance;
  }
}

/* The follow_mode function consists of three steps */
void follow_mode() {
  set_cc();
  auto_follow_algorithm();
  self_checkout();
}

/* Auto follow algorithm */
void auto_follow_algorithm() {
  // Press the Checkout Button to exit auto-following mode
  while (digitalRead(CHECKOUTBUTTON) != LOW) {
    // Object avoidance, move backward if too close to any object
    float distance = sonic.detect();
    if (distance < SAFE_DISTANCE) {
      move_straight(-UNIT_DISTANCE, SPEED);
    }
    
    // CC following
    int blocks = pixy.getBlocks();
    if (blocks) {
      // Find the following CC
      int index = -1;
      for (int i = 0; i < blocks; i++) {
        if (pixy.blocks[i].signature == followCC) {
          index = i;
          break;
        }
      }
      if (index == -1) continue;
      
      // Follow control
      int speed = 0;
      if (pixy.blocks[index].x <= 130) {
        speed = turn_speed(MIDDLEX - pixy.blocks[index].x);
        Serial.print("Left:"); Serial.println(speed);
        make_turn(-UNIT_DEGREE, speed);
      }
      else if (pixy.blocks[index].x >= 190) {
        speed = turn_speed(pixy.blocks[index].x - MIDDLEX);
        Serial.print("Right:"); Serial.println(speed);
        make_turn(UNIT_DEGREE, speed);
      }
      else {
        if (pixy.blocks[index].height >= 110) {
          speed = straight_speed(pixy.blocks[index].height - MIDDLEHEIGHT);
          Serial.print("Backward:"); Serial.println(speed);
          move_straight(-UNIT_DISTANCE, speed);
        }
        else if (pixy.blocks[index].height <= 70) {
          speed = straight_speed(MIDDLEHEIGHT - pixy.blocks[index].height);
          Serial.print("Forward:"); Serial.println(speed);
          move_straight(UNIT_DISTANCE, speed);
        }
        else motors.stop();
      }
    }
    delay(1);
  }

  delay(1000);
  unsigned long startTime = millis();
  while (digitalRead(CHECKOUTBUTTON) == LOW);
  unsigned long endTime = millis();

  if ((startTime - endTime) > CHANGE_CART_TIME) {
    send_info();
  }
}

/* Self checkout algorithm */
void self_checkout() {
  bool findCheckout = false;
  bool arriveCheckout = false;

  // Find the checkout color code
  motors.pivot(SPEED);
  while (findCheckout != true) {
    int blocks = pixy.getBlocks();
    if (blocks) {
      for (int i = 0; i < blocks; i++) {
        if (pixy.blocks[i].signature == CHECKOUTCC) {
          findCheckout = true;
          break;
        }
      }
    }
  }
  motors.brake();

  // Forward to the checkout point
  while (arriveCheckout != true) {
    float distance = sonic.detect();
    if (distance < (SAFE_DISTANCE - 0.03)) {
      move_straight(-UNIT_DISTANCE, SPEED);
    }
    // CC following
    int blocks = pixy.getBlocks();
    if (blocks) {
      // Find the following CC
      int index = 0;
      for (int i = 0; i < blocks; i++) {
        if (pixy.blocks[i].signature == followCC) {
          index = i;
          break;
        }
      }
      
      // Follow control
      int speed = 0;
      if (pixy.blocks[index].x <= 130) {
        speed = turn_speed(MIDDLEX - pixy.blocks[index].x);
        Serial.print("Left:"); Serial.println(speed);
        make_turn(-UNIT_DEGREE, speed);
      }
      else if (pixy.blocks[index].x >= 190) {
        speed = turn_speed(pixy.blocks[index].x - MIDDLEX);
        Serial.print("Right:"); Serial.println(speed);
        make_turn(UNIT_DEGREE, speed);
      }
      else if (distance > 0.1) {
        Serial.print("Forward:"); Serial.println(SPEED);
        move_straight(UNIT_DISTANCE, SPEED);
      }
      else {
        motors.brake();
        arriveCheckout = true;
      }
    }
  }
}

/* Process the self location info and send through XBee */
void send_info() {
  xbee.message[0] = char(int(xCur) / 100) + '0';
  xbee.message[1] = char((int(xCur) % 100) / 10) + '0';
  xbee.message[2] = char((int(xCur) % 100) % 10) + '0';
  xbee.message[3] = char(int(yCur) / 100) + '0';
  xbee.message[4] = char((int(yCur) % 100) / 10) + '0';
  xbee.message[5] = char((int(yCur) % 100) % 10) + '0';
  xbee.message[6] = char(int(followCC) / 10) + '0';
  xbee.message[7] = char(int(followCC) % 10) + '0';
  while (Serial.read() != '!') {
    xbee.emit();
    delay(50);
  }
}

/* The find_mode function consists of three steps */
void find_mode() {
  while (xbee.receive() != true); // Wait until receiving the message
  convert();  // Convert the message from characters to integers
  delay(3000);
  move_toward_goal();

  // Find the corresponding color code
  bool findCC = false;
  motors.pivot(SPEED);
  while (findCC != true) {
    int blocks = pixy.getBlocks();
    if (blocks) {
      for (int i = 0; i < blocks; i++) {
        if (pixy.blocks[i].signature == followCC) {
          findCC = true;
          break;
        }
      }
    }
  }
  motors.brake();
  
  auto_follow_algorithm();
}

/* This function convert the location and color code infromation received by the xbee */
void convert() {
  xGoal = (xbee.message[0] - '0')*100 + (xbee.message[1] - '0')*10 + (xbee.message[2] - '0');
  yGoal = (xbee.message[3] - '0')*100 + (xbee.message[4] - '0')*10 + (xbee.message[5] - '0');
  followCC = (xbee.message[6] - '0')*10 + (xbee.message[7] - '0');
}

/* This function change Robot's orientation and move forward towards the goal point */
void move_toward_goal() {
  float distance = goal_distance();        // Calculate the distance to goal point at RedBot's current location
  float degree = goal_degree(distance);    // Calculate the degree needed to turn RedBot from its current orientation to goal point
  make_turn(degree, SPEED);
  move_straight(distance, SPEED);
}

/* Calculate the proper turning speed for the cart */
int turn_speed(int offset) {
  if (offset > 90) return 180;
  else return int(offset + 90);
}

/* Calculate the proper moving speed for the cart */
int straight_speed(int offset) {
  if (offset > 30) return 180;
  else return int(6 * offset);
}

/* Make a turn for specific degree with speed, degree > 0 turns clockwise, degree < 0 turns anticlockwise */
void make_turn(float degree, int speed) {
  long counts = long(abs(degree) * COUNTSPERDEG); // The counts needed for the encoder to make such a turn

  encoder.clearEnc(BOTH); delay(1);
  if (degree > 0) {
    motors.leftMotor(speed);
    motors.rightMotor(-speed);
  }
  else {
    motors.leftMotor(-speed);
    motors.rightMotor(speed);
  }
  while ((abs(encoder.getTicks(LEFT)) + abs(encoder.getTicks(RIGHT))) < 2 * counts) {
    delay(1);
  }
  motors.brake();

  update_orientation(degree);
}

///* Make a straight move for specific distance with speed, distance > 0 to move forward, < 0 to move backward */
//void move_straight(float distance, int speed) {
//  long counts = long(abs(distance) * COUNTSPERCM); // The counts needed for the encoder to make such a driving distance
//
//  encoder.clearEnc(BOTH);
//  if (distance > 0) { 
//    motors.drive(speed);
//  }
//  else {
//    motors.drive(-speed);
//  }
//  while ((abs(encoder.getTicks(LEFT)) + abs(encoder.getTicks(RIGHT))) < 2 * counts) {
//    delay(0);
//  }
//  motors.brake();
//
//  update_coordinates(distance);
//}

/* Make a straight move for specific distance with speed, distance > 0 to move forward, < 0 to move backward */
void move_straight(float distance, int speed) {
  long lCount = 0;
  long rCount = 0;
  long prevlCount, prevrCount;
  long lDiff, rDiff;
  long targetCount = long(abs(distance) * COUNTSPERCM);
  int leftPower = speed;
  int rightPower = speed;
  int offset = 5;

  encoder.clearEnc(BOTH); delay(1);
  while (rCount+lCount < 2*targetCount)
  {
    // while the right encoder is less than the target count -- debug print 
    // the encoder values and wait -- this is a holding loop.
    lCount = abs(encoder.getTicks(LEFT));
    rCount = abs(encoder.getTicks(RIGHT));

    if (distance > 0) {
      motors.leftMotor(leftPower);
      motors.rightMotor(rightPower);
    }
    else {
      motors.leftMotor(-leftPower);
      motors.rightMotor(-rightPower);
    }

    // calculate the rotation "speed" as a difference in the count from previous cycle.
    lDiff = (lCount - prevlCount);
    rDiff = (rCount - prevrCount);
    // store the current count as the "previous" count for the next cycle.
    prevlCount = lCount;
    prevrCount = rCount;

    // if left is faster than the right, slow down the left / speed up right
    if (lDiff > rDiff) {
      leftPower = leftPower - offset;
      rightPower = rightPower + offset;
    }
    // if right is faster than the left, speed up the left / slow down right
    else if (lDiff < rDiff) {
      leftPower = leftPower + offset;  
      rightPower = rightPower - offset;
    }
    delay(1);  // short delay to give motors a chance to respond.
  }
  motors.brake();  

  update_coordinates(distance);
}

/* This function calculate the straigh distance between the RedBot and the goal point */
float goal_distance() {
  return sqrt((xGoal - xCur) * (xGoal - xCur) + (yGoal - yCur) * (yGoal - yCur));
}

/* This function calculate the degree required to turn from its orientation to the goal point */
float goal_degree(float distance) {
  float theta = 0.0;
  if (distance != 0) {
    theta = float(acos((yGoal - yCur) / distance) / PI * 180.0);
  }
  if (xGoal - xCur >= 0) return (theta - orient);
  else return (-theta - orient);
}

/* This function updates the Cart's current orientation */
void update_orientation(float degree) {
  orient = orient + degree;
  if (abs(orient) > 180) {
    if (degree > 0) orient = orient - 360;
    else orient = orient + 360;
  }
}

/* This function updates the Cart's current coordinates */
void update_coordinates(float distance) {
  xCur = xCur + sin(orient / 180.0 * PI) * distance;
  yCur = yCur + cos(orient / 180.0 * PI) * distance;
}

