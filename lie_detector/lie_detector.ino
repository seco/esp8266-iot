const int touchPin = 0;
const int potPin = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(touchPin, INPUT);
  pinMode(potPin, INPUT);

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int potVal = analogRead(potPin);
  int touchVal = analogRead(touchPin);

  Serial.print("Pot val = ");
  Serial.println(potVal);

  Serial.print("Touch val = ");
  Serial.println(touchVal);

  delay(2000);
}
