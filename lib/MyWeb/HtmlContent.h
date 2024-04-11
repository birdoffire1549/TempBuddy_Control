#ifndef HtmlContent_h
    #define HtmlContent_h

    #include <WString.h>

    /**
     * This is the HTML content that is common to ALL Pages which can be
     * displayed by this device's web service. The page's style is primarily 
     * controlled here and the HTTP Headers are inserted by this template. 
     * The content of this template is placed into the template by replacing the
     * ${content} place-holder.
    */
      const String HTML_PAGE_TEMPLATE = ""
        "<!DOCTYPE HTML> "
        "<html lang=\"en\"> "
            "<head> "
                "<title>${title}</title> "
                "${metainsert} "
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
                "<div id=\"info\">${content}</div> "
            "</div> "
        "</html>"
    ;

    /**
     * This is the HTML content of the Info Page.
     * This HTML has replaceable place-holders for dynamic informaton to be
     * added just prior to sending to client, as well as for adding in the
     * appropriate control section to the page.
    */
    const String INFO_PAGE = ""
        "<p>"
            "<table>"
                "<tr><td>TempBuddy IP:</td><td>${tempbuddyip}</td></tr>"
                "<tr><td>Last known temp:</td><td>${lastknowntemp}&deg;F</td></tr>"
                "<tr><td>Control Type:</td><td>${controltype}</td></tr>"
                "<tr><td>Auto Ctrl Enabled:</td><td>${autocontrolenabled}</td></tr>"
                "<tr><td>Controlled Device is:</td><td>${deviceonstatus}</td></tr>"
            "</table>"
        "</p>"
    ;

    /**
     * This is the HTML content of the Admin/Settings Page.
     * This HTML has replaceable place-holders for dynamic informaton to be
     * added just prior to sending to client.
    */
    const String ADMIN_SETTINGS_PAGE = ""  
        "<form name=\"settings\" method=\"post\" id=\"settings\" action=\"admin\"> "
            "<input type=\"hidden\" id=\"source\" name=\"source\" value=\"settings\">"
            "<h2>WiFi</h2> "
            "<table>"
                "<tr><td>SSID:</td><td><input maxlength=\"32\" type=\"text\" value=\"${ssid}\" name=\"ssid\" id=\"ssid\"></td></tr> "
                "<tr><td>Password:</td><td><input maxlength=\"63\" type=\"text\" value=\"${pwd}\" name=\"pwd\" id=\"pwd\"></td></tr> "
            "</table>"
            "<h2>Application</h2> "
            "<table>"
                "<tr><td>Title:</td><td><input maxlength=\"50\" type=\"text\" value=\"${title}\" name=\"title\" id=\"title\"></td></tr> "
                "<tr><td>Heading:</td><td><input maxlength=\"50\" type=\"text\" value=\"${heading}\" name=\"heading\" id=\"heading\"></td></tr> "
            "</table>"
            "<h2>Admin</h2> "
            "<table>"
                "<tr><td>TempBuddy IP:</td><td><input maxlength=\"14\" type=\"text\" value=\"${budyip}\" name=\"budyip\" id=\"budyip\"></td></tr> "
                "<tr><td>Auto Control:</td></tr> "
                "<tr>"
                    "<td>"
                    "<input type=\"radio\" id=\"enabled\" name=\"autocontrol\" value=\"enabled\" ${autocontrolenabledchecked}>"
                    "<label for=\"enabled\">Enabled</label>"
                    "</td><td>"
                    "<input type=\"radio\" id=\"disabled\" name=\"autocontrol\" value=\"disabled\" ${autocontroldisabledchecked}>"
                    "<label for=\"disabled\">Disabled</label>"
                    "</td>"
                "</tr>"
                "<tr><td>Controlling:</td></tr>"
                "<tr>"
                    "<td>"
                    "<input type=\"radio\" id=\"heat\" name=\"controltype\" value=\"heat\" ${controllingheatchecked}>"
                    "<label for=\"heat\">Heat</label>"
                    "</td><td>"
                    "<input type=\"radio\" id=\"cool\" name=\"controltype\" value=\"cool\" ${controllingcoolchecked}>"
                    "<label for=\"cool\">Cool</label>"
                    "</td>"
                "</tr>"
                "<tr><td>Desired Temp:</td><td><input type=\"number\" id=\"desiredtemp\" name=\"desiredtemp\" min=\"-100.0\" max=\"100.0\" step=\".1\" value=\"${desiredtemp}\"> (&deg;F)</td></tr> "
                "<tr><td>Temp Padding:</td><td><input type=\"number\" id=\"temppadding\" name=\"temppadding\" min=\"0.0\" max=\"100.0\" step=\".1\" value=\"${temppadding}\"> (&deg;F)</td></tr> "
                "<tr><td>Client Timeout:</td><td><input type=\"number\" value=${timeout} name=\"timeout\" id=\"timeout\"> (milliseconds)</td></tr> "
                "<tr><td>Admin Password:</td><td><input maxlength=\"12\" type=\"text\" value=\"${adminpwd}\" name=\"adminpwd\" id=\"adminpwd\"></td></tr> "
            "</table>"
            "<br> "
            "<button type=\"submit\">Submit</button> <a href='/'><h4>Home</h4></a>"
        "</form>"
    ;

    /*
     * This is the Manual Controls section of the Info Page.
     * These controls are only shown on the Info Page when 
     * Manual Control is enabled.
    */
    const String MANUAL_CONTROLS_SECTION = ""
        "<br><hr><br>"
        "<form name=\"manualControls\" method=\"post\" id=\"manualControls\" action=\"/\"> "
            "<input type=\"hidden\" id=\"source\" name=\"source\" value=\"manualcontrols\">"
            "<table>"
                "<tr><td>Auto Control:</td></tr> "
                "<tr>"
                    "<td>"
                        "<input type=\"radio\" id=\"enabled\" name=\"autocontrol\" value=\"enabled\" >"
                        "<label for=\"enabled\">Enabled</label>"
                    "</td><td>"
                        "<input type=\"radio\" id=\"disabled\" name=\"autocontrol\" value=\"disabled\" checked >"
                        "<label for=\"disabled\">Disabled</label>"
                    "</td>"
                "</tr>"
            "</table>"
            "<br>"
            "<button type=\"submit\">Update</button>"
            "<br>"
            "<br>"
            "Manual Controls: " 
            "<button type=\"submit\" formaction=\"/?control=on\">On</button>    <button type=\"submit\" formaction=\"/?control=off\">Off</button>"
        "</form>"
    ;

    /**
     * This is the Auto Controls section of the Info Page.
     * These controls are only shown on the Info Page when 
     * Auto Control is enabled.
    */
    const String AUTO_CONTROLS_SECTION = ""
        "<br><hr><br>"
        "<h2>Auto Controls</h2>" 
        "<form name=\"autoControls\" method=\"post\" id=\"autoControls\" action=\"/\"> "
            "<input type=\"hidden\" id=\"source\" name=\"source\" value=\"autocontrols\">"
            "<table>"
                "<tr><td>Auto Control:</td></tr> "
                "<tr>"
                    "<td>"
                        "<input type=\"radio\" id=\"enabled\" name=\"autocontrol\" value=\"enabled\" checked >"
                        "<label for=\"enabled\">Enabled</label>"
                    "</td><td>"
                        "<input type=\"radio\" id=\"disabled\" name=\"autocontrol\" value=\"disabled\" >"
                        "<label for=\"disabled\">Disabled</label>"
                    "</td>"
                "</tr>"
            "</table>"
            "<br>"
            "<table>"
                "<tr><td><label for=\"desiredtemp\">Desired Temp:</label></td>"
                "<td><input type=\"number\" id=\"desiredtemp\" name=\"desiredtemp\" min=\"-100.0\" max=\"100.0\" step=\".1\" value=\"${desiredtemp}\"> (&deg;F)</td></tr>"
                "<tr><td><label for\"temppadding\">Temp Padding:</label></td>"
                "<td><input type=\"number\" id=\"temppadding\" name=\"temppadding\" min=\"0.0\" max=\"100.0\" step=\".1\" value=\"${temppadding}\"> (&deg;F)</td></tr>"
            "</table>"
            "<br>"
            "<button type=\"submit\">Update</button>"
        "</form>"
    ;

    /**
     * This is the content HTML for the Update Successful 
     * message should saving the modified settings to 
     * memory succeed.
    */
    const String UPDATE_SUCCESSFUL_MSG = ""
        "<div id=\"successful\">* Update was Successful!</div>"
    ;

    /**
     * This is the content HTML for the Update Failed 
     * message should saving the modified settings to 
     * memory fail.
    */
    const String UPDATE_FAILED_MSG = ""
        "<div id=\"failed\">* Update Failed!!!</div>"
    ;
#endif