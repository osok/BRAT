# Arduino Setup

Review instructions here for how to add the ESP 8266 boards to the Arduino IDE: https://github.com/esp8266/Arduino

# Config.h
Change the name of Config-Example.h to Config.h

You may want to pdate the SSID and the password for te Access Point so that others cannot connect as easily.  

# How to install software

Youtube video: https://youtu.be/guUEqy8e27w

Notes from the video
 
> You will need to code the GitHub Repo.
> https://github.com/osok/BRAT
> 
> In order to compile this You will need Ardunio IDE Installed.
> 
> Next you will need to install the NodeMCU Board.
> 
> Once you add the ESP 8266 board : http://arduino.esp8266.com/stable/package_esp8266com_index.json
> 
> Then restart the IDE.
> 
> Then goto tools > Boards > Board Manager and install
> 
> Then Select the Generic ESP8266
> 
> Then change to the Flash size to 4M (1M)
> 
> Then plug your BRAT Into the micro sd, selectt he port.  
> Press and hold the flash button while you plug the cable in.
> After 5 seconds you can release the button, it is now ready to be programmed.
>  
> Then you need to copy the Config-Example.h to Config.h
> In the Config.h Change the SSID and the password.
> 
> Then save and upload.  It will compile first.
> Then upload.
> Then success!!!
