/* door lock test

This sketch tests basic functionality of an electric car door lock mechanisim
using a v2 Adafruit motor shield.

This is a test with a single mechanisim, and uses simple delay() calls to control
timing.  When multiple devices are being used, the structure will need to change
to use the current state, and timing information, to perform later operations in
later passes through void loop().  Similar to the blink without delay example
code.

For use with the Adafruit Motor Shield v2
---->	http://www.adafruit.com/products/1438
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

unsigned const int MOTOR_PORT = 1;

// Timing constants: Tests done with the door lock power directly from the Arduino 5V line.

// Time for (nearly) full travel of the lock mechanisim, at the specified speed
unsigned long const travelTime = 60;//ms
// Speed range is 0 to 255.  For now, full speed for both strike and return
unsigned int const strikeSpeed = 255;
unsigned int const liftSpeed = 255;
// Time to wait after the mechanisim is pulled in (normal 'lock' state)
// This is (will be) the delay after the stick is returned to start, before the
// next beat.
unsigned long const beatDelay = 150;// ms
// The delay at the end of the forward strike, before lifting back up.  This is
// intended to be the time for the stick to bound off of the drum head.
unsigned long const bounceTime = 50;// ms
// In the drum application, the time for one 'beat' cycle, is 2 times the travel time,
// pluss the in and out wait times.

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which motor 'port' to use on the shield
Adafruit_DCMotor *myMotor = AFMS.getMotor(MOTOR_PORT);


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Car Door Lock test for Drumming!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotor->setSpeed(150);
  myMotor->run(FORWARD);
  // turn on motor
  myMotor->run(RELEASE);
}

// For 'good' drum strikes, the mechanisim needs to accelerate the stick toward the drum,
// then 'release' it to travel the final distance without directly being 'pushed', so that
// the stick will 'bounce' off of the drum head.  As the stick comes back up, finish
// pulling it back up for the next strike.
void loop() {
  singleStrike();
  // The time before the next swing, which will need to be the beat spacing minus
  // the time needed to do a single strike and recovery
  delay(beatDelay);
}

void singleStrike() {
  Serial.println("strike");
  swingStick();
  // Wait for the bounce before pulling back
  delay(bounceTime);
  Serial.print("lift");
  recoverStick();
}

void swingStick() {
  myMotor->run(FORWARD);// swing the stick at the drum
  myMotor->setSpeed(strikeSpeed);
  // Time needed to get to the end of the travel for the lock mechanisim
  delay(travelTime);
  // Remove power to the motor while the stick bounces
  myMotor->run(RELEASE);
}

void recoverStick() {
  myMotor->run(BACKWARD);// bring the stick back to the starting position
  myMotor->setSpeed(strikeSpeed); // Currently same speed as the strike
  delay(travelTime); // and the same time to do the movement
  myMotor->run(RELEASE); // remove power
}
