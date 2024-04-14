# TempBuddy Control
## Overview
> [!Document Last updated for]
> Firmware Version: 5.1.1

This software was written to be utilized by an ESP8266 Device in a specialized hardware configuration known as a TempBuddy Control Unit. It is also intended that this unit be operated in conjunction with a TempBuddy Sensor device, however the Sensor is not required as this unit still provides functionality and usefullness without it.

The TempBuddy Sensor is device has the ability to sense and report Temperature and Humidity data via a web interface. 

This unit can be pointed at the IP of a TempBuddy Sensor device and read its temp and then react to the temperature by controlling an outlet which can have a Heating or Cooling device attached to it. If no TempBuddy Sensor is connected to this unit then the user has the ability to manually control the attached outlet through the unit hosted webpage. This unit also hosts a webpage that can be accessed using the unit's IP Address via HTTPS Port 443. Also this unit can be configured by accessing its admin page either via an existing WiFi network the unit is joined to or using the unit's TempBuddy_Ctrl wifi network when it is in AP Mode.

> [!IMPORTANT]
> The device is configured to use HTTPS for its internal webserver by default, 
> however the provided SSL Certs are obviously not secure so you will need to 
> generate your own Certificates and add them to a Secrets.h file in the project 
> before it is built. See the comments in the ExampleSecrets.h file for instructions
> on how to generate your own certificates, or look further below in this document as
> I have updated this README to also contain the instructions.
>
> Sorry, but at this point certs are baked into the binary when compiled and cannot
> be changed out during runtime. I may add that capability at a later time.

### Hosted Endpoints:
| Endpoint | Description |
| :--- | :--- |
| / | This is where the unit's information is deplayed as a web page |
| /admin | This is where the unit's settings are configured. Default User: `admin`, Default Password: `admin` |

## Important Software Details
When the unit is first programmed it boots up as an AccessPoint that can be connected to using a computer or phone, by connecting to the presented network with a name of `TempBuddy_Ctrl` using the WiFi password of `P@ssw0rd123`. Once connected to the unit's WiFi network you can also connect to the unit's admin page for configuring it using a web browser via the URL: http://192.168.1.1/admin. 

This will pop up a dialogue requesting a user and password. Initially the user is `admin` and the password is `admin` but they can be changed. This will display the current unit settings and allow the user to make desired configuration changes to the unit. When the Network settings are changed the unit will reboot and attempt to connect to the configured network. 

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

The last octet is useful if you know the network portion of the IP Address the device would be attaching to but are not sure what the assigned host portion of the address is, of course this is only for network masks of `255.255.255.0`. 


## Building the Unit's Hardware
I have documented the hardware build process and design for the TempBuddy Control Unit as an Instructables Page. That page and information can be found here:

https://www.instructables.com/WiFi-Enabled-Outlet-With-Optional-Temperature-Cont/

The Unit has undergone a major redesign to make it capable of controlling devices requiring more power than the original design could handle. The Instructable does not currently reflect those changes, but may be updated to do so in the future.

## Generating Your Own Certs
To generate your own private certificates for the SSL encryption of the web traffic to and from this unit you can do the following...

### CA Cert and Key
Generate a private certificate-authority certificate and key:
```
openssl genrsa -traditional -out ca_key.pem 2048
openssl req -x509 -new -nodes -key ca_key.pem -days 4096 -out ca_cer.pem
```     

> [!IMPORTANT]
> Keep ca_key.pem absolutely secure, with it someone could make certs 
> that are signed by you!!! Never upload the ca_key.pem to the ESP8266. 
> 
> The ca_cert.pem file is the public certificate for your CA and should be shared.

### Server Key and Request for Cert
Genereate a server key and request file (You need the CA artifacts to do this):
```
openssl genrsa -traditional -out server_key.pem 2048
openssl req -out server_req.csr -key server_key.pem -new
```

> [!NOTE]
> Leave 'extra' attributes blank when filling out information for CSR! 

### Server Cert Using CA Creds
Use CA creds to generate a server certificate from the request:
```
openssl x509 -req -in server_req.csr -out server_cer.pem -sha256 -CAcreateserial -days 4000 -CA ca_cer.pem -CAkey ca_key.pem
```

> [!IMPORTANT]
> Keep server_key.pem secure, this will be used by your server but never
> should it be shared with anyone else!!! The server_cer.pem is the certificate and
> must be shared with others.

## Adding Custom SSL Certificates to Project
To add your own SSL Certificates to the project, simply add a file called `Secrets.h` in the `lib/Secrets` folder of the project. In the Secrets.h file store your Server Certificate using the variable name: `server_cert` and the Server's Private Key using the variable name: `server_key`. The project's `.gitignore` file is setup so that if you choose to upload the code to a git repo the Secrets.h file will not be included, this is to help keep your cert and key from being publicly exposed.

Both variables should be defined similarly to below:
```
#ifndef Secrets_h
#define Secrets_h
#include <pgmspace.h>

const char server_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
BLA,BLA,BLA CERT TEXT HERE...
-----END CERTIFICATE-----
)EOF";

const char server_key[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
BLA,BLA,BLA KEY TEXT HERE...
-----END RSA PRIVATE KEY-----
)EOF";

#endif
```

> [!IMPORTANT]
> !!!ONE MORE THING!!!
> See how the cert and key are defined above??? Proper whitespace is critical for your
> certs to function properly. ESP8266 gives NO ERRORS when they are improperly 
> formatted.
>
> So rather than wasting DAYS trying to figure out why your certs aren't working like 
> I did, just go ahead and throw your neat code indentions out the window and make 
> sure the lines are all shoved to the left of the document. They should work that 
> way.

