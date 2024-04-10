/*
  CLASS: MyWeb

  This class acts as a web-server for the implementing software and handles serving
  up hardcoded web pages. All changes to web content served by this device should
  be made in this class.

  Written by: Scott Griffis
  Date: 10-10-2023
*/

#include "MyWeb.h"




/*
  CLASS CONSTRUCTOR: Allows external instantiation of the class and also
  allows for a reference to the settings to be passed to this class so that
  all needed settings can be obtained from there.

  @param settings - A reference to the address of a settings instance as *Settings.
*/
MyWeb::MyWeb(Settings *settings) {
  this->settings = settings;
}




/*
  Used to start the web service so that it can answer requests from
  clients as they connect. The web service will not work unless this
  function is called before attempting to service any web requests.
  This means that this function must be called prior to using the run() 
  function.
*/
void MyWeb::begin() {
  server.begin();
}




/*
  Used to close down or stop the web service from answering clients'
  requests.
*/
void MyWeb::close() {
  server.close();
}




/*
  This function is critical to the functionality of this web service. This function
  needs to be called repeatedly and at a high frequency as it allows the web server
  to answer and process client requests. The web service will only accept and 
  respond to clients when this method is called.
*/
void MyWeb::run() {
  WiFiClient client = server.accept();
  if (!client) { // No client...
    
    return; // done for now...
  }

  Serial.println(F("WebServer accepted Client."));

  client.setTimeout(settings->getTimeout());

  // Read input and parse request...
  String req = client.readStringUntil('\r');
  int space1 = req.indexOf(' ');
  int space2 = -1;
  if (space1 != -1) { // First space was found...
    space2 = req.indexOf(' ', space1 + 1);
  }
  if (space1 != -1 && space2 != -1) { // Both spaces were found...
    req = req.substring(space1 + 1, space2);
  }

  // Handle the incoming request...
  handleRequest(req, client);
} 





/*
===========================================================================
Private Functions Below
===========================================================================
*/




/*
  PRIVATE: This function is used internally to parse and handle client
  requests. All client requests must be serviced through this function.

  @param request - The client's request as String.
  @param client - A reference to the WiFiClient instance being used to
  communicate with the connected client, as WiFiClient.
*/
void MyWeb::handleRequest(String request, WiFiClient client) {
  // Evaluate and act upon the request...
  int paramsIndex = request.indexOf("?");
  String params = "";
  if (paramsIndex != -1) {
    params = request.substring(paramsIndex + 1);
    request = request.substring(0, paramsIndex);
  }
  
  String reqBody = client.readString();
  reqBody.trim();

  if (request.equals("/")) { // <------------------------- ['/']
    showIndexPage(client, params, ParseUtils::decodeUrlString(reqBody));
  } else if (request.equals("/admin")) { // <------------- ['/admin']
    showAdminPage(client, params, ParseUtils::decodeUrlString(reqBody));
  }
}




/*
=================================================================================
                          HTML Page Rendering Functions
=================================================================================
*/




/******************************************************
 * INFO/ROOT PAGE
 ******************************************************
  This function shows the info page to a given client and renders the 
  information which it contains prior to sending its content to the client.

  @param client - The WiFiClient used to communicate with the client.
  @param parameters - The URL parameter section of the client's request as String.
  @param bodyIn - The body of the Client's POST request if one exists as String.
 */
