#include <sys/syslimits.h>
#include "WiFiServer.h"
#ifndef MyWeb_h
  #define MyWeb_h

  #include <ESP8266WiFi.h>
  #include <ESP8266WebServerSecure.h>
  #include <WString.h>
  #include "Settings.h"
  #include "ParseUtils.h"
  #include "HtmlContent.h"

  #include "ExampleSecrets.h"
  #include "Secrets.h"

  
  /*
    CLASS: MyWeb

    This class acts as a web-server for the implementing software and handles serving
    up hardcoded web pages. All changes to web content served by this device should
    be made in this class.

    Written by: Scott Griffis
    Date: 10-10-2023
  */
  class MyWeb {
    private:
      BearSSL::ESP8266WebServerSecure webServer = ESP8266WebServerSecure(443);
      BearSSL::ServerSessions serverCache = ServerSessions(5);
      Settings *settings;

      String htmlPageTemplate(String title, String heading, String content, String redirectUrl = "", int delayTime = 0);

      void endpointHandlerGETRoot(void);
      void endpointHandlerAdmin(void);
      void notFoundHandler(void);
      void fileUploadHandler(void);

    public:
      MyWeb(Settings *settings);

      void begin();
      void stop();
      void run();
  };

#endif