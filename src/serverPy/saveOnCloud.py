import serial, json
from random import randint
import httplib, urllib, time
from random import uniform
#ser = serial.Serial('/dev/USB_SERIAL', 115200);
while True:
	#x = ser.readline()
	x = '{"temp":'+str(randint(0,9))+', "id": 1010, "from": 1234}';
	j = json.loads(x);
	t = j["temp"];
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
