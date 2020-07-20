#ifndef TMP1075_h
#define TMP1075_h

//Use conditional include between particle and arduino enviroment 
#if defined(ARDUINO) && ARDUINO >= 100 
	#include <Arduino.h>
	#include <Wire.h>
#elif defined(PARTICLE)
	#include <Particle.h>
#endif

#define ADR_BASE 0x48
#define TEMP_REG 0x00
#define TEMP_ALERT_LOW 0x02
#define TEMP_ALERT_HIGH 0x03
#define CONFIG_REG 0x01
#define ID_REG 0x0F

#define TEMP_LSB 0.0625

#define CONFIG_DEFAULT 0x7800 //Low conversion rate, 4 faults for alert, alert active low, alert in comparator mode, continuious conversion 

class TMP1075
{
	public:
		TMP1075(uint8_t ADR_ = ADR_BASE); //Default to base ADR
		uint8_t begin();
		uint8_t Sleep(bool State);
		float GetTemp();
		void SetAlert(float AlertTempHigh, float AlertTempLow);

	private:
		uint8_t WriteWord(uint8_t Reg, int16_t Data, uint8_t Adr); 
		uint16_t ReadWord(uint8_t Reg, uint8_t Adr); 
		int ADR = 0x48; //Default address
};

#endif