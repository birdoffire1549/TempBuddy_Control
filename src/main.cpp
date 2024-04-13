/*
  Project Name: ... TempBuddy Control
  Written by: ..... Scott Griffis
  Email: .......... birdoffire1549@gmail.com
  Date: ........... 10-01-2023

  Overview:
  This software was written to be utilized by an ESP8266 Device.
  The device using this software is refered to as a TempBuddy Control Unit.
  It was designed to work with another project that is refered to as a TempBuddy, that particular
  device has the ability to sense and report Temperature and Humidity data via a web interface. 
  This device can be pointed at the IP of a TempBuddy device and read its temp and then react
  to the temperature by controlling an outlet which can have a Heating or Cooling device attached
  to it. If no TempBuddy is connected to this device then the user has the ability to manually 
  control the attached outlet through the hosted webpage. This device also hosts a webpage that can
  be accessed using the device's IP Address and Port 80. Also this device can be configured by 
  accessing its admin page either via an existing WiFi or the device's TempBuddy_Ctrl wifi network 
  when it is in AP Mode.

  SECURITY RELATED PLEASE NOTE: 
  Communication with this Device is NOT encrypted. This means that when configuring the settings of the
  device using the Admin page, the data is transmitted to and from the client in clear text on the network you
  and the device are connected to. The potential for compromise is highest when doing initial settings with the
  device in AP mode as there is no password used for the TempBuddy_Ctrl network. Be cautious as this could result in
  the leaking of target network's SSID and Password when initially setup, also if changing the device's admin
  password don't use one that you use for other sensitive things as it will be sent to the device in clear text.
  [TODO: Add SSL usage]

  Hosted Endpoints:
  /        - This is where the device's information is deplayed as a web page
  /admin   - This is where the device's settings are configured. Default Password: admin

  More Detailed:
  When device is first programmed it boots up as an AccessPoint that can be connected to using a computer, 
  by connecting to the presented network with a name of 'TempBuddy_Ctrl' and no password. Once connected to the
  device's WiFi network you can connect to it for configuration using a web browser via the URL:
  http://192.168.4.1/admin. This will take you to a page requesting a password. Initially the password is
  simply 'admin' but can be changed. This will display the current device settings and allow the user to make desired
  configuration changes to the device. When the Network settings are changed the device will reboot and 
  attempt to connect to the configured network. This code also allows for the device to be equiped with a 
  factory reset button. To perform a factory reset the factory reset button must supply a HIGH to its input
  while the device is rebooted. Upon reboot if the factory reset button is HIGH the stored settings in flash will
  be replaced with the original factory default setttings. The factory reset button also serves another purpose 
  during the normal operation of the device. If pressed breifly the device will flash out the last octet of its 
  IP Address. It does this using the built-in LED. Each digit of the last octet is flashed out with a breif 
  rapid flash between the blink count for each digit. Once all digits have been flashed out the LED will do
  a long rapid flash. Also, one may use the factory reset button to obtain the full IP Address of the device by 
  keeping the ractory reset button pressed during normal device operation for more than 6 seconds. When flashing
  out the IP address the device starts with the first digit of the first octet and flashes slowly that number of 
  times, then it performs a rapid flash to indicate it is on to the next digit. Once all digits in an octet have
  been flashed out the device performs a second after digit rapid flash to indicate it has moved onto a new 
  octet. 
  
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

#include <Arduino.h>
#include <ESP_EEPROM.h>
#include "ExampleSecrets.h"
#include "Secrets.h"
#include <ESP8266WebServerSecure.h>
#include "HtmlContent.h"
#include <ESP8266WiFi.h>

#include "Utils.h"
#include "MyWiFi.h"
#include "Settings.h"
#include "ParseUtils.h"

#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

// ************************************************************************************
// Define Statements
// ************************************************************************************

#define FIRMWARE_VERSION "5.0.1"

#define LED_PIN 5
#define OUTLET_PIN 4
#define RESTORE_PIN 14

// ************************************************************************************
// Setup of Services
// ************************************************************************************
Settings settings = Settings();
MyWiFi myWifi = MyWiFi();
BearSSL::ESP8266WebServerSecure webServer(/*Port*/443);
BearSSL::ServerSessions serverCache(5);

// ************************************************************************************
// Global worker variables
// ************************************************************************************
bool firstLoop = true;

// ************************************************************************************
// Function Prototypes
// ************************************************************************************

