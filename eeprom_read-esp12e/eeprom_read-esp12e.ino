/*
   EEPROM Read

   Reads the value of each byte of the EEPROM and prints it
   to the computer.
   This example code is in the public domain.
*/

#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
int value;
int max_mem = 1 * 1024 ; // 1 b
void setup()
{
  // initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  EEPROM.begin(max_mem);
}

void loop()
{
  // read a byte from the current address of the EEPROM
  value = EEPROMReadInt(address);//EEPROM.read(address);

  Serial.print(address);
  Serial.print("\t");
  Serial.print(value);
  Serial.println();

  // advance to the next address of the EEPROM
  address = address + 2;

  // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
  // on address 512, wrap around to address 0
  if (address == max_mem)
    address = 0;

  delay(50);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

