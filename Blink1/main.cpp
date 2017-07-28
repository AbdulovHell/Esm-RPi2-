#include <wiringPi.h>
//#include <wiringPiSPI.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "sock_serv.h"

#include "SPI_DRV.h"

#include <wiringSerial.h>

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
	
	int fd = serialOpen("/dev/ttyAMA0", 9600);
	if (fd == -1)
		return 0;
	/*if (SPISetup(0, 625000, 0) == -1) {
		printf("error init spi");
		return 1;
	}*/
	/*char* rBuf=new char[25];
	char* wBuf=new char[25];
	memset(wBuf,0xf5,25);
	memset(rBuf, '0', 25);
	wBuf[0] = 0xF5;*/
	//int fd = lcdInit(16, 2, 4, RS, E, 0, 0, 0, 0, DB4, DB5, DB6, DB7);
	//lcdPrintf(fd, "message");
	int read_cnt=0;
	while (true)
	{
		read_cnt=serialDataAvail(fd);
		if (read_cnt > 0) {
			char* buf = new char[read_cnt+1];
			for (int i = 0; i < read_cnt; i++)
				buf[i]=serialGetchar(fd);
			buf[read_cnt] = 0;
			serialPuts(fd, buf);
		}
		//printf("input ");
		//scanf("%s",buf);
		//sprintf(wBuf, "asdasdjhakhw");
		//if (wBuf[0] == '0') {
		//	break;
		//}
		//else {
		/*int ret = SPIDataRW(0, (unsigned char*)rBuf, (unsigned char*)wBuf, 25);
		uint32_t sym = 0;
		for (int i = 0; i < 25; i++)
			sym += rBuf[i];
		if (sym > 0)
			printf("%s\n", rBuf);
		*///}
	}
	return 0;
}