/*
  CLASS: MyWiFi

  This class was developed as a way to separate out the code which handles the 
  setup, management and configuration of the WiFi for the device.

  Written by: Scott Griffis
  Date: 10-10-2023
*/

#include "MyWiFi.h"




/*
  CLASS CONSTRUCTOR: Used to externally instantiate the class while also 
  providing what SSID should be used for the device when in AP mode.

  @param apSsid - The SSID for the AP mode as String.
*/
MyWiFi::MyWiFi(String apSsid) {
  this->apSsid = apSsid;
}




/*
  Used to place the device's WiFi into AP mode. This 
  allows for clients to connect to the WiFi that is produced
  by this device. This particularly useful when the device isn't
  yet configured to connect to an external wifi network yet.

  @return Returns true if AP mode starts properly otherwise a false as bool.
*/
bool MyWiFi::startAPMode() {
  Serial.print(F("Configuring AP mode... "));
  WiFi.setHostname("TempBuddy-Ctrl");
  WiFi.mode(WiFiMode::WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,1,1), IPAddress(0,0,0,0), IPAddress(255,255,255,0));
  bool ret = WiFi.softAP(apSsid.c_str(), "password123"); // TODO: Locate this somewhere else!
  Serial.println(ret ? F("Complete.") : F("Failed!"));

  return ret;
}




/*
  This function is used to get the IP Address of the device regardless if it is in 
  AP mode or connected to an external WiFi Network.

  @return Returns the IP Address of this device in dot notation as String.
*/
String MyWiFi::getIpAddress() {
  if (WiFi.getMode() == WiFiMode::WIFI_AP) {

    return WiFi.softAPIP().toString();
  }
  
  return WiFi.localIP().toString();
}




/*
  This function is used to invoke the device to connect to an existing 
  external WiFi Network where the ssid and password are specified. If 
  this device cannot sucessfully connect to the given network within 
  10 seconds then it stops trying and falls back to AP mode. This is to
  allow the user to connect and correct a possible typo in the given 
  ssid or password.

  @param ssid - The SSID to connect to as address to const char array.
  @param pwd - The password to connect to the given network as address to const char array.

  @return Returns true if successfully connects to external wifi as bool.
*/
bool MyWiFi::connectToNetwork(const char *ssid, const char *pwd) {
  // Connect to WiFi network...
  Serial.print(F("\n\nConnecting to "));
  Serial.print(String(ssid));
  
  WiFi.disconnect();
  delay(50);
  WiFi.mode(WiFiMode::WIFI_STA);
  WiFi.begin(ssid, pwd);
  bool connected = false;
  unsigned long start = millis();
  while (!(connected = !(WiFi.status() != WL_CONNECTED)) && (millis() - start < 10000UL)) { // WiFi is not yet connected...
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  if (connected) {  
    Serial.println(F("WiFi connected"));
  } else {
    Serial.println(F("Unable to connect to WiFi, falling back to APMode!"));
    startAPMode();
  }

  return connected;
}




/*
  Indicates if the WiFi is in a connected status.

  @return Returns true if the device is in a conencted wifi status as bool.
*/
bool MyWiFi::isConnected() {

  return WiFi.isConnected();
}