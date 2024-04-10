/*
  Settings - A class to contain, maintain, store and retreive settings needed
  by the application. This Class object is intented to be the sole manager of 
  data used throughout the applicaiton. It handles storing both volitile and 
  non-volatile data, where by definition the non-volitile data is persisted
  in flash memory and lives beyond the running life of the software and the 
  volatile data is lost and defaulted each time the software runs.

  Written by: Scott Griffis
  Date: 10-01-2023
*/

#include "Settings.h"




/*
  CLASS CONSTRUCTOR: Allows for external instantiation of
  the class into an object.
*/
Settings::Settings() {
  // Initially default the settings...
  defaultSettings();
}




/*
  Performs a factory default on the information maintained by this class
  where that the data is first set to its factory default settings then
  it is persisted to flash.

  @return Returns true if successful saving defaulted settings otherwise
  returns false as bool.
*/
bool Settings::factoryDefault() {
  defaultSettings();
  bool ok = saveSettings();

  return ok;
}




/*
  Used to load the settings from flash memory.
  After the settings are loaded from flash memory the sentinel value is 
  checked to ensure the integrity of the loaded data. If the sentinel 
  value is wrong then the contents of the memory are deemed invalid and
  the memory is wiped and then a factory default is instead performed.

  @return Returns true if data was loaded from memory and the sentinel 
  value was valid.
*/
bool Settings::loadSettings() {
  bool ok = false;
  // Setup EEPROM for loading and saving...
  EEPROM.begin(sizeof(NonVolatileSettings));

  // Persist default settings or load settings...
  delay(15);
  // Load from EEPROM if applicable...
  if (EEPROM.percentUsed() >= 0) { // Something is stored from prior...
    Serial.println(F("\nLoading settings from EEPROM..."));
    EEPROM.get(0, nvSettings);
    if (nvSettings.sentinel != SENTINEL_VALUE) {
      EEPROM.wipe();
      factoryDefault();
      Serial.println("Stored settings footprint invalid, stored settings have been wiped and defaulted!");
    } else {
      Serial.print(F("Percent of ESP Flash currently used is: "));
      Serial.print(EEPROM.percentUsed());
      Serial.println(F("%"));
      ok = true;
    }
  }
  
  EEPROM.end();

  return ok;
}




/*
  Used to save or persist the current value of the non-volatile settings
  into flash memory.

  @return Returns a true if save was successful otherwise a false as bool.
*/
bool Settings::saveSettings() {
  EEPROM.begin(sizeof(NonVolatileSettings));

  EEPROM.wipe(); // usage seemd to grow without this.
  EEPROM.put(0, nvSettings);
  
  bool ok = EEPROM.commit();

  EEPROM.end();
  
  return ok;
}




/*
  Used to determine if the current network settings are in default values.

  @return Returns a true if default values otherwise a false as bool.
*/
bool Settings::isFactoryDefault() {
  // TODO: This might need to be adjusted since there have been significant functionality changes!!!
  if (!String(nvSettings.ssid).equals("SET_ME") && !String(nvSettings.pwd).equals("SET_ME")) {

    return false;
  }

  return true;
}




/*
=================================================================
Getter and Setter Functions
=================================================================
*/




String Settings::getSsid() {
  
  return String(nvSettings.ssid);
}


void Settings::setSsid(const char *ssid) {
  if (sizeof(ssid) <= sizeof(nvSettings.ssid)) {
    strcpy(nvSettings.ssid, ssid);
  }
}


String Settings::getPwd() {

  return String(nvSettings.pwd);
}


void Settings::setPwd(const char *pwd) {
  if (sizeof(pwd) <= sizeof(nvSettings.pwd)) {
    strcpy(nvSettings.pwd, pwd);
  }
}


String Settings::getAdminPwd() {

  return String(nvSettings.adminPwd);
}


void Settings::setAdminPwd(const char *pwd) {
  if (sizeof(pwd) <= sizeof(nvSettings.adminPwd)) {
    strcpy(nvSettings.adminPwd, pwd);
  }
}


float Settings::getDesiredTemp() {

  return nvSettings.desiredTemp;
}


void Settings::setDesiredTemp(float temp) {
  nvSettings.desiredTemp = temp;
}


String Settings::getHeading() {

  return String(nvSettings.heading);
}


void Settings::setHeading(const char *heading) {
  if (sizeof(heading) <= sizeof(nvSettings.heading)) {
    strcpy(nvSettings.heading, heading);
  }
}


bool Settings::getIsHeat() {

  return nvSettings.isHeat;
}


void Settings::setIsHeat(bool isHeat) {
  nvSettings.isHeat = isHeat;
}


String Settings::getTempBuddyIp() {

  return String(nvSettings.tempBuddyIp);
}


void Settings::setTempBuddyIp(const char *ip) {
  if (sizeof(ip) <= sizeof(nvSettings.tempBuddyIp)) {
    strcpy(nvSettings.tempBuddyIp, ip);
  }
}


float Settings::getTempPadding() {

  return nvSettings.tempPadding;
}


void Settings::setTempPadding(float padding) {
  nvSettings.tempPadding = padding;
}


unsigned long Settings::getTimeout() {

  return nvSettings.timeout;
}


void Settings::setTimeout(unsigned long timeout) {
  nvSettings.timeout = timeout;
}


String Settings::getTitle() {

  return String(nvSettings.title);
}


void Settings::setTitle(const char *title) {
  if (sizeof(title) <= sizeof(nvSettings.title)) {
    strcpy(nvSettings.title, title);
  }
}


bool Settings::getIsControlOn() {

  return vSettings.isControlOn;
}


void Settings::setIsControlOn(bool isOn) {
  vSettings.isControlOn = isOn;
}


bool Settings::getIsAutoControl() {

  return nvSettings.isAutoControl;
}


void Settings::setIsAutoControl(bool autoOn) {
  nvSettings.isAutoControl = autoOn;
}


float Settings::getLastKnownTemp() {

  return vSettings.lastKnownTemp;
}


void Settings::setLastKnownTemp(float temp) {
  vSettings.lastKnownTemp = temp;
}




/*
=================================================================
Private Functions
=================================================================
*/




/*
  PRIVATE: This function is used to set or reset all settings to 
  factory default values but does not persist the value 
  changes to flash.
*/
void Settings::defaultSettings() {
  // Default the settings..
  strcpy(nvSettings.ssid, "SET_ME");
  strcpy(nvSettings.pwd, "SET_ME");
  strcpy(nvSettings.adminPwd, "admin");
  strcpy(nvSettings.title, "Temp Buddy Control - IOT");
  strcpy(nvSettings.heading, "Device Info");
  nvSettings.timeout = 5000;
  nvSettings.isAutoControl = false;
  nvSettings.isHeat = true;
  nvSettings.sentinel = SENTINEL_VALUE;

  vSettings.isControlOn = false;
  vSettings.lastKnownTemp = 0.0;
}