void dumpFirmwareVersion(void);
void doHandleReadTempBuddy(void);
void doHandleDeviceOperations(void);
void resetOrLoadSettings(void);
void doStartNetwork(void);
void checkIpDisplayRequest(void);
String htmlPageTemplate(String title, String heading, String content, String redirectUrl = "", int delaySeconds = 3);
void fileUploadHandler(void);
void notFoundHandler(void);
void endpointHandlerAdmin(void);
void endpointHandlerRoot(void);
void initWebServer(void);

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

    // Initialize the device...
    dumpFirmwareVersion();
    Serial.print(F("\nInitializing device... "));

    resetOrLoadSettings();
    doStartNetwork();
    initWebServer();
    delay(50);

    Serial.println(F("Device Initialization Complete."));
}

/** 
 * #### LOOP() - REQUIRED FUNCTION ####
 * 
 * This is the required loop() function of the applicaiton.
 * Here is where all functionality happens or starts to happen.
*/
void loop() {
    if (firstLoop) { // It's the firt time through the loop...
        firstLoop = false;
        Serial.println("Device has begun normal operation.");
    }

    checkIpDisplayRequest();

    // Handle incoming web requests...
    webServer.handleClient();

    doHandleReadTempBuddy();
    doHandleDeviceOperations();
    delay(15);
}

