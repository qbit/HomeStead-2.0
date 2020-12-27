#define RGB_NOTIFICATIONS_CONNECTING_ONLY 1
#define NAME GreenHouse

#include <WebServer.h>
#include <SparkFun_Photon_Weather_Shield_Library.h>

Weather sensor;

WebServer webserver("/metrics", 80);

void getData(WebServer &server, WebServer::ConnectionType type, char *, bool) {
	float hpascals = sensor.readPressure() / 100;
	server.httpSuccess();
	if (type == WebServer::GET) {
		P(humidity) =	"# HELP humidity Last sampled humidity of weather shield\r\n"
				"# TYPE humidity gauge\r\n"
				sprintf("humidity{name=\"%s\"}", NAME);

		P(baroTemp) =	"# HELP barometer_temperature_f Last sampled barometer temperature of weather shield\r\n"
				"# TYPE barometer_temperature_f gauge\r\n"
				sprintf("barometer_temperature_f{name=\"%s\"} ", NAME};

		P(temp) =	"# HELP temp_f Last sampled temperature in F of weather shield\r\n"
				"# TYPE temp_f gauge\r\n"
				sprintf("temp_f{name=\"%s\"} ", NAME);

		P(pressHPA) =	"# HELP pressure_hpa Last sampled pressure in hectopascals from weather shield\r\n"
				"# TYPE pressure_hpa gauge\r\n"
				sprintf("pressure_hpa{name=\"%s\"} ", NAME);

		P(pressHG) =	"# HELP pressure_hg Last sampled pressure in inches if mercury from weather shield\r\n"
				"# TYPE pressure_hg gauge\r\n"
				sprintf("pressure_hg{name=\"%s\"} ", NAME);

		server.printP(humidity);
		server.print(sensor.getRH());
		server.printCRLF();

		server.printP(baroTemp);
		server.print(sensor.readBaroTempF());
		server.printCRLF();

		server.printP(temp);
		server.print(sensor.getTempF());
		server.printCRLF();

		server.printP(pressHPA);
		server.print(hpascals);
		server.printCRLF();

		server.printP(pressHG);
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
