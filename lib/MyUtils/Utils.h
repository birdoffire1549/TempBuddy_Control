#ifndef Utils_h
  #define Utils_h

  #include <WString.h>
  #include <Arduino.h>

  /*
    CLASS: Utils

    This is a Utility function class intented to contain useful utility functions for use in 
    the containing application.

    Written by: Scott Griffis
    Date: 10-01-2023
  */
  class Utils {
    private:
      Utils();

      static bool displayDigit(int ledPin, int digit);
      static void displayDone(int ledPin);
      static void displayNextDigitIndicator(int ledPin);
      static void displayNextOctetIndicator(int ledPin);
      static void displayOctet(int ledPin, int octet);

    public:
      static String genDeviceIdFromMacAddr(String macAddress);
      static String hashString(String str);
      static void signalIpAddress(int ledPin, String ipAddress, bool quick);
      static float convertCelciusToFahrenheit(float celcius);
  };

#endif