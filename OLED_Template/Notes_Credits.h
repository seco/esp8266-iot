/*
  Modified, refactored & additions by Ray Burnette, July 2015
  Arduino IDE 1.6.1
    Sketch uses 246,665 bytes (47%) of program storage space.
  Arduino 1.6.6
    Sketch uses 248,088 bytes (47%) of program storage space. Maximum is 524,288 bytes.

    =============================== OLED Connections =============================
    OLED: http://www.aliexpress.com/item/Free-shipping-1Pcs-white-128X64-OLED-LCD-LED-Display-Module-For-Arduino-0-96-I2C-IIC/32234039563.html
    Connections://Wire.pins(int sda, int scl)  Wire.pins(0, 2); //on ESP-01. 
    ==== ESP8266 Pin====    ==== OLED Pin====
            Vcc  1                Vcc  1
            Rxd  2                
            Rst  3                
           GPIO0 4                SDA  4             use 1.8K Ohm pullup
           CH_PD 5                
           CPIO2 6                SCL  3             " ditto "
             Txd 7                
             Gnd 8                Gnd  2
            

  Credits: 
          DanBicks added OLED display to ESP01 using cheap Ebay 0.96" display
          Lightweight OLED routines by Mike-Rankin added in this project
          http://www.esp8266.com/viewtopic.php?f=29&t=3256
          Original source: https://github.com/costonisp/ESP8266-I2C-OLED
          OLED code supports either SDD1306 or with SH1106 controller
*/


