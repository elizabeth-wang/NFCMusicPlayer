/*
   tagDecoder.cpp
   Description: Read a NFC tag's UID and return associated ID.
                Return -1 if the tag's UID is not associated with an ID.
                Return 0 if there is no tag UID present.
   Author: Elizabeth Wang
   01-25-24 V1: Initial commit
*/

#include <MFRC522.h>
#include <SPI.h>
#include <Arduino.h>
#include "tagDecoder.h"
#define SS_PIN 10
#define RST_PIN 9
int flag;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void tagDecoderSetup() {
  //  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

int tagDecoderLoop() {
  /* Look for new cards.
  Scans of the same tag will alternate flag between 0/1.
  desired functionality: consistent 1 for continual scan of same tag */
  flag = 0;
  if (mfrc522.PICC_IsNewCardPresent())
  {
    flag = 1; // card present
  }
  else  // try again to catch the toggle effect,
  {
    if (mfrc522.PICC_IsNewCardPresent())
      flag = 1; // card present
  }

  if (flag)
  {
    //Serial.println("Card present");
    mfrc522.PICC_ReadCardSerial();
  }
  else
  {
    //Serial.println("No card");
    return 0;
  }
  delay(500);

  /* Obtaining UID */
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  content.trim();
  if (content == "04 CD E4 C3 79 00 00") {
    return 1;
    delay(500);
  } else if (content == "04 67 E5 C3 79 00 00") {
    return 2;
    delay(500);
  } else if (content == "04 CB E4 C3 79 00 00") {
    return 3;
    delay(500);
  } else if (content == "04 2E E5 C3 79 00 00") {
    return 4;
    delay(500);
  } else if (content == "04 C9 E4 C3 79 00 00") {
    return 5;
    delay(500);
  } else if (content == "04 73 E5 C3 79 00 00") {
    return 6;
    delay(500);
  } else {
    return -1;
  }
}
