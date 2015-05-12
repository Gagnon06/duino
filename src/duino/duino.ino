#include <IRremote.h>
#include <ServoTimer2.h>
#include <RCSwitch.h>
#include <VirtualWire.h>
#include "RfCom.h"

#define BASE_REF 123

bool debug = true;
int index = 0;

char messageBuffer[30];
char cmd[3];
char pin[3];
char val[13];
char aux[4];
char type[2];
char addr[5];
char rfMsg[26];

ServoTimer2 servo;
RFCom rfcom(9, 7, BASE_REF, BASE_REF);
IRsend irsend;

void setup() {
  Serial.begin(115200);
  rfcom.init();
}

void loop() {
  while(Serial.available() > 0) {
    char x = Serial.read();
    if (x == '!') index = 0;      // start
    else if (x == '.') process(); // end
    else messageBuffer[index++] = x;
  }
}

/*
 * Deal with a full message and determine function to call
 */
void process() {
  index = 0;

  strncpy(cmd, messageBuffer, 2);
  cmd[2] = '\0';
  strncpy(pin, messageBuffer + 2, 2);
  pin[2] = '\0';

  if (debug) {
    Serial.println(messageBuffer);
  }
  MsgType cmdid = (MsgType)atoi(cmd);

  if (cmdid == 95) {
    strncpy(type, messageBuffer + 4, 1);
    type[1] = '\0';
    strncpy(val, messageBuffer + 5, 9);
    val[8] = '\0';
    strncpy(addr, messageBuffer + 14, 4);
    addr[4] = '\0';
  } else if (cmdid == 96 || cmdid == 94) {
    strncpy(val, messageBuffer + 4, 12);
    val[12] = '\0';
  } else if (cmdid == 93) {
    strncpy(rfMsg, messageBuffer + 4, 25);
    rfMsg[25] = '\0';
  }else if (cmdid > 96) {
    strncpy(val, messageBuffer + 4, 2);
    val[2] = '\0';
    strncpy(aux, messageBuffer + 6, 3);
    aux[3] = '\0';
  } else {
    strncpy(val, messageBuffer + 4, 3);
    val[4] = '\0';
  }
  
  //Empty the buffer for the next message
  memset(messageBuffer, 0, 30 * (sizeof messageBuffer[0]) );

// Serial.println(cmdid);
// Serial.println(pin);
// Serial.println(val);
// Serial.println(aux);

  switch(cmdid) {
    case SM:  sm(pin,val);                   break;
    case DW:  dw(pin,val);                   break;
    case DR:  dr(pin,val);                   break;
    case AW:  aw(pin,val);                   break;
    case AR:  ar(pin,val);                   break;
    case RF: handleRFCom(rfMsg);            break;
    case RCDecimal: handleRCDecimal(pin, val);     break;
    case IR: handleIRsend(type, val, addr); break;
    case RCTristate: handleRCTriState(pin, val);    break;
    case Ping: handlePing(pin,val,aux);       break;
    case Servo: handleServo(pin,val,aux);      break;
    case Debug: toggleDebug(val);              break;
    default:                                break;
  }
}

/*
 * Toggle debug mode
 */
void toggleDebug(char *val) {
  if (atoi(val) == 0) {
    debug = false;
    Serial.println("goodbye");
  } else {
    debug = true;
    Serial.println("hello");
  }
}

/*
 * Set pin mode
 */
void sm(char *pin, char *val) {
  if (debug) Serial.println("sm");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  if (atoi(val) == 1) {
    pinMode(p, OUTPUT);
  } else {
    pinMode(p, INPUT);
  }
}

/*
 * Digital write
 */
void dw(char *pin, char *val) {
  if (debug) Serial.println("dw");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  pinMode(p, OUTPUT);
  if (atoi(val) == 0) {
    digitalWrite(p, LOW);
    Serial.println(val);
  } else {
    digitalWrite(p, HIGH);
  }
}

/*
 * Digital read
 */
void dr(char *pin, char *val) {
  if (debug) Serial.println("dr");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  pinMode(p, INPUT);
  int oraw = digitalRead(p);
  char m[7];
  sprintf(m, "%02d::%02d", p,oraw);
  Serial.println(m);
}

/*
 * Analog read
 */
void ar(char *pin, char *val) {
  if(debug) Serial.println("ar");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  pinMode(p, INPUT); // don't want to sw
  int rval = analogRead(p);
  char m[8];
  sprintf(m, "%s::%03d", pin, rval);
  Serial.println(m);
}

void aw(char *pin, char *val) {
  if(debug) Serial.println("aw");
  int p = getPin(pin);
  pinMode(p, OUTPUT);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  analogWrite(p,atoi(val));
}

