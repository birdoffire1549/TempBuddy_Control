    /*
    Settings - A class to contain, maintain, store and retreive settings needed
    by the application. This Class object is intented to be the sole manager of 
    data used throughout the applicaiton. It handles storing both volitile and 
    non-volatile data, where by definition the non-volitile data is persisted
    in flash memory and lives beyond the running life of the software and the 
    volatile data is lost and defaulted each time the software runs.

    Written by: Scott Griffis
    Date: 10-01-2023
    */

#ifndef Settings_h
    #define Settings_h

    #include <string.h> // NEEDED by ESP_EEPROM and MUST appear before WString
    #include <ESP_EEPROM.h>
    #include <WString.h>
    #include <core_esp8266_features.h>
    #include <HardwareSerial.h>
    #include <Hash.h>

    class Settings {
        const unsigned long SENTINEL_VALUE = 0xFD2F; // Helps ensure memory location contains expected contents.

        private:
            // *****************************************************************************
            // Structure used for storing of settings related data and persisted into flash
            // *****************************************************************************
            struct NonVolatileSettings {
                char           hostname         [64]  ; // 63 chars is max size + 1 null
                char           ssid             [33]  ; // 32 chars is max size + 1 null
                char           pwd              [64]  ; // 63 chars is max size + 1 null
                char           adminUser        [13]  ;
                char           adminPwd         [13]  ;
                char           apSsid           [33]  ;
                char           apPwd            [64]  ;
                char           apNetIp          [16]  ;
                char           apSubnet         [16]  ;
                char           apGateway        [16]  ;
                char           title            [51]  ;
                char           heading          [51]  ;
                char           tempBuddyIp      [16]  ;
                float          desiredTemp            ;
                float          tempPadding            ;
                bool           isHeat                 ;
                bool           isAutoControl          ;
                unsigned long  timeout                ;
                bool           isFactory              ;
                unsigned long  sentinel               ;
            } nvSettings;

            // ******************************************************************
            // Structure used for storing of settings related data NOT persisted
            // ******************************************************************
            struct VolatileSettings {
                bool           isControlOn            ;
                float          lastKnownTemp          ;
            } vSettings;
            
            void defaultSettings();
            

        public:
            Settings();

            bool factoryDefault();
            bool loadSettings();
            bool saveSettings();
            bool isFactoryDefault();
            bool isNetworkSet();

            /*
            =========================================================
                                Getters and Setters 
            =========================================================
            */
            void           settingsChanged   ()                       ;

            void           setHostname       (const char *hostname)   ;
            String         getHostname       ()                       ;
            void           setSsid           (const char *ssid)       ;
            String         getSsid           ()                       ;
            void           setPwd            (const char *pwd)        ;
            String         getPwd            ()                       ;
            void           setAdminUser      (const char *user)       ;
            String         getAdminUser      ()                       ;
            void           setAdminPwd       (const char *pwd)        ;
            String         getAdminPwd       ()                       ;
            
            void           setApSsid         (const char *ssid)       ;
            String         getApSsid         ()                       ;
            void           setApPwd          (const char *pwd)        ;
            String         getApPwd          ()                       ;
            void           setApNetIp        (const char *ip)         ;
            String         getApNetIp        ()                       ;
            void           setApSubnet       (const char *ip)         ;
            String         getApSubnet       ()                       ;    
            void           setApGateway      (const char *ip)         ;
            String         getApGateway      ()                       ;
        
            void           setTitle          (const char *title)      ;
            String         getTitle          ()                       ;
            void           setHeading        (const char *heading)    ;
            String         getHeading        ()                       ;
            void           setTempBuddyIp    (const char *ip)         ;
            String         getTempBuddyIp    ()                       ;
            void           setDesiredTemp    (float temp)             ;
            float          getDesiredTemp    ()                       ;
            void           setTempPadding    (float padding)          ;
            float          getTempPadding    ()                       ;
            void           setIsHeat         (bool isHeat)            ;
            bool           getIsHeat         ()                       ;
            void           setTimeout        (unsigned long timeout)  ;
            unsigned long  getTimeout        ()                       ;
            void           setIsControlOn    (bool isOn)              ;
            bool           getIsControlOn    ()                       ;
            void           setIsAutoControl  (bool autoOn)            ; 
            bool           getIsAutoControl  ()                       ;
            void           setLastKnownTemp  (float temp)             ;
            float          getLastKnownTemp  ()                       ;
    };
    
#endif