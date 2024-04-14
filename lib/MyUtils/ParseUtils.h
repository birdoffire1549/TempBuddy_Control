/*
  ParseUtils - A class of utility functions to aid in the parsing and deriving
  of information from one form to another.

  Written by: Scott Griffis
  Date: 10-01-2023
  Version: 2.0.2
*/

#ifndef ParseUtils_h
#define ParseUtils_h

#include <WString.h>
#include <string>
#include <math.h>

class ParseUtils {
    private:
        ParseUtils();

    public:
        static String arrangeDigitsUsingPattern(String inputString, String inputPattern, String desiredPattern);
        static std::string arrangeDigitsUsingPattern(std::string inputString, std::string inputPattern, std::string desiredPattern);

        static int countConsecutiveRepeatingChars(String string, int beginIndex);
        static int countConsecutiveRepeatingChars(std::string str, int beginIndex);

        static String decodeUrlString(String string);
        static std::string decodeUrlString(std::string str);
        
        static unsigned int hexStringToInt(String hex);
        static unsigned int hexStringToInt(std::string hex);

        static int occurrences(String string, char toCnt);
        static int occurrences(std::string str, char toCnt);
        static int occurrences(std::string str, std::string toCnt);
        static int occurrences(String str, String toCnt);

        static String parseByKeyword(String string, String keyword, String terminator);
        static std::string parseByKeyword(std::string str, std::string keyword, std::string terminator);

        static std::string replace(std::string str, std::string find, std::string replaceWith);

        static void split(String string, char separator, String *storage, int sizeOfStorage);
        static void split(std::string str, char separator, std::string *storage, unsigned int sizeOfStorage);
        static std::string substring(std::string str, int beginIndex, int endIndex);
        static std::string substring(std::string str, int beginIndex);

        static int toInt(std::string str);
        static float toFloat(std::string str);
        static double toDouble(std::string str);
        static std::string trim(std::string str);
        static String trunc(String string, unsigned int length);
        
        static bool validDotNotationIp(String string);
};
#endif