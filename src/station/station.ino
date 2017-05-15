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

void sendToServer(String msg){
  
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
        if(nextHopId == SERVER_ID){
          sendToServer(msg["data"]);
        }else{
          mesh.sendSingle(nextHopId, msg_str);
        }
    }break;
  }
   
}


void setup() {
  mesh.setReceiveCallback(&receivedCallback);
  //Controlla che il server sia raggiungibile
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.begin(115200);
  
}

void loop() {
  char x;
  String msg;
  if(Serial.available()){
    x = Serial.read();
    if( x == 'P' ){
       msg = getSerialJSON();
    }
    mesh.sendSingle(nextHopId, msg);
  }
  
}

