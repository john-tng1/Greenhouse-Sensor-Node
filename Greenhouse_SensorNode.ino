/* Using wifi function of the MKR 1010 board*/
#include <SPI.h>
#include <WiFiNINA.h>
/* MQTT client */
#include <PubSubClient.h>

/* Using DHT22 sensors for Temperature and Humidity level   */
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "arduino_secrets.h"

// Configure Moisture Sensor
#define DHTPIN 2      // what pin we're connected to
#define DHTTYPE DHT22 // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);
// Declare value for humidity and temperature
float h = 0;
float t = 0;
int m = analogRead(A1);

// Configure network here
WiFiClient wifiClient;
PubSubClient client(wifiClient);

char ssid[] = SECRET_SSID;   //  network SSID (name)
char pass[] = SECRET_PASS;   //  network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS; // the Wifi radio's status

// Configure MQTT values
const char broker[] = "192.168.68.115";
int port = 1883;
const char topic[] = "Outside/Greenhouse";
const char *mqttUser = "";
const char *mqttPassword = "";

long loopNumber = 0;
String incomingMessage = "";

void connectWifi()
{
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  // failed, retry
  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    Serial.print(".");
    delay(5000);
  }
  Serial.println("");
  Serial.println("You're connected to the network");
  Serial.println();
}

//Stuff to deal with the MQTT incoming messages
void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.println();
  Serial.println();
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  incomingMessage = "";

  Serial.print("Message: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    incomingMessage += (char)payload[i];
  }

  Serial.println();
  Serial.println("The total message is:" + incomingMessage);
  Serial.println("---------------------------------------------------");
}

void connectMQTT()
{
  client.setServer(broker, port);
  client.setCallback(callback);
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT... ");

    if (client.connect("SensorNode Client", mqttUser, mqttPassword))
    {
      Serial.println("connected!");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }

    client.publish("Outside/Greenhouse", "Hello from sensor");
    client.subscribe("Outside/Greenhouse");
  }
}

void readSensor()
{
  h = dht.readHumidity();
  // Read temperature as Celsius
  t = dht.readTemperature();
  
  /* Read mosture sensor value
    The sensor value description
    # 0  ~300     dry soil
    # 300~700     humid soil
    # 700~950     in water
  */
  String soilMoisture = "";
  if (m > 700)
  {
    soilMoisture = String(m) + ", in water";
  }
  else if (m > 300)
  {
    soilMoisture = String(m) + ", humid soil";
  }
  else
  {
    soilMoisture = String(m) + ", dry soil";
  }

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  Serial.print("Soil Moisture Value: ");
  Serial.println(soilMoisture);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  dht.begin();
  // attempt to connect to WiFi network and MQTT broker
  connectWifi();
  connectMQTT();
}

void loop()
{

  // put your main code here, to run repeatedly:
  // Wait a few seconds between measurements.
  delay(2000);

  client.loop();

  //Are we connected to MQTT? If not, reconnect
  if (!client.connected())
  {
    Serial.println("MQTT connection lost - trying again.");
    connectMQTT();
  }
  
  readSensor();
}
