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
//primary bin, cream color

//y r g
#define _1l1 6          //33%
#define _1l2 13         //66%
#define _1l3 15         //99%

#define _2l1 5          //33%
#define _2l2 10         //66%
#define _2l3 15         //99%

#define maxiGas 250

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <Ticker.h>


MDNSResponder mdns;

// Replace with your network credentials
const char* ssid = "test";
//const char* password = "YOUR_PASSWORD";
const char* host = "smartbin";

ESP8266WebServer server(80);

String webPage = "";

// defines pins numbers
const int trigPin = 4;  //D2
const int echoPin = 5;  //D1

const int led1 = 12;     //D6 red
const int led2 = 13;     //D7 green
const int led3 = 14;     //D5 yellow

unsigned long currentMillis = 0;
unsigned long previousMillis = 3000;

const char* mqtt_server = "iot.eclipse.org";

// defines variables
long duration;
int distance1 = 0;
int distance2 = 0;

int gasReading2 = 0;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];

Ticker blinkTicker;
Ticker _3s;
boolean _3sb = false;
boolean currentGasTrig = false;
boolean prevGasTrig = false;

void setup(void) {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  digitalWrite(trigPin, LOW);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

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

  pinMode(A0, INPUT);
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
    digitalWrite(led1, LOW);
  }
  digitalWrite(led1, HIGH);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("smartbin", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", []() {
    server.send(500, "text/html", webPage);
  });
  server.begin();
  Serial.println("HTTP server started");
  previousMillis = millis();
  _3s.attach(3, setTrue);
}

void setTrue()
{
  _3sb = true;
}

