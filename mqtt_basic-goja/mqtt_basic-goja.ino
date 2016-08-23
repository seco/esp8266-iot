/*
  Basic MQTT example

  - connects to an MQTT server
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
ADC_MODE(ADC_VCC);

const char *ssid =	"shivashambu";		// cannot be longer than 32 characters!
const char *pass =	"2013846675-86";		//

// Update these with values suitable for your network.
IPAddress server(54, 84, 99, 110);

void callback(const MQTT::Publish& pub) {
  Serial.println(pub.payload_string());
}

WiFiClient wclient;
PubSubClient client(wclient, server);

void setup() {
  // Setup console

  
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();

  client.set_callback(callback);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("arduinoClient")) {

        client.subscribe("inTopic");
      }
    }

    if (client.connected())
      client.loop();
  }
  
  String data = "Internal voltage:";
  float v = ESP.getVcc()/1024;
  data += v;
  client.publish("outTopic", data);
  delay(1000);
}

