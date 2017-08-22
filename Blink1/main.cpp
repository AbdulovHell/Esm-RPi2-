#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <iostream>

#include "threading.h"
// Контакт LED — контакт 0 wiringPi равен BCM_GPIO 17.
// При инициализации с использованием wiringPiSetupSys нужно применять нумерацию BCM
// При выборе другого ПИН-кода используйте нумерацию BCM, также
// обновите команду "Страницы свойств" — "События сборки" — "Удаленное событие после сборки". 
// , которая использует gpio export для настройки значений для wiringPiSetupSys

//using namespace Threading;

int main(int argc, char* argv[])
{
	int channel = 6;
	int att = 0;
	int port = 4550;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			printf("Esminec main programm, possible params:\n-h	-	help message\n-p <number>	-	set TCP server port number(200...50000)\n-ch <number>	-	set initial channel value(1...20)\n-at <number>	-	set initial attenuation value(0...35)\n\n");
			return 0;
		}
		else if (strcmp(argv[i], "-p") == 0) {
			i++;
			int tempPort = strtol(argv[i], NULL, 10);
			if (tempPort > 200 && tempPort < 50000)
				port = tempPort;
			else
				printf("Invalid port number \"%d\" (200...50000), default 4550\n", tempPort);

		}
		else if (strcmp(argv[i], "-ch") == 0) {
			i++;
			int tempChannel = strtol(argv[i], NULL, 10);
			if (tempChannel < 1 || tempChannel > 20/*?*/) {
				printf("Invalid channel number \"%d\" (1...20), default 6\n", tempChannel);
			}
			else {
				channel = tempChannel;
			}
		}
		else if (strcmp(argv[i], "-at") == 0) {
			i++;
			int tempAtt = strtol(argv[i], NULL, 10);
			if (tempAtt < 0 || tempAtt > 35/*?*/) {
				printf("Invalid attenuation number \"%d\" (0...35), default 0\n", tempAtt);
			}
			else {
				att = tempAtt;
			}
		}
	}
	Threading::TCPServerThread* thrd;
	thrd = new Threading::TCPServerThread(port);
	
	printf("Initial setup... Channel:%d Att:%d\n", channel, att);
	//system("gpio export 27 output && gpio export 17 output && gpio export 22 output && gpio export 26 output && gpio export 19 output && gpio export 13 output && gpio export 6 output");
	//system("gpio load spi");
	//int ec = wiringPiSetupSys();
	//digitalWrite(RW, LOW);

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
	while (true)
	{
		//if (outStrs.size()) {
		//	//pause threads
		//	for(int i=0;i<outStrs.size();i++)
		//		cout << outStrs[i];
		//	outStrs.clear();
		//	//resume
		//}
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