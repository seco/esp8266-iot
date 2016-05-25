/////////////////////////////////////////////////////////
// ESP8266-01 & ADXL345 acceleration sensor
// in Arduino IDE
//
// Stefan Thesen 09/2015
//
// Free for anybody - no warranties
// code majorly taken from
//    https://www.sparkfun.com/tutorials/240
//    and adapted to ESP8266
/////////////////////////////////////////////////////////

#include "math.h"
#include <Wire.h>

#define DEVICE (0x53) // Device address as specified in data sheet

byte _buff[6];
char POWER_CTL = 0x2D;    //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;    //X-Axis Data 0
char DATAX1 = 0x33;    //X-Axis Data 1
char DATAY0 = 0x34;    //Y-Axis Data 0
char DATAY1 = 0x35;    //Y-Axis Data 1
char DATAZ0 = 0x36;    //Z-Axis Data 0
char DATAZ1 = 0x37;    //Z-Axis Data 1

void setup()
{
  Serial.begin(9600);   // start serial for output. Make sure you set your Serial Monitor to the same!
  Serial.println("Init - S. Thesen ESP8266 & ADXL345 Demo");

  // i2c bus SDA = GPIO0; SCL = GPIO2
 // Wire.begin(14, 12);
 Wire.begin(0, 2);
  // Put the ADXL345 into +/- 2G range by writing the value 0x01 to the DATA_FORMAT register.
  // FYI: 0x00 = 2G, 0x01 = 4G, 0x02 = 8G, 0x03 = 16G
  writeTo(DATA_FORMAT, 0x00);

  // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeTo(POWER_CTL, 0x08);
}

void loop()
{
  readAccel();  // read the x/y/z tilt
  delay(1000);   // only read every 200ms
}

void readAccel()
{
  uint8_t howManyBytesToRead = 6;
  readFrom( DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345

  // each axis reading comes in 10 bit resolution, ie 2 bytes. Least Significat Byte first!!
  // thus we are converting both bytes in to one
  // cave: esp8266 is 32bit, thus use short and not int as in Arduino examples
  short x = (((short)_buff[1]) << 8) | _buff[0];
  short y = (((short)_buff[3]) << 8) | _buff[2];
  short z = (((short)_buff[5]) << 8) | _buff[4];

  Serial.print("x: ");
  Serial.print( x * 2. / 512 );
  Serial.print(" y: ");
  Serial.print( y * 2. / 512 );
  Serial.print(" z: ");
  Serial.print( z * 2. / 512 );
  Serial.print("  Total: ");
  Serial.println( sqrtf(x * x + y * y + z * z) * 2. / 512 );
}

void writeTo(byte address, byte val)
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // send register address
  Wire.write(val); // send value to write
  Wire.endTransmission(); // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte _buff[])
{
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.write(address); // sends address to read from
  Wire.endTransmission(); // end transmission
  Wire.beginTransmission(DEVICE); // start transmission to device
  Wire.requestFrom(DEVICE, num); // request 6 bytes from device

  int i = 0;
  while (Wire.available()) // device may send less than requested (abnormal)
  {
    _buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); // end transmission
}

