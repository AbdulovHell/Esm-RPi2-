#include "sys_headers.h"
#define _DEBUG
#include "Task.h"
#include "DisplayControl.h"
#include "threading.h"
#include "usart.h"
#include "I2C.h"
#include "SPI.h"
#include "Colorize.h"
#include "stuff.h"
#include "main.h"
#include "input.h"
#include "TCP.h"
#include "IPChanger.h"
#include "settings.h"
//#include "LCD.h"
// Контакт LED — контакт 0 wiringPi равен BCM_GPIO 17.
// При инициализации с использованием wiringPiSetupSys нужно применять нумерацию BCM
// При выборе другого ПИН-кода используйте нумерацию BCM, также
// обновите команду "Страницы свойств" — "События сборки" — "Удаленное событие после сборки". 
// , которая использует gpio export для настройки значений для wiringPiSetupSys

//#define UART_TEST //успешно
//#define I2C_TEST	//успешно
//#define SPI_TEST	//

using namespace Threading;
using namespace Stuff;

namespace Threading {
	bool Working = true;
	TCPServerThread* thrd;
	LCDControlThread* lcdThrd;
	ButtonsInputThread* biThrd;
	vector<Threading::Task*> MainTasks;
	std::mutex* TasksMutex;



#ifdef _DEBUG
	ConsoleInputThread* ciThrd;
#endif

	void AddTask(Threading::Task * tsk)
	{
		TasksMutex->lock();
		MainTasks.push_back(tsk);
		TasksMutex->unlock();
	}
}

void InitShutdown() {
	Working = false;
#ifdef _DEBUG
	ciThrd->~ConsoleInputThread();
#endif
	biThrd->~ButtonsInputThread();
	thrd->~TCPServerThread();
	lcdThrd->~LCDControlThread();
	for (size_t i = 0; i < Listeners.size(); i++) {
		Listeners[i]->~TCPReciverThread();
	}
#ifdef _DEBUG
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%s: Threads stopped", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
	cout << buf << endl;
#endif
}

int main(int argc, char* argv[])
{
	int channel = 6;
	int att = 0;
	int port = 4550;

	wiringPiSetupGpio();

	Stuff::Storage = new Stuff::Settings();

	ListenersMutex = new mutex();
	TasksMutex = new mutex();
	thrd = new TCPServerThread(port);
	/*
#ifdef _DEBUG
	printf("%s: Start...\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str());
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
			if (tempChannel < 1 || tempChannel > 20) {
				printf("Invalid channel number \"%d\" (1...20), default 6\n", tempChannel);
			}
			else {
				channel = tempChannel;
			}
		}
		else if (strcmp(argv[i], "-at") == 0) {
			i++;
			int tempAtt = strtol(argv[i], NULL, 10);
			if (tempAtt < 0 || tempAtt > 35) {
				printf("Invalid attenuation number \"%d\" (0...35), default 0\n", tempAtt);
			}
			else {
				att = tempAtt;
			}
		}
	}
	printf("%s: Initial setup... Channel:%d Att:%d\n", Stuff::MakeColor("MAIN", Stuff::Green).c_str(), channel, att);
	ciThrd = new ConsoleInputThread();
	MainTasks.push_back(new TaskSetAttCh((uint8_t)att, (uint8_t)channel));
#endif
	*/
	MainTasks.push_back(new TaskSetFreq(Storage->GetFreq()));
	MainTasks.push_back(new TaskSetAtt(Storage->GetRFAtt(), Storage->GetIFAtt()));
	MainTasks.push_back(new TaskSetOutput(Storage->GetIF()));
	MainTasks.push_back(new TaskAdjustBL(Storage->GetBrightLvl()));
	MainTasks.push_back(new TaskChangeRef(Storage->GetRef()));
	//system("gpio export 27 output && gpio export 17 output && gpio export 22 output && gpio export 26 output && gpio export 19 output && gpio export 13 output && gpio export 6 output");
	//device tree enabled
	//system("gpio load spi");
	//system("gpio load i2c");

	

	//Threading::ReadIP();

	lcdThrd = new LCDControlThread();
	biThrd = new ButtonsInputThread();

#ifdef UART_TEST
	IO::Usart dev(9600);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;
	int size = 0;
#endif

#ifdef I2C_TEST
	IO::I2C dev(10);
	if (!dev.IsOpen())
		cout << dev.GetError() << endl;

	uint64_t cnt = 0;
#endif

#ifdef SPI_TEST
	IO::SPI dev(0, 10000);
#endif

	/*for (int i = 0x20; i < 0x28; i++) {
		IO::I2C dev(i);
		if (!dev.IsOpen())
			cout << dev.GetError() << endl;

		uint16_t dt=dev.Read2bytesReg(5);
		printf("Addr %x : %x\n",i,dt);
	}*/

	while (Working)
	{
		this_thread::sleep_for(std::chrono::microseconds(1));
#ifdef SPI_TEST
		uint8_t buf[2] = { 0x55,0xAA };
		//for (int i = 0; i < 2; i++)
		//	buf[i] = 10;
		dev.DataRW(buf, 2);
#endif
		TasksMutex->lock();
		int size = MainTasks.size();
		if (size > 0) {
			MainTasks[0]->Run();
			MainTasks.erase(MainTasks.begin());
		}
		TasksMutex->unlock();

		ListenersMutex->lock();
		vector<Threading::TCPReciverThread*>::iterator itRecv = Listeners.begin();
		for (size_t i = 0; i < Listeners.size(); i++, itRecv++) {
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
		uint8_t dt[3] = { 0x55,0xaa,0x55 };
		if (cnt % 500000 == 0) {
			//cout << "i2c w:" << dev.Write(dt, 3) << endl;
			dev.Write(dt, 3);
		}
		cnt++;
		/*int res = dev.Read();
		if (res != -1)
			cout << "i2c r:" << res << endl;*/
#endif

	}
	return 0;
}