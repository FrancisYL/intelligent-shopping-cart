#ifndef Cart_h
#define Cart_h

#include <Arduino.h>
#include <math.h>
#include <Pixy.h>
#include <SPI.h>

// Define for Interrupt
#define LENCODER      2
#define RENCODER      3
#define PCINT_A0      0
#define PCINT_A1      1
#define PCINT_A2      2
#define PCINT_A3      3
#define PCINT_A4      4
#define PCINT_A5      5
#define PCINT_3       6
#define PCINT_9       7
#define PCINT_10      8
#define PCINT_11      9

// Define for Cart
#define FOLLOWBUTTON   2
#define CHECKOUTBUTTON 3
#define N              8
enum WHEEL {LEFT, RIGHT, BOTH};
const int CTRL_R_1 = 5;
const int CTRL_R_2 = 6;
const int CTRL_L_1 = 9;
const int CTRL_L_2 = 10;
const int LPIN = A0;
const int RPIN = A1;
const int TRIG = 7;
const int ECHO = 8;

// Interrupt functions
void setPinChangeInterrupt(int pin, byte role);
void pinFunctionHandler(byte pinIndex);
void PC0Handler(byte PBTemp);
void PC1Handler(byte PCTemp);
void PC2Handler(byte PDTemp);

// Declaration for class motors
class Motors {
  public:
    Motors(int ctrl_r_1, int ctrl_r_2, int ctrl_l_1, int ctrl_l_2);
    void drive(int speed);
    void drive(int speed, int duration);
    void pivot(int speed);
    void pivot(int speed, int duration);
    void stop();
    void brake();
    void rightMotor(int speed);
    void rightMotor(int speed, int duration);
    void rightStop();
    void rightBrake();
    void leftMotor(int speed);
    void leftMotor(int speed, int duration);
    void leftStop();
    void leftBrake();
  private:
    int _ctrl_r_1;
    int _ctrl_r_2;
    int _ctrl_l_1;
    int _ctrl_l_2;
    void rightFwd(byte speed);
    void rightRev(byte speed);
    void leftFwd(byte speed);
    void leftRev(byte speed);
};

// Declaration for class encoder
class Encoder
{
  friend class Motors;
  friend void pinFunctionHandler(byte pinIndex);
  
  public:
    Encoder(int lPin, int rPin);
    void clearEnc(WHEEL wheel);
    long getTicks(WHEEL wheel);
  private:
    void wheelTick(WHEEL wheel);
    long lCounts;
    long rCounts;
    char lDir;
    char rDir;
};

// Declaration for class ultrasonic
class Ultrasonic {
	public:
		Ultrasonic(int trig, int echo);
		float detect();
	private:
		int _trig;
		int _echo;
};

// Declaration for class xbee
class XBee {
  public:
    XBee();
    void emit();
    bool receive();
    void printMes();   
    char message[N];
};

#endif

