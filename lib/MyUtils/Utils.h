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

      /*
        PRIVATE: This displays the Next Digit Indicator which is simply a way to visually
        break up digit flashes.

        @param ledPin - The pin number of the LED as int.
      */
      static void displayNextDigitIndicator(int ledPin);

      /*
        PRIVATE: This displays or signals the Display Done flashes.
        This is a visual way for the Device to say it is done 
        signaling the requested IP Address or last Octet.

        @param ledPin - The pin number of the LED as int.
      */
      static void displayDone(int ledPin);

      /*
        PRIVATE: Displays or signals the separator between octets which
        is simply 2 Next Digit Indicators.

        @param ledPin - The pin number of the LED as int.
      */
      static void displayNextOctetIndicator(int ledPin);

      /*
        PRIVATE: This function's job is to generate flashes for 
        a single digit.

        @param ledPin - The pin number of the LED as int.
        @param digit - The digit being a 0 to 9 value as int.
        @return Returns true if digit was a non-zero value otherwise false as bool.
      */
      static bool displayDigit(int ledPin, int digit);

      /*
        PRIVATE: This function is in charge of displaying or signaling a single
        octet of an IP address.

        @param ledPin - The pin number the LED is attached to as int.
        @param octet - The value of the octet to signal as int. 
      */
      static void displayOctet(int ledPin, int octet);

    public:
      /*
        Function handles flashing of the LED for signaling the given IP Address
        entirely or simply its last octet as determined by the passed boolean 
        refered to as quick. If quick is TRUE then last Octet is signaled, if 
        FALSE then entire IP is signaled.

        @param ledPin - The pin number the LED is attached to as int.
        @param ipAddress - The IP Address in dot notation as String.
        @param quick - Indicates if signaling is for last octet or whole IP as bool.
      */
      static void signalIpAddress(int ledPin, String ipAddress, bool quick);
  };

#endif