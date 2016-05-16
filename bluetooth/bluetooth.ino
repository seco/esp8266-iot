int ledPin = 13;
int state = 0;
int flag = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(12, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(9600); // Default connection rate for my BT module
}

void loop() {

  if (Serial.available() > 0) {
    state = Serial.read();
    Serial.println(state                                    );
    flag = 0;
  }

  if (state == '0') {
    digitalWrite(ledPin, LOW);
    digitalWrite(12, LOW);
    if (flag == 0) {
      Serial.println("LED: off");
      digitalWrite (13, LOW);
      flag = 1;
    }
  }

  else if (state == '1') {
    digitalWrite(ledPin, HIGH);
    digitalWrite(12, HIGH);
    if (flag == 0) {
      Serial.println("LED: on");
      digitalWrite (13, HIGH);
      flag = 1;
    }
  }
}
