#include <WebServer.h>
#include <SparkFun_Photon_Weather_Shield_Library.h>

Weather sensor;

WebServer webserver("/", 80);

void getData(WebServer &server, WebServer::ConnectionType type, char *, bool) {
	server.httpSuccess();
	if (type == WebServer::GET) {
		P(humidity) =	"# HELP humidity Last sampled humidity of weather shield\r\n"
				"# TYPE humidity gauge\r\n"
				"humidity ";

		P(baroTemp) =	"# HELP barometer_temperature_f Last sampled barometer temperature of weather shield\r\n"
				"# TYPE barometer_temperature_f gauge\r\n"
				"barometer_temperature_f ";

		P(temp) =	"# HELP temp_f Last sampled temperature in F of weather shield\r\n"
				"# TYPE temp_f gauge\r\n"
				"temp_f ";

		server.printP(humidity);
		server.print(sensor.getRH());
		server.printCRLF();

		server.printP(baroTemp);
		server.print(sensor.readBaroTempF());
		server.printCRLF();

		server.printP(temp);
		server.print(sensor.getTempF());
		server.printCRLF();
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
