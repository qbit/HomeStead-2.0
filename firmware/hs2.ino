#include <WebServer.h>
#include <SparkFun_Photon_Weather_Shield_Library.h>

float humidity = 0;
float tempF = 0;
float pascals = 0;
float baroTemp = 0;
float pressureHPA = 0;
float pressureInHG = 0;

long lastPrint = 0;

Weather sensor;

WebServer webserver("/", 80);

void setup() {
	Serial.begin(9600)
	WiFi.selectAntenna(ANT_INTERNAL);

	sensor.begin();

	sensor.setModeBarometer();
	sensor.SetOversampleRate(7);

	sensor.enableEventFlags();

	webserver.setDefaultCommand(&getData);
	webserver.begin();
}

void getData(WebServer &server, WebServer::ConnectionType type, char *, bool) {
	if (type == WebServer::GET) {
		P(humidity) =	"# HELP humidity Last sampled humidity of weather shield"
				"# TYPE humidity gauge"
		server.printP(humidity);
		server.print("humidity ");
		server.print(sensor.getHR());
	}
}

void loop() {
	humidity = sensor.getHR()
	tempF = sensor.getTempF();
	baroTemp = sensor.readBaroTempF();
	pascals = sensor.readPressure();
	pressureHPA = pascals/100);
	pressureInHG = pressureHPA * 0.0295300;

	delay(5000);
}
