/*
  Project Name: ... TempBuddy Control
  Written by: ..... Scott Griffis
  Email: .......... birdoffire1549@gmail.com
  Date: ........... 10-01-2023

  Overview:
  This software was written to be utilized by an ESP8266 Device.
  The device using this software is refered to as a TempBuddy Control Unit.
  It was designed to work with another project that is refered to as a TempBuddy Sensor, that
  particular device has the ability to sense and report Temperature and Humidity data via a web
  interface. The sensor device also broadcasts its data over the network. This device can use these
  broadcasts to find and track a sensor on the same network. By doing this the control can read its 
  temp and then react to the temperature by controlling an outlet which can have a Heating or Cooling device
  attached to it. If no sensor is being tracked by this device then the user has the ability to
  manually control the attached outlet through the hosted webpage. This device also hosts a webpage
  that can be accessed using the device's IP Address and Port 80. Also this device can be configured
  by accessing its admin page either via an existing WiFi or the device's 'TempBuddy_Ctrl_<uid>' wifi network
  when it is in AP Mode.

  Hosted Endpoints:
  /        - This is where the device's information is deplayed as a web page
  /admin   - This is where the device's settings are configured. Default User: admin, Default Password: admin

  More Detailed:
  When device is first programmed it boots up as an AccessPoint that can be connected to using a computer,
  by connecting to the presented network with a name of 'TempBuddy_Ctrl_<uid>' and no password. Once connected 
  to the device's WiFi network you can connect to it for configuration using a web browser via the URL:
  http://192.168.1.1/admin. This will pop up an authentication dialogue requesting a user and password.
  Initially the user is 'admin' and password is 'admin' but can be changed. This will display the current
  device settings and allow the user to make desired configuration changes to the device. When the Network
  settings are changed the device will reboot and attempt to connect to the configured network. This code
  also allows for the device to be equiped with a factory reset button. To perform a factory reset the factory
  reset button must supply a HIGH to its input while the device is rebooted. Upon reboot if the factory reset
  button is HIGH the stored settings in flash will be replaced with the original factory default setttings.
  The factory reset button also serves another purpose during the normal operation of the device. If pressed
  breifly the device will flash out the last octet of its IP Address. It does this using the built-in LED. Each
  digit of the last octet is flashed out with a breif rapid flash between the blink count for each digit. Once
  all digits have been flashed out the LED will do a long rapid flash. Also, one may use the factory reset button
  to obtain the full IP Address of the device by keeping the ractory reset button pressed during normal device
  operation for more than 6 seconds. When flashing out the IP address the device starts with the first digit of
  the first octet and flashes slowly that number of times, then it performs a rapid flash to indicate it is on
  to the next digit. Once all digits in an octet have been flashed out the device performs a second after digit
  rapid flash to indicate it has moved onto a new octet.

  I will demonstrate how this works below by representting a single flash of the LED as a dash '-'. I will represent
  the post digit rapid flash with three dots '...', and finally I will represent the end of sequence long flash
  using 10 dots '..........'.

  Using the above the IP address of 192.168.123.71 would be flashed out as follows:
  1                     9       2       . 1               6                   8       . 1       2         3       .             7     1
  - ... - - - - - - - - - ... - - ... ... - ... - - - - - - ... - - - - - - - - ... ... - ... - - ... - - - ... ... - - - - - - - ... - ..........

  The short button press version of the above would simply be the last octet so in this case it would be:
              7     1
  - - - - - - - ... - ..........

  The last octet is useful if you know the network portion of the IP Address the device would be attaching to but
  are not sure what the assigned host portion of the address is, of course this is for network masks of 255.255.255.0.
*/

// ************************************************************************************
// Include Statements
// ************************************************************************************

#include <map>
#include <Arduino.h>
#include <ESP_EEPROM.h>
#include <ESP8266WebServer.h> 
#include <DNSServer.h>
#include <HtmlContent.h>
#include <ArduinoJson.h>

#include <Utils.h>
#include <IpUtils.h>
#include <Settings.h>
#include <ParseUtils.h>
#include <WiFiUdp.h>

#include <ESP8266HTTPClient.h>
#include <WString.h>

// ************************************************************************************
// Define Statements
// ************************************************************************************

#define FIRMWARE_VERSION "6.1.1"

