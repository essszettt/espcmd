        ______           ______              __
       / ____/________  / ____/___ ___  ____/ /
      / __/ / ___/ __ \/ /   / __ `__ \/ __  / 
     / /___(__  ) /_/ / /___/ / / / / / /_/ /  
    /_____/____/ .___/\____/_/ /_/ /_/\__,_/   
              /_/                              

Application to send AT-commands to ESP8266 on ZX Spectrum Next.

It is important that the baudrate of the ESP8266 is set to "115200 bit/s" (default).

![usage.bmp](https://github.com/essszettt/espcmd/blob/main/doc/usage.bmp)

---

### USAGE

![help.bmp](https://github.com/essszettt/espcmd/blob/main/doc/help.bmp)

Return Values:

- "no error"      => "OK" from ESP8266
- "bad state"     => "ERROR" from ESP8266
- "out of range"  => "FAIL" from ESP8266
- "timeout error" => communication error with UART/ESP8266
- "invalid value" => error in command line


---

### HISTORY

- 0.0.3  First public release to test
- 0.1.0  First official release
- 0.1.1  Fixed bug in timeout detection
- 0.2.0  Added options to set baudrate and timeout
