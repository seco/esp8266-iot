/**
  This Arduino code implements some of the advanced features available in the
  ADXL345 accelerometer such as power management, single tap, double tap, freefall
*/

#include <Wire.h>
#include "binary_const.h" // with this we can use something B8(01010101) that it will convert to 85 at compile time

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>

// the above cames really handy and readable when doing per bit configuration in the ADXL345 registers

#define DEVICE (0x53)    //ADXL345 device address (with SDO tied to ground)
#define TO_READ (6)      //num of bytes we are going to read each time (two bytes for each axis)

#define INTERRUPTPIN 2   // Arduino pin which is connected to INT1 from the ADXL345

const char* ssid     = "c";
const char* password = "-86";
const char* host = "192.168.1.7";
int eprom_address = 0;
int max_eprom_mem = 4 * 1024 * 1024;// 4 mb

int dump_pin = 5;


// Register map: see ADXL345 datasheet page 14
const int R_DEVID = 0;
const int R_THRESH_TAP = 29;
const int R_OFSX = 30;
const int R_OFSY = 31;
const int R_OFSZ = 32;
const int R_DUR = 33;
const int R_LATENT = 34;
const int R_WINDOW = 35;
const int R_THRESH_ACT = 36;
const int R_THRESH_INACT = 37;
const int R_TIME_INACT = 38;
const int R_ACT_INACT_CTL = 39;
const int R_THRESH_FF = 40;
const int R_TIME_FF = 41;
const int R_TAP_AXES = 42;
const int R_ACT_TAP_STATUS = 43;
const int R_BW_RATE = 44;
const int R_POWER_CTL = 45;
const int R_INT_ENABLE = 46;
const int R_INT_MAP = 47;
const int R_INT_SOURCE = 48;
const int R_DATA_FORMAT = 49;
const int R_DATAX0 = 50;
const int R_DATAX1 = 51;
const int R_DATAY0 = 52;
const int R_DATAY1 = 53;
const int R_DATAZ0 = 54;
const int R_DATAZ1 = 55;
const int R_FIFO_CTL = 56;
const int R_FIFO_STATUS = 57;


byte buff[TO_READ];    //6 bytes buffer for saving data read from the device
char str[512];                      //string buffer to transform data before sending it to the serial port
boolean inspected = 0;


