#ifndef MyWiFi_h
  #define MyWiFi_h

  #include <ESP8266WiFi.h>


  /*
    CLASS: MyWiFi

    This class was developed as a way to separate out the code which handles the 
    setup, management and configuration of the WiFi for the device.

    Written by: Scott Griffis
    Date: 10-10-2023
  */
  class MyWiFi {
    private:
      String apSsid;

      
    public: 
      /*
        CLASS CONSTRUCTOR: Used to externally instantiate the class while also 
        providing what SSID should be used for the device when in AP mode.

        @param apSsid - The SSID for the AP mode as String.
      */
      MyWiFi(String apSsid);

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
      bool connectToNetwork(const char *ssid, const char *pwd);

      /*
        This function is used to get the IP Address of the device regardless if it is in 
        AP mode or connected to an external WiFi Network.

        @return Returns the IP Address of this device in dot notation as String.
      */
      String getIpAddress();

      /*
        Indicates if the WiFi is in a connected status.

        @return Returns true if the device is in a conencted wifi status as bool.
      */
      bool isConnected();

      /*
        Used to place the device's WiFi into AP mode. This 
        allows for clients to connect to the WiFi that is produced
        by this device. This particularly useful when the device isn't
        yet configured to connect to an external wifi network yet.

        @return Returns true if AP mode starts properly otherwise a false as bool.
      */
      bool startAPMode();
  };

#endif