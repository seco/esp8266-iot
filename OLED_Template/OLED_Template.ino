// Please inspect Notes_Credits tab for references to this work
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>


String form =                                             // String form to sent to the client-browser
  "<p>"
  "<center>"
  "<h1>Talk to me! </h1>"
  "<img src='http://yt3.ggpht.com/-vOfCsox2nxk/AAAAAAAAAAI/AAAAAAAAAAA/1JXT_EWqap4/s100-c-k-no/photo.jpg'>"
  "<form action='msg'><p>Type something nice... <input type='text' name='msg' size=50 autofocus> <input type='submit' value='Submit'></form>"
  "</center>";

long period;
#define SSID "........"                                   // insert your SSID
#define PASS "........"                                   // insert your password

// Instantiate server class                               // See tab Network for #include
ESP8266WebServer server(80);                              // HTTP server will listen at port 80


void setup(void) 
{
  //ESP.wdtDisable();                                     // used to debug, disable wachdog timer,
  Serial.begin(115200);                                   // full speed to monitor
  Wire.begin(0, 2);                                       // Initialize I2C and OLED Display
  init_OLED();                                            // Ray Burnette: added OLED, see Notes_Credits tab
  reset_display();
  WiFi.begin(SSID, PASS);                                 // Connect to WiFi network

  while (WiFi.status() != WL_CONNECTED) {                 // Wait for connection
    delay(500);
    Serial.print(".");
    SendChar('.');
  }

  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
  server.on("/", []()
  {
    server.send(200, "text/html", form);
  });

  server.on("/msg", handle_msg);                          // And as regular external functions:
  server.begin();                                         // Start the server

  Serial.print("SSID : ");                                // prints SSID in monitor
  Serial.println(SSID);                                   // to monitor
  clear_display();                                        // Clears the OLED display
  sendStrXY("SSID:" , 0, 0);                              // OLED line 0
  sendStrXY(SSID, 0, 7);                                  // prints SSID on OLED

  char result[16];
  sprintf(result, "%3d.%3d.%3d.%3d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);

  sendStrXY("WebServer ready:", 1, 0);                    // OLED line 1
  sendStrXY(result, 3, 0);                                // OLED line 3
  Serial.println("WebServer ready:   ");
  Serial.println(WiFi.localIP());                         // Serial monitor prints localIP
  Serial.println(); 
  Serial.println(result);

/*
  // Just for fun... 'cause we can
  sprintf(result, "Analog A0 = %4d",analogRead(A0));
  Serial.println(result);
  sendStrXY(result, 6, 0);
*/
  sendStrXY("Point browser to", 5, 0);                    // OLED line 5
  sendStrXY("http://<address>", 6, 0);                    // OLED line 6
  sendStrXY("shown above.    ", 7, 0);                    // OLED line 7

}


void loop(void) {
  server.handleClient();                                  // checks for incoming messages
}


