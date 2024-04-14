#ifndef MyWiFi_h
  #define MyWiFi_h

  #include <ESP8266WiFi.h>
  #include "IpUtils.h"
  #include "Settings.h"

  /*
    CLASS: MyWiFi

    This class was developed as a way to separate out the code which handles the
    setup, management and configuration of the WiFi for the device.

    Written by: Scott Griffis
    Date: 10-10-2023
  */
  class MyWiFi
  {
    private:
      String hostname;
      String apIp;
      String apSubnet;
      String apGateway;
      String apSsid;
      String apPwd;

      bool startAPMode();

    public:
      MyWiFi();

      bool connectToNetwork(String hostname, String ssid, String pwd);
      String getIpAddress();
      bool isConnected();
      bool startAPMode(String hostname, String ip, String subnet, String gateway, String ssid, String pwd);
      bool isApMode();
      bool isStaMode();
  };

#endif