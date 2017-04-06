/*
  This file is flashed on ESP-01 chips. This execute a packet fowarding from source to server
*/
#include <ArduinoJson.h>
#include <easyMesh.h>

#define   LED             5       // GPIO number of connected LED

#define   MESH_PREFIX     "meshNet"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555

#define DISCOVERY 0
#define DATA 1
#define DISCOVERY_REPLY 2
#define DATA_ACK 3

#define MAX_SIZE 512
easyMesh  mesh;
StaticJsonBuffer<512> jsonBuffer;

int costToServer = -1;
uint32_t nextHopId = 0;

void setup() {
  mesh.setReceiveCallback(&receivedCallback);
  while(costToServer == -1){ //Attendo di trovare la rotta giusta
    discovery();
    delay(500); //Attendo la risposta
    if(costToServer == -1){
      delay(1000);
    }
  }
  
}

void loop() {
  

}

/*
 * Aggiorna i valori del nextHop e del costo per arrivare al server
 */
void discovery(){
  JsonObject& msg = jsonBuffer.createObject();
  msg["type"] = DISCOVERY;
  msg["id"] = mesh.getChipId();
  
  char[MAX_SIZE] msgString;
  msg.printTo(msgString, MAX_SIZE);

  //Resetto i miei valori attuali
  nextHopId = 0;
  costToServer = -1;
  
  mesh.sendBroadcast(msgString, MAX_SIZE);
}

void receivedCallback( uint32_t from, String &msg ){
  JsonObject& msg = jsonBuffer.parseObject(msg);
  if(msg["type"] == DISCOVERY){
    JsonObject& msg = jsonBuffer.createObject();
    msg["type"] = DISCOVERY_REPLY;
    msg["id"] = mesh.getChipId();
    msg["cost"] = costToServer;
    msg["nhop"] = 
  } else if(msg["type"] == DISCOVERY_REPLY){
    if((costToServer == -1) || (msg["cost"] > 0 && msg["cost"] < costToServer )){
      nextHopId = from;
      costToServer = msg["cost"]+1;
    }
  } else if(msg["type"] == DATA){
    if(nextHopId == -1){
      mesh.sendSingle(from, 
    }
    mesh.sendSingle(nextHopId, msg);
    
  }
}

