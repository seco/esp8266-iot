int led = 13;

void setup ()

{

  pinMode (13, OUTPUT); // We configure the output pin 13

  digitalWrite (led, HIGH);

  Serial.begin (9600); // we begin the serial port

}

void loop ()

{

  while (Serial.available ())

  {

    Serial.read char data = ();

    if (data == 'a') {// If we send an 'a' turn off the led

      Serial.write (data);

      data = 0;

      digitalWrite (led, LOW);

    }

    if (data == 'b') {// If we send a 'b' turn on the LED

      Serial.write (data);

      data = 0;

      digitalWrite (led, HIGH);

    }

  }

}
