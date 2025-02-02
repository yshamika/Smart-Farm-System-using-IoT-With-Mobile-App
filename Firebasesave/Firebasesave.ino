#include <FirebaseRealtime.h>
#include "DHT.h"

#define FIREBASE_REALTIME_URL "https://mygggg-340c8-default-rtdb.firebaseio.com/"
#define FIREBASE_REALTIME_SECRET "uwVrR9QofZR1D6xc4nmXz7T0QjmnVc6YLe3tXx1W"

#define RELAY D2
#define DHTPIN D5
#define DHTTYPE DHT11
#define FLOW_SENSOR_PIN D6
#define LITERS_PER_PULSE 0.001

const int waterLevelPin = A0;

//water flow
long totalMilliLitres = 0;
unsigned long startTime;
boolean flowState = false;
float totalLiters = 0.0;

char ssid[] = "iPhone";
char pass[] = "qwqwqwqw";

FirebaseRealtime firebaseRealtime;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  Serial.begin(9600);
  firebaseRealtime.begin(FIREBASE_REALTIME_URL, FIREBASE_REALTIME_SECRET, ssid, pass);
  dht.begin();
  
}

void loop() {

  DynamicJsonDocument fetchDoc(1024);
  int fetchResponseCode = firebaseRealtime.fetch("FirebaseIOT","1",fetchDoc);
  int led = fetchDoc["led"];
  Serial.println("led: " + led);
  int push = fetchDoc["push"];
  Serial.println("push: " + push);
  fetchDoc.clear();

  if (led == 1) {
    Serial.println("pump on");
    digitalWrite(RELAY, HIGH);
  } 

   
   //water flow
   int sensorValue = digitalRead(FLOW_SENSOR_PIN);
  if (sensorValue == HIGH)
   {if (flowState == false) {
      flowState = true;
      startTime = millis();
    }
  } 
  else 
  {
    if (flowState == true) 
    {
      flowState = false;
      totalMilliLitres += millis() - startTime;
      totalLiters = totalMilliLitres * LITERS_PER_PULSE;
    }
  }

  int waterLevel = analogRead(waterLevelPin);    //water level
  float Temperatue = dht.readTemperature();      //Temperatue
  float Humidity = dht.readHumidity();           //Humidity
  float waterFlow=100;
  String level ;
  int pull;
  int pump;


  Serial.print("Temperature: ");
  Serial.println(Temperatue);
  Serial.print("Humidity: ");
  Serial.println(Humidity);
  Serial.print("Water Level: ");
  Serial.println(waterLevel);
  Serial.print("Total Flow: ");
  Serial.print(totalLiters, 2);
  Serial.println(" Liters");
 
 //push button  
  if (push == 1) 
  {
    Serial.println("push on");
      totalLiters=0;
      pull == 0;
  }
//water level
  if(waterLevel<500) //low
  {
    Serial.println("water level low");
    level="LOW";
    digitalWrite(RELAY, HIGH);
    pump == 1;
  }
  else if(waterLevel<620)  //medium 550
  {
    Serial.println("water level medium");
    level="MEDIUM";
  }
  else if (waterLevel>620)//high  580
  {
    Serial.println("water level high");
    Serial.println("pump off");
    digitalWrite(RELAY, LOW); 
    pump == 0;
    level="HIGH";
  }

//send deta to firebase
  DynamicJsonDocument saveDoc(1024);
  saveDoc["temperature"] = Temperatue;
  saveDoc["humidity"] = Humidity;
  saveDoc["waterflow"] = totalLiters;
  saveDoc["waterLevel"] = level;
  saveDoc["push"] = pull;
  saveDoc["led"] = pump;
  String saveJSONData;
  serializeJson(saveDoc, saveJSONData);
  firebaseRealtime.save("FirebaseIOT","1", saveJSONData, true);


  delay(2000);
}