# Description
BRAT-GUI is used on the BRAT-RPi to read the list of Hex Card values from the BRAT.  It is a java application cmpiled into a self executing JAR 


# Building the BRAT-GUI and BRAT-RPi

## BRAT-RPi

It's easiest to connect the RPi to a keyboard and mouse, while doing the set up.  You can use a HDMI monitor or the 7" Touch screen.

1. Install Ubunutu Mate : Follow instructions here https://ubuntu-mate.org/
2. Update the Ubuntu
```
sudo apt-get update
sudo apt-get upgrade
```
3. Install Proxmark3 for Ubuntu : Follow instructions here  https://github.com/Proxmark/proxmark3/wiki/Ubuntu-Linux
4. Install Java 8 to Ubuntu
```
sudo apt-get install openjdk-8-jdk
```
5. Download the BRAT-GUI.jar : https://github.com/osok/BRAT/tree/master/BRAT-GUI/dist
6. Download the lua script write_hid.lua and place it in the proxmark3/client/scripts directory : https://github.com/osok/BRAT/tree/master/proxmark3-scripts