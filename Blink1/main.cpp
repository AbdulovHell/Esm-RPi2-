#include <wiringPi.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <unistd.h>

#include "threading.h"
#include "usart.h"
#include "I2C.h"
// Контакт LED — контакт 0 wiringPi равен BCM_GPIO 17.
// При инициализации с использованием wiringPiSetupSys нужно применять нумерацию BCM
// При выборе другого ПИН-кода используйте нумерацию BCM, также
// обновите команду "Страницы свойств" — "События сборки" — "Удаленное событие после сборки". 
// , которая использует gpio export для настройки значений для wiringPiSetupSys

//#define UART_TEST //успешно
#define I2C_TEST	//успешно
//#define SPI_TEST	//

using namespace Threading;

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
	ListenersMutex = new mutex();
	TasksMutex = new mutex();
	TCPServerThread* thrd;
	thrd = new TCPServerThread(port);

	printf("MAIN: Initial setup... Channel:%d Att:%d\n", channel, att);
	Tasks.push_back(new Task(att, channel));
	system("gpio export 27 output && gpio export 17 output && gpio export 22 output && gpio export 26 output && gpio export 19 output && gpio export 13 output && gpio export 6 output");
	//device tree enabled
	//system("gpio load spi");
	//system("gpio load i2c");
	int ec = wiringPiSetupSys();

	UserInputThread* inThrd;
	inThrd = new UserInputThread();

#ifdef UART_TEST
	IO::Usart dev(9600);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;
	int size = 0;
#endif

#ifdef I2C_TEST
	IO::I2C dev(0x4b);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;

	uint64_t cnt = 0;
#endif

	while (true)
	{
		TasksMutex->lock();
		int size = Tasks.size();
		if (size > 0) {
			char buf[128];
			memset(buf, 0, 128);
			sprintf(buf, "MAIN: Set channel: %d Att: %d", Tasks[0]->Ch(), Tasks[0]->Att());
			cout << buf << endl;
			Tasks.erase(Tasks.begin());
		}
		TasksMutex->unlock();

		ListenersMutex->lock();
		vector<Threading::TCPReciverThread*>::iterator itRecv = Listeners.begin();
		for (int i = 0; i < Listeners.size(); i++, itRecv++) {
			if (Listeners[i]->Stoped) {
				Listeners.erase(itRecv);
			}
		}
		ListenersMutex->unlock();

#ifdef UART_TEST
		//char* buf = new char[32];
		//memset(buf, 0, 32);
		//sprintf(buf, "Hello world!\n");
		//dev.Write(buf, strlen(buf));
		if (size = dev.DataAvail() > 0) {
			char* rBuf = new char[size];
			dev.Read(rBuf, size);
			cout << rBuf << endl;

			dev.Write(rBuf, size);
			delete[] rBuf;
		}
#endif

#ifdef I2C_TEST
		//int data = dev.Read();
		uint8_t dt[6] = { cnt % 2,cnt % 3,cnt % 5,cnt % 7,cnt % 11,cnt % 13 };
		if (cnt % 5000000 == 0) {
			dev.Write(dt, 6);
		}
		cnt++;
#endif

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
		//}
	}
	return 0;
}