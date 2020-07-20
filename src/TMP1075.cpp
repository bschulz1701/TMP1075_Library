#include <TMP1075.h>

TMP1075::TMP1075(uint8_t ADR_) 
{
	ADR = ADR_; //Copy address to local
}

uint8_t TMP1075::begin()
{
	//Only use isEnabled() if using particle
	#if defined(ARDUINO) && ARDUINO >= 100 
		Wire.begin();
	#elif defined(PARTICLE)
		if(!Wire.isEnabled()) Wire.begin(); //Only initialize I2C if not done already //INCLUDE FOR USE WITH PARTICLE 
	#endif
	uint8_t Error = WriteWord(CONFIG_REG, CONFIG_DEFAULT, ADR); //Set default values
	if(ReadWord(ID_REG, ADR) != 0x7500 && Error == 0) return 5; //If not equal to device ID, return "other error", meaning device present is not the expected one 
	else return Error; 
}

float TMP1075::GetTemp() 
{
	int16_t Temp = ReadWord(TEMP_REG, ADR);
	// Serial.println(Temp, HEX);
	if((Temp && 0x000F) != 0x00) { //Make sure lower block is 0 to make sure not an error
		Temp = (Temp >> 4); //Shift out blank bits  
		// Serial.println(Temp, HEX);
		return Temp*TEMP_LSB;
	}
	else return -9999.0; //Return error value otherwise 
}

uint16_t TMP1075::ReadWord(uint8_t Reg, uint8_t Adr)  //Send command value, returns entire 16 bit word
{
	Wire.beginTransmission(Adr);
	Wire.write(Reg);
	uint8_t Error = Wire.endTransmission(); //Store Error

	Wire.requestFrom(Adr, 2);
	uint8_t ByteHigh = Wire.read();  //Read in high and low bytes (big endian)
	uint8_t ByteLow = Wire.read();

	return ((ByteHigh << 8) | ByteLow); //DEBUG!
}

uint8_t TMP1075::WriteWord(uint8_t Reg, int16_t Data, uint8_t Adr)
{
	Wire.beginTransmission(Adr);
	Wire.write(Reg);
	Wire.write(Data >> 8); //Write high byte
	Wire.write(Data & 0xFF); //Write low byte 
	return Wire.endTransmission();
}

void TMP1075::SetAlert(float AlertTempHigh, float AlertTempLow)
{
	int16_t AlertValHigh = 0; //Used for converted alert temp
	int16_t AlertValLow = 0;

	AlertValHigh = floor(AlertTempHigh/TEMP_LSB); //Find high bit value
	AlertValLow = ceil(AlertTempLow/TEMP_LSB); //Find low bit value

	AlertValHigh = AlertValHigh << 4; //Shift up to match format
	AlertValLow = AlertValLow << 4;
	if(AlertValHigh > 0xEFF0) AlertValHigh = 0xEFF0; //IF beyond range, set to max (off)
	if(AlertValLow > 0x8000) AlertValLow = 0x8000; //If beyond range, set to min (off)

	Serial.print(AlertValHigh, HEX); //DEBUG!
	Serial.print("\t"); //DEBUG!
	Serial.println(AlertValLow, HEX); //DEBUG!
	WriteWord(TEMP_ALERT_HIGH, AlertValHigh, ADR); //Set high threshold register 
	WriteWord(TEMP_ALERT_LOW, AlertValLow, ADR); //Set low threshold register 
	WriteWord(CONFIG_REG, CONFIG_DEFAULT, ADR); //Set default configuration to alert 
}

uint8_t TMP1075::Sleep(bool State)
{
	uint16_t TempConfig = ReadWord(CONFIG_REG, ADR); //Read config reg in
	if(State) TempConfig = TempConfig | 0x0100; //Set sleep bit
	if(!State) TempConfig = TempConfig & 0xFEFF; //Clear sleep bit
	return WriteWord(CONFIG_REG, TempConfig, ADR); //Write value back 
}