/*
   NFCplayer.ino
   Description: Intake an ID and play, pause, and resume DFPlayer songs.
                FileNum is in write order of files to USB.
                If a previous song was playing and tag is removed, pause the song.
                If a paused song's tag is presented, resume the song.
                Unmatched IDs print an error message and do not affect functionality.
   Depends on tagDecoder.ino, tagDecoder.h
   Install DFRobot_DF1201S, MFRC522 libraries
   Author: Elizabeth Wang
   01-25-24 V1: Initial commit
*/

#include <DFRobot_DF1201S.h>
#include "SoftwareSerial.h"
#include "tagDecoder.h"

SoftwareSerial DF1201SSerial(2, 3);  /* RX  TX */
DFRobot_DF1201S DF1201S;
int prevScan, curScan, lastPlayed;

void setup() {
  Serial.begin(115200);
  DF1201SSerial.begin(115200);
  while (!DF1201S.begin(DF1201SSerial)) {
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
  }
  DF1201S.setBaudRate(115200);
  /* Set volume */
  DF1201S.setVol(/* VOL = */1);
  Serial.print("VOL: ");
  /* Get volume */
  Serial.println(DF1201S.getVol());
  /* Enable amplifier chip */
  DF1201S.enableAMP();
  /* Enter music mode */
  DF1201S.setPrompt(false);
  DF1201S.switchFunction(DF1201S.MUSIC);
  /* Initial start and pause after switchFunction fixes buggy first NFC tag approach that swaps play/pause functions */
  DF1201S.start();
  DF1201S.pause();
  /* Set playback mode to "repeat all" */
  DF1201S.setPlayMode(DF1201S.SINGLE);
  Serial.print("PlayMode: ");
  /* Get playback mode */
  Serial.println(DF1201S.getPlayMode());

  tagDecoderSetup();
  curScan = -1;
  prevScan = -1;
  lastPlayed = -1;
}

void loop() {
  while (lastPlayed < 0) {
    Serial.println("Haven't gotten a valid UID yet.");
    delay(500);
    curScan = tagDecoderLoop();
    if (curScan > 0) {
      lastPlayed = 0;
      break;
    }
  }
  if (curScan < 0) {
    Serial.println("Error: That UID/file pair hasn't been matched yet.");
  } else if (curScan == 0 && lastPlayed > 0) { /* lastPlayed > 0 indicates a previous song */
    Serial.print("Paused: ");
    DF1201S.pause();
    Serial.println(DF1201S.getFileName());
  } else if (curScan > 0 && curScan == lastPlayed) {
    if (prevScan == 0) {
      Serial.print("Unpausing: ");
      Serial.println(DF1201S.getFileName());
      DF1201S.start();
    } else {
      Serial.print("Already playing: ");
      Serial.println(DF1201S.getFileName());
    }
  } else {
    Serial.print("Playing: ");
    DF1201S.playFileNum(curScan);
    Serial.println(DF1201S.getFileName());
    lastPlayed = curScan;
  }
  prevScan = curScan;
  curScan = tagDecoderLoop();
  delay(200);
}