#define LED_PIN 5
#define OUTLET_PIN 4
#define RESTORE_PIN 14

// ************************************************************************************
// Setup of Services
// ************************************************************************************
Settings settings = Settings();
ESP8266WebServer webServer(80);
WiFiUDP udp;
DNSServer dnsServer;

// ************************************************************************************
// Global worker variables
// ************************************************************************************
byte udpPktBuf[UDP_TX_PACKET_MAX_SIZE];
String deviceId = "";
float sensorLastTempRead = 0.0f;
std::map<String/*ID*/, unsigned long> recentSensorLastSeen;
std::map<String/*ID*/, String/*Name*/> recentSensorNames;

// ************************************************************************************
// Function Prototypes
// ************************************************************************************

/* Endpoint Functions */
void endpointHandlerAdmin(void);
void endpointHandlerRoot(void);

/* Page Send Funcitons */
void sendInfoPageWithoutControls(String statusMessage);
void sendInfoPageWithControls(String statusMessage);
void sendAdminPage(void);

/* DO Functions */
void doHandleDeviceOperations(void);
void doHandleBroadcasts(void);
bool doSaveAdminSettings(void);
void doUpdateAutoSettings(void);
bool doWiFiSTAMode(void);
bool doWiFiAPMode(void);
void doStartNetwork(void);
void doCheckIpDisplayRequest(void);

/* Utility Functions */
void initWebServer(void);
String getIpAddress(void);
void resetOrLoadSettings(void);

/**
 * #### SETUP() - REQUIRED FUNCTION ####
 * This function is the required setup() function and it is
 * where the initialization of the application happens.
*/
void setup() {
    // Configure pin modes...
    pinMode(OUTLET_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(RESTORE_PIN, INPUT);

    // Initialize output pins...
    digitalWrite(LED_PIN, LOW);
    digitalWrite(OUTLET_PIN, LOW);

    // Initialize Serial for logging...
    Serial.begin(115200);
    delay(15);

    resetOrLoadSettings();
    doStartNetwork();
    initWebServer();

    delay(50);
    Serial.printf("Initialization Complete.\nFirmware Version: %s\n\n", FIRMWARE_VERSION);
}

/**
 * #### LOOP() - REQUIRED FUNCTION ####
 *
 * This is the required loop() function of the applicaiton.
 * Here is where all functionality happens or starts to happen.
*/
void loop() {
    doCheckIpDisplayRequest();

    webServer.handleClient();
    dnsServer.processNextRequest();

    doHandleBroadcasts();
    doHandleDeviceOperations();

    yield();
}

/**
 * Detects and reacts to a reqest for factory reset
 * during the boot-up. Also loads settings from
 * EEPROM if there are saved settings.
*/
void resetOrLoadSettings() {
    if (digitalRead(RESTORE_PIN) == HIGH) { // Restore button pressed on bootup...
        settings.factoryDefault();
        while (digitalRead(RESTORE_PIN) == HIGH) { // Wait for pin to be released to continue...
            yield();
        }
    } else { // Normal load restore pin not pressed...
        // Load from EEPROM if applicable...
        settings.loadSettings();
    }
}

/**
 * This function is in charge of initially starting the
 * network, in either AP Mode or External WiFi mode based
 * on if newtwork settings are factory default or not.
*/
void doStartNetwork() {
    dnsServer.stop();
    udp.stop();

    deviceId = Utils::genDeviceIdFromMacAddr(WiFi.macAddress());
    
    bool ok = false;
    if (settings.isNetworkSet()) {
      ok = doWiFiSTAMode();
      if (ok) {
        Serial.println("WiFi Connected.");
        // Start UDP for getting broadcasts from Temp Sensor
        udp.begin(61549);
      } else {
        Serial.println("Connection Failed; Attempting to fallback to AP Mode...");
        ok = doWiFiAPMode();
      }
    } else {
      ok = doWiFiAPMode();
    }

    if (ok && WiFi.getMode() == WIFI_AP) {
      Serial.println("AP Mode Successful.");
      dnsServer.start(53u, "*", IpUtils::stringIPv4ToIPAddress(settings.getApNetIp()));
    } else if (!ok) {
      Serial.println("AP Mode Failed!");
      Serial.println("Rebooting in 30 Seconds...");
      yield();
      delay(30000);
      
      ESP.restart();
    }
}

/**
 * This function attemptes to connect to a specific WiFi network.
 * This function will attempt to verify connection for up to 10 seconds,
 * after that time it will simply return the current connection status.
 * 
 * @return Returns the connection status as bool, a true will be returned
 * if a connection was successful, otherwise a false is returned indicating
 * a connection failure.
 */
bool doWiFiSTAMode() {
  WiFi.setOutputPower(20.5F);
  WiFi.setHostname(settings.getHostname(deviceId).c_str());
  WiFi.mode(WiFiMode::WIFI_STA);
  WiFi.begin(settings.getSsid(), settings.getPwd());
  unsigned long start = millis();
  Serial.printf("Connecting to '%s'...", settings.getSsid().c_str());
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000UL) {
    yield();
    delay(500);
    Serial.print(".");
  }

  return WiFi.status() == WL_CONNECTED;
}

