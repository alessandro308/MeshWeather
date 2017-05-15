/*
  This file is flashed on ESP-01 chips. This execute a packet fowarding from source to server
*/
#include <ArduinoJson.h>
#include <easyMesh.h>
#include "ESP8266WiFi.h"
#define   LED             5       // GPIO number of connected LED

#define   MESH_PREFIX     "meshNet"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555
#define   SERVER_SSID     "serverssid"
#define   RSSI_THRESHOLD  50 

#define DISCOVERY 0
#define DATA 1
#define DISCOVERY_REPLY 2
#define DATA_ACK 3
#define DOH 4

#define MAX_SIZE 512

easyMesh  mesh;
StaticJsonBuffer<512> jsonBuffer;
char msgString[MAX_SIZE];

String str;
int costToServer = -1; //Misura il numero di archi tra lui e il server
uint32_t nextHopId = 0; //0 se direttamente connesso al server, chipId del nextHop altrimenti

/*
 * Per ogni connessione, controlla che il nome sia quello del server.
 * Se il valore coincide e la potenza è sufficientemente buona, ci si connette
 * e diventa un nodo direttamente connesso al server
 */
void checkServer(){
  int n = WiFi.scanNetworks();
  for(int i = 0; i<n; i++){
    if(strcmp(WiFi.SSID(i).c_str(),SERVER_SSID) == 0 && WiFi.RSSI(i) > RSSI_THRESHOLD){
      costToServer=1;
      nextHopId=0;
      return;
    }
  }
}

bool sendError(uint32_t to, const char* msg){
  JsonObject& msg_toSend = jsonBuffer.createObject();
  msg_toSend["type"] = DOH;
  msg_toSend["msg"] = msg;
  msg_toSend.printTo(str);
  mesh.sendSingle(to, str);
}

/*
 * Aggiorna i valori del nextHop e del costo per arrivare al server
 * Come funziona: manda ad ogni nodo una richiesta di comunicare il proprio
 * costo per arrivare al server e il proprio nextHop (per evitare che si creino loop)
 */
void discovery(){
  //Resetto i miei valori attuali
  nextHopId = 0;
  costToServer = -1;
  checkServer();

  if(costToServer = 1)
    return;
  JsonObject& msg = jsonBuffer.createObject();
  msg["type"] = DISCOVERY;
  msg["id"] = mesh.getChipId();

  
  msg.printTo(msgString); 

  mesh.sendBroadcast(str);
}

/*
 * Funzione che viene invocata ad ogni pacchetto ricevuto.
 */
void receivedCallback( uint32_t from, String &msg_str ){
  JsonObject& msg = jsonBuffer.parseObject(msg_str);
  int type = msg["type"];
  switch(type){  
    case(DISCOVERY):{
      /*
       * Risponde alla richiesta di discovery comunicando 
       * la sua distanza dal server e il suo nextHop.
       */
      JsonObject& msg_toSend = jsonBuffer.createObject();
      msg_toSend["type"] = DISCOVERY_REPLY;
      msg_toSend["id"] = mesh.getChipId();
      msg_toSend["cost"] = costToServer;
      msg_toSend["nhop"] = nextHopId;
      msg.printTo(str);
      
      mesh.sendSingle(from, str);
    }break;
      
    case(DISCOVERY_REPLY):{
      /*
       * Aggiorna i valori di nextHopId e costToServer se conveniente rispetto ai valori attuali
       */
      if((
            (costToServer == -1) || 
            (msg["cost"] > 0 && msg["cost"] < costToServer ) )
          && msg["nhop"] != mesh.getChipId() //evita la creazione di cicli
         ){
        nextHopId = from;
        int c = msg["cost"];
        costToServer = c+1;
      }
    }break;
      
    case(DATA):{
      if(nextHopId == -1){
        /*
         * Se non ho un nextHop, ritorna indietro l'errore
         */
        sendError(from, msg["msg"]);
      }
      if(!mesh.sendSingle(nextHopId, str)){
         sendError(from, msg["msg"]); 
         discovery();
      }
    }break;
      
    case(DOH):{
      /*
       * Aggiorna la rotta e rispefisci il messaggio
       */
      do{
        discovery();
        delay(1000);
      }while(nextHopId == -1);
      
      const char* tmp = msg["msg"];
      String s(tmp);
      mesh.sendSingle(nextHopId, s);
    }break;
      
    default:
      
      break;
  }

}


void setup() {
  mesh.setReceiveCallback(&receivedCallback);
  //Controlla che il server sia raggiungibile
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  checkServer();

  /* Se non è connesso al server, cerca la rotta migliore
   * per connettersi al server.
   */
  while(costToServer == -1){ 
    discovery(); 
    delay(500); //Attendo la risposta
    if(costToServer == -1){
      delay(1000);
    }
  }
  
}

void loop() {
  

}

