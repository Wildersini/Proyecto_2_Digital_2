// Adafruit IO Publish & Subscribe Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#define TXD2 17
#define RXD2 16
#include "config.h"
#define IO_LOOP_DELAY 5000
unsigned long lastUpdate = 0;


char seconds[2];
char minutes[2];
char hours [2];
// set up the 'counter' feed
AdafruitIO_Feed *Hour = io.feed("Hour"); //set up of feed 
AdafruitIO_Feed *Minute = io.feed("Minute");
AdafruitIO_Feed *Seconds = io.feed("Seconds");
AdafruitIO_Feed *LED_1 = io.feed("LED_1");
AdafruitIO_Feed *LED_2 = io.feed("LED_2");

void setup() {

   // start the serial connection
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial.print("Connecting to Adafruit IO");
    // connect to io.adafruit.com
  io.connect(); 
  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  LED_1->onMessage(handleMessage);// setting up of feed as on message 
  LED_2->onMessage(handleMessage2);// setting up of feed as on message 
  
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
   // we are connected
  Serial.println();
  Serial.println(io.statusText());
  LED_1->get();
  LED_2->get();

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  Serial2.write(0X34);
  //revcive segundos from pic 
  Serial2.readBytesUntil(0x3A,seconds,3);//the data that is send has a safety character
  delay (2200);
  Seconds->save(seconds);//send to wifi moduel
  Serial.print("sending_s -> ");
  Serial.println(seconds);
  
  //revcive minuto from pic 
  Serial2.write(0X35);
  Serial2.readBytesUntil(0x3A,minutes,3);//the data that is send has a safety character
  delay (2200);
  Minute->save(minutes);
  Serial.print("sending_m -> ");
  Serial.println(minutes);//send to wifi moduel
  
  //revcive hora from pic 
  Serial2.write(0X36);
  Serial2.readBytesUntil(0x3A,hours,3);
  delay (2200);
  Hour->save(hours); //subida de datos a adafruit
  //Prueba para ver en el monitos serial que se esta enviando
  Serial.print("envio -> ");
  Serial.println(hours);
}

// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {
   if (data -> toString()== "ON"){Serial2.write(0X31);}
   if (data -> toString()== "OFF"){Serial2.write(0X30);}
    Serial.print("received <- ");
    Serial.println(data->value());
}
void handleMessage2(AdafruitIO_Data *data) { 
   if (data -> toString()== "ON"){Serial2.write(0X33);}
   if (data -> toString()== "OFF"){Serial2.write(0X32);}
  Serial.print("received2 <- ");
  Serial.println(data->value());
}