/**
 * This function sets-up the device to serve its own WiFi network
 * in AP Mode. Returning the status of successfullness.
 * 
 * @return Returns a true as bool if the AP setup is successful otherwise
 * returns a false indicating failure.
 */
bool doWiFiAPMode() {
  WiFi.setOutputPower(20.5F);
  WiFi.setHostname(settings.getHostname(deviceId).c_str());
  WiFi.mode(WiFiMode::WIFI_AP);
  WiFi.softAPConfig(
    IpUtils::stringIPv4ToIPAddress(settings.getApNetIp()), 
    IpUtils::stringIPv4ToIPAddress(settings.getApGateway()), 
    IpUtils::stringIPv4ToIPAddress(settings.getApSubnet())
  );

  return WiFi.softAP(settings.getApSsid(deviceId), settings.getApPwd());
}

/**
 * Check to see if the factory reset pin is being held down during
 * normal operation of the device. If it is, then count for how long
 * it is held down for. If less than 6 seconds then signal the last
 * octet of the IP Address. If longer than 6 seconds then signal the
 * entire IP Address.
*/
void doCheckIpDisplayRequest() {
  int counter = 0;

  while (digitalRead(RESTORE_PIN) == HIGH) { // The restore button is being pressed...
    counter++;
    delay(1000);
  }

  if (counter > 0 && counter < 6) { // Reset button was pressed for less than 6 seconds...        
    Utils::signalIpAddress(LED_PIN, getIpAddress(), true);
  } else if (counter >= 6) { // Reset button was pressed for 6 seconds or more...
    Utils::signalIpAddress(LED_PIN, getIpAddress(), false);
  }
}

/**
 * This function is used to get the IP Address of the device regardless if it is in 
 * AP mode or connected to an external WiFi Network.
 * 
 * @return Returns the IP Address of this device in dot notation as String.
*/
String getIpAddress() {
  if (WiFi.getMode() == WiFiMode::WIFI_AP) { // WiFi is in AP mode...

    return WiFi.softAPIP().toString();
  } 
  
  // WiFi is not in AP mode
  return WiFi.localIP().toString();
}

/**
 * This function handles receiving broadcasts from sensors on the network and tracks the 
 * sensors so that they can be selected from a list by a user later on so that the device
 * knows what senor it is linked to. When linked to a sensor, this function will parse and 
 * track the temperature data associated with that sensor.
 */
