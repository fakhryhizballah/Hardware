#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <WiFiManager.h>

#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Wire.h>

int Led_OnBoard = 2;
const char *serverPost = "http://app.spairum.my.id/transfer/post/proto";
const char *serverGet = "http://app.spairum.my.id/transfer/get/proto";
const char *serverPostMesin = "http://app.spairum.my.id/mesin/edit/proto";

SoftwareSerial linkSerial(D6, D5); // (Rx, Tx)

int data1;

void setup()
{
    // put your setup code here, to run once:
    delay(1000);
    pinMode(Led_OnBoard, OUTPUT);
    pinMode(0, INPUT); // LDR

    Serial.begin(115200);
    linkSerial.begin(115200);

    WiFiManager wifiManager;
    wifiManager.autoConnect("Spairum DWS");
    Serial.println("connected...yeey :)"); 
    Serial.println("Connecting");

    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(Led_OnBoard, LOW);
    delay(2000);
    digitalWrite(Led_OnBoard, HIGH);
    delay(2000);
    StaticJsonDocument<500> doc;
}

void loop()
{
    getTrans();
    delay(1000);

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
    
    String httpRequestMesin, isi, indikator;
    indikator = ind;
    isi = KG;
    httpRequestMesin = "isi=" + isi + "&indikator=" + indikator;

    HTTPClient httpMp; //Declare object of class HTTPClient

    httpMp.begin(serverPostMesin);                                         //Specify request destination
    httpMp.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header

    int httpCode3 = httpMp.POST(httpRequestMesin); //Send the request
    String payload3 = httpMp.getString();          //Get the response payload

    Serial.println(httpCode3); //Print HTTP return code
    Serial.println(payload3);  //Print request response payload

    httpMp.end(); //Close connection

    delay(1000);
    digitalWrite(Led_OnBoard, LOW);
    delay(2000);
    digitalWrite(Led_OnBoard, HIGH);
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
    delay(2000);
    digitalWrite(Led_OnBoard, LOW);
    delay(2000);
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
    delay(2000);
    digitalWrite(Led_OnBoard, HIGH);
}
