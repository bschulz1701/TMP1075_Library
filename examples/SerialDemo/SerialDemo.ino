#include <Wire.h>
#include <TMP1075.h>

TMP1075 TempSense; //Initalize with default ADR

void setup() {
	Serial.begin(115200);
	while(!Serial); //Wait until serial conencts
	Serial.println("It's gonna be a Hot Night, tonight!");
	uint8_t Error = TempSense.begin();
	if(Error == 0) Serial.println("TMP1075 Ready...");
	else {
		Serial.print("TMP1075 FAILED: ");
		Serial.println(Error);
	}

	TempSense.SetAlert(60, 50); //Set for +60ºC, +50ºC alert (Alert is forced above 60 and hysteresis to below 50)
}

void loop() {
	float Val = TempSense.GetTemp();
	Serial.print("Temperature = ");
	Serial.print(Val);
	Serial.print("ºC\n"); //Print deg symbol

	delay(1000); //Wait fixed period 
}


