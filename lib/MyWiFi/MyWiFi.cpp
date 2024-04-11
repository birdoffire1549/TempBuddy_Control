/*
  CLASS: MyWiFi

  This class was developed as a way to separate out the code which handles the 
  setup, management and configuration of the WiFi for the device.

  Written by: Scott Griffis
  Date: 10-10-2023
*/

#include "MyWiFi.h"

/**
 * #### CLASS CONSTRUCTOR #### 
 * Used to externally instantiate the class.
*/
MyWiFi::MyWiFi() {
  // Do nothing...
}

/**
 * Used to place the device's WiFi into AP mode. This 
 * allows for clients to connect to the WiFi that is produced
 * by this device. This particularly useful when the device isn't
 * yet configured to connect to an external wifi network yet.
 * 
 * @param hostname The hostname of the device as String.
 * @param ip The IP Address of the AP as well as the network portion of the AP's DHCP network is
 * derived from the network portion of this address, as String.
 * @param subnet The subnet address of the AP's network as String.
 * @param gateway The gateway address for the AP's network as String.
 * @param ssid The SSID of the network being broadcast by the AP as String.
 * @param pwd The password for the wireless network as String.
 * 
 * @return Returns true if AP mode starts properly otherwise a false as bool.
 * 
*/
bool MyWiFi::startAPMode(String hostname, String ip, String subnet, String gateway, String ssid, String pwd) {
  this->hostname = String(hostname);
  this->apIp = String(ip);
  this->apSubnet = String(subnet);
  this->apGateway = String(gateway);
  this->apSsid = String(ssid);
  this->apPwd = String(pwd);

  Serial.print(F("Configuring AP mode... "));
  WiFi.setHostname(hostname.c_str());
  WiFi.mode(WiFiMode::WIFI_AP);
  WiFi.softAPConfig(IpUtils::stringIPv4ToIPAddress(ip), IpUtils::stringIPv4ToIPAddress(gateway), IpUtils::stringIPv4ToIPAddress(subnet));
  bool ret = WiFi.softAP(ssid, pwd);
  Serial.println(ret ? F("Complete.") : F("Failed!"));

  return ret;
}

/**
 * #### PRIVATE FUNCTION ####
 * This function should only be called internally after the public startAPMode function
 * is called first. Essentially this function simply uses the public one passing back in
 * the last values used.
*/
bool MyWiFi::startAPMode() {
  
  return startAPMode(this->hostname, this->apIp, this->apSubnet, this->apGateway, this->apSsid, this->apPwd);
}

/**
 * This function is used to get the IP Address of the device regardless if it is in 
 * AP mode or connected to an external WiFi Network.
 * 
 * @return Returns the IP Address of this device in dot notation as String.
*/
String MyWiFi::getIpAddress() {
  if (WiFi.getMode() == WiFiMode::WIFI_AP) { // WiFi is in AP mode...

    return WiFi.softAPIP().toString();
  } // ELSE: WiFi is not in AP mode...
  
  return WiFi.localIP().toString();
}

/**
 * This function is used to invoke the device to connect to an existing 
 * external WiFi Network where the ssid and password are specified. If 
 * this device cannot sucessfully connect to the given network within 
 * 10 seconds then it stops trying and falls back to AP mode. This is to
 * allow the user to connect and correct a possible typo in the given 
 * ssid or password.
 * 
 * @param hostname The hostname for the device as String.
 * @param ssid The SSID to connect to as String.
 * @param pwd The password to connect to the given network as String.
 * 
 * @return Returns true if successfully connects to external wifi as bool.
*/
bool MyWiFi::connectToNetwork(String hostname, String ssid, String pwd) {
  // Connect to WiFi network...
  Serial.print(F("\n\nConnecting to "));
  Serial.print(ssid);
  
  WiFi.disconnect();
  delay(50);
  WiFi.setHostname(hostname.c_str());
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