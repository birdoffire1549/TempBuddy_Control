# TempBuddy Control
## Overview
This software was written to be utilized by an ESP8266 Device in a specialized hardware configuration known as a TempBuddy Control Unit. It is also intended that this unit be operated in conjunction with a TempBuddy Sensor device, however the Sensor is not required as this unit still provides functionality and usefullness without it.

The TempBuddy Sensor is device has the ability to sense and report Temperature and Humidity data via a web interface. 

This unit can be pointed at the IP of a TempBuddy Sensor device and read its temp and then react to the temperature by controlling an outlet which can have a Heating or Cooling device attached to it. If no TempBuddy Sensor is connected to this unit then the user has the ability to manually control the attached outlet through the unit hosted webpage. This unit also hosts a webpage that can be accessed using the unit's IP Address via Port 80. Also this unit can be configured by accessing its admin page either via an existing WiFi network the unit is joined to or using the unit's TempBuddy_Ctrl wifi network when it is in AP Mode.

> [!CAUTION]
> SECURITY RELATED PLEASE NOTE: 
> Communication with this Device is NOT encrypted. This means that when configuring the 
> settings of the unit using the Admin page, the data is transmitted to and from the client
> in clear text on the network you and the unit are connected to. The potential for
> compromise is highest when doing initial settings with the unit in AP mode as there is no 
> password used for the TempBuddy_Ctrl network. Be cautious as this could result in the 
> leaking of target network's SSID and Password when initially setup, also if changing the 
> unit's admin password don't use one that you use for other sensitive things as it will be 
> sent to the unit in clear text.
>
> FYI: The fix for this issue is in progress and expected to be complete soon. (01/10/2024)

### Hosted Endpoints:
| Endpoint | Description |
| :--- | :--- |
| / | This is where the unit's information is deplayed as a web page |
| /admin | This is where the unit's settings are configured. Default Password: `P@ssw0rd123` |

## Important Software Details
When the unit is first programmed it boots up as an AccessPoint that can be connected to using a computer or phone, by connecting to the presented network with a name of `TempBuddy_Ctrl` using the WiFi password of `P@ssw0rd123`. Once connected to the unit's WiFi network you can also connect to the unit's admin page for configuring it using a web browser via the URL: http://192.168.1.1/admin. 

This will take you to a page requesting a password. Initially the password is simply `admin` but can be changed. This will display the current unit settings and allow the user to make desired configuration changes to the unit. When the Network settings are changed the unit will reboot and attempt to connect to the configured network. 

This firmware also allows for the unit to be equiped with a factory reset button. To perform a factory reset the factory reset button must supply a HIGH to its input while the unit is rebooted. Upon reboot if the factory reset button is HIGH the stored settings in flash will be replaced with the original factory default setttings. The factory reset button also serves another purpose during the normal operation of the unit. If pressed breifly the unit will flash out the last octet of its IP Address. It does this using the built-in LED. Each digit of the last octet is flashed out with a breif rapid flash between the blink count for each digit. 

Once all digits have been flashed out the LED will do a long rapid flash. Also, one may use the factory reset button to obtain the full IP Address of the unit by keeping the factory reset button pressed during normal unit operation for more than 6 seconds. When flashing out the IP address the unit starts with the first digit of the first octet and flashes slowly that number of times, then it performs a rapid flash to indicate it is on to the next digit. Once all digits in an octet have been flashed out the unit performs a second after digit rapid flash to indicate it has moved onto a new octet. 
  
I will demonstrate how this works below by representting a single flash of the LED as a dash `-`. I will represent the post digit rapid flash with three dots `...`, and finally I will represent the end of sequence long flash using 10 dots `..........`. 

Using the above, an IP address of 192.168.123.71 would be flashed out as follows:
```
1                     9       2      .  1               6                   8      .  
- ... - - - - - - - - - ... - - ... ... - ... - - - - - - ... - - - - - - - - ... ... 
1       2         3      .              7     1           
- ... - - ... - - - ... ... - - - - - - - ... - ..........
```
  
The short button press version of the above would simply be the last octet so in this case it would be:
```
              7     1
  - - - - - - - ... - ..........
```

The last octet is useful if you know the network portion of the IP Address the device would be attaching to but are not sure what the assigned host portion of the address is, of course this is only for network masks of 255.255.255.0. 


## Building the Unit's Hardware
I have documented the build process and design for the TempBuddy Control Unit as an Instructables Page. That page and information can be found here:

https://www.instructables.com/WiFi-Enabled-Outlet-With-Optional-Temperature-Cont/

The Unit has undergone a major redesign to make it capable of controlling devices requiring more power than the original design could handle. The Instructable does not currently reflect those changes, but may be updated to do so in the future.

