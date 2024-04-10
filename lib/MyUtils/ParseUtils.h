/*
  ParseUtils - A class of utility functions to aid in the parsing and deriving 
  of information from one form to another.

  Written by: Scott Griffis
  Date: 10-01-2023
  Version: 2.0.0
*/

#ifndef ParseUtils_h
  #define ParseUtils_h
  
  #include <WString.h>
  #include <string>
  #include <math.h>

  /*
    CLASS: ParseUtils

    A class of static functions which are intended for use without the 
    instantiation of the class.
  */
  class ParseUtils {
    private:
      /*
        CLASS CONSTRUCTOR: This constructor is private to prevent
        the external instantiation of this class into an object.
      */
      ParseUtils();




    public:

      /*
        This function allows for an inputPattern to be applied to a given inputString, then using the given
        desiredPattern the characters masked by the inputPattern are rearranged to match the given desiredPattern.
        
        Example: 
        String output = ParseUtils::arrangeDigitsUsingPattern("23022023", "ddmmyyyy", "mmddyyyy");
        output will be "02232023"

        Mask patterns can be anything and hold no meaning to this function.
        Note: I wrote this a while ago for a particular use case and as of right now am not fully aware of its full
        abilities or faults. I hope to one day revisit it and nail-down its exact streangths and/or weaknesses.

        @param inputString - The string to rearrange as String.
        @param inputPatter - The masking pattern for the inputString as String.
        @param desiredPattern - The desired arrangement of the inputPattern and the characters it masks as String.

        @return Returns the rearranged string based on the shuffling of the inputString as masked by the inputPatter
        rearranged to match the desiredPattern, as String.
      */
      static String  arrangeDigitsUsingPattern  (String inputString, String inputPattern, String desiredPattern);

      /*
        This function allows for an inputPattern to be applied to a given inputString, then using the given
        desiredPattern the characters masked by the inputPattern are rearranged to match the given desiredPattern.
        
        Example: 
        std::string output = ParseUtils::arrangeDigitsUsingPattern("23022023", "ddmmyyyy", "mmddyyyy");
        output will be "02232023"

        Mask patterns can be anything and hold no meaning to this function.
        Note: I wrote this a while ago for a particular use case and as of right now am not fully aware of its full
        abilities or faults. I hope to one day revisit it and nail-down its exact streangths and/or weaknesses.

        @param inputString - The string to rearrange as std::string.
        @param inputPatter - The masking pattern for the inputString as std::string.
        @param desiredPattern - The desired arrangement of the inputPattern and the characters it masks as std::string.

        @return Returns the rearranged string based on the shuffling of the inputString as masked by the inputPatter
        rearranged to match the desiredPattern, as std::string.
      */
      static std::string  arrangeDigitsUsingPattern  (std::string inputString, std::string inputPattern, std::string desiredPattern);

      /*
        Counts the number of repeating characters in the given string from the specified
        beginIndex and then returns that count.

        @param string - The string to count from as String
        @param beginIndex - The index of the first character to begin the count from as int.

        @return Returns the count as int.
      */
      static int countConsecutiveRepeatingChars(String string, int beginIndex);

      /*
        Counts the number of repeating characters in the given string from the specified
        beginIndex and then returns that count.

        @param string - The string to count from as std::string
        @param beginIndex - The index of the first character to begin the count from as int.

        @return Returns the count as int.
      */
      static int countConsecutiveRepeatingChars(std::string str, int beginIndex);

      /*
        Decodes a URL encoded string.

        @param string - The string to decode as String. 
      */
      static String decodeUrlString(String string);

      /*
        Decodes a URL encoded string.

        @param str - The string to decode as std::string. 
      */
      static std::string decodeUrlString(std::string str);

      /*
        This is used to convert a string representation of a hex value into the
        equivelent unsigned int value.

        For Example:
        The String "2B" would be converted to an unsigned int value of 43.
        
        As such this function makes it rather trivial to do a compairison of 
        the String "2B" to the actual hex value of 0x2B.

        @param hex - The hex string to perform conversion on as String.

        @return Returns the hex value as an unsigned int.
      */
      static unsigned int hexStringToInt(String hex);

      /*
        This is used to convert a string representation of a hex value into the
        equivelent unsigned int value.

        For Example:
        The String "2B" would be converted to an unsigned int value of 43.
        
        As such this function makes it rather trivial to do a compairison of 
        the String "2B" to the actual hex value of 0x2B.

        @param hex - The hex string to perform conversion on, as std::string.

        @return Returns the hex value as an unsigned int.
      */
      static unsigned int hexStringToInt(std::string hex);
      
      /*
        Counts the number of occurrences of a specific character within a 
        given String.

        @param string - The String from which the occurances of the given character will be counted.
        @param toCnt - The character of which to count the occurrances of.

        @return Returns the number of occurrences counted as int.
      */
      static int occurrences(String string, char toCnt);

      /*
        Counts the number of occurrences of a specific character within a 
        given string.

        @param str - The string from which the occurances of the given character will be counted, as std::string.
        @param toCnt - The character of which to count the occurrances of, as char.

        @return Returns the number of occurrences counted as int.
      */
      static int occurrences(std::string str, char toCnt);

      /*
        Counts the number of occurrences of a specific string within a 
        given string.

        @param str - The string from which the occurances of the given string will be counted as std::string.
        @param toCnt - The string of which to count the occurrances of as std::string.

        @return Returns the number of occurrences counted as int.
      */
      static int occurrences(std::string str, std::string toCnt);

      /*
        Counts the number of occurrences of a specific String within a 
        given String.

        @param str - The String from which the occurances of the given String will be counted as String.
        @param toCnt - The String of which to count the occurrances of as String.

        @return Returns the number of occurrences counted as int.
      */
      static int occurrences(String str, String toCnt);

      /*
        Allows for information to be parsed out of a String between a Keyword and a Terminating
        String. If the Terminator doesn't exist then this function will parse to the end of the line.
        If character marking the end of the line then the terminator will be the end of the String.
        This is particularly useful when parsing data from key/value pairs.
        
        Example:
        If string was equal to "This is a sentence and it may contain a lot of data", and one was to
        set the keyword to "a " and the terminator to " and", then you would get as a result the word
        "sentence", as a String. If the prior is true but the terminator was "$" the result would be, 
        "sentence and it may contain a lot of data", since the "$" terminator would not be found.

        @param string - The string from which to parse data as String.
        @param keyword - The keyword to start parsing data just after as String.
        @param terminator - The terminator to parse up until as String.

        @return Returns the parsed data as String
      */
      static String parseByKeyword(String string, String keyword, String terminator);

      /*
        Allows for information to be parsed out of a string between a Keyword and a Terminating
        string. If the Terminator doesn't exist then this function will parse to the end of the line.
        If character marking the end of the line then the terminator will be the end of the string.
        This is particularly useful when parsing data from key/value pairs.
        
        Example:
        If str was equal to "This is a sentence and it may contain a lot of data", and one was to
        set the keyword to "a " and the terminator to " and", then you would get as a result the word
        "sentence", as a string. If the prior is true but the terminator was "$" the result would be, 
        "sentence and it may contain a lot of data", since the "$" terminator would not be found.

        @param str - The string from which to parse data as std::string.
        @param keyword - The keyword to start parsing data just after as std::string.
        @param terminator - The terminator to parse up until as std::string.

        @return Returns the parsed data as std::string
      */
      static std::string parseByKeyword(std::string str, std::string keyword, std::string terminator);

      /*
        Used to replace a specified string of characters from within a given string, with another
        string of characters. This supports the replaceWith string being larger than the string 
        being replaced as specified with the 'find' string.

        @param str - The string containing the string to be replaced as std::string.
        @param find - The string to find for replacement as std::string.
        @param replaceWith - The string to replace the found string with as std::string.

        @return Returns the resulting string as std::string. 
      */
      static std::string replace(std::string str, std::string find, std::string replaceWith);

      /*
        Splits the given String up into multiple segments based on the given seporator. The
        split up data segments are stored into a given String based storage array of a specific
        size. If the storage array is too small for all of the data segments then it will be filled
        with what it has space for, and the rest will be discarded.

        @param string - The string to perform the operation on as String.
        @param separator - The character to use as a separator for the splitting process as char.
        @param storage - An array of strings for storage of the results of the splitting process as String pointer.
        @param sizeOfStorage - The size of the storage array provided as int.
      */
      static void split(String string, char separator, String* storage, int sizeOfStorage);

      /*
        Splits the given string up into multiple segments based on the given seporator. The
        split up data segments are stored into a given string based storage array of a specific
        size. If the storage array is too small for all of the data segments then it will be filled
        with what it has space for, and the rest will be discarded.

        @param str - The string to perform the operation on as std::string.
        @param separator - The character to use as a separator for the splitting process as char.
        @param storage - An array of strings for storage of the results of the splitting process as std::string pointer.
        @param sizeOfStorage - The size of the storage array provided as int.
      */
      static void split(std::string str, char separator, std::string* storage, int sizeOfStorage);

      /*
        Performs a substring type function on the given string where what is returned
        is determined by parsing the data out inclusively from the beginIndex and 
        exclusively up to the endIndex specified. Remember the first character in the
        string is considered to be at index zero.

        @param str - The string to parse from as std::string.
        @param beginIndex - The index to inclusively begin parsing from as int.
        @param endIndex - The index to exclusively parse up to as int.

        @return Returns the parsed string as std::string.
      */
      static std::string substring(std::string str, int beginIndex, int endIndex);

      /*
        Performs a substring type function on the given string where what is returned
        is determined by parsing the data out inclusively from the beginIndex and 
        up to the end of the given string. Remember the first character in the
        string is considered to be at index zero.

        @param str - The string to parse from as std::string.
        @param beginIndex - The index to inclusively begin parsing from as int.

        @return Returns the parsed string as std::string.
      */
      static std::string substring(std::string str, int beginIndex);
      
      /*
        Used to parse an int out from a string that contains a valid integer 
        number. If the contents of the given string are not a valid integer 
        then the integer value of zero will be returned.

        @param str - The string to be parsed as an integer as std::string.
        
        @return Returns the parsed value as int.
      */
      static int toInt(std::string str);

      /*
        Used to parse a float out from a string that contains a valid floating point 
        number. If the contents of the given string are not a valid floating point number
        then the value of 0.0 will be returned.

        @param str - The string to be parsed as a floating point number, as std::string.

        @return Returns the parsed value as float.
      */
      static float toFloat(std::string str);

      /*
        Used to parse a double out from a string that contains a valid double 
        number. If the contents of the given string are not a valid double number
        then the value of zero will be returned.

        @param str - The string to be parsed as a double number, as std::string.

        @return Returns the parsed value as double.
      */
      static double toDouble(std::string str);

      /*
        Trims whitespace from both ends of the given string, returning
        the given string without any leading or trailing whitespace.

        @param str - The string to trim as std::string.

        @return Returns the resulting string as std::string.
      */
      static std::string trim(std::string str);      

      /*
        This is used to tell if the given string is a valid Dot Notation
        IP Address. If it is valid then true is returned otherwise false
        as bool.

        @param string - The string to validate as String.

        @return Returns the result as bool.
      */
      static bool validDotNotationIp(String string);
  };
#endif