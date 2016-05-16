/*  L298N_Dual_H_Bridge

  This program controls 2 separate DC motors using the L298N
  H Bridge Chip.


           THE SUBROUTINES WORK AS FOLLOWS

  motorA(mode, speed)
  % replace A with B to control motor B %

  mode is a number 0 -> 3 that determines what the motor
  will do.
  0 = coast/disable the H bridge
  1 = turn motor clockwise
  2 = turn motor counter clockwise
  3 = brake motor

  speed is a number 0 -> 100 that represents percentage of
  motor speed.
  0 = off
  50 = 50% of full motor speed
  100 = 100% of full motor speed

  EXAMPLE
  Say you need to have motor A turn clockwise at 33% of its
  full speed.  The subroutine call would be the following...

  motorA(1, 33);

  Created by
  Iain Portalupi http://www.youtube.com/iainportalupi
  1/2/2014

  This code is in the public domain.
*/

#define ENA 12  //enable A on pin 5 (needs to be a pwm pin)
#define ENB 11  //enable B on pin 3 (needs to be a pwm pin)
#define IN1 2  //IN1 on pin 2 conrtols one side of bridge A
#define IN2 4  //IN2 on pin 4 controls other side of A
#define IN3 7  //IN3 on pin 6 conrtols one side of bridge B
#define IN4 8  //IN4 on pin 7 controls other side of B

#define delay_duration 1000
void setup()
{
  //set all of the outputs
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop()
{

  int power = map(2, 0, 100, 0, 255);

  //go forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, power);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
  analogWrite(ENB, power);
  digitalWrite(ENB, HIGH);

  //stop
  delay(3000);


  //go back
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, power);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  analogWrite(ENB, power);
  digitalWrite(ENB, HIGH);


  delay(5000);

  //  digitalWrite(IN3, LOW);
  //  digitalWrite(IN4, LOW);
  //  analogWrite(ENB, duty);
  //  digitalWrite(ENB, HIGH);
  //
  //
  //  digitalWrite(IN1, LOW);
  //  digitalWrite(IN2, LOW);
  //  analogWrite(ENA, duty);
  //  digitalWrite(ENA, HIGH);


  //  delay(5000);

  //  motorA(1, 15);  //have motor A turn clockwise at 15% speed
  //
  //  delay(delay_duration);  //let motor A run for 5 seconds
  //
  //  motorA(3, 100);  //brake motor A with 100% braking power
  //
  //  motorB(1, 15);  //have motor B turn clockwise at 15% speed
  //
  //  delay(delay_duration);  //let motor B run for 5 seconds
  //
  //  //have motor A turn counter-clockwise at 15% speed
  //  motorA(2, 15);
  //
  //  delay(delay_duration);  //let motor A and motor B run for 5 seconds
  //
  //  motorB(3, 50);  //brake motor B with 50% braking power
  //
  //  motorA(0, 100);  //let motor A coast
  //
  //  delay(delay_duration);  //wait 5 seconds

}

//******************   Motor A control   *******************
void motorA(int mode, int percent)
{

  //change the percentage range of 0 -> 100 into the PWM
  //range of 0 -> 255 using the map function
  int duty = map(percent, 0, 100, 0, 255);

  switch (mode)
  {
    case 0:  //disable/coast
      digitalWrite(ENA, LOW);  //set enable low to disable A
      break;

    case 1:  //turn clockwise
      //setting IN1 high connects motor lead 1 to +voltage
      digitalWrite(IN1, HIGH);

      //setting IN2 low connects motor lead 2 to ground
      digitalWrite(IN2, LOW);

      //use pwm to control motor speed through enable pin
      analogWrite(ENA, duty);

      break;

    case 2:  //turn counter-clockwise
      //setting IN1 low connects motor lead 1 to ground
      digitalWrite(IN1, LOW);

      //setting IN2 high connects motor lead 2 to +voltage
      digitalWrite(IN2, HIGH);

      //use pwm to control motor speed through enable pin
      analogWrite(ENA, duty);

      break;

    case 3:  //brake motor
      //setting IN1 low connects motor lead 1 to ground
      digitalWrite(IN1, LOW);

      //setting IN2 high connects motor lead 2 to ground
      digitalWrite(IN2, LOW);

      //use pwm to control motor braking power
      //through enable pin
      analogWrite(ENA, duty);

      break;
  }
}
//**********************************************************


//******************   Motor B control   *******************
void motorB(int mode, int percent)
{

  //change the percentage range of 0 -> 100 into the PWM
  //range of 0 -> 255 using the map function
  int duty = map(percent, 0, 100, 0, 255);

  switch (mode)
  {
    case 0:  //disable/coast
      digitalWrite(ENB, LOW);  //set enable low to disable B
      break;

    case 1:  //turn clockwise
      //setting IN3 high connects motor lead 1 to +voltage
      digitalWrite(IN3, HIGH);

      //setting IN4 low connects motor lead 2 to ground
      digitalWrite(IN4, LOW);

      //use pwm to control motor speed through enable pin
      analogWrite(ENB, duty);

      break;

    case 2:  //turn counter-clockwise
      //setting IN3 low connects motor lead 1 to ground
      digitalWrite(IN3, LOW);

      //setting IN4 high connects motor lead 2 to +voltage
      digitalWrite(IN4, HIGH);

      //use pwm to control motor speed through enable pin
      analogWrite(ENB, duty);

      break;

    case 3:  //brake motor
      //setting IN3 low connects motor lead 1 to ground
      digitalWrite(IN3, LOW);

      //setting IN4 high connects motor lead 2 to ground
      digitalWrite(IN4, LOW);

      //use pwm to control motor braking power
      //through enable pin
      analogWrite(ENB, duty);

      break;
  }
}
//**********************************************************
