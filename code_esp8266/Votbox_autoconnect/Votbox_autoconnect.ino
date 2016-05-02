/*
 * Vot'box V1.1
 */


// Bibliothèque Wifi manager
#include <WiFiManager.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal

// Bibliothèques WIFI
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "reseau_maison";
const char* password = "12345678912345678912345678";

#include <Ticker.h>
Ticker connect_blink; // clignotements led Verte
Ticker vote_blink; // clignotements led Rouge
Ticker ap_mode_blink; // clignotements Point d'acces

WiFiServer server(80); // serveur de remise à zero
WiFiClient client_raz;
String http_site = "http://moumoute.biz/sondage_sms_2/smswall/add_sms.php?phrases="; // url du site de vote

int reboot=0;
char* vote = ""; // vote en cours
int voted = 0; // a voté
const int LED_VERTE = 4;
const int LED_ROUGE = 5;





void tick_V()
{
  //toggle state
  int state = digitalRead(LED_VERTE);
  digitalWrite(LED_VERTE, !state);
}

void tick_R()
{
  //toggle state
  int state = digitalRead(LED_ROUGE);
  digitalWrite(LED_ROUGE, !state);
}

void tick_AP_MODE()
{
  //toggle state
  int state = digitalRead(LED_ROUGE);
  int state2 = digitalRead(LED_VERTE);
  digitalWrite(LED_ROUGE, !state);
  digitalWrite(LED_VERTE, !state2);
}

void envoi_get(String val) // fonction d'envoi get au site php
{
  HTTPClient http;
  http.setReuse(true);
  http.begin(http_site + val); //HTTP
  int httpCode = http.GET();
  http.end();
}


