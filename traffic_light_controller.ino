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
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(RED_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(GREEN_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
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
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    if(strcmp(topic, "/lghs/traffic_light/green") == 0) {
      Serial.println("sending LOW on 2 (green)");
      digitalWrite(GREEN_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    }
    if(strcmp(topic, "/lghs/traffic_light/red") == 0) {
      Serial.println("sending LOW on 0 (red)");
      digitalWrite(RED_PIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    }
  } else {
    if(strcmp(topic, "/lghs/traffic_light/green") == 0) {
      Serial.println("sending HIGH on 2 (green)");
      digitalWrite(GREEN_PIN, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    }
    if(strcmp(topic, "/lghs/traffic_light/red") == 0) {
      Serial.println("sending HIGH on 0 (red)");
      digitalWrite(RED_PIN, HIGH);   // Turn the LED on (Note that LOW is the voltage level
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
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

