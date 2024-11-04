#ifndef HtmlContent_h
    #define HtmlContent_h

    #include <WString.h>
    #include <pgmspace.h>

    /**
     * This is the HTML content of the Info Page.
     * This HTML has replaceable place-holders for dynamic informaton to be
     * added just prior to sending to client, as well as for adding in the
     * appropriate control section to the page.
    */
    const char PROGMEM INFO_PAGE_WO_CONTROLLS[] = {
        "<!DOCTYPE HTML> "
        "<html lang=\"en\"> "
            "<head> "
                "<title>${title}</title> "
                "<style> "
                    "body { background-color: #FFFFFF; color: #000000; } "
                    "h1 { text-align: center; background-color: #5878B0; color: #FFFFFF; border: 3px; border-radius: 15px; } "
                    "h2 { text-align: center; background-color: #58ADB0; color: #FFFFFF; border: 3px; } "
                    "#successful { text-align: center; color: #02CF39; } "
                    "#failed { text-align: center; color: #CF0202; } "
                    "#wrapper { background-color: #E6EFFF; padding: 20px; margin-left: auto; margin-right: auto; max-width: 700px; box-shadow: 3px 3px 3px #333; } "
                    "#info { font-size: 25px; font-weight: bold; line-height: 150%; } "
                    "button { background-color: #5878B0; color: white; font-size: 16px; padding: 10px 24px; border-radius: 12px; border: 2px solid black; transition-duration: 0.4s; } "
                    "button:hover { background-color: white; color: black; } "
                "</style> "
            "</head> "
            ""
            "<div id=\"wrapper\"> "
                "<h1>${heading}</h1> "
                "Firmware Version: ${version}"
                "<div id=\"info\">"
                    "<p>"
                        "${status_message}"
                        "<table>"
                            "<tr><td>Connected Sensor:</td><td>${sensor_name}</td></tr>"
                            "<tr><td>Temperature:</td><td>${temp}&deg;F</td></tr>"
                            "<tr><td>Control Type:</td><td>${control_type}</td></tr>"
                            "<tr><td>Mode:</td><td>${auto_manual}</td></tr>"
                            "<tr><td>Current Status:</td><td>${on_off_status}</td></tr>"
                        "</table>"
                        "<br><br>"
                        "<form action=\"/\" method=\"post\">"
                            "<button type=\"submit\" name=\"do\" value=\"show_controls\">Show Controls</button>"
                        "</form>"
                    "</p>"
                "</div> "
            "</div> "
        "</html>"
    };

    const char PROGMEM INFO_PAGE_W_CONTROLLS[] = {
        "<!DOCTYPE HTML> "
        "<html lang=\"en\"> "
            "<head> "
                "<title>${title}</title> "
                "<style> "
                    "body { background-color: #FFFFFF; color: #000000; } "
                    "h1 { text-align: center; background-color: #5878B0; color: #FFFFFF; border: 3px; border-radius: 15px; } "
                    "h2 { text-align: center; background-color: #58ADB0; color: #FFFFFF; border: 3px; } "
                    "#successful { text-align: center; color: #02CF39; } "
                    "#failed { text-align: center; color: #CF0202; } "
                    "#wrapper { background-color: #E6EFFF; padding: 20px; margin-left: auto; margin-right: auto; max-width: 700px; box-shadow: 3px 3px 3px #333; } "
                    "#info { font-size: 25px; font-weight: bold; line-height: 150%; } "
                    "button { background-color: #5878B0; color: white; font-size: 16px; padding: 10px 24px; border-radius: 12px; border: 2px solid black; transition-duration: 0.4s; } "
                    "button:hover { background-color: white; color: black; } "
                "</style> "
            "</head> "
            ""
            "<div id=\"wrapper\"> "
                "<h1>${heading}</h1> "
                "Firmware Version: ${version}"
                "<div id=\"info\">"
                    "<p>"
                        "${status_message}"
                        "<table>"
                            "<tr><td>Connected Sensor:</td><td>${sensor_name}</td></tr>"
                            "<tr><td>Temperature:</td><td>${temp}&deg;F</td></tr>"
                            "<tr><td>Control Type:</td><td>${control_type}</td></tr>"
                            "<tr><td>Mode:</td><td>${auto_manual}</td></tr>"
                            "<tr><td>Current Status:</td><td>${on_off_status}</td></tr>"
                        "</table>"
                        "<br><br>"
                        "<form action=\"/\" method=\"post\">"
                            "<button type=\"submit\" name=\"do\" value=\"hide_controls\">Hide Controls</button>"
                            "<br /><br />"
                            "<table>"
                                "<tr><td>Automatic Control</td><td></td></tr>"
                                "<tr><td>" // Enable
                                    "<input type=\"radio\" id=\"enabled\" name=\"autocontrol\" value=\"auto_enabled\" ${auto_enabled_checked}>"
                                    "<label for=\"enabled\">Enabled</label>"
                                "</td><td>" // Disable
                                    "<input type=\"radio\" id=\"disabled\" name=\"autocontrol\" value=\"auto_disabled\" ${auto_disabled_checked}>"
                                    "<label for=\"disabled\">Disabled</label>"
                                "</td></tr>" 
                            "</table>"
                            "<br />"
                            "<table ${auto_hide}>"
                                "<tr><td>Type:</td><td>" // Heat
                                    "<input type=\"radio\" id=\"heat\" name=\"controltype\" value=\"heat\" ${heat_checked}>"
                                    "<label for=\"heat\">Heat</label>"
                                "</td><td>" // Cool
                                    "<input type=\"radio\" id=\"cool\" name=\"controltype\" value=\"cool\" ${cool_checked}>"
                                    "<label for=\"cool\">Cool</label>"
                                "</td></tr>" // Sensor
                                "<tr><td>Temp Sensor:</td><td><select id=\"sensor\" name=\"sensor\" style=\"width: 10em;\">${sensor_options}</select></td></tr>"
                                "<tr><td></td><tr>" // Temp Settings
                                "<tr><td>Desired Temp:</td><td><input type=\"number\" id=\"desiredtemp\" name=\"desiredtemp\" min=\"-100.0\" max=\"100.0\" step=\".1\" value=\"${desired_temp}\" /> (&deg;F)</td></tr>"
                                "<tr><td>Temp Padding:</td><td><input type=\"number\" id=\"temppadding\" name=\"temppadding\" min=\"0.0\" max=\"100.0\" step=\".1\" value=\"${temp_padding}\" /> (&deg;F)</td></tr>"
                            "</table>"
                            "<br />"
                            "<table>"
                                "<tr>"
                                    "<td>"
                                        "<button type=\"submit\" name=\"do\" value=\"update_auto\">Update</button>"
                                    "</td><td></td><td>"
                                        "<button type=\"submit\" name=\"do\" value=\"settings\">Settings</button>"
                                    "</td>"
                                "</tr>"
                            "</table>"
                            "<br />"
                            "<table ${manual_hide}>"
                                "<tr><td>Manual Controls</td></tr>"
                                "<tr><td><button type=\"submit\" name=\"do\" value=\"btn_on\">On</button></td><td><button type=\"submit\" name=\"do\" value=\"btn_off\">Off</button></td></tr>"
                            "</table>"
                        "</form>"
                    "</p>"
                "</div> "
            "</div> "
        "</html>"
    };

    const char PROGMEM SENSOR_OPTION[] = {
        "<option value=\"${id}\" ${selection_flag}>${description}</option>"
    };

    const char PROGMEM STATUS_MESSAGE[] = {
        "<script>alert(\"${message}\");</script>"
    };

    /**
     * This is the HTML content of the Admin/Settings Page.
     * This HTML has replaceable place-holders for dynamic informaton to be
     * added just prior to sending to client.
    */
    const char PROGMEM ADMIN_SETTINGS_PAGE[] = {
        "<!DOCTYPE HTML> "
        "<html lang=\"en\"> "
            "<head> "
                "<title>${title}</title> "
                "<style> "
                    "body { background-color: #FFFFFF; color: #000000; } "
                    "h1 { text-align: center; background-color: #5878B0; color: #FFFFFF; border: 3px; border-radius: 15px; } "
                    "h2 { text-align: center; background-color: #58ADB0; color: #FFFFFF; border: 3px; } "
                    "#successful { text-align: center; color: #02CF39; } "
                    "#failed { text-align: center; color: #CF0202; } "
                    "#wrapper { background-color: #E6EFFF; padding: 20px; margin-left: auto; margin-right: auto; max-width: 700px; box-shadow: 3px 3px 3px #333; } "
                    "#info { font-size: 25px; font-weight: bold; line-height: 150%; } "
                    "button { background-color: #5878B0; color: white; font-size: 16px; padding: 10px 24px; border-radius: 12px; border: 2px solid black; transition-duration: 0.4s; } "
                    "button:hover { background-color: white; color: black; } "
                "</style> "
            "</head> "
            ""
            "<div id=\"wrapper\"> "
                "<h1>${heading}</h1> "
                "Firmware Version: ${version}"
                "<div id=\"info\">"
                    "<form method=\"post\" action=\"/\"> "
                        "<h2>Application</h2> "
                        "<table>"
                            "<tr><td>Title:</td><td><input maxlength=\"50\" type=\"text\" value=\"${title}\" name=\"title\" id=\"title\"></td></tr> "
                            "<tr><td>Heading:</td><td><input maxlength=\"50\" type=\"text\" value=\"${heading}\" name=\"heading\" id=\"heading\"></td></tr> "
                        "</table>"
                        "<h2>WiFi</h2> "
                        "<div>Note: Leave these settings at 'SET_ME' to keep device in AP Mode.</div>"
                        "<table>"
                            "<tr><td>SSID:</td><td><input maxlength=\"32\" type=\"text\" value=\"${ssid}\" name=\"ssid\" id=\"ssid\"></td></tr> "
                            "<tr><td>Password:</td><td><input maxlength=\"63\" type=\"text\" value=\"${pwd}\" name=\"pwd\" id=\"pwd\"></td></tr> "
                        "</table>"
                        "<h2>Admin</h2> "
                        "<table>"
                            "<tr><td>Admin User:</td><td><input maxlength=\"12\" type=\"text\" value=\"${adminuser}\" name=\"adminuser\" id=\"adminuser\"></td></tr> "
                            "<tr><td>Admin Password:</td><td><input maxlength=\"12\" type=\"text\" value=\"${adminpwd}\" name=\"adminpwd\" id=\"adminpwd\"></td></tr> "
                        "</table>"
                        "<br /> "
                        "<button type=\"submit\" name=\"do\" value=\"admin_save\">Save</button><button type=\"submit\" name=\"do\" value=\"admin_exit\">Exit</button>"
                    "</form>"
                "</div> "
            "</div> "
        "</html>"
    };
#endif