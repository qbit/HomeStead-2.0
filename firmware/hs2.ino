#include <WebServer.h>
#include <SparkFun_Photon_Weather_Shield_Library.h>

const char *stationName = "GreenHouse";

Weather sensor;

WebServer webserver("/metrics", 80);

void setup_leds()
{
	#define RGB_NOTIFICATIONS_CONNECTING_ONLY 1
	RGB.control(true);
	RGB.brightness(0);
}

STARTUP(setup_leds());

void getData(WebServer &server, WebServer::ConnectionType type, char *, bool) {
	float hpascals = sensor.readPressure() / 100;

	server.httpSuccess();
	if (type == WebServer::GET) {
		P(humidity) =	"# HELP wstation_humidity Last sampled humidity of weather shield\n"
				"# TYPE wstation_humidity gauge\n";

		P(baroTemp) =	"# HELP wstation_barometer_temperature_f Last sampled barometer temperature of weather shield\n"
				"# TYPE wstation_barometer_temperature_f gauge\n";

		P(tempF) =	"# HELP wstation_temp_f Last sampled temperature in F of weather shield\n"
				"# TYPE wstation_temp_f gauge\n";

		P(tempC) =	"# HELP wstation_temp_c Last sampled temperature in C of weather shield\n"
				"# TYPE wstation_temp_f gauge\n";

		P(pressHPA) =	"# HELP wstation_pressure_hpa Last sampled pressure in hectopascals from weather shield\n"
				"# TYPE wstation_pressure_hpa gauge\n";

		P(pressHG) =	"# HELP wstation_pressure_hg Last sampled pressure in inches if mercury from weather shield\n"
				"# TYPE wstation_pressure_hg gauge\n";

		server.printP(humidity);
		server.printf("wstation_humidity{name=\"%s\"} ", stationName);
		server.print(sensor.getRH());
		server.print("\n");

		server.printP(baroTemp);
		server.printf("wstation_barometer_temperature_f{name=\"%s\"} ", stationName);
		server.print(sensor.readBaroTempF());
		server.print("\n");

		server.printP(tempF);
		server.printf("wstation_temp_f{name=\"%s\"} ", stationName);
		server.print(sensor.getTempF());
		server.print("\n");

		server.printP(tempC);
		server.printf("wstation_temp_c{name=\"%s\"} ", stationName);
		server.print(sensor.getTemp());
		server.print("\n");

		server.printP(pressHPA);
		server.printf("wstation_pressure_hpa{name=\"%s\"} ", stationName);
		server.print(hpascals);
		server.print("\n");

		server.printP(pressHG);
		server.printf("wstation_pressure_hg{name=\"%s\"} ", stationName);
		server.print(hpascals * 0.0295300);
		server.print("\n");
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
