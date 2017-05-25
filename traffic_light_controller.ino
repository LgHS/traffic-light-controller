#include "ssid.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define RED_PIN 13
#define GREEN_PIN 12

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {
  pinMode(RED_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(GREEN_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
 
  // INVERTED LOGIC !!! those turn the lights off
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(RED_PIN, HIGH);;
  /*


  */
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  delay(50);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
    // if wifi disconnected
    blinkLeds(1, 2, 100);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // as soon as wifi is connected, we blink green 5 times
  blinkLeds(0, 10, 10);
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if (strcmp(topic, "/lghs/traffic_light/blink") == 0) {
    int secs = payload[0] - '0';
    if (secs > 0 && secs < 9) {
      blinkSeconds(secs);
    }

  } else {
    // Switch off the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      if (strcmp(topic, "/lghs/traffic_light/green") == 0) {
        Serial.println("sending LOW on 2 (green)");
        digitalWrite(GREEN_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
      }
      if (strcmp(topic, "/lghs/traffic_light/red") == 0) {
        Serial.println("sending LOW on 0 (red)");
        digitalWrite(RED_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
      }
    } else if ((char)payload[0] == '0') {
      if (strcmp(topic, "/lghs/traffic_light/green") == 0) {
        Serial.println("sending HIGH on 2 (green)");
        digitalWrite(GREEN_PIN, HIGH);   // Turn the LED on (Note that LOW is the voltage level
      }
      if (strcmp(topic, "/lghs/traffic_light/red") == 0) {
        Serial.println("sending HIGH on 0 (red)");
        digitalWrite(RED_PIN, HIGH);   // Turn the LED on (Note that LOW is the voltage level
      }
    }
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("traffic_light")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("/lghs/traffic_light/red");
      client.subscribe("/lghs/traffic_light/green");
      client.subscribe("/lghs/traffic_light/blink");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

}

/**
   blink a led
   @param _color : 0 : green, 1 : red
   @param _times : how many blinks
   @param _delay between blinks
*/
void blinkLeds(int _color, int _times, int _delay) {
  _delay = constrain(_delay, 0, 500);
  if (_color > 1) _color = 1;
  for (int i = 0; i < _times; i++) {
    digitalWrite(GREEN_PIN + _color, HIGH);
    delay(_delay);
    digitalWrite(GREEN_PIN + _color, LOW);
  }
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(RED_PIN, HIGH);
}

void blinkSeconds(int secs) {
  long startPoint = millis();
  while (millis() < startPoint + secs * 1000) {
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
    delay(70);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(RED_PIN, HIGH);
    delay(70);
  }
  
  
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(RED_PIN, HIGH);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
}

