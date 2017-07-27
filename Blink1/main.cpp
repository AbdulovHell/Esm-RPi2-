#include <wiringPi.h>
//#include <wiringPiSPI.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "sock_serv.h"

#include "SPI_DRV.h"

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

int main()
{
	pthread_t ServerThrd;
	pthread_create(&ServerThrd, NULL, SocketServer, NULL);

	system("gpio export 27 output && gpio export 17 output && gpio export 22 output && gpio export 26 output && gpio export 19 output && gpio export 13 output && gpio export 6 output");
	//system("gpio load spi");
	int ec = wiringPiSetupSys();
	digitalWrite(RW, LOW);

	if (SPISetup(0, 625000, 0) == -1) {
		printf("error init spi");
		return 1;
	}
	char* rBuf=new char[100];
	char* wBuf=new char[100];
	memset(wBuf,'0',100);
	memset(rBuf, '0', 100);
	wBuf[0] = '1';
	//int fd = lcdInit(16, 2, 4, RS, E, 0, 0, 0, 0, DB4, DB5, DB6, DB7);
	//lcdPrintf(fd, "message");
	while (true)
	{
		//printf("input ");
		//scanf("%s",buf);
		//sprintf(wBuf, "asdasdjhakhw");
		//if (wBuf[0] == '0') {
		//	break;
		//}
		//else {
		int ret = SPIDataRW(0, (unsigned char*)rBuf, (unsigned char*)wBuf, 100);
		uint32_t sym = 0;
		for (int i = 0; i < 100; i++)
			sym += rBuf[i];
		if (sym > 0)
			printf("%s\n", rBuf);
		//}
	}
	return 0;
}