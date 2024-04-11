/*
  CLASS: MyWeb

  This class acts as a web-server for the implementing software and handles serving
  up hardcoded web pages. All changes to web content served by this device should
  be made in this class.

  Written by ...... : Scott Griffis
  Original Date ... : 10-10-2023
  ReWrite ......... : 04-11-2024
*/

#include "MyWeb.h"

/**
 * CLASS CONSTRUCTOR: Allows external instantiation of the class and also
 * allows for a reference to the settings to be passed to this class so that
 * all needed settings can be obtained from there.
 * 
 * @param settings - A reference to the address of a settings instance as *Settings.
*/
MyWeb::MyWeb(Settings *settings) {
  this->settings = settings;
  #ifndef Secrets_h
    webServer.getServer().setRSACert(new BearSSL::X509List(SAMPLE_SERVER_CERT), new BearSSL::PrivateKey(SAMPLE_SERVER_KEY));
  #else
    webServer.getServer().setRSACert(new BearSSL::X509List(server_cert), new BearSSL::PrivateKey(server_key));
  #endif
  webServer.getServer().setCache(&serverCache);

  /* Setup Endpoint Handlers */
  webServer.on("/", HTTP_GET, [this](){endpointHandlerGETRoot();});
  webServer.on("/admin", [this](){endpointHandlerAdmin();});
  webServer.onNotFound([this](){notFoundHandler();});
  webServer.onFileUpload([this](){fileUploadHandler();});
}

/**
 * Used to start the web service so that it can answer requests from
 * clients as they connect. The web service will not work unless this
 * function is called before attempting to service any web requests.
 * This means that this function must be called prior to using the run() 
 * function.
*/
void MyWeb::begin() {
  webServer.begin();
}

/**
 * Used to close down or stop the web service from answering clients'
 * requests.
*/
void MyWeb::stop() {
  webServer.stop();
}

/**
 * This function is critical to the functionality of this web service. This function
 * needs to be called repeatedly and at a high frequency as it allows the web server
 * to answer and process client requests. The web service will only accept and 
 * respond to clients when this method is called.
*/
void MyWeb::run() {
  webServer.handleClient();
} 

/*
===========================================================================
Private Functions Below
===========================================================================
*/

void MyWeb::endpointHandlerGETRoot() {
  String content = String("");
  
  // Handle incoming parameters...
  if (webServer.arg("source").equalsIgnoreCase("manualcontrols") && !settings->getIsAutoControl()) { // <----------------------- AutoControl is OFF...
    String autoControl = webServer.arg("autocontrol");
    if (!autoControl.isEmpty()) { // Incoming auto control update from Manual Controls...
      settings->setIsAutoControl(autoControl.equalsIgnoreCase("enabled"));
      settings->saveSettings();
    } 
    if (!settings->getIsAutoControl()) {
      if (webServer.arg("control").equalsIgnoreCase("off")) { // Parameter found to turn control off...
        settings->setIsControlOn(false);
      } else if (webServer.arg("control").equalsIgnoreCase("on")) { // Parameter found to turn control on...
        settings->setIsControlOn(true);
      }
    }
  } else if (webServer.arg("source").equalsIgnoreCase("autocontrols") && settings->getIsAutoControl()) { // <------------------- AutoControl is ON...
    bool updateSuccessful = false;
    bool wasUpdate = false;
    String desiredTemp = webServer.arg("desiredtemp");
    String tempPadding = webServer.arg("temppadding");
    String autoControl = webServer.arg("autocontrol");
    if (!autoControl.isEmpty()) { // Handle updating of the enable status of AutoControl...
      settings->setIsAutoControl(autoControl.equalsIgnoreCase("enabled"));
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

  webServer.send(200, "text/html", htmlPageTemplate(String(settings->getTitle()), String(settings->getHeading()), content));
}

/** 
 * #### ADMIN/SETTINGS PAGE ####
 * 
 * This function shows the admin page for the device. The admin page is also
 * known as the settings page. It is a password protected page that allows 
 * for the software's non-volatile settings to be configured.
 * 
*/
void MyWeb::endpointHandlerAdmin() {
  /* Ensure user authenticated */
  if (!webServer.authenticate("admin", settings->getAdminPwd().c_str())) { // User not authenticated...
    
    return webServer.requestAuthentication(DIGEST_AUTH, "AdminRealm", "Authentication failed!");
  }

  // Initial content is the login page unless authenticated
  String content = ADMIN_SETTINGS_PAGE;
  
  bool requiresReboot = false;
  bool autoToHome = false;
  
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
    webServer.send(200, "text/html", htmlPageTemplate(settings->getTitle(), "Device Settings", content));
    delay(1000);
    ESP.restart();
  } else { // No reboot required...
    if (autoToHome) {
      webServer.send(200, "text/html", htmlPageTemplate(settings->getTitle(), "Device Settings", content, "/", 5));
    } else {
      webServer.send(200, "text/html", htmlPageTemplate(settings->getTitle(), "Device Settings", content));
    }
  }
}

void MyWeb::notFoundHandler() {
  webServer.send(404, "text/html", htmlPageTemplate("404 Not Found", "OOPS! You broke it!!!", "Just kidding...<br>But seriously what you were looking for doesn't exist."));
}

void MyWeb::fileUploadHandler() {
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