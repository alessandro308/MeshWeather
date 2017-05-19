#include <ArduinoJson.h>
#include <easyMesh.h>
#include "ESP8266WiFi.h"

#define   MESH_PREFIX     "meshNet"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555
#define   SERVER_SSID     "serverssid"
#define   RSSI_THRESHOLD  50 
#define   SERVER_ID       -1
#define   SERVER_IP       
#define   SERVER_PORT     
#define   MAX_SIZE        512
easyMesh  mesh;
StaticJsonBuffer<512> jsonBuffer;
char msgString[MAX_SIZE];

uint32_t nextHopId = 0; //0 se direttamente connesso al server, chipId del nextHop altrimenti
#define DISCOVERY_REQ 0
#define DATA 1
int update = 0;

int lastPId[30];
uint32_t lastCId[30];

void alreadySent(int id, uint32_t from){
  int i;
  for(i=0; i<30; i++){
    if(lastPId[i] == id && lastCId[i] == from)
      return true;
  }
  return false;
}

int lastInserted = 0;
void addSentMessage(int id, uint32_t from){
  lastInserted=(lastInserted+1)%30;
  lastPId[lastInserted]=id;
  lastCId[lastInserted]=from;
}

String getSerialJSON(){
  int s = 0;
  String json("");
  do{
    char x = Serial.read();
    json += x;
    if(x == '{')
      s++;
    else if(s == '}')
      s--;
  }while(s);
  return json;
}


/*
 * Funzione che viene invocata ad ogni pacchetto ricevuto.
 */
void receivedCallback( uint32_t from, String &msg_str ){
  JsonObject& msg = jsonBuffer.parseObject(msg_str);
  int type = msg["type"];
  switch(type){  
    case(DISCOVERY_REQ):{
        if(msg["update_number"] > update){
          update = msg["update_number"];
          nextHopId = msg["sender_id"];
        }
    }break;
    case(DATA):{
        if(!alreadySent(msg["id"], msg["from"]))
            if(nextHopId != -1)
              mesh.sendSingle(nextHopId, msg_str);
            else
              mesh.sendBroadcast(msg_str);     
    }break;
    default:{
    }break;
  }
   
}

void newConnectionCallback( bool adopt ) {
  
}

void setup() {
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.setReceiveCallback(&receivedCallback);
  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE );
  //Controlla che il server sia raggiungibile
  mesh.setNewConnectionCallback( &newConnectionCallback );
  Serial.begin(115200);
  
}

void loop() {
  mesh.update();
  char x;
  /*
   * TODO: Costruire un pacchetto esempio con id, from, msg e id che incrementa
   */
  String msg("{temp: 23, from: 001010}");
  /*if(Serial.available()){
    x = Serial.read();
    if( x == 'P' ){
       msg = getSerialJSON();
    }
    if( nextHopId != -1)
      mesh.sendSingle(nextHopId, msg);
    else
      mesh.sendBroadcast(msg);
  }*/
  mesh.sendBroadcast(msg);
  
  delay(5000);
}

