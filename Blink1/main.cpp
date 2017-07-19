#include <wiringPi.h>
#include <lcd.h>

#include <stdlib.h>
#include <sys/socket.h>


// Контакт LED — контакт 0 wiringPi равен BCM_GPIO 17.
// При инициализации с использованием wiringPiSetupSys нужно применять нумерацию BCM
// При выборе другого ПИН-кода используйте нумерацию BCM, также
// обновите команду "Страницы свойств" — "События сборки" — "Удаленное событие после сборки". 
// , которая использует gpio export для настройки значений для wiringPiSetupSys
#define RW 27
#define RS 17
#define E 22

#define DB7 26
#define DB6 19
#define DB5 13
#define DB4 6

int main(void)
{
	int sock = socket(AF_INET, SOCK_STREAM,0);
	sockaddr sa;
	sa.sa_family = ;
	int b = bind(sock,&sa,sizeof(sockaddr));
	int l=listen(sock, 3);
	system("gpio export 27 output && gpio export 17 output && gpio export 22 output && gpio export 26 output && gpio export 19 output && gpio export 13 output && gpio export 6 output");
	int ec = wiringPiSetupSys();
	digitalWrite(RW, LOW);
	int fd = lcdInit(16, 2, 4, RS, E, 0, 0, 0, 0, DB4, DB5, DB6, DB7);
	lcdPrintf(fd, "message");
	while (true)
	{
		ec++;
	}
	return 0;
}