int getPin(char *pin) { //Converts to A0-A5, and returns -1 on error
  int ret = -1;
  if(pin[0] == 'A' || pin[0] == 'a') {
    switch(pin[1]) {
      case '0':  ret = A0; break;
      case '1':  ret = A1; break;
      case '2':  ret = A2; break;
      case '3':  ret = A3; break;
      case '4':  ret = A4; break;
      case '5':  ret = A5; break;
      default:             break;
    }
  } else {
    ret = atoi(pin);
    if(ret == 0 && (pin[0] != '0' || pin[1] != '0')) {
      ret = -1;
    }
  }
  return ret;
}

/*
 * Handle Ping commands
 * fire, read
 */
void handlePing(char *pin, char *val, char *aux) {
  if (debug) Serial.println("ss");
  int p = getPin(pin);

  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  Serial.println("got signal");

  // 01(1) Fire and Read
  if (atoi(val) == 1) {
    char m[16];

    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
    delayMicroseconds(2);
    digitalWrite(p, HIGH);
    delayMicroseconds(5);
    digitalWrite(p, LOW);

    Serial.println("ping fired");

    pinMode(p, INPUT);
    sprintf(m, "ping::%s::read::%08d", pin, pulseIn(p, HIGH));
    Serial.println(m);

    delay(50);
  }
}

/*
 * Handle Servo commands
 * attach, detach, write, read, writeMicroseconds, attached
 */
void handleServo(char *pin, char *val, char *aux) {
  if (debug) Serial.println("ss");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  Serial.println("signal: servo");

  // 00(0) Detach
  if (atoi(val) == 0) {
    servo.detach();
    char m[12];
    sprintf(m, "%s::detached", pin);
    Serial.println(m);

  // 01(1) Attach
  } else if (atoi(val) == 1) {
    // servo.attach(p, 750, 2250);
    servo.attach(p);
    char m[12];
    sprintf(m, "%s::attached", pin);
    Serial.println(m);

  // 02(2) Write
  } else if (atoi(val) == 2) {
    Serial.println("writing to servo");
    Serial.println(atoi(aux));
    // Write to servo
    servo.write(atoi(aux));
    delay(15);

  // 03(3) Read
  } else if (atoi(val) == 3) {
    Serial.println("reading servo");
    int sval = servo.read();
    char m[13];
    sprintf(m, "%s::read::%03d", pin, sval);
    Serial.println(m);
  }
}

/*
 * Handle RC commands
 * handleRCTriState("10", "0FFF0FFFFF0F")
 */
void handleRCTriState(char *pin, char *val) {
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  if (debug) Serial.println("RC");
  RCSwitch rc = RCSwitch();
  rc.enableTransmit(p);
  rc.sendTriState(val);
}

/*
 * Handle RC commands via decimal code
 * For those sockets that don't use tri-state.
 * handleRCDecimal("10", "5522351")
 */
void handleRCDecimal(char *pin, char *val) {
  int p = getPin(pin);
  if (p == -1) { if (debug) Serial.println("badpin"); return; }
  if (debug) Serial.println("RCdec" + atol(val));
  RCSwitch rc = RCSwitch();
  rc.enableTransmit(p);
  rc.send(atol(val), 24);
}

/*
 * Handle IR commands
 */
void handleIRsend(char *type, char *val, char *addr) {
  if (debug) Serial.println("IR");
  switch (atoi(type)) {
    case 1:
      irsend.sendRC5(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 2:
      irsend.sendRC6(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 3:
     irsend.sendNEC(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 4:
      irsend.sendSony(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 5:
      irsend.sendDISH(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 6:
      irsend.sendSharp(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 7:
      irsend.sendPanasonic(strtol(addr, (char **)0, 16), strtol(val, (char **)0, 16));
      break;
    case 8:
      irsend.sendJVC(atoi(val), atoi(addr), 1);
      break;
  }
}

/*
 * Handle RF com 
 */
void handleRFCom(char *msg) {
  if(debug) {
    Serial.println("RF");
    Serial.println(msg);
    Serial.println(strlen(msg));
  }
  RfMessage rfmsg;

  char recv[3];
  char type[3];
  char param[26];

  strncpy(recv, msg + 3, 2);
  recv[2] = '\0';
  rfmsg.recvId = (MsgType)atoi(recv);
  strncpy(type, msg + 5, 2);
  type[2] = '\0';
  rfmsg.type = (MsgType)atoi(type);
  strncpy(param, msg + 7, 25);
  param[25] = '\0';
  rfmsg.param = (MsgType)atoi(param);

  rfcom.sendMessage(rfmsg, 200);
  if(debug) Serial.println("RF done");
}
