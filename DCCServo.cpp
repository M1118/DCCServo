#include <DCCServo.h>

DCCServo::DCCServo(int pin, int limit1, int limit2, unsigned int travelTime)
{
  this->pin = pin;
  this->limit1 = limit1;
  this->limit2 = limit2;
  this->tlimit1 = limit1 * 10;
  this->tlimit2 = limit2 * 10;
  this->travelTime = travelTime * 1000;
  this->angle = tlimit1;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
  this->refresh = millis() + this->interval;
  this->moving = false;
  this->active = false;
  this->clockwise = true;
  this->servo.attach(this->pin);
  this->servo.write(this->limit1);
  Serial.print("Servo on pin ");
  Serial.println(pin);
  Serial.print("TravelTime is ");
  Serial.println(this->travelTime);
  Serial.print("Limit1 is ");
  Serial.println(this->limit1);
  Serial.print("Limit2 is ");
  Serial.println(this->limit2);
  Serial.print("Interval is ");
  Serial.println(this->interval);
  Serial.println("-------------------------------");
}

void DCCServo::loop()
{
  int  newangle;
  
  if (this->refresh > millis())
    return;
  
  if (this->active == false || this->moving == false)
  {
    this->refresh = millis() + this->interval;
    return;
  }

  newangle = this->angle;
  if (this->clockwise && this->angle < this->tlimit2)
    newangle = this->angle + 1;
  else if (this->clockwise == false && this->angle > this->tlimit1)
    newangle = this->angle - 1;
  else
    this->moving = false;


  if (newangle != this->angle)
  {
    this->writeTenths(newangle);
    this->angle = newangle;
  }
  unsigned long delta = (this->interval * 100) / this->percentage;
  this->refresh = millis() + delta;
 
}

void DCCServo::setSpeed(int percentage, boolean clockwise)
{
  this->percentage = percentage;
  this->clockwise = clockwise;
  this->moving = (percentage > 0);
}

void DCCServo::setActive(boolean active)
{
  if ((active == true) && (this->active == false))
  {
    this->servo.attach(this->pin);
    this->refresh = millis() + ((this->interval * 100) / this->percentage);
  }
  else if ((active == false) && (this->active == true))
  {
    this->servo.detach();
  }
  this->active = active;
}

void DCCServo::setStart(int start)
{
  if (start < 0)
    start = 0;
  else if (start > 180)
    start = 180;
  this->limit1 = start;
  this->tlimit1 = start * 10;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
}

void DCCServo::setEnd(int angle)
{
  if (angle < 0)
    angle = 0;
  else if (angle > 180)
    angle = 180;
  this->limit2 = angle;
  this->tlimit2 = angle * 10;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
}

void DCCServo::setTravelTime(int time)
{
  this->travelTime = time * 1000;
  this->interval = this->travelTime / (this->tlimit2 - this->tlimit1);
}

#define SERVO_MIN() (MIN_PULSE_WIDTH)  // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH)  // maximum value in uS for this servo 

void DCCServo::writeTenths(int tenth)
{
	this->servo.writeMicroseconds(map(tenth, 0, 1800,
					SERVO_MIN(),  SERVO_MAX()));
}
