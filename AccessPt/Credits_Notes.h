/*
 * https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/example-sketch-ap-web-server
 * Arduino 1.6.6 IDE 
 * Sketch uses 243,753 bytes (46%) of program storage space. Maximum is 524,288 bytes.
 * 
 * Password for Access Point is "sparkfun"
 * Webserver is on http://192.168.4.1 
 * 
 */
 
/*
 *   Author of OLED routines:
     https://github.com/mike-rankin/ESP_LUA_Loader
     Oled Display has an i2c address of 0x0C (even if it says 0x78)
     ESP-01 has only two I/O pins: 0=SDA, 2=SCL
*/

/*    I did not write the OLED functions... note that sendStrXY is in reality sendStrYX !!!
 *    Just deal with it: maybe original author had screen in a different orientation plane.
      sendStrXY("Line 0..........", 0, 0);  // a Full line #0 Max lines =7 for 8 total lines
      sendStrXY("Line 1.", 1, 0);
      sendStrXY("Line 2.", 2, 0);
      sendStrXY("Line 3.", 3, 0);
      sendStrXY("Line 4.", 4, 0);
      sendStrXY("Line 5.", 5, 0);
      sendStrXY("Line 6.", 6, 0);
      sendStrXY("Line 7.", 7, 0);
      delay(10000);
  */
  
