#include  <Servo.h>

#define ENA 16  //enable A on pin 5 (needs to be a pwm pin)
#define ENB 14  //enable B on pin 3 (needs to be a pwm pin)
#define IN1 12  //IN1 on pin 2 conrtols one side of bridge A
#define IN2 13  //IN2 on pin 4 controls other side of A
#define IN3 5  //IN3 on pin 6 conrtols one side of bridge B
#define IN4 4  //IN4 on pin 7 controls other side of B



#define SERVO_PIN 0 
#define echoPin 2 // Echo Pin
#define trigPin 15 // Trigger Pin

#define LEDPin 10 // Onboard LED

#define delay_duration 1000


int ledPin = 13;
int state = 0;
int flag = 0;
int power = 0;

int maximumRange = 30; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance
Servo myServo;
int servoAngle = 0;

void setup()
{

  power = map(100, 0, 100, 0, 255);

  //set all of the outputs
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)

  Serial.begin(9600); // Default connection rate for my BT module

  myServo.attach(SERVO_PIN);
  myServo.write(90);

}

void senseObstacles()
{
  //if (flag == 1) {


  //int sd = senseDistance();
  //Serial.print    ("sensing obstacles, distance = "); Serial.println(sd);

  if ( isThereObstacleInFrontOfMe() == true)
  {
    //
    Serial.println("sensed obstacle");
    stop();
    delay(100);

    int d = 0;


    servoAngle = 0;
    moveServo(0);
    int maxObstacleFreeDistanceCw = 0;
    int maxObstacleFreeDistanceCcw = 0;
    //sweep cw
    while (  servoAngle <= 90)
    {
      moveServo(servoAngle);
      delay(20);
      d = senseDistance();
      servoAngle += 10;

      //if (d > maxObstacleFreeDistanceCw)
      maxObstacleFreeDistanceCw += d;

      Serial.print("max obstacle free distance cw ="); Serial.println(maxObstacleFreeDistanceCw);
      Serial.print("moving cw...servo angle="); Serial.println(servoAngle);
    }

    Serial.println("cw sweep complete no success :(");

    //sweep ccw
    while (servoAngle <= 180)
    {
      moveServo(servoAngle);
      delay(20);
      d = senseDistance();
      servoAngle += 10;

      //if (d > maxObstacleFreeDistanceCcw)
      maxObstacleFreeDistanceCcw += d;

      Serial.print("max obstacle free distance ccw ="); Serial.println(maxObstacleFreeDistanceCcw);

      Serial.print("distance="); Serial.println(d);
      Serial.print("moving ccw...servo angle="); Serial.println(servoAngle);
    }
    Serial.println("no  obstacle :)");


    goBack();
    delay(500);
    stop();



    int relativeAngle = servoAngle;
    //float offset = 2;

    debugPause(2000);

    moveServo(90);

    //delay(2000);

    if ( maxObstacleFreeDistanceCw > maxObstacleFreeDistanceCcw)
    {
      Serial.println("motor move cw ");
      //Serial.println(abs(relativeAngle) * offset + 50);
      // goClockwise( relativeAngle * offset + 50);
      d = 0;
      do
      {
        //d = senseDistance();
        delay(200);
        goClockwise(300);

        Serial.println("motor move cw in steps , d ="); Serial.print(d);
        debugPause(2000);

      } while ( isThereObstacleInFrontOfMe() == true );
    }
    else
    {
      Serial.println("motor move ccw ");
      //Serial.println(abs(relativeAngle) * offset + 50);
      //goAntiClockwise( relativeAngle * offset + 50);
      d = 0;
      do
      {
        //d = senseDistance();
        delay(200);
        goAntiClockwise(300);

        Serial.println("motor move ccw in steps ");
        debugPause(2000);

      } while ( isThereObstacleInFrontOfMe() == true );

      // if (state == '1')
      // {


      //debugPause(3000);
    }

    flag = 0;


  }
  else
  {
    //    if (state == '1')
    //    {
    //      Serial.println("motor move forward ... ");
    //      goForward();
    //
    //    }
  }
  //}
}


void loop()
{



  senseObstacles();


  if (Serial.available() > 0) {
    state = Serial.read();
    flag = 0;

    Serial.print(state);
  }



  if (state == '0') {
    digitalWrite(ledPin, LOW);
    digitalWrite(12, LOW);
    if (flag == 0) {

      Serial.println("stopped");
      flag = 1;
      stop();
    }
  }

  else if (state == '1') {
    digitalWrite(ledPin, HIGH);
    digitalWrite(12, HIGH);
    if (flag == 0) {
      Serial.println("moving forward");
      digitalWrite (13, LOW);
      flag = 1;

      //go forward
      goForward();
      //moveServo(0);

    }
  }

  else if (state == '2') {
    digitalWrite(ledPin, HIGH);
    digitalWrite(12, HIGH);
    if (flag == 0) {
      Serial.println("moving backward");
      digitalWrite (13, HIGH);
      flag = 1;
      goBack();
      // moveServo(180);

    }
  }

  else if (state == '3') {
    digitalWrite(ledPin, HIGH);
    digitalWrite(12, HIGH);
    if (flag == 0) {
      Serial.println("clockwise");
      digitalWrite (13, HIGH);
      flag = 1;
      goClockwise(50);

    }
  }

  else if (state == '4') {
    digitalWrite(ledPin, HIGH);
    digitalWrite(12, HIGH);
    if (flag == 0) {
      Serial.println("counter clockwise");
      digitalWrite (13, HIGH);
      flag = 1;
      goAntiClockwise(50);

    }
  }

  delay(100);

}


void  debugPause(int time)
{
  return;
  delay(time);
  Serial.println("debug pause");
}


void goAntiClockwise(int angle)
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, power);
  digitalWrite(ENA, LOW);


  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  analogWrite(ENB, power);
  digitalWrite(ENB, HIGH);
  delay(angle);
  stop();

  //moveServo(0);
}

void goClockwise(int angle)
{


  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, power);
  digitalWrite(ENA, LOW);


  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
  analogWrite(ENB, power);
  digitalWrite(ENB, HIGH);
  delay(angle);
  stop();

  //moveServo(10);
}

void goForward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, power);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
  analogWrite(ENB, power);
  digitalWrite(ENB, HIGH);


  //moveServo(0);
}


void goBack()
{
  //go back
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, power);
  digitalWrite(ENA, HIGH);

  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
  analogWrite(ENB, power);
  digitalWrite(ENB, HIGH);

  // moveServo(170);

}

void moveServo(int angle)
{
  // myServo.attach(9);
  // delay(100);
  myServo.write(angle);
  // delay(1000);
  //  myServo.detach();
}


void stop()
{

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(ENB, LOW);
  digitalWrite(ENA, LOW);
}


bool isThereObstacleInFrontOfMe()
{
  int distance = senseDistance();


  if (distance >= maximumRange || distance <= minimumRange)
  {
    return false;
  }
  else {
    return true;
  }

}

int senseDistance()
{

  int distance = -1;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration / 58.2;
  return distance;

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
