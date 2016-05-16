void setup()
{
  Serial.begin(9600);
  //Serial1.begin(9600);
}
void loop()
{
  

  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
}
