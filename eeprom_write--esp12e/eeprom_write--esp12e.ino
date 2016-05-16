/*
   EEPROM Write

   Stores values read from analog input 0 into the EEPROM.
   These values will stay in the EEPROM when the board is
   turned off and may be retrieved later by another sketch.
*/

#include <EEPROM.h>

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;
int max_mem = 1 * 1024 ; // 1 b
void setup()
{
  EEPROM.begin(max_mem);
  Serial.begin(9600);
}

void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);

  Serial.print("addr:");Serial.print(p_address);
  Serial.print(", val:");Serial.println(p_value);
}


void loop()
{
  // need to divide by 4 because analog inputs range from
  // 0 to 1023 and each byte of the EEPROM can only hold a
  // value from 0 to 255.
  int val = addr; //analogRead(A0) / 4;
  Serial.begin(9600);
  // write the value to the appropriate byte of the EEPROM.
  // these values will remain there when the board is
  // turned off.
  //EEPROM.write(addr, val);
  EEPROMWriteInt(addr, val);

  // advance to the next address.  there are 512 bytes in
  // the EEPROM, so go back to 0 when we hit 512.
  // save all changes to the flash.
  addr = addr + 2;
  if (addr == max_mem)
  {
    addr = 0;
    EEPROM.commit();
    Serial.println("commit complete !!");
  }

  //Serial.println(addr);
  delay(100);
}
