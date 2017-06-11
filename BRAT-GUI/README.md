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

## BRAT-GUI
1. Download the BRAT-GUI.jar : https://github.com/osok/BRAT/tree/master/BRAT-GUI/dist
2. Download run.sh from the dist directory and place it in the same directory as the BRAT-GUI.jar
3. change the permissions on the run.sh file
```
chmod 700 run.me
```
4. Download the lua script write_hid.lua and place it in the proxmark3/client/scripts directory : https://github.com/osok/BRAT/tree/master/proxmark3-scripts

# Operation

1. with the BRAT turned running
2. turn on the BRAT-GUI
3. open a terminal window and run proxmark client
```
sudo ./promark3 /dev/ttyACM0
```
4. run the run.sh
5. if you changed the IP address of the ESP (you would know it if you did), you can configure the Server IP in the configuration screen.
6. Press the Read Hex Codes
7. Select a Hex code to write_hid
8. Press write Hex Codes
9. Press the copy button
10. make sure there isa T5557 card in the card slot on the BRAT-RPi case
11. copy the contents of te clip board into the proxmark client terminal and hit enter


That's it.