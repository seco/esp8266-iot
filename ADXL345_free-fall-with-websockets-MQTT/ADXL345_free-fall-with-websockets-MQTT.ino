//Arduino 1.0+ Only!
//Arduino 1.0+ Only!

#include <Wire.h>
#include <ADXL345.h>
#include <ESP8266WiFi.h>
//#include <ESP8266HTTPClient.h>
//#include <DHT.h>
#include "PubSubClient.h"
const char* mqtt_server = "ec2-54-84-99-110.compute-1.amazonaws.com";

WiFiClient espClient;
PubSubClient client(espClient);


ADXL345 adxl; //variable adxl is an instance of the ADXL345 library
//DHT dht(16, DHT11, 15);
const int RED_LED = 15;
const int BLUE_LED = 4;
const int GREEN_LED = 5;

void flashLeds(int state)
{

  if (state == HIGH)
  {
    analogWrite(RED_LED, 10);
    analogWrite(BLUE_LED , 10);
    analogWrite(GREEN_LED, 10);
  }
  else
  {
    analogWrite(RED_LED, 0);
    analogWrite(BLUE_LED , 0);
    analogWrite(GREEN_LED, 0);
  }



  //  digitalWrite(RED_LED, state);
  //  digitalWrite(BLUE_LED , state);
  //  digitalWrite(GREEN_LED, state);

  //  delay(1000);
  //  digitalWrite(RED_LED, LOW);
  //  digitalWrite(BLUE_LED , LOW);
  //  digitalWrite(GREEN_LED, LOW);

}

void setup() {
  Serial.begin(9600);
  adxl.powerOn(14, 12);
  //dht.begin();
  pinMode(A0, INPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?

  //look of activity movement on this axes - 1 == on; 0 == off
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);

  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);

  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(1);
  adxl.setTapDetectionOnY(1);
  adxl.setTapDetectionOnZ(1);

  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(5000); //62.5mg per increment
  adxl.setTapDuration(15); //625μs per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment

  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(0x05); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(0x5); //(20 - 70) recommended - 5ms per increment

  //setting all interupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );

  //register interupt actions - 1 == on; 0 == off
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

  //wifi setup

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println("shivashambu");

  WiFi.begin("shivashambu", "2013846675-86");
  // WiFi.begin("attwifi", "2013846675");
  while (WiFi.status() != WL_CONNECTED) {
    flashLeds(HIGH);
    delay(500);
    Serial.print(".");
    flashLeds(LOW);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {


  //read temp

  float ADC = analogRead(A0) / 1024.0;
  float temperatureC = (ADC - 0.5) * 100 ;
  //Serial.print(temperatureC); Serial.println(" degrees C");
  int temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF); Serial.println(" degrees F");


  //Boring accelerometer stuff
  int x, y, z;
  adxl.readAccel(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

  char str[512];
  sprintf(str, "{\"coordinates\":\"%d,%d,%d,%d\"}", x, y, z, temperatureF);
  Serial.println(str); 
  postToServer(str);
  // Output x,y,z values - Commented out
  //  Serial.print("g on x, y and z axis -  "); Serial.print(x);Serial.print(", ");
  //  Serial.print(y);Serial.print(", ");
  //  Serial.println(z);


  //Fun Stuff!
  //read interrupts source and look for triggerd actions

  //getInterruptSource clears all triggered actions after returning value
  //so do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();

  // freefall
  if (adxl.triggered(interrupts, ADXL345_FREE_FALL)) {
    Serial.println("freefall");
    //add code here to do when freefall is sensed
  }

  //inactivity
  if (adxl.triggered(interrupts, ADXL345_INACTIVITY)) {
    Serial.println("inactivity");
    //add code here to do when inactivity is sensed
  }

  //activity
  if (adxl.triggered(interrupts, ADXL345_ACTIVITY)) {
    Serial.println("activity");
    //add code here to do when activity is sensed
  }

  //double tap
  if (adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)) {
    Serial.println("double tap");
    //add code here to do when a 2X tap is sensed
  }

  //tap
  if (adxl.triggered(interrupts, ADXL345_SINGLE_TAP)) {
    Serial.println("tap");
    if (adxl.isTapSourceOnX())
      postToServer("{\"xaxis_tapped\":true}");
    else if (adxl.isTapSourceOnY())
      postToServer("{\"yaxis_tapped\":true}");
    else if (adxl.isTapSourceOnZ())
      postToServer("{\"zaxis_tapped\":true}");
    //add code here to do when a tap is sensed
  }
  //freefall
  if (adxl.triggered(interrupts, ADXL345_FREE_FALL)) {
    Serial.println("free fall");
    postToServer("{\"freefall\":true}");
    //add code here to do when a tap is sensed
  }


  //dht
  //  float h = dht.readHumidity();
  //  float t = dht.readTemperature(true);
  //  if (isnan(h) || isnan(t)) {
  //    Serial.println("Failed to read from DHT sensor!");
  //    delay(1000);
  //    return;
  //  }
  //
  //  Serial.print("Temperature: ");
  //  Serial.print(t);
  //  Serial.print(" degrees Celcius Humidity: ");
  //  Serial.print(h);

  //tmp35


  //  int reading = analogRead(A0);
  //  float voltage = reading * 5.0;
  //  voltage /= 1024.0;
  //
  //  // print out the voltage
  //  Serial.print(voltage); Serial.println(" volts");
  //
  //  // now print out the temperature
  //  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //  //to degrees ((voltage - 500mV) times 100)
  //  Serial.print(temperatureC); Serial.println(" degrees C");
  //
  //  // now convert to Fahrenheit
  //  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  //  Serial.print(temperatureF); Serial.println(" degrees F");


  flashLeds( HIGH);
  delay(1000);
  flashLeds( LOW);
  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void postToServer(char* data) {


  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println(data);
  client.publish("outTopic", data);

}
