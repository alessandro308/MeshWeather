#include <ArduinoJson.h>
#include <easyMesh.h>
#include "ESP8266WiFi.h"

#define   MESH_PREFIX     "meshNet"
#define   MESH_PASSWORD   "meshPassword"
#define   MESH_PORT       5555
#define   SERVER_SSID     "serverssid"
#define   RSSI_THRESHOLD  50 
#define   SERVER_ID       -1
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
/*
 * Funzione che viene invocata ad ogni pacchetto ricevuto.
 */
void receivedCallback( uint32_t from, String &msg_str ){
  /* Stampa sul seriale tutti i pacchetti che riceve */
  Serial.println(msg_str);
}

void newConnectionCallback( bool adopt ) {
  
}

void setup() {
  Serial.begin(115200);
  Serial.println("INIT START");
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
  Serial.println(mesh.getChipId());
  Serial.println("INIT END");
  mesh.setReceiveCallback(&receivedCallback);
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS );
  //Controlla che il server sia raggiungibile
  mesh.setNewConnectionCallback( &newConnectionCallback );
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  mesh.update();
  
  /*TOOD: Aggiungere richiesta di interesse / Discovery
   */
}
