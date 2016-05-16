#include             <ESP8266WiFi.h>

const int             ledPin                 = 0;

// ---------------------------------------------------------------------------
void setup() {

  char                pass[]                = "1234";
  char                ssid[]                = "AP101";
  int                 status;

  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  Serial.print("Tryingonnect to ");
  Serial.println(ssid);

  // attempt to connect to Wifi network:
  status = WiFi.begin(ssid, pass);

  status = WiFi.waitForConnectResult();
  if (status != WL_CONNECTED) {
    Serial.println("Connection Failed");
    while (true) {
      delay(500);
      Serial.println(".");
    }
  }

  Serial.println("Connected.");
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP:  ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Channel");
  Serial.println(WiFi.channel());
  Serial.print("Status");
  Serial.println(WiFi.status());

} // setup

// ---------------------------------------------------------------------------
void loop() {

  int                    k;

  for (k = 0; k <= 100; k = k + 1) {

    Serial.println("on");
    digitalWrite(ledPin, HIGH);
    delay(500);

    Serial.println("off");
    digitalWrite(ledPin, LOW);
    delay(500);
  }

  WiFi.disconnect(true);

  while (true) {}

}
