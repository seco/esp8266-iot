boolean AdminEnabled = true;    // Enable Admin Mode for a given Time
const int RED_LED = 16;
const int BLUE_LED = 4;
const int GREEN_LED = 5;
const int ADMIN_MODE_PIN = 13;

#define   AdminTimeOut 60  // Defines the Time in Seconds, when the Admin-Mode will be diabled

#include <Wire.h>
#include <ADXL345.h>
#include "PubSubClient.h"

#include "TinyGPS++.h"
#include <SoftwareSerial.h>


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"
#include "global.h"
/*
  Include the HTML, STYLE and Script "Pages"
*/

#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "example.h"


#define ACCESS_POINT_NAME  "goja-ups-iot-ap"
#define ACCESS_POINT_PASSWORD  "11111111"

WiFiClient espClient;
PubSubClient client(espClient);
ADXL345 adxl; //variable adxl is an instance of the ADXL345 library
//DHT dht(16, DHT11, 15);

//gps setup
static const int RXPin = 4, TXPin = 5; // blue is T , yellow R
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);


void setup ( void ) {
  EEPROM.begin(1024);
  Serial.begin(4800);//115200  4800
  delay(500);

  pinMode(ADMIN_MODE_PIN, OUTPUT);

  if (digitalRead(ADMIN_MODE_PIN) == HIGH)
  {
    AdminEnabled = true;
  }
  else
  {
    AdminEnabled = false;
  }

  Serial.println("Starting ES8266");
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "shivashambu";
    config.password = "";
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.ntpServerName = "0.de.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  0;
    config.timezone = -10;
    config.daylight = true;
    config.DeviceName = "Not Named";
    config.AutoTurnOff = false;
    config.AutoTurnOn = false;
    config.TurnOffHour = 0;
    config.TurnOffMinute = 0;
    config.TurnOnHour = 0;
    config.TurnOnMinute = 0;
    WriteConfig();
    Serial.println("General config applied");
  }


  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);

    server.on ( "/", processExample  );
    server.on ( "/admin/filldynamicdata", filldynamicdata );

    server.on ( "/favicon.ico",   []() {
      Serial.println("favicon.ico");
      server.send ( 200, "text/html", "" );
    }  );


    server.on ( "/admin.html", []() {
      Serial.println("admin.html");
      server.send ( 200, "text/html", PAGE_AdminMainPage );
    }  );
    server.on ( "/config.html", send_network_configuration_html );
    server.on ( "/info.html", []() {
      Serial.println("info.html");
      server.send ( 200, "text/html", PAGE_Information );
    }  );
    server.on ( "/ntp.html", send_NTP_configuration_html  );
    server.on ( "/general.html", send_general_html  );
    //	server.on ( "/example.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
    server.on ( "/style.css", []() {
      Serial.println("style.css");
      server.send ( 200, "text/plain", PAGE_Style_css );
    } );
    server.on ( "/microajax.js", []() {
      Serial.println("microajax.js");
      server.send ( 200, "text/plain", PAGE_microajax_js );
    } );
    server.on ( "/admin/values", send_network_configuration_values_html );
    server.on ( "/admin/connectionstate", send_connection_state_values_html );
    server.on ( "/admin/infovalues", send_information_values_html );
    server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
    server.on ( "/admin/generalvalues", send_general_configuration_values_html);
    server.on ( "/admin/devicename",     send_devicename_value_html);

    server.onNotFound ( []() {
      Serial.println("Page Not Found");
      server.send ( 400, "text/html", "Page not Found" );
    }  );
    server.begin();
    Serial.println( "HTTP server started" );
    tkSecond.attach(1, Second_Tick);
    UDPNTPClient.begin(2390);  // Port for NTP receive
  }
  else
  {
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to ");
    Serial.println(config.ssid.c_str());
    WiFi.begin (config.ssid.c_str(), config.password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());


    //ConfigureWifi();
    //setup_adxl();
    gps_setup();
  }
}


void loop ( void ) {

  if (!AdminEnabled)
  {
    //loop_adxl_logic();

    // yield();
    //analogWrite(BLUE_LED, 300);
    //delay(100);
    //yield();
    gps_loop();
    //analogWrite(BLUE_LED, 0);
    //delay(100);
    //yield();
  }
  else
  {
    server.handleClient();
  }


}

//my code


void setup_adxl()
{
  //adxl
  adxl.powerOn(14, 12);//sda,scl
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

  client.setServer(config.mqttServer.c_str(), 1883);
  client.setCallback(callback);
  Serial.print("conneting to mqqt server @ ");
  Serial.println(config.mqttServer);

}
void loop_adxl_logic() {


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

    for (int i = 0 ; i < 5; i++)
      flashLed(RED_LED, 100, 10);
  }
  //freefall
  if (adxl.triggered(interrupts, ADXL345_FREE_FALL)) {
    Serial.println("free fall");
    postToServer("{\"freefall\":true}");
    //add code here to do when a tap is sensed
    for (int i = 0 ; i < 10; i++)
      flashLed(RED_LED, 100, 1023);
  }

}


void flashLed(int led, int del, int pwm_strength)
{
  yield();
  analogWrite(led, pwm_strength);
  delay(del);
  yield();
  analogWrite(led, 0);
  delay(del);
  yield();
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

void postToServer(char* data)
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println(data);
  client.publish("outTopic", data);
}

//gps
void gps_setup()
{
  //Serial.begin(4800);
  ss.begin(GPSBaud);

  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}


void gps_loop()
{


  //Serial.println("in gps loop.");


  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    //while (true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