void doHandleBroadcasts() {
  int size = udp.parsePacket();
  if (size > 0) {
    int read = udp.read(udpPktBuf, UDP_TX_PACKET_MAX_SIZE);
    if (read < UDP_TX_PACKET_MAX_SIZE) udpPktBuf[read] = 0;
    String input = String((char*) udpPktBuf);
    int p1 = input.indexOf("::");
    if (p1 != -1) { // Check if message has desired header
      if (input.substring(0, p1).equals("TempBuddy-Sensor")) { // It is the right kind of message
        int p2 = input.indexOf("::", p1 + 2);
        if (p2 != -1) { // Found Device IP; but don't care about it
          p1 = input.indexOf("::", p2 + 2);
          if (p1 != -1) { // Found device location
            String deviceLocation = input.substring(p2 + 2, p1);
            p2 = input.indexOf("::", p1 + 2);
            if (p2 != -1) { // Found the device id
              String deviceId = input.substring(p1 + 2, p2);
              recentSensorLastSeen[deviceId] = millis();
              recentSensorNames[deviceId] = deviceLocation;
              // Update the tracked temp if this is our connected device
              if (settings.getTempSensorID().equals(deviceId)) { // This is our connected device
                int tStartIdx = input.indexOf("::T_", p1 + 2);
                if (tStartIdx != -1) {
                  tStartIdx += 4;
                  int tEndIdx = input.indexOf("::", tStartIdx);
                  if (tEndIdx != -1) { // Looks like we got the temp
                    sensorLastTempRead = input.substring(tStartIdx, tEndIdx).toFloat();
                  }
                }
              }
              // Prune the last seen list
              String oldIds[recentSensorLastSeen.size()];
              int oldIdsSize = 0;
              std::map<String, unsigned long>::iterator it;
              // Discover old entries
              for (it = recentSensorLastSeen.begin(); it != recentSensorLastSeen.end(); it ++) {
                if (millis() - it->second > 60000ul) {
                  // Record is old; remember it for later
                  oldIds[++oldIdsSize] = it->first;
                } 
              }
              // Remove old entries
              for (int i = 0; i < oldIdsSize; i++) {
                recentSensorNames.erase(oldIds[i]);
                recentSensorLastSeen.erase(oldIds[i]);
              }
            }
          }
        }
      }
    }
  }
}

/**
 * This function handles the operations that are specific to the device.
 * Specifically speaking it handles turning on or off the controlled outlet
 * based on user input if in Manual Mode or based on Temperature if in Auto Mode.
 * This is the only place in code that should be controlling the controlled outlet
 * of the device. Everywere else simply interacts with this function by maintaining
 * the key values in settings.
*/
void doHandleDeviceOperations() {
    // Handle the Auto Control functionality
    if (settings.getIsAutoControl() && !settings.getTempSensorID().isEmpty()) { // Auto Control is active...
        if (settings.getIsHeat()) { // In Heat control mode
            if (settings.getLastKnownTemp() > settings.getDesiredTemp()) { // It is too warm
                settings.setIsControlOn(false);
            } else if (settings.getLastKnownTemp() < settings.getDesiredTemp() - settings.getTempPadding()) { // It's too cool
                settings.setIsControlOn(true);
            }
        } else { // In Cold control mode
            if (settings.getLastKnownTemp() < settings.getDesiredTemp()) { // It is too cold
                settings.setIsControlOn(false);
            } else if (settings.getLastKnownTemp() > settings.getDesiredTemp() + settings.getTempPadding()) { // It's too warm
                settings.setIsControlOn(true);
            }
        }
    }

    // Handle the toggling of the controlled device on/off
    if (settings.getIsControlOn()) { // Controls should be ON
        if (digitalRead(OUTLET_PIN) == LOW) { // Control is NOT on but should be
           digitalWrite(OUTLET_PIN, HIGH);
        }
    } else { // Controls should be OFF
        if (digitalRead(OUTLET_PIN) == HIGH) { // Controls is ON but should NOT be
            digitalWrite(OUTLET_PIN, LOW);
        }
    }
}

/**
 * This function handles initialization of the WebServer.
 * 
*/
void initWebServer() {
  /* Setup Endpoint Handlers */
  webServer.on("/", endpointHandlerRoot);
  webServer.on("/admin", endpointHandlerAdmin);
  webServer.onNotFound(endpointHandlerRoot);
  webServer.begin();
}

/**
 * ENDPOINT HANDLER ("/" AKA Root)
 * 
 * This is the Root endpoint handler when the client sends a
 * request to the Root endpoint.
*/
void endpointHandlerRoot() {
  static bool showControls = false;
  String statusMessage = "";

  // Handle incoming parameters
  if (webServer.method() == HTTP_POST) {
    String doAction = webServer.arg("do");
    Serial.printf("Do Action is: %s\n", doAction.c_str());
    if (doAction == nullptr || doAction.isEmpty()) { // <---- No Action
      // Do nothing for now
    } else if (doAction.equals("show_controls")) { // <------ Show Controls
      showControls = true;
    } else if (doAction.equals("hide_controls")) { // <------ Hide Controls
      showControls = false;
    } else if (doAction.equals("update_auto")) { // <-------- Update Auto Controls
      doUpdateAutoSettings();
      statusMessage = "Update Successful!";
    } else if (
      doAction.equals("btn_on") 
      && !settings.getIsAutoControl()
    ) { // <------------------------------------------------- Manual Switch On
      settings.setIsControlOn(true);
    } else if (
      doAction.equals("btn_off") 
      && !settings.getIsAutoControl()
    ) { // <------------------------------------------------- Manual Switch Off
      settings.setIsControlOn(false);
    } else if (doAction.equals("admin_save")) { // <--------- Admin Save
      bool needsReboot = doSaveAdminSettings();
      if (needsReboot) {
        sendInfoPageWithoutControls("Save Complete!\nReboot Required!!\n\nRebooting Now.");
        yield();
        delay(6000);

        ESP.restart();
      }
      statusMessage = "Save Complete!";
    }
  }

  if (showControls) {
    sendInfoPageWithControls(statusMessage);
  } else {
    sendInfoPageWithoutControls(statusMessage);
  }
}

