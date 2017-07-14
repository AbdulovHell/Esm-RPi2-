#include "lcd.h"
#include <wiringPi.h>

void InitLCD()
{
	pinMode(RW, OUTPUT);
	pinMode(RS, OUTPUT);
	pinMode(E, OUTPUT);

	pinMode(DB7, OUTPUT);
	pinMode(DB6, OUTPUT);
	pinMode(DB5, OUTPUT);
	pinMode(DB4, OUTPUT);

	digitalWrite(DB7, LOW);
	digitalWrite(DB6, LOW);
	digitalWrite(DB5, LOW);
	digitalWrite(DB4, LOW);


	digitalWrite(RS, HIGH);
	digitalWrite(RW, LOW);
	digitalWrite(E, LOW);
}

void ClearDisplay()
{
	digitalWrite(RS, LOW);
	digitalWrite(RW, LOW);
	delay(1);
	digitalWrite(RS, HIGH);
	delay(1);
	digitalWrite(E, HIGH);
	delay(1);
	
}
