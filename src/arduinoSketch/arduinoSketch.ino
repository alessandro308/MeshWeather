void setup() {
  Serial.begin(115200);

}

void loop() {
  Serial.print("P{\"temp\": 23}");
  delay(10000);
}