/**
 * Saves changes to the Admin Settings.
 * 
 */
bool doSaveAdminSettings() {
  String title = webServer.arg("title");
  String heading = webServer.arg("heading");
  String ssid = webServer.arg("ssid");
  String pwd = webServer.arg("pwd");
  String adminuser = webServer.arg("adminuser");
  String adminpwd = webServer.arg("adminpwd");

  bool reboot = false;
  bool update = false;

  // Update Title...
  title.trim();
  if (!title.isEmpty() && !title.equals(settings.getTitle())) {
    settings.setTitle(title.c_str());
    update = true;
  }

  // Update Heading...
  heading.trim();
  if (!heading.isEmpty() && !heading.equals(settings.getHeading())) {
    settings.setHeading(heading.c_str());
    update = true;
  }

  // Update SSID...
  ssid.trim();
  if (!ssid.isEmpty() && !ssid.equals(settings.getSsid())) {
    settings.setSsid(ssid.c_str());
    update = true;
    reboot = true;
  }

  // Update PWD...
  pwd.trim();
  if (!pwd.isEmpty() && !pwd.equals(settings.getPwd())) {
    settings.setPwd(pwd.c_str());
    update = true;
    reboot = true;
  }

  // Update Admin User...
  adminuser.trim();
  if (!adminuser.isEmpty() && !adminuser.equals(settings.getAdminUser())) {
    settings.setAdminUser(adminuser.c_str());
    update = true;
  }

  // Update Admin Pwd...
  adminpwd.trim();
  if (!adminpwd.isEmpty() && !adminpwd.equals(settings.getAdminPwd())) {
    settings.setAdminPwd(adminpwd.c_str());
    update = true;
  }
  
  // Save Settings...
  if (update) {
    settings.saveSettings();
  }

  return reboot;
}

/**
 * Used to update the Autocontrol Settings.
 * 
 */
void doUpdateAutoSettings() {
  settings.setIsAutoControl(webServer.arg("autocontrol").equals("auto_enabled"));
  settings.setIsHeat(webServer.arg("controltype").equals("heat"));
  if (settings.getIsAutoControl()) {
    settings.setTempSensorID(webServer.arg("sensor").c_str());
    settings.setTempSensorName(recentSensorNames[settings.getTempSensorID()].c_str());
  } else {
    settings.setTempSensorID("");
    settings.setTempSensorName("");
  }
  settings.setDesiredTemp((webServer.arg("desiredtemp").isEmpty() ? 999.9 : webServer.arg("desiredtemp").toFloat()));
  settings.setTempPadding((webServer.arg("temppadding").isEmpty() ? 999.9 : webServer.arg("temppadding").toFloat()));
  
  settings.saveSettings();
}

/**
 * Sends the Admin Settings Page.
 * 
 */
void sendAdminPage() {
  String content = ADMIN_SETTINGS_PAGE;

  content.replace("${title}", settings.getTitle());
  content.replace("${heading}", settings.getHeading());
  content.replace("${version}", FIRMWARE_VERSION);

  content.replace("${title}", settings.getTitle());
  content.replace("${heading}", settings.getTitle());
  content.replace("${ssid}", settings.getSsid());
  content.replace("${pwd}", settings.getPwd());
  content.replace("${adminuser}", settings.getAdminUser());
  content.replace("${adminpwd}", settings.getAdminPwd());

  webServer.send(200, "text/html", content);
  yield();
}

/**
 * Sends the Information Page version without Controls Showing.
 * 
 */
