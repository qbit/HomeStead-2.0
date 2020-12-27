#include <WebServer.h>
#include <SparkFun_Photon_Weather_Shield_Library.h>

Weather sensor;

WebServer webserver("/", 80);

void getData(WebServer &server, WebServer::ConnectionType type, char *, bool) {
	server.httpSuccess();
	if (type == WebServer::GET) {
		P(humidity) =	"# HELP humidity Last sampled humidity of weather shield\n\n"
				"# TYPE humidity gauge\n\n"
				"humidity ";
		server.printP(humidity);
		server.print(sensor.getRH());
	}
}


void setup() {
	Serial.begin(9600);
	WiFi.selectAntenna(ANT_INTERNAL);

	sensor.begin();

	sensor.setModeBarometer();
	sensor.setOversampleRate(7);

	sensor.enableEventFlags();

	webserver.setDefaultCommand(&getData);
	webserver.begin();
}

void loop() {
	webserver.processConnection();
}
