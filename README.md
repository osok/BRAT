Badge Reader Active  Tap

BRAT is designed to connect to the Data0 & Data1 lines inside of a Wiegand
based card reader.  It will store the card data on the file system of an ESP8266-12.  The device also sets up
an Accesspoint that once connected to, also presents a web page that allos the user to interact with it.  
This web page shows card data info, and allows replay directly into the card reader.  

With the Card data provided a proxmark device can be used to program the card data onto a new card.

When I orginally set out to bypass the HID Prox devices, I did a lot of research        
I came along the Tastic RFID Reader by Bishop Fox.  After building a Tastic Reader 
I got the idea that I could somehow wirelessly send the card data to a coworker who
could write the data to a proxmark3, then allowing a 3rd person to use the card to 
gain entry.

From that I realized that I could build a tap which used power from the reader to run,
and it would tap the data0 and data1 lines.  

Seperately I have been playing around with the ESP8266-12 and node-mcu cards and realized
the possiblities of using that platform to buld this on.

Then I went to ShmooCon 2017, where gentleman (who's name I don't recall, so I'll refer 
to him as a gentleman), presented the ESPKey.  Just goes to prove every idea I come up with
someone else does too.  If I heard him right he plans to sell his for $100.  Its like $10 parts
and a little bit of code.  So I'm not doing this to steal from him, but rather just build it and
allow anyone else with enough skill to do so for much cheaper than $100.  I'm not selling these,
so if you want to buy one, search for ESP Key.  If you are doing physical penetration assesments 
it's a great tool to have in your bag.

I want to reconoginize the gentleman that spoke about the ESPKey, since he did so before I built this.
He deserves a lot of credit for building it and talking about it.  Also, I want to thank the guys at
Bishop Fox for the Tastic.  I am using their code base as a refierece for the eading of the Wiegand protocol.