void loop(void) {
  currentMillis = millis();
  if (_3sb) {
    _3sb = false;
    Serial.println("Executed");
    getDistance();
    previousMillis = currentMillis + 4000;
    webPage = "";
    //  webPage += "<h1>ESP8266 Web Server</h1><p>Socket #1 <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></p>";
    //  webPage += "<p>Socket #2 <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></p>";
    webPage += "<head>";
    webPage += "<title>SMART BIN </title>";
    webPage += "<meta name=\"viewport\" content=\"width = device - width, initial - scale = 1\">";
    webPage += "<style>";
    webPage += "body {background-color:#ffffff;background-repeat:no-repeat;background-position:top left;background-attachment:fixed;}";
    webPage += "h1{text-align:center;font-family:Georgia, serif;color:#000000;background-color:#ffffff;font-size:70px;font-weight:bold;}";
    webPage += "h2{text-align:center;font-family:Georgia, serif;color:#000000;background-color:#ffffff;font-size:48px;font-weight:bold;}";
    webPage += "p {text-align:center;font-family:Helvetica, sans-serif;font-size:48px;font-style:normal;font-weight:bold;color:#ff0000;background-color:#ffffff;}";
    webPage += "</style>";
    webPage += "</head>";
    webPage += "<body>";
    webPage += "<h1>SMART BIN</h1>";
    webPage += "<h2>Lets manage our waste in a smarter way.</h2>";
    webPage += "<style type=\"text/css\">";
    webPage += ".tg  {border-collapse:collapse;border-spacing:0;border-color:#999;}";
    webPage += ".tg td{font-family:Arial, sans-serif;font-size:14px;padding:20px 20px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#444;background-color:#F7FDFA;}";
    webPage += ".tg th{font-family:Arial, sans-serif;font-size:14px;font-weight:normal;padding:20px 20px;border-style:solid;border-width:1px;overflow:hidden;word-break:normal;border-color:#999;color:#fff;background-color:#26ADE4;}";
    webPage += ".tg .tg-fity{font-size:36px;font-family:\"Arial Black\", Gadget, sans-serif !important;;color:#000000;text-align:center;vertical-align:top}";
    //    webPage += ".tg .tg-eumt{font-size:20px;font-family:Arial, Helvetica, sans-serif !important;;color:#fe0000;text-align:center;vertical-align:top}";
    webPage += ".tg .tg-eumt{font-size:20px;font-family:Arial, Helvetica, sans-serif !important;;color:";
    if (gasReading2 >= maxiGas) {
      webPage += "#fe0000";
    }
    else {
      webPage += "#34ff34";
    }
    webPage += ";text-align:center;vertical-align:top}";
    //    webPage += ".tg .tg-9z0u{font-size:20px;color:#34ff34;vertical-align:top}";
    webPage += ".tg .tg-9z0u{font-size:20px;color:";
    if (distance1 <= _1l1) {
      webPage += "#fe0000";
      if (!currentGasTrig) {
        Serial.println("HIGH");
        digitalWrite(led3, HIGH);
      }
    }
    else if (distance1 > _1l1 && distance1 <= _1l2) {
      if (!currentGasTrig) {
        Serial.println("LOW");
        digitalWrite(led3, LOW);
      }
      webPage += "#ffa500";
    }
    else if (distance1 > _1l2 && distance1 <= _1l3) {
      webPage += "#34ff34";
      if (!currentGasTrig) {
        Serial.println("LOW");
        digitalWrite(led3, LOW);
      }
    }
    else {
      webPage += "#800000";
      if (!currentGasTrig) {
        Serial.println("LOW");
        digitalWrite(led3, LOW);
      }
    }
    //#fe0000 red
    //#34ff34 green
    //#FFA500 orange
    //#800000 maroon
    webPage += ";vertical-align:top}";
    //    webPage += ".tg .tg-9z1u{font-size:20px;color:#34ff34;vertical-align:top}";
    webPage += ".tg .tg-9z1u{font-size:20px;color:";
    if (distance2 <= _2l1) {
      webPage += "#fe0000";
    }
    else if (distance2 > _2l1 && distance2 <= _2l2) {
      webPage += "#ffa500";
    }
    else if (distance2 > _2l2 && distance2 <= _2l3) {
      webPage += "#34ff34";
    }
    else {
      webPage += "#800000";
    }
    webPage += ";vertical-align:top}";
    //    webPage += ".tg .tg-whro{font-size:20px;font-family:Arial, Helvetica, sans-serif !important;;color:#fe0000;text-align:center;vertical-align:top}";
    webPage += ".tg .tg-whro{font-size:20px;font-family:Arial, Helvetica, sans-serif !important;;color:";
    if (analogRead(A0) >= maxiGas) {
      webPage += "#fe0000";
      prevGasTrig = currentGasTrig;
      currentGasTrig = true;
    }
    else {
      webPage += "#34ff34";
      prevGasTrig = currentGasTrig;
      currentGasTrig = false;
    }
    webPage += ";text-align:center;vertical-align:top}";
    webPage += ".tg .tg-ruh6{font-size:36px;font-family:\"Arial Black\", Gadget, sans-serif !important;;color:#000000;text-align:center;vertical-align:top}";
    webPage += ".tg .tg-r5uq{font-size:20px;font-family:\"Lucida Sans Unicode\", \"Lucida Grande\", sans-serif !important;;vertical-align:top}";
    webPage += "</style>";
    webPage += "<table align=\"center\" ;table class=\"tg\">";
    webPage += "<tr>";
    webPage += "<th class=\"tg-ruh6\">SMART BIN</th>";
    webPage += "<th class=\"tg-ruh6\">Percent full</th>";
    webPage += "<th class=\"tg-fity\">Smoke</th>";
    webPage += "</tr>";
    webPage += "<tr>";
    webPage += "<td class=\"tg-r5uq\">Bin 1</td>";
    //    webPage += "<td class=\"tg-9z0u\">__%</td>";
    webPage += "<td class=\"tg-9z0u\">";
    if (distance1 >= _1l3 || distance1 < 0) {
      webPage += "Lid Open!";
    }
    else {
      webPage += 100 - (((float)distance1 / _1l3) * 100);
      webPage += "%</td>";
    }
    //    webPage += "<td class=\"tg-whro\">Detected/Not detected</td>";
    webPage += "<td class=\"tg-whro\">";
    //    webPage += "Detected/Not detected";
    //    webPage += analogRead(A0);
    if (analogRead(A0) >= maxiGas)
    {
      webPage += "Gas Detected!";
    }
    else {
      webPage += "No Gas Detected!";
    }
    Serial.println("Gas");
    Serial.println(analogRead(A0));
    webPage += "</td>";
    webPage += "</tr>";
    webPage += "<tr>";
    webPage += "<td class=\"tg-r5uq\">Bin 2</td>";
    webPage += "<td class=\"tg-9z1u\">";
    if (distance2 >= _2l3 || distance2 < 0) {
      webPage += "Lid Open!";
    }
    else {
      webPage += 100 - ((((float)distance2 * 100) / _2l3));
      webPage += "%</td>";
    }
    webPage += "<td class=\"tg-eumt\">";
    if (gasReading2 >= maxiGas)
    {
      webPage += "Gas Detected!";
    }
    else {
      webPage += "No Gas Detected!";
    }
    webPage += "</td>";
    webPage += "</tr>";
    webPage += "</table>";
    webPage += "</body>";
    webPage += "</html>";
    char snum [5];
    itoa(analogRead(A0), snum, 10);
    client.publish("miniproject/smartbin/bin1/gas", snum);
  }

  if (!client.connected()) {
    reconnect();
  }
  digitalWrite(led2, HIGH);
  client.loop();
  server.handleClient();

  if (!prevGasTrig && currentGasTrig) {
    Serial.println("Ticker Attach");
    blinkTicker.attach(1, ledBlink);
  }
  if (prevGasTrig && !currentGasTrig) {
    Serial.println("Ticker Detach");
    blinkTicker.detach();
  }
}

void ledBlink(void)
{
  Serial.println("In Ticker Function");
  digitalWrite(led3, !digitalRead(led3));
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
  distance1 = duration * 0.034 / 2;
  // Prints the distance1 on the Serial Monitor
  Serial.println("Distance:");
  Serial.println(distance1);
  char nnum[5];
  itoa(distance1, nnum, 10);
  client.publish("miniproject/smartbin/bin1", nnum);
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
  if (String("miniproject/smartbin/bin2").equals(topic)) {
    distance2 = p_payload.toInt();
  }
  if (String("miniproject/smartbin/bin2/gas").equals(topic)) {
    gasReading2 = p_payload.toInt();
  }

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
