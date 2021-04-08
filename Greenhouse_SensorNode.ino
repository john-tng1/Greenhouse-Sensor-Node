/*
    Using wifi function of the MKR 1010 board
*/
#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include <PubSubClient.h>

/*
   Using DHT22 sensors for Temperature and Humidity level
*/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2        // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);


// Configure network here

char ssid[] = SECRET_SSID;        //  network SSID (name)
char pass[] = SECRET_PASS;    //  network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the Wifi radio's status
const char* mqtt_server = "192.168.68.115"; // 

void setup() {
  Serial.begin(9600);
  dht.begin();
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
    //
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print the MAC address of the router you're attached to:
    byte bssid[6];
    WiFi.BSSID(bssid);
    Serial.print("BSSID: ");
    

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);

    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
}

void loop() {
  // put your main code here, to run repeatedly:
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  /* Read mosture sensor value
    The sensor value description
    # 0  ~300     dry soil
    # 300~700     humid soil
    # 700~950     in water
  */

  String soilMoisture = "";
  int m = analogRead(A1);
  if (m > 700) {
    soilMoisture = String(m) + ", in water";
  } else if (m > 300) {
    soilMoisture = String(m) + ", humid soil";
  } else {
    soilMoisture = String(m) + ", dry soil";
  }

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
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
