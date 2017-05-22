
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
#define   SYNCINTERVAL    1800000

easyMesh  mesh;
uint32_t lastSyncTime = 0;
char msgString[MAX_SIZE];
int update = 0;
uint32_t nextHopId = 0;
DynamicJsonBuffer jsonBuffer(MAX_SIZE);


/*
* This function starts the periodic discovery and the coverage tree building
*/
void discoveryTree(){ 
  char msg[256];
  sprintf(msg, "{\"from\": %d, \"update_number\": %d, \"sender_id\": %d, \"type\": 0}", mesh.getChipId(), ++update, mesh.getChipId());
  /*Prevent overflow*/
  if(update == INT_MAX)
    update = 0;
  String p(msg);
  mesh.sendBroadcast(p);
  lastSyncTime = mesh.getNodeTime();
  return; 
}

void receivedCallback( uint32_t from, String &msg_str ){
  JsonObject& message = jsonBuffer.parseObject(msg_str);
  int type = message["type"];
  if(type!=DISCOVERY_REQ)
      Serial.println(msg_str);
}

void newConnectionCallback( bool adopt ){}

void setup(){
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  mesh.setReceiveCallback(&receivedCallback);
  mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE );
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
