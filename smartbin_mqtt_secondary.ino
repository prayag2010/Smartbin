/*
   Smart Garbage Bin
   By:
   Shubham Ambokar
   Shweta Bhale
   Prayag Desai
   Mini Project
   Sem 5
   TE ET - 1
*/
//secondary bin, red color

//g r y
#define _2l1 5          //33%
#define _2l2 10         //66%
#define _2l3 15         //99%

#define maxiGas 275

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <Ticker.h>

const char* ssid = "test";

const int trigPin = 2;  //D4
const int echoPin = 0;  //D3

unsigned long currentMillis = 0;
unsigned long previousMillis = 3000;

const int led1 = 12;     //D6 red
const int led2 = 14;     //D7 green
const int led3 = 13;     //D5 yellow

const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;
PubSubClient client(espClient);

// defines variables
long duration;
int distance2 = 0;

Ticker blinkTicker;
boolean prevGasTrig = false;
boolean currentGasTrig = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);                   // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);                    // Sets the echoPin as an Input

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  digitalWrite(led1, HIGH);
  delay(1000);
  digitalWrite(led2, HIGH);
  delay(1000);
  digitalWrite(led3, HIGH);
  delay(1000);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(led1, HIGH);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  previousMillis = millis();

}

void loop() {
  // put your main code here, to run repeatedly:
  currentMillis = millis();
  if (currentMillis - previousMillis <= 1500) {
    Serial.println("Executed");
    getDistance();
    if (distance2 <= _2l1) {
      if (!currentGasTrig) {
        Serial.println("HIGH");
        digitalWrite(led3, HIGH);
      }
    }
    else if (distance2 > _2l1 && distance2 <= _2l2) {
      if (!currentGasTrig) {
        Serial.println("LOW");
        digitalWrite(led3, LOW);
      }
    }
    else if (distance2 > _2l2 && distance2 <= _2l3) {
      if (!currentGasTrig) {
        Serial.println("LOW");
        digitalWrite(led3, LOW);
      }
    }
    else {
      if (!currentGasTrig) {
        Serial.println("LOW");
        digitalWrite(led3, LOW);
      }
    }
    int reading = analogRead(A0);
    if (reading >= maxiGas) {
      prevGasTrig = currentGasTrig;
      currentGasTrig = true;
    }
    else {
      prevGasTrig = currentGasTrig;
      currentGasTrig = false;
    }
    char nnum[5];
    char snum [5];  // long enough for 16 bit integer, sign, 5 digits and null.
    itoa(distance2, snum, 10);            // value, char array, base
    itoa(reading, nnum, 10);
    Serial.println("buffer:");
    Serial.println(snum);
    client.publish("miniproject/smartbin/bin2", snum);
    delay(5);
    client.publish("miniproject/smartbin/bin2/gas", nnum);
    Serial.println("Gas");
    Serial.println(reading);
    previousMillis = currentMillis + 4000;
  }
  if (!client.connected()) {
    reconnect();
  }
  digitalWrite(led2, HIGH);
  client.loop();

  if (!prevGasTrig && currentGasTrig) {
    Serial.println("Ticker Attach");
    blinkTicker.attach(1, ledBlink);
  }
  if (prevGasTrig && !currentGasTrig) {
    Serial.println("Ticker Detach");
    blinkTicker.detach();
  }

}

void getDistance(void)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance1
  distance2 = duration * 0.034 / 2;
  // Prints the distance1 on the Serial Monitor
  Serial.println("Distance:");
  Serial.println(distance2);
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("callback");
  String p_payload;
  for (uint8_t i = 0; i < length; i++) {
    p_payload.concat((char)payload[i]);
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    digitalWrite(led2, LOW);
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("miniproject/smartbin/bin2");
      client.subscribe("miniproject/smartbin/bin2/gas");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void ledBlink(void)
{
  Serial.println("In Ticker Function");
  digitalWrite(led3, !digitalRead(led3));
}