void MyWeb::showIndexPage(WiFiClient client, String parameters, String bodyIn) {
  String content = String("");
  
  // Handle incoming parameters...
  if (bodyIn.indexOf("source=manualcontrols") != -1 && !settings->getIsAutoControl()) { // <----------------------- AutoControl is OFF...
    bodyIn.trim(); // Because trailing whitespace can affect last parameter value.
    String autoControl = ParseUtils::parseByKeyword(bodyIn, "autocontrol=", "&");
    if (!autoControl.isEmpty()) { // Incoming auto control update from Manual Controls...
      settings->setIsAutoControl(autoControl.equals("enabled"));
      settings->saveSettings();
    } 
    if (!settings->getIsAutoControl()) {
      if (parameters.indexOf("control=off") != -1) { // Parameter found to turn control off...
        settings->setIsControlOn(false);
      } else if (parameters.indexOf("control=on") != -1) { // Parameter found to turn control on...
        settings->setIsControlOn(true);
      }
    }
  } else if (bodyIn.indexOf("source=autocontrols") != -1 && settings->getIsAutoControl()) { // <------------------- AutoControl is ON...
    bool updateSuccessful = false;
    bool wasUpdate = false;
    bodyIn.trim();  // Because trailing whitespace can affect last parameter value.
    String desiredTemp = ParseUtils::parseByKeyword(bodyIn, "desiredtemp=", "&");
    String tempPadding = ParseUtils::parseByKeyword(bodyIn, "temppadding=", "&");
    String autoControl = ParseUtils::parseByKeyword(bodyIn, "autocontrol=", "&");
    if (!autoControl.isEmpty()) { // Handle updating of the enable status of AutoControl...
      settings->setIsAutoControl(autoControl.equals("enabled"));
      wasUpdate = true;
    }
    if (!desiredTemp.isEmpty() && !tempPadding.isEmpty()) {
      settings->setDesiredTemp(desiredTemp.toFloat());
      settings->setTempPadding(tempPadding.toFloat());
      wasUpdate = true;
    }

    if (wasUpdate && settings->saveSettings()) {
      updateSuccessful = true;
    }

    if (wasUpdate) {
      if (updateSuccessful) {
        content += UPDATE_SUCCESSFUL_MSG;
      } else {
        content += UPDATE_FAILED_MSG;
      }
    }
  }

  // Build and send Information Page...
  String tBudId = String(settings->getTempBuddyIp());
  tBudId.trim();
  bool tempBuddyEnabled = !tBudId.isEmpty();

  String temp = INFO_PAGE;
  temp.replace("${tempbuddyip}", (!tempBuddyEnabled ? "Not Set" : String(settings->getTempBuddyIp())));
  temp.replace("${lastknowntemp}", (!tempBuddyEnabled ? "N/A" : String(settings->getLastKnownTemp())));
  temp.replace("${controltype}", (settings->getIsHeat() ? "Heat" : "Cool"));
  temp.replace("${autocontrolenabled}", (settings->getIsAutoControl() ? "True" : "False"));
  temp.replace("${deviceonstatus}", (settings->getIsControlOn() ? "ON" : "OFF"));
  content += temp;

  // Only show Manual Controls if AutoControl is OFF...
  if (!settings->getIsAutoControl()) { // AutoControl is OFF...
    content += MANUAL_CONTROLS_SECTION;
  } else { // AutoControl is ON...
    String temp = AUTO_CONTROLS_SECTION;
    temp.replace("${desiredtemp}", String(settings->getDesiredTemp()));
    temp.replace("${temppadding}", String(settings->getTempPadding()));
    content += temp;    
  }

  client.println(htmlPageTemplate(String(settings->getTitle()), String(settings->getHeading()), content));
  client.flush();
}




