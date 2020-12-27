#define RGB_NOTIFICATIONS_CONNECTING_ONLY 1
#define NAME "GreenHouse"

#include <WebServer.h>
#include <SparkFun_Photon_Weather_Shield_Library.h>

Weather sensor;

WebServer webserver("/metrics", 80);

void getData(WebServer &server, WebServer::ConnectionType type, char *, bool) {
	float hpascals = sensor.readPressure() / 100;
	server.httpSuccess();
	if (type == WebServer::GET) {
		P(humidity) =	"# HELP humidity Last sampled humidity of weather shield\r\n"
				"# TYPE humidity gauge\r\n";

		P(baroTemp) =	"# HELP barometer_temperature_f Last sampled barometer temperature of weather shield\r\n"
				"# TYPE barometer_temperature_f gauge\r\n";

		P(temp) =	"# HELP temp_f Last sampled temperature in F of weather shield\r\n"
				"# TYPE temp_f gauge\r\n";

		P(pressHPA) =	"# HELP pressure_hpa Last sampled pressure in hectopascals from weather shield\r\n"
				"# TYPE pressure_hpa gauge\r\n";

		P(pressHG) =	"# HELP pressure_hg Last sampled pressure in inches if mercury from weather shield\r\n"
				"# TYPE pressure_hg gauge\r\n";

		server.printP(humidity);
		server.printf("humidity{name=\"%s\"}", NAME);
		server.print(sensor.getRH());
		server.printCRLF();

		server.printP(baroTemp);
		server.printf("barometer_temperature_f{name=\"%s\"} ", NAME};
		server.print(sensor.readBaroTempF());
		server.printCRLF();

		server.printP(temp);
		server.printf("temp_f{name=\"%s\"} ", NAME);
		server.print(sensor.getTempF());
		server.printCRLF();

		server.printP(pressHPA);
		server.printf("pressure_hpa{name=\"%s\"} ", NAME);
		server.print(hpascals);
		server.printCRLF();

		server.printP(pressHG);
		server.printf("pressure_hg{name=\"%s\"} ", NAME);
		server.print(hpascals * 0.0295300);
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
