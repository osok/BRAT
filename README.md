Badge Reader Active  Tap

At the present time this is a collection of my notes and thoughts as I build the device.  Once completed, I will arrange the page to be instructive.

Current Status:  3/6/2017.  Rev 1 Board build and looking to test this weekend.

BRAT is designed to connect to the Data0 & Data1 lines inside of a Wiegand based card reader.  It will store the card data on the file system of an ESP8266-12.  The device also sets up an Accesspoint that once connected to, also presents a web page that allows the user to interact with it.  The web page shows card data info, and allows replay directly into the card reader.  

With the Card data provided a proxmark device can be used to program the card data onto a new card.

When I orginally set out to bypass the HID Prox devices, I did a lot of research.  I came along the Tastic RFID Reader by Bishop Fox.  After building a Tastic Reader I got the idea that I could somehow wirelessly send the card data to a coworker who could write the data to a proxmark3, then allowing a 3rd person to use the card to gain entry.

From that I realized that I could build a tap which used power from the reader to run, and it would tap the data0 and data1 lines.  

Seperately I have been playing around with the ESP8266-12 and node-mcu cards and realized
the possiblities of using that platform to buld this on.

Then I went to ShmooCon 2017, where gentleman (who's name I don't recall, so I'll refer to him as a gentleman), presented the ESPKey.  Just goes to prove every idea I come up with someone else does too.  If I heard him right he plans to sell his for $100.  Its like $10 parts and a little bit of code.  So I'm not doing this to steal from him, but rather just build it and allow anyone else with enough skill to do so for much cheaper than $100.  I'm not selling these, so if you want to buy one, search for ESP Key.  If you are doing physical penetration assesments it's a great tool to have in your bag.

I want to reconoginize the gentleman that spoke about the ESPKey, since he did so before I built this.  He deserves a lot of credit for building it and talking about it.  Also, I want to thank the guys at Bishop Fox for the Tastic.  I am using their code base as a refierece for the eading of the Wiegand protocol.

I have finally pulled this together a little more.  I got the code working a few weeks ago. I had a problem with the replay.  I got hung up on how the ESP Key was demo'd at ShmooCon.  He spliced the four wires +18V / GND / Data0 / Data1.  There was no cutting of the wires, just punched them down.  As it turns out the readers hold the Data0 and Data1 at +5V (High). There is No way to replay the bits that are collected from those wires back to those wires.  There is a port conflict.  So to fix that I had to read in on one set of GPIO Pins and write out to another.  As well the ESP is only capable of 3.3V Logic levels, so a converted had to be used to get the signal to the correct levels.

I spents some time this weekend working on circuits that would support the use of the ESP8266-12e directly.  1 out of 12 units I had were good.  So I decided that the circuit would use the nodmcu v0.9 directly.  That is easy to program and only required power regulation from 18V to 5V, it took care of the 3.3 volts.  The board in the eagle folder is yet to be tested.  As of 2/20/17 it was ordered from OSHPark.  It'll take a few weeks for the board to arrive.

* I used some quick release connectors which can be ordered at: https://www.pololu.com/product/2427.
* The nodemcu can be orcered from ebay or amazon: https://www.amazon.com/Industry-Park-NodeMcu-Internet-Development/dp/B01MG1LEU2/
* There are 2 Electrolytic Capacitors of 10 uF and at least 25V, I'll be using 50V. I have these as part of a larger kit.
* The voltage regulator is a LM7805.  I picked up about 20 of them from eBay.
* The level converter can be found on amazon: https://www.amazon.com/gp/product/B0148BLZGE/


These are the components that I was able to fritz together that worked.  So I'm confident that the board will work.  The board file (BRAT.brd) can be uploaded to www.oshpark.com.  There is a minimum order of 3 boards.  the price comes to about $22.

So the cost individual cost is ~$24, depends on quantity and shipping.
* NodeMCU $9
* Capacitors $1
* Level Converter $2
* Connectors $4
* Board $8

I recieved the board from OSHPark today, and solder the components together.  I have the firmware loaded.  I'm just waiting for a new tastic board, since I overloaded the ones I had with a design of the board (self fabricated) that used smd.  I let the magic smoke out of something that inadvertantly fried a tastic board.  I went with this design because represents exactly what I had been fritizing, and I didn't have to rely on faulty ESP boards.

Front:
![](photos/Resized_top.jpeg)

Back
![](photos/Resized_bottom.jpeg)

Eagle Cad Schematic Screen shot:
![](photos/schematic.png)
