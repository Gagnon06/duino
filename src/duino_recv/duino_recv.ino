#include <VirtualWire.h>
#include <IRremote.h>

char *baseRef = "123"; //Définition du code de la base
char *recvRef = "00"; //Different for each remote arduino

boolean debug=true;

char base[4];
char recv[3];
char type[3];
char param[26];

IRsend irsend;

void setup()
{
  Serial.begin(115200);
  vw_setup(2000); //Vitesse de la com RF (idem à celle de la base)
  vw_set_rx_pin(7); //Définition du pin RX
  vw_rx_start(); //Démarrage de l'écoute des données RF
}

void loop()
{
  if(receiveMsg())
  {
    //Message recu et mis dans les variables
    //Verification de la base et du receiver
    if(strcmp(baseRef, base) == 0 && strcmp(recv, recvRef) == 0)
    {
      //Message valide
      Serial.println("Signal in");
      switch(atoi(type)) {
        case 0:  sm(param);                   break;
        case 1:  dw(param);                   break;
        
        case 3:  aw(param);                   break;
    
        case 95: handleIRsend(param);         break;
        default: break;
      }
      
    }
  }
  
}

//Fonction qui vérifie la précense d'un message RF
//Retourne vrai si un message a bien été reçu
//Modifie directement les variables définie plus haut.
boolean receiveMsg(void)
{
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  uint8_t buf[buflen];
  
  if(vw_get_message(buf, &buflen))
  {
    char charBuf[buflen];
    for(int i = 0; i< buflen; i++)
      charBuf[i] = buf[i];
    strncpy(base, charBuf, 3);
    base[3] = '\0';
    strncpy(recv, charBuf + 3, 2);
    recv[2] = '\0';
    strncpy(type, charBuf + 5, 2);
    type[2] = '\0';
    strncpy(param, charBuf + 7, 25);
    param[25] = '\0';
    
    return true;
  }
  else
  {
    return false;
  }
}


void sm(char *param) {
  if (debug) Serial.println("sm");
  int p = getPin(param);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  char val[2];
  strncpy(val, param + 2, 1);
  val[1] = '\0';
  if (atoi(val) == 0) {
    pinMode(p, OUTPUT);
  } else {
    pinMode(p, INPUT);
  }
}

/*
 * Digital write
 */
void dw(char *param) {
  if (debug) Serial.println("dw");
  int p = getPin(param);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  pinMode(p, OUTPUT);
  char val[2];
  strncpy(val, param + 2, 1);
  val[1] = '\0';
  if (atoi(val) == 0) {
    digitalWrite(p, LOW);
  } else {
    digitalWrite(p, HIGH);
  }
}

void aw(char *param) {
  if(debug) Serial.println("aw");
  int p = getPin(param);
  pinMode(p, OUTPUT);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  char val[4];
  strncpy(val, param + 2, 3);
  val[3] = '\0';
  analogWrite(p,atoi(val));
}


int getPin(char *param) { //Converts to A0-A5, and returns -1 on error
  char pin[3];
  strncpy(pin, param, 2);
  pin[2] = '\0';
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
 * Handle IR commands
 */
void handleIRsend(char *param) {
  if (debug) Serial.println("IR");
  Serial.println(param);
  char type[2];
  char val[9];
  char addr[4];
  
  strncpy(type, param + 2, 1);
  type[1] = '\0';
  strncpy(val, param + 3, 9);
  val[8] = '\0';
  strncpy(addr, param + 12, 3);
  addr[3] = '\0';
  Serial.println(atoi(type));
  Serial.println(strtol(val, (char **)0,16));
  Serial.println(atoi(addr));
  
  switch (atoi(type)) {
    case 1:
      irsend.sendRC5(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 2:
      irsend.sendRC6(strtol(val, (char **)0, 16), atoi(addr));
      break;
    case 3:
     Serial.println("NEC");
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
