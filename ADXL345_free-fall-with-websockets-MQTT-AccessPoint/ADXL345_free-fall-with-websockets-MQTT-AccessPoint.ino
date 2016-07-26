/*
  ESP_WebConfig

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public




*/

#include <Wire.h>
#include <ADXL345.h>
#include "PubSubClient.h"

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


#define ACCESS_POINT_NAME  "goja-ups-iot-access-point"
#define ACCESS_POINT_PASSWORD  "12345678"
#define AdminTimeOut 180  // Defines the Time in Seconds, when the Admin-Mode will be diabled

WiFiClient espClient;
PubSubClient client(espClient);
ADXL345 adxl; //variable adxl is an instance of the ADXL345 library
//DHT dht(16, DHT11, 15);
const int RED_LED = 15;
const int BLUE_LED = 4;
const int GREEN_LED = 5;


void setup ( void ) {
  EEPROM.begin(512);
  Serial.begin(115200);
  delay(500);
  Serial.println("Starting ES8266");
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "shivashambu";
    config.password = "2013846675-86";
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
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }

  ConfigureWifi();


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


  //adxl
  adxl.powerOn(14, 12);
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


  char __mqttServer [sizeof(config.mqttServer)];
  config.mqttServer.toCharArray(__mqttServer, sizeof(__mqttServer));
  client.setServer(__mqttServer, 1883);
  client.setCallback(callback);



}


void loop ( void ) {
  if (AdminEnabled)
  {
    if (AdminTimeOutCounter > AdminTimeOut)
    {
      AdminEnabled = false;
      Serial.println("Admin Mode disabled!");
      WiFi.mode(WIFI_STA);
    }
  }
  if (config.Update_Time_Via_NTP_Every  > 0 )
  {
    if (cNTP_Update > 5 && firstStart)
    {
      NTPRefresh();
      cNTP_Update = 0;
      firstStart = false;
    }
    else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
    {

      NTPRefresh();
      cNTP_Update = 0;
    }
  }

  if (DateTime.minute != Minute_Old)
  {
    Minute_Old = DateTime.minute;
    if (config.AutoTurnOn)
    {
      if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
      {
        Serial.println("SwitchON");
      }
    }


    Minute_Old = DateTime.minute;
    if (config.AutoTurnOff)
    {
      if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
      {
        Serial.println("SwitchOff");
      }
    }
  }
  server.handleClient();


  /*
       Your Code here
  */
  if (!AdminEnabled)
  {
    loop_adxl_logic();
  }

  if (Refresh)
  {
    Refresh = false;
    ///Serial.println("Refreshing...");
    //Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
  }





}

//my code

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


  //  flashLeds( HIGH);
  delay(100);
  //  flashLeds( LOW);
  //  delay(1000);
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

}