void sendInfoPageWithoutControls(String statusMessage) {
  String content = INFO_PAGE_WO_CONTROLLS;

  content.replace("${title}", settings.getTitle());
  content.replace("${heading}", settings.getHeading());
  content.replace("${version}", FIRMWARE_VERSION);

  if (statusMessage.isEmpty()) {
    content.replace("${status_message}", "");
  } else {
    String message = STATUS_MESSAGE;
    message.replace("${message}", statusMessage);
    content.replace("${status_message}", message);
  }

  if (settings.getTempSensorID().isEmpty()) {
    content.replace("${sensor_name}", "None");
    content.replace("${temp}", "N/A");
  } else {
    content.replace("${sensor_name}", settings.getTempSensorName().c_str());
    content.replace("${temp}", String(Utils::convertCelciusToFahrenheit(sensorLastTempRead)));
  }

  content.replace("${control_type}", (settings.getIsHeat() ? "Heat" : "Cool"));
  content.replace("${auto_manual}", (settings.getIsAutoControl() ? "Auto" : "Manual"));
  content.replace("${on_off_status}", (settings.getIsControlOn() ? "On" : "Off"));

  webServer.send(200, "text/html", content);
  yield();
}

/**
 * Sends the Information Page version With Controls Showing.
 * 
 */
void sendInfoPageWithControls(String statusMessage) {
  String content = INFO_PAGE_W_CONTROLLS;

  content.replace("${title}", settings.getTitle());
  content.replace("${heading}", settings.getHeading());
  content.replace("${version}", FIRMWARE_VERSION);

  if (statusMessage.isEmpty()) {
    content.replace("${status_message}", "");
  } else {
    String message = STATUS_MESSAGE;
    message.replace("${message}", statusMessage);
    content.replace("${status_message}", message);
  }

  if (settings.getTempSensorID().isEmpty()) {
    content.replace("${sensor_name}", "None");
    content.replace("${temp}", "N/A");
  } else {
    content.replace("${sensor_name}", settings.getTempSensorName().c_str());
    content.replace("${temp}", String(Utils::convertCelciusToFahrenheit(sensorLastTempRead)));
  }

  content.replace("${control_type}", (settings.getIsHeat() ? "Heat" : "Cool"));
  content.replace("${auto_manual}", (settings.getIsAutoControl() ? "Auto" : "Manual"));
  content.replace("${on_off_status}", (settings.getIsControlOn() ? "On" : "Off"));
  content.replace("${auto_enabled_checked}", (settings.getIsAutoControl() ? "checked" : ""));
  content.replace("${auto_disabled_checked}", (settings.getIsAutoControl() ? "" : "checked"));
  content.replace("${heat_checked}", (settings.getIsHeat() ? "checked" : ""));
  content.replace("${cool_checked}", (settings.getIsHeat() ? "" : "checked"));
  
  String seenSensorOptions = "";
  std::map<String/*ID*/, String/*Name*/>::iterator it;
  for (it = recentSensorNames.begin(); it != recentSensorNames.end(); it++) {
    String option = SENSOR_OPTION;
    option.replace("${id}", it -> first);
    option.replace("${description}", it -> second);
    option.replace("${selection_flag}", (settings.getTempSensorID().equals(it -> first) ? "selected" : ""));
    seenSensorOptions.concat(option);
  }

  content.replace("${sensor_options}", seenSensorOptions);
  content.replace("${desired_temp}", String(settings.getDesiredTemp()));
  content.replace("${temp_padding}", String(settings.getTempPadding()));
  content.replace("${manual_hide}", (settings.getIsAutoControl() ? "hidden" : ""));

  webServer.send(200, "text/html", content);
  yield();
}

/**
 * #### ENDPOINT HANDLER ("/admin") ####
 *
 * This function shows the admin page for the device. The admin page is also
 * known as the settings page. It is a password protected page that allows
 * for the software's non-volatile settings to be configured.
 *
*/
void endpointHandlerAdmin() {
  /* Ensure user authenticated */
  if (!webServer.authenticate(settings.getAdminUser().c_str(), settings.getAdminPwd().c_str())) {
    // User not yet authenticated

    return webServer.requestAuthentication(DIGEST_AUTH, "AdminRealm", "Authentication failed!");
  }

  sendAdminPage();
}