void setup()
{
  Wire.begin(14, 12);       // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  Serial.print("starting up....");
  pinMode(INTERRUPTPIN, INPUT);
  EEPROM.begin(max_eprom_mem);
  pinMode(dump_pin, INPUT);

  // interrupts setup
  writeTo(DEVICE, R_INT_MAP, 0); // send all interrupts to ADXL345's INT1 pin
  writeTo(DEVICE, R_INT_ENABLE, B8(1111100)); // enable signle and double tap, activity, inactivity and free fall detection


  // free fall configuration
  writeTo(DEVICE, R_TIME_FF, 0x14); // set free fall time
  writeTo(DEVICE, R_THRESH_FF, 0x05); // set free fall threshold

  // single tap configuration
  writeTo(DEVICE, R_DUR, 0x1F); // 625us/LSB
  writeTo(DEVICE, R_THRESH_TAP, 48); // 62.5mg/LSB  <==> 3000mg/62.5mg = 48 LSB as datasheet suggestion
  writeTo(DEVICE, R_TAP_AXES, B8(111)); // enable tap detection on x,y,z axes

  // double tap configuration
  writeTo(DEVICE, R_LATENT, 0x10);
  writeTo(DEVICE, R_WINDOW, 0xFF);

  // inactivity configuration
  writeTo(DEVICE, R_TIME_INACT, 10); // 1s / LSB
  writeTo(DEVICE, R_THRESH_INACT, 3); // 62.5mg / LSB
  // also working good with high movements: R_TIME_INACT=5, R_THRESH_INACT=16, R_ACT_INACT_CTL=B8(00000111)
  // but unusable for a quite slow movements

  // activity configuration
  writeTo(DEVICE, R_THRESH_ACT, 8); // 62.5mg / LSB

  // activity and inctivity control
  writeTo(DEVICE, R_ACT_INACT_CTL, B8(11111111)); // enable activity and inactivity detection on x,y,z using ac

  // set the ADXL345 in measurement and sleep Mode: this will save power while while we will still be able to detect activity
  // set the Link bit to 1 so that the activity and inactivity functions aren't concurrent but alternatively activated
  // set the AUTO_SLEEP bit to 1 so that the device automatically goes to sleep when it detects inactivity
  writeTo(DEVICE, R_POWER_CTL, B8(111100));

  //wifi setup

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

/*
  Prints out the state of all registers
  void inspectRegisters() {
  int regCount = R_FIFO_STATUS - R_THRESH_TAP + 1;
  byte regBuff[regCount];
  readFrom(DEVICE, R_THRESH_TAP, regCount, regBuff);
  for(int i=0; i<regCount; i++) {
    Serial.println(R_THRESH_TAP + i);
    Serial.println(regBuff[i], BIN);
  }
  }
*/

unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void loop()
{

  if (digitalRead(dump_pin) == HIGH)
  {
    int address = 0;
    for (address = 0 ; address < max_eprom_mem ; )
    {
      int value = EEPROMReadInt(address);
      address += 2;
      Serial.print("x:"); Serial.println(value);

      value = EEPROMReadInt(address);
      address += 2;
      Serial.print("y:"); Serial.println(value);

      value = EEPROMReadInt(address);
      address += 2;
      Serial.print("z:"); Serial.println(value);

      address += 2;

      delay(500);
    }

    return;
  }
  /*
    if(inspected == 0) {
    //delay(1000);
    Serial.println("inspecting registers");
    inspectRegisters();
    delay(1000);
    inspected = 1;
    }
  */

  // we use a digitalRead instead of attachInterrupt so that we can use delay()
  if (digitalRead(INTERRUPTPIN)) {
    int interruptSource = readByte(DEVICE, R_INT_SOURCE);
    //Serial.print("### ");
    //Serial.println(interruptSource, BIN);


    if (interruptSource & B8(100)) {
      Serial.println("### FREE_FALL");
      postToServer("freefall=true");
    }
    if (interruptSource & B8(1000)) {
      Serial.println("### Inactivity");
      // we don't need to put the device in sleep because we set the AUTO_SLEEP bit to 1 in R_POWER_CTL
      // set the LOW_POWER bit to 1 in R_BW_RATE: with this we get worst measurements but we save power
      int bwRate = readByte(DEVICE, R_BW_RATE);
      writeTo(DEVICE, R_BW_RATE, bwRate | B8(10000));
    }
    if (interruptSource & B8(10000)) {
      Serial.println("### Activity");

      // get current power mode
      int powerCTL = readByte(DEVICE, R_POWER_CTL);
      // set the device back in measurement mode
      // as suggested on the datasheet, we put it in standby then in measurement mode
      // we do this using a bitwise and (&) so that we keep the current R_POWER_CTL configuration
      writeTo(DEVICE, R_POWER_CTL, powerCTL & B8(11110011));
      delay(10); // let's give it some time (not sure if this is needed)
      writeTo(DEVICE, R_POWER_CTL, powerCTL & B8(11111011));

      // set the LOW_POWER bit to 0 in R_BW_RATE: get back to full accuracy measurement (we will consume more power)
      int bwRate = readByte(DEVICE, R_BW_RATE);
      writeTo(DEVICE, R_BW_RATE, bwRate & B8(01111));
    }
    if (interruptSource & B8(100000)) {
      Serial.print("### DOUBLE_TAP Axes: ");
      printTapAxes();
      Serial.println(""); // closing Axes line
    }
    else if (interruptSource & B8(1000000)) { // when a double tap is detected also a signle tap is deteced. we use an else here so that we only print the double tap
      Serial.print("### SINGLE_TAP Axes: ");
      printTapAxes();
      Serial.println(""); // closing Axes line
    }
    delay(150);
  }

  int regAddress = 0x32;    //first axis-acceleration-data register on the ADXL345
  int x, y, z;

  readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345

  //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
  //thus we are converting both bytes in to one int
  x = (((int)buff[1]) << 8) | buff[0];
  y = (((int)buff[3]) << 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];

  //we send the x y z values as a string to the serial port
  sprintf(str, "%d,%d,%d", x, y, z);
  Serial.println(str);
  //Serial.print(10);
  String data = "coordinates=" ;
  data +=  str ;

  postToServer(data);
  Serial.println(data);
  //It appears that delay is needed in order not to clog the port

  EEPROMWriteInt(eprom_address, x);
  eprom_address += 2;

  EEPROMWriteInt(eprom_address, y);
  eprom_address += 2;

  EEPROMWriteInt(eprom_address, z);
  eprom_address += 2;

  EEPROM.commit();
  
  delay(500);
}


void printTapAxes() {
  int tapStatus = readByte(DEVICE, R_ACT_TAP_STATUS);
  if (tapStatus & B8(100)) {
    Serial.print("x ");
    postToServer("xaxis_tapped=true");
  }
  if (tapStatus & B8(10)) {
    Serial.print("y ");
    postToServer("yaxis_tapped=true");
  }
  if (tapStatus & B8(1)) {
    Serial.print("z ");
    postToServer("zaxis_tapped=true");
  }
}

//---------------- Functions
//Writes val to address register on device
void writeTo(int device, byte address, byte val) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        // send register address
  Wire.write(val);        // send value to write
  Wire.endTransmission(); //end transmission
}


//reads num bytes starting from address register on device in to buff array
void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, num);    // request 6 bytes from device

  int i = 0;
  while (Wire.available())   //device may send less than requested (abnormal)
  {
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}


// read a single bite and returns the readed value
byte readByte(int device, byte address) {
  Wire.beginTransmission(device); //start transmission to device
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission

  Wire.beginTransmission(device); //start transmission to device
  Wire.requestFrom(device, 1);    // request 1 byte from device

  int readed = 0;
  if (Wire.available())
  {
    readed = Wire.read(); // receive a byte
  }
  Wire.endTransmission(); //end transmission
  return readed;
}

void postToServer2(String data)
{


  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 1337;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/esp8266";
  //    url += streamId;
  //    url += "?private_key=";
  //    url += privateKey;
  //    url += "&value=";
  //    url += value;

  Serial.print("Posting to URL: ");
  Serial.println(url);

  // This will send the request to the server
  //    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  //                 "Host: " + host + "\r\n" +
  //                 "Connection: close\r\n\r\n");

  String httpPost =   "POST /meteo HTTP/1.1\r\n"
                      "Host: 192.168.0.131:5000\r\n"
                      "User-Agent: Arduino/1.0\r\n"
                      "Connection: close\r\n"
                      "Content-Type: application/x-www-form-urlencoded;\r\n"
                      "\r\n" + data + "\n";

  Serial.println(httpPost);
  client.print(httpPost);

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

}


void postToServer(String data) {

  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
  http.begin("http://192.168.1.7:1337/postADXL345Data"); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  //int httpCode = http.GET();

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(data);

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();


  //delay(10000);
}

void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);

  //Serial.print("addr:");Serial.print(p_address);
  //Serial.print(", val:");Serial.println(p_value);
}


