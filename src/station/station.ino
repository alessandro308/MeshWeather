#include <ArduinoJson.h>
#include <easyMesh.h>
#include <limits.h>
#include <map>
#include "ESP8266WiFi.h"

#define   MESH_PREFIX     "meshNet"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555
#define   SERVER_SSID     "serverssid"
#define   RSSI_THRESHOLD  50 
#define   SERVER_ID       -1
#define   SERVER_IP       
#define   SYNCINTERVAL    1800000
#define   SERVER_PORT     
#define   MAX_SIZE        512
#define   DISCOVERY_REQ   0
#define   DATA            1

static int tempPin = 2;
easyMesh mesh;
std::map<uint32_t ,int> lastSentMsg;
DynamicJsonBuffer jsonBuffer(MAX_SIZE);
uint32_t lastSyncTime = 0; //Used to guarantee the route consistency
char msgString[MAX_SIZE];
uint32_t delayTime = 15000;
uint32_t totTime = 0;
uint32_t nextHopId = 0;
int update = 0;
int lastPId[30];
uint32_t lastCId[30];
short packetSendNumber = 0;
int lastInserted = 0;

void propagateDiscovery(JsonObject& m){
  char msg[256];
  sprintf(msg, "{\"from\": %d, \"update_number\": %d, \"sender_id\": %d, \"type\": 0}", m["from"], m["update_number"], mesh.getChipId());
  String p(msg);
  mesh.sendBroadcast(p);
  return;
}

void propagateData(String& msg_str, uint32_t from, int id ){
  /*If the route is expired, nextHopId is set to -1*/
  if(mesh.getNodeTime()%SYNCINTERVAL<SYNCINTERVAL)
    nextHopId = -1;
  if(nextHopId != -1)
    mesh.sendSingle(nextHopId, msg_str);
  else
    mesh.sendBroadcast(msg_str);
  if(id==99)
    /*Prevent overflow*/
    lastSentMsg[from] = -1;  
  else
    lastSentMsg[from] = id;
  return;
}

void receivedCallback( uint32_t from, String &msg_str ){
  JsonObject& msg = jsonBuffer.parseObject(msg_str);
  int type = msg["type"];
  switch(type){  
    case(DISCOVERY_REQ):{
        if(msg["update_number"] > update){
          update = msg["update_number"];
          if(update == INT_MAX)
            /*Prevent overflow*/
            update = 0;
          nextHopId = msg["sender_id"];
          propagateDiscovery(msg);
        }
    }break;
    case(DATA):{
      /*
      * If the node has already sent a data packet from message["from"], before forewarding it
      * we need to check that it's newer than the last sent in order to avoid useless communications
      */
      if(lastSentMsg[msg["from"]] != NULL && lastSentMsg[msg["from"]] != 0)
        if(lastSentMsg[msg["from"]] < msg["id"])
          propagateData(msg_str, msg["from"], msg["id"]);
      else
        propagateData(msg_str, msg["from"], msg["id"]);
    }break;
    default:{}break;
  }
}

float readTemp(){ 
  float millivolts = analogRead(tempPin)/1023.0*5000;
  float celsius = millivolts/10; 
  return celsius; /* return 23.57; */ 
}

void newConnectionCallback( bool adopt ){}

void setup(){
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.setReceiveCallback(&receivedCallback);
  mesh.setDebugMsgTypes(ERROR); //| MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE );
  mesh.setNewConnectionCallback( &newConnectionCallback );
  Serial.begin(115200);
}

void loop(){
  mesh.update();
  /*Send the temperature data to the next hop or broadcasts it*/
  char msg[256];
  /*Prevent overflow*/
  packetSendNumber = (packetSendNumber+1)%100;
  sprintf(msg, "{\"from\": %d, \"id\": %d, \"temp\": %f, \"type\": 1}", mesh.getChipId(), packetSendNumber, readTemp());
  String p(msg);
  if(nextHopId != -1)
    mesh.sendBroadcast(p);
  else
    mesh.sendSingle(nextHopId, p);
  /*If the route is expired, nextHopId is set to -1*/
  totTime = lastSyncTime+delayTime;
  if(totTime%SYNCINTERVAL<SYNCINTERVAL)
    nextHopId = -1;
  delay((long)delayTime);
}

