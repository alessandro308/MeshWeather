import serial, json
import random
import httplib, urllib, time
from random import uniform
#ser = serial.Serial('/dev/cu.SLAB_USBtoUART', 115200);
while True:
	#x = ser.readline()
	x = '{"temp":26.4, "id": 1010, "from": 1234}';
	j = json.loads(x);
	t = j["temp"]+random.uniform(-0.5, 0.5)
	print "Temperatura: "+str(t);
	body = urllib.urlencode (
		{ "field1": t, 
		"key":"UN9Y3TDAXRQ3CIX4"}
		)
	headers = {"Content-type": "application/x-www-form-urlencoded"}
	conn = httplib.HTTPConnection("api.thingspeak.com:80")
	try:
		conn.request("POST", "/update", body, headers)
		conn.close()
		print "Richiesta mandata"
	except:
		print "Connessione fallita: dato non registrato."
	time.sleep(16)