/****************************************************
  * ADMIN/SETTINGS PAGE
  ***************************************************
  This function shows the admin page for the device. The admin page is also
  known as the settings page. It is a password protected page that allows 
  for the software's non-volatile settings to be configured.

  @param client - The WiFiClient used to communicate with the client.
  @param parameters - The URL parameter section of the client's request as String.
  @param bodyIn - The body of the Client's POST request if one exists as String.
*/
void MyWeb::showAdminPage(WiFiClient client, String parameters, String bodyIn) {
  // Initial content is the login page unless authenticated
  String content = ADMIN_AUTH_FORM;

  bodyIn.trim();
  
  bool requiresReboot = false;
  bool autoToHome = false;
  if (bodyIn.indexOf("source=login") != -1 && bodyIn.indexOf("adminpwd=") != -1) { // From login page so verify then settings...
    String adminPwd = ParseUtils::parseByKeyword(bodyIn, "adminpwd=", "&");
    if (settings->getAdminPwd().equals(adminPwd)) { // Authorized...
      content = ADMIN_SETTINGS_PAGE;

      // Insert data into page contents...
      content.replace("${ssid}", settings->getSsid());
      content.replace("${pwd}", settings->getPwd());
      content.replace("${title}", settings->getTitle());
      content.replace("${heading}", settings->getHeading());
      content.replace("${budyip}", settings->getTempBuddyIp());
      content.replace("${autocontrolenabledchecked}", settings->getIsAutoControl() ? "checked" : "");
      content.replace("${autocontroldisabledchecked}", settings->getIsAutoControl() ? "" : "checked");
      content.replace("${controllingheatchecked}", settings->getIsHeat() ? "checked" : "");
      content.replace("${controllingcoolchecked}", settings->getIsHeat() ? "" : "checked");
      content.replace("${desiredtemp}", String(settings->getDesiredTemp()));
      content.replace("${temppadding}", String(settings->getTempPadding()));
      content.replace("${timeout}", String(settings->getTimeout()));
      content.replace("${adminpwd}", settings->getAdminPwd());
    }
  } else if (bodyIn.indexOf("source=settings") != -1) { // Refered from settings page so do update...
    // Aquire the incoming new settings...
    String ssid = ParseUtils::parseByKeyword(bodyIn, "ssid=", "&");
    String pwd = ParseUtils::parseByKeyword(bodyIn, "pwd=", "&");
    String title = ParseUtils::parseByKeyword(bodyIn, "title=", "&");
    String heading = ParseUtils::parseByKeyword(bodyIn, "heading=", "&");
    String budyIp = ParseUtils::parseByKeyword(bodyIn, "budyip=", "&");
    String isAutoCtrl = ParseUtils::parseByKeyword(bodyIn, "autocontrol=", "&");
    String isHeat = ParseUtils::parseByKeyword(bodyIn, "controltype=", "&");
    String desiredTemp = ParseUtils::parseByKeyword(bodyIn, "desiredtemp=", "&");
    String tempPadding = ParseUtils::parseByKeyword(bodyIn, "temppadding=", "&");
    String timeout = ParseUtils::parseByKeyword(bodyIn, "timeout=", "&");
    String adminPwd = ParseUtils::parseByKeyword(bodyIn, "adminpwd=", "&");

    // Verify the validity of the incoming new settings...
    if (!ssid.isEmpty() && !ssid.equals(settings->getSsid())) {
      requiresReboot = true;
      settings->setSsid(ssid.c_str());
    }
    if (!pwd.isEmpty() && !pwd.equals(settings->getPwd())) {
      requiresReboot = true;
      settings->setPwd(pwd.c_str());
    }
    if (!title.isEmpty()) {
      settings->setTitle(title.c_str());
    }
    if (!heading.isEmpty()) {
      settings->setHeading(heading.c_str());
    }
    if (isAutoCtrl.equals("enabled")) {
      settings->setIsAutoControl(true);
    } else if (isAutoCtrl.equals("disabled")) {
      settings->setIsAutoControl(false);
    }
    if (isHeat.equals("heat")) {
      settings->setIsHeat(true);
    } else if (isHeat.equals("cool")) {
      settings->setIsHeat(false);
    }
    float fTemp = 0;
    if (!desiredTemp.isEmpty() && (fTemp = desiredTemp.toFloat()) >= -100.0 && fTemp <= 100.0) {
      settings->setDesiredTemp(fTemp);
    }
    if (budyIp.isEmpty() || ParseUtils::validDotNotationIp(budyIp)) {
      if (!budyIp.isEmpty() && settings->getTempBuddyIp().isEmpty()) {
        // FYI: This prevents inital action before first read
        settings->setLastKnownTemp(settings->getDesiredTemp());
      }
      settings->setTempBuddyIp(budyIp.c_str());
    }
    if (!tempPadding.isEmpty() && (fTemp = tempPadding.toFloat()) >= 0.0 && fTemp <= 100.0) {
      settings->setTempPadding(fTemp);
    }
    long lTemp = 0;
    if (!timeout.isEmpty() && (lTemp = timeout.toInt()) >= 1) {
      settings->setTimeout((unsigned long) lTemp);
    }
    if (!adminPwd.isEmpty() && adminPwd.length() <= 12) {
      settings->setAdminPwd(adminPwd.c_str());
    }

    if (settings->saveSettings()) { // Successful...
      if (requiresReboot) { // Needs to reboot...
        content = "<div id=\"successful\">Settings update Successful!</div>"
          "<h4>Device will reboot now...</h4>";
      } else { // No reboot needed; Send to home page...
        autoToHome = true;
        content = "<div id=\"success\">Settings update Successful!</div>"
          "<a href='/'><h4>Home Page</h4></a>";
      }
    } else { // Error...
      autoToHome = true;
      requiresReboot = false;
      content = "<div id=\"failed\">Error Saving Settings!!!</div>";
    }
  }
  
  // Send the page...
  if (requiresReboot) { // Reboot required...
    client.println(htmlPageTemplate(settings->getTitle(), "Device Settings", content));
    client.flush();
    delay(1000);
    ESP.restart();
  } else { // No reboot required...
    if (autoToHome) {
      client.println(htmlPageTemplate(settings->getTitle(), "Device Settings", content, "/", 5));
    } else {
      client.println(htmlPageTemplate(settings->getTitle(), "Device Settings", content));
    }
  }
}




/**********************************************************************
  * HTML PAGE TEMPLATE
  *********************************************************************
  This function is used to Generate the HTML for a web page where the 
  title, heading and content is provided to the function as parameters.

  @param title - The page's title as String.
  @param heading - The heading that appears on the info page as String.
  @param content - The main content of the web page as String.
  @param redirectUrl - OPTIONAL PARAM, used to specify a page that this page should
  redirect to after a specified amount of time.
  @param delaySeconds - OPTIONAL PARAM, the number of seconds to delay before sending
  the client to the redirectUrl, as int.
*/
String MyWeb::htmlPageTemplate(String title, String heading, String content, String redirectUrl, int delaySeconds) {
  String result = HTML_PAGE_TEMPLATE;

  // Prepare the contents of the HTML page...
  result.replace("${title}", title);
  result.replace("${heading}", heading);
  result.replace("${content}", content);
  if (redirectUrl.isEmpty()) { // No redirect URL was specified...
    result.replace("${metainsert}", "");
  } else { // A redirect was specified...
    result.replace("${metainsert}", String("") + "<meta http-equiv=\"refresh\" content=\"" + String(delaySeconds) + "\"; URL=\"" + redirectUrl + "\" />");
  }

  return result;
}
