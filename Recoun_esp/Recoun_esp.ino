#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>
#include <Arduino_JSON.h>
#include <PubSubClient.h>
#include "EspMQTTClient.h"
#include <SoftwareSerial.h>
#include <Wire.h>

int Led_OnBoard = 2;
int Relay = 5; // D1 pin digital
const char *serverPost = "http://app.spairum.my.id/transfer/post/proto";
const char *serverGet = "http://app.spairum.my.id/transfer/get/proto";

#define echoPin 0 // attach pin D3 Arduino to pin Echo of HC-SR04
#define trigPin 1 //attach pin D4 Arduino to pin Trig of HC-SR04
// defines variables
long duration; // variable for the duration of sound wave travel
int distance;  // variable for the distance measurement

SoftwareSerial linkSerial(D6, D5); // (Rx, Tx)
int data1;
EspMQTTClient client(
    "ws.spairum.my.id", // MQTT Broker server ip
    1883,               // The MQTT port, default to 1883. this line can be omitted
    "spairum",          // Can be omitted if not needed MQTTUsername
    "broker",           // Can be omitted if not needed MQTTPassword
    "Prototipe"         // Client name that uniquely identify your device
);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200); //115200
    pinMode(Led_OnBoard, OUTPUT);
    pinMode(Relay, OUTPUT);
    digitalWrite(Led_OnBoard, LOW);
    delay(2000);
    digitalWrite(Led_OnBoard, HIGH);
    delay(2000);

    WiFiManager wifiManager;
    wifiManager.autoConnect("Spairum1");
    Serial.println("connected...yeey :)");
    Serial.println("Connecting");

    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    // WiFi.hostname("AlifaF");
    Serial.println(WiFi.hostname());

    client.enableDebuggingMessages();                           // Enable debugging messages sent to serial output
    client.enableHTTPWebUpdater();                              // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
    client.enableLastWillMessage("Test", "I am going offline"); // You can activate the retain flag by setting the third parameter to true
}

void onConnectionEstablished()
{
    client.subscribe("Web", [](const String &payload) {
        Serial.println(payload);
        StaticJsonDocument<64> doc;

        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            //            Serial.println(error.f_str());
            return;
        }

        const char *id = doc["id"];     // "proto"
        const char *akun = doc["akun"]; // "Admin"
        Serial.print(id);
        Serial.println(akun);
        if (id = "proto")
        {
            Serial.println("OPEN");
            digitalWrite(Relay, HIGH);
            delay(6000);
            digitalWrite(Relay, LOW);
            delay(1000);
        };
    });
}

void loop()
{
    client.loop();
    digitalWrite(Led_OnBoard, HIGH);
    mersure();
    if (linkSerial.available())
    {
        // This one must be bigger than for the sender because it must store the strings
        StaticJsonDocument<200> doc;
        DeserializationError err = deserializeJson(doc, linkSerial);
        if (err == DeserializationError::Ok)
        {
            // Print the values
            // (we must use as<T>() to resolve the ambiguity)
            Serial.print("Token: ");
            data1 = doc["Sisa"];
            Serial.print(data1);
            postTrans();
        }
        else
        {
            Serial.print("deserializeJson() returned ");
            Serial.println(err.c_str());
            while (linkSerial.available() > 0)
                linkSerial.read();
        }
    }
    digitalWrite(Led_OnBoard, LOW);
    getTrans();
}

void mersure()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Mehitung Ketingan dan volume
    // unsigned int Liter = 0;
    // Liter = Luas - (P * L * distance)
    Serial.println(distance);

    String httpRequestMesin, isi, id_mesin, status;
    id_mesin = "proto";
    status = "belum di hitung";
    isi = distance;

    JSONVar doc;
    doc["id_mesin"] = id_mesin;
    doc["status"] = status;
    doc["isi"] = isi;

    String jsonString = JSON.stringify(doc);
    client.publish("arSpairum", jsonString.c_str());
    delay(1000);
}

void getTrans()
{
    HTTPClient client;
    client.begin(serverGet);

    int httpCode2 = client.GET();
    String payload2 = client.getString();

    Serial.print("Response Code:");
    Serial.println(httpCode2);
    Serial.println(payload2);

    if (httpCode2 > 0)
    {

        StaticJsonDocument<200> doc;

        auto error = deserializeJson(doc, payload2);
        if (error)
        {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(error.c_str());
            return;
        }

        Serial.println(F("Response:"));
        Serial.println(doc["vaule"].as<char *>());
        serializeJson(doc, linkSerial);
    }
    client.end();
    delay(1000);
    digitalWrite(Led_OnBoard, LOW);
    delay(1000);
    digitalWrite(Led_OnBoard, HIGH);
}

void postTrans()
{
    //post Transfer
    Serial.println();
    String httpRequestData, data2;
    data2 = data1;
    httpRequestData = "vaule=" + data2;

    HTTPClient http; //Declare object of class HTTPClient

    http.begin(serverPost);                                              //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header

    int httpCode = http.POST(httpRequestData); //Send the request
    String payload = http.getString();         //Get the response payload

    Serial.println(httpCode); //Print HTTP return code
    Serial.println(payload);  //Print request response payload

    http.end(); //Close connection

    delay(1000);
    digitalWrite(Led_OnBoard, LOW);
    delay(1000);
    digitalWrite(Led_OnBoard, HIGH);
}
