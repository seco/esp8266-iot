
#define ENA 16  //enable A on pin 5 (needs to be a pwm pin)
#define ENB 14  //enable B on pin 3 (needs to be a pwm pin)
#define IN1 12  //IN1 on pin 2 conrtols one side of bridge A
#define IN2 13  //IN2 on pin 4 controls other side of A
#define IN3 5  //IN3 on pin 6 conrtols one side of bridge B
#define IN4 4  //IN4 on pin 7 controls other side of B

void setup()
{
  Serial.begin(115200);

  //set all of the outputs
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
bool s = false;
void loop()
{

  delay(1000);


  Serial.printf("Left Forward!\n");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 500);

  delay(5000);
  analogWrite(ENA, LOW);

  Serial.printf("Right Forward!\n");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 700);
  delay(5000);
  analogWrite(ENB, LOW);

  //    motorA(1, 15);  //have motor A turn clockwise at 15% speed
  //
  //    delay(1000);  //let motor A run for 5 seconds
  //
  //    motorA(3, 100);  //brake motor A with 100% braking power
  //
  //    motorB(1, 15);  //have motor B turn clockwise at 15% speed
  //
  //    delay(1000);  //let motor B run for 5 seconds
  //
  //    //have motor A turn counter-clockwise at 15% speed
  //    motorA(2, 15);
  //
  //    delay(1000);  //let motor A and motor B run for 5 seconds
  //
  //    motorB(3, 50);  //brake motor B with 50% braking power
  //
  //    motorA(0, 100);  //let motor A coast
  //
  //    delay(1000);  //wait 5 seconds

}

//******************   Motor A control   *******************
void motorA(int mode, int percent)
{

  Serial.print("mode="); Serial.print(mode); Serial.print(", percent="); Serial.print(percent);

  //change the percentage range of 0 -> 100 into the PWM
  //range of 0 -> 255 using the map function
  int duty = map(percent, 0, 100, 600, 1023);
  Serial.print(", duty="); Serial.println(duty);

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