/**
 * Detects and reacts to a reqest for factory reset
 * during the boot-up. Also loads settings from 
 * EEPROM if there are saved settings.
*/
void resetOrLoadSettings() {
    if (digitalRead(RESTORE_PIN) == HIGH) { // Restore button pressed on bootup...
        Serial.println(F("\nPerforming Factory Reset..."));
        settings.factoryDefault();
        Serial.println(F("Factory reset complete."));
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
    if (settings.isNetworkSet()) {
        myWifi.connectToNetwork(settings.getHostname(), settings.getSsid(), settings.getPwd());
    } else {
        myWifi.startAPMode(settings.getHostname(), settings.getApNetIp(), settings.getApSubnet(), settings.getApGateway(), settings.getApSsid(), settings.getApPwd());
    }
}

/**
 * Check to see if the factory reset pin is being held down during
 * normal operation of the device. If it is, then count for how long
 * it is held down for. If less than 6 seconds then signal the last
 * octet of the IP Address. If longer than 6 seconds then signal the
 * entire IP Address.
*/
void checkIpDisplayRequest() {
    int counter = 0;
    while (digitalRead(RESTORE_PIN) == HIGH) { // The restore button is being pressed...
        counter++;
        delay(1000);
    }

    if (counter > 0) { // The reset button was pressed...
        dumpFirmwareVersion();
        // TODO: !!! In AP Mode, dump SSID as well and maybe memory usage?
    }
    
    if (counter > 0 && counter < 6) { // Reset button was pressed for less than 6 seconds...
        Serial.print("Device Address is: ");
        Serial.println(myWifi.getIpAddress());

        Utils::signalIpAddress(LED_PIN, myWifi.getIpAddress(), true);
    } else if (counter >= 6) { // Reset button was pressed for 6 seconds or more...
        Serial.print("Device Address is: ");
        Serial.println(myWifi.getIpAddress());

        Utils::signalIpAddress(LED_PIN, myWifi.getIpAddress(), false);
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
    // Handle the Auto Control functionality...
    if (settings.getIsAutoControl() && !settings.getTempBuddyIp().isEmpty()) { // Auto Control is active...
        if (settings.getIsHeat()) { // In Heat control mode...
            if (settings.getLastKnownTemp() > settings.getDesiredTemp()) { // It is too warm...
                settings.setIsControlOn(false);
            } else if (settings.getLastKnownTemp() < settings.getDesiredTemp() - settings.getTempPadding()) { // It's too cool...
                settings.setIsControlOn(true);
            }
        } else { // In Cold control mode...
            if (settings.getLastKnownTemp() < settings.getDesiredTemp()) { // It is too cold...
                settings.setIsControlOn(false);
            } else if (settings.getLastKnownTemp() > settings.getDesiredTemp() + settings.getTempPadding()) { // It's too warm...
                settings.setIsControlOn(true);
            }
        }
    }

    // Handle the toggling of the controlled device on/off...
    if (settings.getIsControlOn()) { // Controls should be ON...
        if (digitalRead(OUTLET_PIN) == LOW) { // Control is NOT on but should be...
           digitalWrite(OUTLET_PIN, HIGH);
        }
    } else { // Controls should be OFF...
        if (digitalRead(OUTLET_PIN) == HIGH) { // Controls is ON but should NOT be...
            digitalWrite(OUTLET_PIN, LOW);
        }
    }
}


// Used by the doHandleReadTempBuddy function below...
unsigned long lastTempBuddyRead = 0UL;

/**
 * This function handles reaching out to the TempBuddy device for the current temperature
 * periodically. This functionality is throttled to only run once a minute as running it too 
 * often can degrade the device's ability to provide other functionality like answer clients'
 * web requests and signal IP Address as requested.
*/
void doHandleReadTempBuddy() {
    if (!settings.getTempBuddyIp().isEmpty() && (lastTempBuddyRead == 0UL || millis() - lastTempBuddyRead >= 60000)) { // Need to check TempBuddy...
        lastTempBuddyRead = millis();
        if (ParseUtils::validDotNotationIp(settings.getTempBuddyIp())) { // IP Address is valid...
            if (myWifi.isConnected()) { // Connected to WiFi...
                WiFiClient client;
                HTTPClient http;

                http.begin(client, String("http://") + settings.getTempBuddyIp() + "/");

                int respCode = http.GET();
                if (respCode >= 200 && respCode <= 299) { // Good response...
                    Serial.printf("Got a '%d' response code from TempBuddy.\n", respCode);
                    String payload = http.getString();
                    if (!payload.isEmpty()) { // Something in payload...
                        String temp = ParseUtils::parseByKeyword(payload, "Temperature:", "&deg;");
                        temp.trim();
                        if (!temp.isEmpty()) { // String not empty...
                           settings.setLastKnownTemp(temp.toFloat());
                        }
                    }
                }

                http.end();
              }
          }
      }
}

/**
 * The purpose of this function is to simply dump the software
 * version to the serial console as desired. Additional information
 * can be added here if needed going forward.
*/
void dumpFirmwareVersion() {
    Serial.println("==================================");
    Serial.print("Firmware Version: ");
    Serial.println(FIRMWARE_VERSION);
    Serial.println("==================================");
    Serial.println("");
}

/**
 * #### INITIALIZE ####
 * This is an initialization function for the WebServer.
*/
void initWebServer() {
  #ifndef Secrets_h
    webServer.getServer().setRSACert(new BearSSL::X509List(SAMPLE_SERVER_CERT), new BearSSL::PrivateKey(SAMPLE_SERVER_KEY));
  #else
    webServer.getServer().setRSACert(new BearSSL::X509List(server_cert), new BearSSL::PrivateKey(server_key));
  #endif
  webServer.getServer().setCache(&serverCache);

  /* Setup Endpoint Handlers */
  webServer.on("/", endpointHandlerRoot);
  webServer.on("/admin", endpointHandlerAdmin);
  webServer.onNotFound(notFoundHandler);
  webServer.onFileUpload(fileUploadHandler);

  webServer.begin();
}

/**
 * #### ENDPOINT HANDLER ("/" AKA Root) ####
 * This is the Root endpoint handler when the client sends a 
 * request to the Root endpoint.
*/
void endpointHandlerRoot() {
  String content = "";
  Serial.println("Client requested endpoint: '/'; Generating response content...");

  // Handle incoming parameters...
  if (webServer.arg("source").equalsIgnoreCase("manualcontrols") && !settings.getIsAutoControl()) { // <----------------------- AutoControl is OFF...
    String autoControl = webServer.arg("autocontrol");
    if (!autoControl.isEmpty()) { // Incoming auto control update from Manual Controls...
      settings.setIsAutoControl(autoControl.equalsIgnoreCase("enabled"));
      settings.saveSettings();
    } 
    if (!settings.getIsAutoControl()) {
      if (webServer.arg("control").equalsIgnoreCase("off")) { // Parameter found to turn control off...
        settings.setIsControlOn(false);
      } else if (webServer.arg("control").equalsIgnoreCase("on")) { // Parameter found to turn control on...
        settings.setIsControlOn(true);
      }
    }
  } else if (webServer.arg("source").equalsIgnoreCase("autocontrols") && settings.getIsAutoControl()) { // <------------------- AutoControl is ON...
    bool updateSuccessful = false;
    bool wasUpdate = false;
    String desiredTemp = webServer.arg("desiredtemp");
    String tempPadding = webServer.arg("temppadding");
    String autoControl = webServer.arg("autocontrol");
    if (!autoControl.isEmpty()) { // Handle updating of the enable status of AutoControl...
      settings.setIsAutoControl(autoControl.equalsIgnoreCase("enabled"));
      wasUpdate = true;
    }
    if (!desiredTemp.isEmpty() && !tempPadding.isEmpty()) {
      settings.setDesiredTemp(desiredTemp.toFloat());
      settings.setTempPadding(tempPadding.toFloat());
      wasUpdate = true;
    }

    if (wasUpdate && settings.saveSettings()) {
      updateSuccessful = true;
    }

    if (wasUpdate) {
      if (updateSuccessful) {
        content = content + String(UPDATE_SUCCESSFUL_MSG);
      } else {
        content = content + String(UPDATE_FAILED_MSG);
      }
    }
  }

  // Build and send Information Page...
  String tBudId = settings.getTempBuddyIp();
  bool tempBuddyEnabled = !tBudId.isEmpty() && !tBudId.equals("0.0.0.0");

  String temp = INFO_PAGE;
  temp.replace("${tempbuddyip}", (!tempBuddyEnabled ? "Not Set" : String(settings.getTempBuddyIp())));
  temp.replace("${lastknowntemp}", (!tempBuddyEnabled ? "N/A" : String(settings.getLastKnownTemp())));
  temp.replace("${controltype}", (settings.getIsHeat() ? "Heat" : "Cool"));
  temp.replace("${autocontrolenabled}", (settings.getIsAutoControl() ? "True" : "False"));
  temp.replace("${deviceonstatus}", (settings.getIsControlOn() ? "ON" : "OFF"));
  content.concat(temp);

  // Only show Manual Controls if AutoControl is OFF...
  bool isAutoCtrl = settings.getIsAutoControl();
  String result = "";
  if (!isAutoCtrl) { // AutoControl is OFF...
    content = content + String(MANUAL_CONTROLS_SECTION);
  } else { // AutoControl is ON...
    String temp = String(AUTO_CONTROLS_SECTION);
    temp.replace("${desiredtemp}", String(settings.getDesiredTemp()));
    temp.replace("${temppadding}", String(settings.getTempPadding()));
    content = content + temp;    
  }

  Serial.println("Sending html to client for endpoint: '/'");
  webServer.send(200, "text/html", htmlPageTemplate(String(settings.getTitle()), String(settings.getHeading()), content));
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
  if (!webServer.authenticate("admin", settings.getAdminPwd().c_str())) { // User not authenticated...
    
    return webServer.requestAuthentication(DIGEST_AUTH, "AdminRealm", "Authentication failed!");
  }

  // Initial content is the login page unless authenticated
  String content = String(ADMIN_SETTINGS_PAGE);
  
  bool requiresReboot = false;
  bool autoToHome = false;
  
  // Insert data into page contents...
  content.replace("${ssid}", settings.getSsid());
  content.replace("${pwd}", settings.getPwd());
  content.replace("${title}", settings.getTitle());
  content.replace("${heading}", settings.getHeading());
  content.replace("${budyip}", settings.getTempBuddyIp());
  content.replace("${autocontrolenabledchecked}", settings.getIsAutoControl() ? "checked" : "");
  content.replace("${autocontroldisabledchecked}", settings.getIsAutoControl() ? "" : "checked");
  content.replace("${controllingheatchecked}", settings.getIsHeat() ? "checked" : "");
  content.replace("${controllingcoolchecked}", settings.getIsHeat() ? "" : "checked");
  content.replace("${desiredtemp}", String(settings.getDesiredTemp()));
  content.replace("${temppadding}", String(settings.getTempPadding()));
  content.replace("${timeout}", String(settings.getTimeout()));
  content.replace("${adminpwd}", settings.getAdminPwd());
  
  if (webServer.arg("source").equalsIgnoreCase("settings")) { // Refered from settings page so do update...
    // Aquire the incoming new settings...
    String ssid = webServer.arg("ssid");
    String pwd = webServer.arg("pwd");
    String title = webServer.arg("title");
    String heading = webServer.arg("heading");
    String budyIp = webServer.arg("budyip");
    String isAutoCtrl = webServer.arg("autocontrol");
    String isHeat = webServer.arg("controltype");
    String desiredTemp = webServer.arg("desiredtemp");
    String tempPadding = webServer.arg("temppadding");
    String adminPwd = webServer.arg("adminpwd");

    // Verify the validity of the incoming new settings...
    if (!ssid.isEmpty() && !ssid.equals(settings.getSsid())) {
      requiresReboot = true;
      settings.setSsid(ssid.c_str());
    }
    if (!pwd.isEmpty() && !pwd.equals(settings.getPwd())) {
      requiresReboot = true;
      settings.setPwd(pwd.c_str());
    }
    if (!title.isEmpty()) {
      settings.setTitle(title.c_str());
    }
    if (!heading.isEmpty()) {
      settings.setHeading(heading.c_str());
    }
    if (isAutoCtrl.equals("enabled")) {
      settings.setIsAutoControl(true);
    } else if (isAutoCtrl.equals("disabled")) {
      settings.setIsAutoControl(false);
    }
    if (isHeat.equals("heat")) {
      settings.setIsHeat(true);
    } else if (isHeat.equals("cool")) {
      settings.setIsHeat(false);
    }
    float fTemp = 0;
    if (!desiredTemp.isEmpty() && (fTemp = desiredTemp.toFloat()) >= -100.0 && fTemp <= 100.0) {
      settings.setDesiredTemp(fTemp);
    }
    if (budyIp.isEmpty() || ParseUtils::validDotNotationIp(budyIp)) {
      if (!budyIp.isEmpty() && settings.getTempBuddyIp().isEmpty()) {
        // FYI: This prevents inital action before first read
        settings.setLastKnownTemp(settings.getDesiredTemp());
      }
      settings.setTempBuddyIp(budyIp.c_str());
    }
    if (!tempPadding.isEmpty() && (fTemp = tempPadding.toFloat()) >= 0.0 && fTemp <= 100.0) {
      settings.setTempPadding(fTemp);
    }
    if (!adminPwd.isEmpty() && adminPwd.length() <= 12) {
      settings.setAdminPwd(adminPwd.c_str());
    }

    if (settings.saveSettings()) { // Successful...
      if (requiresReboot) { // Needs to reboot...
        content = "<div id=\"successful\">Settings update Successful!</div>"
          "<h4>Device will reboot now...</h4>";
        
        // TODO: REBOOT REQUIRED BUT SPECIFY NEW? IP ADDRESS!!!
      } else { // No reboot needed; Send to home page...
        
        return webServer.send(
          200, 
          "text/html", 
          htmlPageTemplate(
            settings.getTitle(), 
            "Device Settings", 
            "<div id=\"success\">Settings update Successful!</div><a href='/'><h4>Home</h4></a>", 
            "/", 
            5
          )
        );
      }
    } else { // Error...

      return webServer.send(500, "text/html", htmlPageTemplate("500 - Server Error", "Server Error!", "<div id=\"failed\">Error Saving Settings!!!</div>", "/", 5));
    }
  }
  
  // Send the page...
  if (requiresReboot) { // Reboot required...
    webServer.send(200, "text/html", htmlPageTemplate(settings.getTitle(), "Device Settings", content));
    delay(1000);
    ESP.restart();
  } else { // No reboot required...
    if (autoToHome) {
      webServer.send(200, "text/html", htmlPageTemplate(settings.getTitle(), "Device Settings", content, "/", 5));
    } else {
      webServer.send(200, "text/html", htmlPageTemplate(settings.getTitle(), "Device Settings", content));
    }
  }
}

/**
 * #### HANDLER - NOT FOUND ####
 * This is a function which is used to handle web requests when the requested resource is not valid.
 * 
*/
void notFoundHandler() {
  webServer.send(404, "text/html", htmlPageTemplate("404 Not Found", "OOPS! You broke it!!!", "Just kidding...<br>But seriously what you were looking for doesn't exist."));
}

void fileUploadHandler() {
  webServer.send(400, "text/html", htmlPageTemplate("400 Bad Request", "Uhhh, Wuuuuut!?", "Um, I don't want your nasty files, go peddle that junk elsewhere!"));
}

/** 
 * #### HTML PAGE TEMPLATE ####
 *
 * This function is used to Generate the HTML for a web page where the 
 * title, heading and content is provided to the function as parameters.
 * 
 * @param title - The page's title as String.
 * @param heading - The heading that appears on the info page as String.
 * @param content - The main content of the web page as String.
 * @param redirectUrl - OPTIONAL PARAM, used to specify a page that this page should
 * redirect to after a specified amount of time.
 * @param delaySeconds - OPTIONAL PARAM, the number of seconds to delay before sending
 * the client to the redirectUrl, as int.
*/
String htmlPageTemplate(String title, String heading, String content, String redirectUrl, int delaySeconds) {
  String result = HTML_PAGE_TEMPLATE;

  // Prepare the contents of the HTML page...
  result.replace("${title}", title);
  result.replace("${heading}", heading);
  result.replace("${content}", content);
  if (redirectUrl.isEmpty()) { // No redirect URL was specified...
    result.replace("${metainsert}", "");
  } else { // A redirect was specified...
    String temp = "<meta http-equiv=\"refresh\" content=\"";
    temp.concat(String(delaySeconds));
    temp.concat("\"; URL=\"");
    temp.concat(redirectUrl);
    temp.concat("\" />");

    result.replace("${metainsert}",  temp);
  }

  return result;
}