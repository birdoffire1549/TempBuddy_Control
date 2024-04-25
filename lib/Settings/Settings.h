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
    #include <MD5Builder.h>

    class Settings {
        private:
            // *****************************************************************************
            // Structure used for storing of settings related data and persisted into flash
            // *****************************************************************************
            struct NonVolatileSettings {
                char           ssid             [33]  ; // 32 chars is max size + 1 null
                char           pwd              [64]  ; // 63 chars is max size + 1 null
                char           adminUser        [13]  ;
                char           adminPwd         [13]  ;
                char           title            [51]  ;
                char           heading          [51]  ;
                char           tempSensorIp     [16]  ;
                float          desiredTemp            ;
                float          tempPadding            ;
                bool           isHeat                 ;
                bool           isAutoControl          ;
                char           sentinel         [33]  ; // Holds a 32 MD5 hash + 1
            } nvSettings;

            struct NonVolatileSettings factorySettings = {
                "SET_ME", // <--------------- ssid
                "SET_ME", // <--------------- pwd
                "admin", // <---------------- adminUser
                "admin", // <---------------- adminPwd
                "TempBuddy Control", // <---- title
                "Device Info", // <---------- heading
                "0.0.0.0", // <-------------- tempBuddyIp
                72.0, // <------------------- desiredTemp
                0.5, // <-------------------- tempPadding
                true, // <------------------- isHeat
                false, // <------------------ isAutoControl
                "NA" // <-------------------- sentinel
            };

            // ******************************************************************
            // Structure used for storing of settings related data NOT persisted
            // ******************************************************************
            struct VolatileSettings {
                bool           isControlOn            ;
                float          lastKnownTemp          ;
            } vSettings;

            // *****************************************************************************
            // Structure used for storing of settings related data that is set prior to 
            // compile time and constant in nature.
            // *****************************************************************************
            struct ConstSettings {
                String hostname;
                String apSsid;
                String apPwd;
                String apNetIp;
                String apSubnet;
                String apGateway;
            } cSettings = {
                "TempBuddy", // <---------- hostname (*later ID is added)
                "TempBuddy_Ctrl_", // <---- apSsid (*later ID is added)
                "P@ssw0rd123", // <-------- apPwd
                "192.168.1.1", // <-------- apNetIp
                "255.255.255.0", // <------ apSubnet
                "0.0.0.0", // <------------ apGateway
            };
            
            void defaultSettings();
            String hashNvSettings(NonVolatileSettings nvSet);


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
            
            void           setSsid           (const char *ssid)       ;
            String         getSsid           ()                       ;
            void           setPwd            (const char *pwd)        ;
            String         getPwd            ()                       ;
            void           setAdminUser      (const char *user)       ;
            String         getAdminUser      ()                       ;
            void           setAdminPwd       (const char *pwd)        ;
            String         getAdminPwd       ()                       ;
        
            void           setTitle          (const char *title)      ;
            String         getTitle          ()                       ;
            void           setHeading        (const char *heading)    ;
            String         getHeading        ()                       ;
            void           setTempSensorIp    (const char *ip)         ;
            String         getTempSensorIp    ()                       ;
            void           setDesiredTemp    (float temp)             ;
            float          getDesiredTemp    ()                       ;
            void           setTempPadding    (float padding)          ;
            float          getTempPadding    ()                       ;
            void           setIsHeat         (bool isHeat)            ;
            bool           getIsHeat         ()                       ;
            void           setIsControlOn    (bool isOn)              ;
            bool           getIsControlOn    ()                       ;
            void           setIsAutoControl  (bool autoOn)            ; 
            bool           getIsAutoControl  ()                       ;
            void           setLastKnownTemp  (float temp)             ;
            float          getLastKnownTemp  ()                       ;

            String         getHostname       (String deviceId)        ;
            String         getApSsid         (String deviceId)        ;
            String         getApPwd          ()                       ;
            String         getApNetIp        ()                       ;
            String         getApSubnet       ()                       ;    
            String         getApGateway      ()                       ;
    };
    
#endif