int keyboard() {

  int touche = 5555;
  /* Principe on balaye les colonne et on tcheck si une ligne est appuyée
     = 14 ; # = 15 ; A = 19 ; B = 18 ; C = 17 ;  D = 16 */
  // colonne des 1 4 7 *
  digitalWrite(13, LOW);
  digitalWrite(12, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(16, HIGH);

  if (!digitalRead(3)) touche = 14; // *
  if (!digitalRead(10)) touche = 7;
  if (!digitalRead(2)) touche = 4;
  if (!digitalRead(0)) touche = 1;

  // colonne des 2 5 8 0
  digitalWrite(13, HIGH);
  digitalWrite(12, LOW);
  digitalWrite(14, HIGH);
  digitalWrite(16, HIGH);

  if (!digitalRead(3)) touche = 0;
  if (!digitalRead(10)) touche = 8;
  if (!digitalRead(2)) touche = 5;
  if (!digitalRead(0)) touche = 2;

  // colonne des 3 6 9 #
  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(14, LOW);
  digitalWrite(16, HIGH);

  if (!digitalRead(3)) touche = 15; // #
  if (!digitalRead(10)) touche = 9;
  if (!digitalRead(2)) touche = 6;
  if (!digitalRead(0)) touche = 3;

  // colonne des A B C D
  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(16, LOW);

  if (!digitalRead(3)) touche = 16; //D
  if (!digitalRead(10)) touche = 17; // C
  if (!digitalRead(2)) touche = 18; // B
  if (!digitalRead(0)) touche = 19; // A
  return touche;

}


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Pas de réseau connu, demarrage du point d'acces");
  connect_blink.detach();
  digitalWrite(LED_VERTE,LOW);
  digitalWrite(LED_ROUGE,LOW);
  ap_mode_blink.attach(0.6, tick_AP_MODE);


  // print the ssid that we should connect to to configure the ESP8266
  Serial.print("Nom du point d'acces :  ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.print("Adresse IP :  ");
  Serial.println(WiFi.softAPIP());
  reboot=1;
}

void setup() {
  Serial.begin(115200);
  delay(800);
  WiFiManager wifiManager; // demarrage de wifimanager
  wifiManager.setAPCallback(configModeCallback);
  Serial.println("Vot'Box Version 1.1");
  connect_blink.attach(0.6, tick_V); // clignotement vert ==> recherche de connexion en cours/



  // Initialisation des soties LEDS
  pinMode(LED_VERTE, OUTPUT); // led verte
  pinMode(LED_ROUGE, OUTPUT); // led rouge


  // Initialisation du clavier
  pinMode(13, OUTPUT); // colonne des 1 4 7 *
  pinMode(12, OUTPUT); // colonne des 2 5 8 0
  pinMode(14, OUTPUT); // colonne des 3 6 9 #
  pinMode(16, OUTPUT); // colonne des A B C D

  pinMode(0, INPUT_PULLUP); // ligne des 1 2 3 A
  pinMode(2, INPUT_PULLUP); // ligne des 4 5 6 B
  pinMode(10, INPUT_PULLUP); // ligne des 7 8 9 C
  pinMode(3, INPUT_PULLUP); // ligne des * 0 # D (ATTENTION : sortie RX à remplacer dans la V2 GPIO15)
  
  if (!digitalRead(3) && !digitalRead(14)) { // si # appuyé au demarrage on reset factory
    WiFi.disconnect();
    ESP.reset();
  }
  
  // Initialisation du WIFI
  //version SSID defini
  //WiFi.begin(ssid, password);
  //version WifiManager
  const char* AP_NAME = "VotBox_";
  Serial.println(AP_NAME);
  if (!wifiManager.autoConnect(AP_NAME)) {
    Serial.println();
    Serial.println("Probleme de connexion ... reboot ");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }



  Serial.println();
  Serial.println("Attente de connection au réseau... ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connecté au réseau : ");
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
  server.begin();
  digitalWrite(LED_VERTE, LOW);
  digitalWrite(LED_ROUGE, LOW);
  connect_blink.detach();
  if(reboot) ESP.restart();
}



void loop() {
  String req = "";
  client_raz = server.available(); // test de la requete sur le serveur

  if (client_raz) {
    req = client_raz.readStringUntil('\r'); // récupération de la requette
    Serial.print(" reception d'une requete");
    Serial.println(req);
  }

  if (req.indexOf("/RAZ") != -1) { //Si RAZ alors on remets tout à zero !
    voted = 0;
    String s = "HTTP/1.1 200 OK\r\n";
    s += "Content-Type: text/html\r\n\r\n";
    s += "<html>\n";
    s += "ok;" ;
    s += vote;
    s += "</html>\n";
    client_raz.print(s);
    Serial.println("Remise à zero");
    delay(1);
    client_raz.flush();
    digitalWrite(LED_ROUGE, LOW);
    vote = "";
  }
  if (WiFi.status() != 3) {
    Serial.println(" déconnécté, trop loin du point d'acces ? ");
    digitalWrite(LED_ROUGE, LOW);
    vote_blink.detach();
    digitalWrite(LED_VERTE, HIGH);
    delay(600);
    digitalWrite(LED_VERTE, LOW);
    delay(600);
  } else {
    digitalWrite(LED_VERTE, HIGH);
  }


  int val_keyboard = keyboard();

  if (val_keyboard != 5555 && voted == 0) {
    Serial.println(val_keyboard);
    switch (val_keyboard) {
      case 14: // correction du vote
        vote = "";
        vote_blink.detach();
        digitalWrite(LED_ROUGE, LOW);
        delay(300);
        break;
      case 15: // envoi du vote
        if (vote != "") {
          Serial.println("envoi du vote");
          Serial.println(vote);
          envoi_get(vote);
          vote_blink.detach();
          digitalWrite(LED_ROUGE, HIGH);
          voted = 1;
        }
        break;
      case 19:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "A";
        }
        break;
      case 18:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "B";
        }
        break;
      case 17:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "C";
        }
        break;
      case 16:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "D";
        }
        break;
      case 1:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "1";
        }
        break;
      case 2:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "2";
        }
        break;
      case 3:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "3";
        }
        break;
      case 4:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "4";
        }
        break;
      case 5:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "5";
        }
        break;
      case 6:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "6";
        }
        break;
      case 7:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "7";
        }
        break;
      case 8:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "8";
        }
        break;
      case 9:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "9";
        }
        break;
      case 0:
        if (vote == "") {
          vote_blink.attach(0.3, tick_R);
          vote = "0";
        }
        break;
    }
  }
}



