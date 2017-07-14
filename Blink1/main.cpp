#include <wiringPi.h>
#include "lcd.h"

// Контакт LED — контакт 0 wiringPi равен BCM_GPIO 17.
// При инициализации с использованием wiringPiSetupSys нужно применять нумерацию BCM
// При выборе другого ПИН-кода используйте нумерацию BCM, также
// обновите команду "Страницы свойств" — "События сборки" — "Удаленное событие после сборки". 
// , которая использует gpio export для настройки значений для wiringPiSetupSys
#define	LED	17

int main(void)
{
	wiringPiSetupSys();

	//pinMode(LED, OUTPUT);
	InitLCD();

	while (true)
	{
		ClearDisplay();
		//digitalWrite(LED, HIGH);  // Вкл.
		//delay(100); // мс
		//digitalWrite(LED, LOW);	  // Откл.
		//delay(100);
	}
	return 0;
}