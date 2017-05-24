
#include <ArduinoJson.h>
#include <easyMesh.h>
#include <limits.h>
#include "ESP8266WiFi.h"

#define   MESH_PREFIX     "meshNet"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555
#define   SERVER_SSID     "serverssid"
#define   RSSI_THRESHOLD  50 
#define   SERVER_ID       -1
#define   MAX_SIZE        512
#define   DISCOVERY_REQ   0
#define   DATA            1
#define   SYNCINTERVAL    10000000//1800000000 mezz'ora in microsecondi

easyMesh  mesh;
uint32_t lastSyncTime = 0;
char msgString[MAX_SIZE];
int updateNumber = 0;
uint32_t nextHopId = 0;
DynamicJsonBuffer jsonBuffer(MAX_SIZE);
uint32_t chipId = 0;

/*
* This function starts the periodic discovery and the coverage tree building
*/
void discoveryTree(){ 
  char msg[256];
  /*Prevent overflow*/
  if(updateNumber == INT_MAX)
    updateNumber = 0;
  sprintf(msg, "{\"from\": %"PRIu32", \"update_number\": %d, \"sender_id\": %"PRIu32", \"type\": 0}", chipId, ++updateNumber, chipId);
  String p(msg);
  mesh.sendBroadcast(p);
  lastSyncTime = mesh.getNodeTime();
  return; 
}
int rpn =0;
void receivedCallback( uint32_t from, String &msg_str ){
  if(from == 2008034){
    /* IN DEMO MODE IGNORE PACKET FROM SOURCE TO SIMULATE DISTANCE */
    return;
  }
    
  JsonObject& message = jsonBuffer.parseObject(msg_str);
  int type = message["type"];
  
  if(type!=DISCOVERY_REQ){
    Serial.println(msg_str);
    Serial.flush();
  }
}

void newConnectionCallback( bool adopt ){}

void setup(){
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  chipId = mesh.getChipId();
  lastSyncTime = mesh.getNodeTime(); 
  mesh.setReceiveCallback(&receivedCallback);
  mesh.setDebugMsgTypes( ERROR); // | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE );
  //Controlla che il server sia raggiungibile
  mesh.setNewConnectionCallback( &newConnectionCallback );
  Serial.begin(115200); 
}

void loop(){
  mesh.update();
  //Periodic Discovery builds the forewarding tree (no cycles)
  if(mesh.getNodeTime() - lastSyncTime >= SYNCINTERVAL)
    discoveryTree();
}
