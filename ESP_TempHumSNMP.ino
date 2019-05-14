#include <ESP8266WiFi.h>
//----- OTA ---------
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino_SNMP.h>
#include <SNMPResponse.h>
#include <SNMPTrap.h>
#include <VarBinds.h>
#include <BER.h>
#include <SNMPRequest.h>
#include <SimpleDHT.h>

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
SNMPAgent snmp = SNMPAgent("public");
char* strHumidity;
char* strTemperature;

//Object reading the humidity
SimpleDHT22 dht;
 
//Variable to store the value of humidity
float humidity = 0;
 
//Variable to store the temperature value
float temperature = 0;

//*************************************************************************************************
//NodeMCU v1.0 ESP8266-12e Pin Definitions (makes it much easier as these match the board markings)
//*************************************************************************************************
//#define LED_BUILTIN 16
//#define BUILTIN_LED 16
//
//#define D0 16  //no internal pullup resistor
//#define D1  5
//#define D2  4
//#define D3  0  //must not be pulled low during power on/reset, toggles value during boot
//#define D4  2  //must not be pulled low during power on/reset, toggles value during boot
#define D5 14
//#define D6 12
//#define D7 13
//#define D8 15  //must not be pulled high during power on/reset



#define DHTPIN D5
#define INTERVAL 100

#define SSID "YOUR SSID"
#define PASSWORD "YOUR PASS"
#define IP "STATIC IP HERE"

int millisecondTracker = millis();
int eth = 1; // Number of network interfaces
int rssi;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
 
  strHumidity = (char*)malloc(6);
  strTemperature = (char*)malloc(6);
  memset(strHumidity, 0, 6);
  memset(strTemperature, 0, 6);

   
  setupWiFi();
  setupSNMP();

 rssi = WiFi.RSSI();

}

void loop() {
  // put your main code here, to run repeatedly:
  readSensor();
  verifySNMP();
  delay(INTERVAL);

}

void setupWiFi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
 
  WiFi.begin(SSID, PASSWORD);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
 
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
 
  //Configura o IP
  IPAddress ipAddress;
  ipAddress.fromString(IP);
  WiFi.config(ipAddress, WiFi.gatewayIP(), WiFi.subnetMask());
 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupSNMP()
{
  //Initializes the SNMP
  snmp.setUDP(&udp);
  snmp.begin();
  //Adds OID to humidity (read only)
  snmp.addStringHandler(".1.3.6.1.4.1.12345.0", &strHumidity, false);
  //Adds OID to temperature (read only)
  snmp.addStringHandler(".1.3.6.1.4.1.12345.1", &strTemperature, false);
  //Adds OID to TimeTicks/Up Time since ESP was last restarted
  snmp.addIntegerHandler(".1.3.6.1.2.1.1.3.0", &millisecondTracker, false);
  //Adds OID to RSSI Signal strength
  snmp.addIntegerHandler(".1.3.6.1.2.1.1.3.0", &rssi, false);
  
  
snmp.addIntegerHandler(".1.3.6.1.2.1.2.1.0", &eth, false); // Number of ethernet ports

//snmp.addIntegerHandler(".1.3.6.1.2.1.1.3.0", &strM, false);

}


void verifySNMP()
{
  //Should always be called during the main loop
  snmp.loop();
 
  //If there was a change in one of the values
//  if(snmp.setOccurred)
//  {
    //Saves the values
//    saveConfig();
    //Reset the change flag
//    snmp.resetSetOccurred();
//  }
}


void readSensor(){
  float h, t;
  int status = dht.read2(DHTPIN, &t, &h, NULL);
  millisecondTracker = millis();
  rssi = WiFi.RSSI();
  
  if (status == SimpleDHTErrSuccess) {
    humidity = h;
    temperature = 1.8*t+32; // Converting Celsius to Fahrenheit
    //Transforms data into string
    String strH = String(humidity);
    strH.toCharArray(strHumidity, strH.length());
    String strT = String(temperature);
    strT.toCharArray(strTemperature, strT.length());
   }
}
