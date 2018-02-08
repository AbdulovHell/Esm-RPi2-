#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <wchar.h>
#include <mutex>
#include "../Blink1/Colorize.h"
#include "threading.h"
#include "../src/stuff.h"
#include <list>
#include "LCD.h"
#include "Screen.h"

namespace Threading {
	vector<Threading::TCPReciverThread*> Listeners;
	vector<Task*> Tasks;
	std::mutex* ListenersMutex;
	std::mutex* TasksMutex;

	void * UserInputThread::ProceedInput(void * ptr_null)
	{
		char buf[512];
		printf("%s: Start working\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
		while (1) {
			scanf("%s", buf);
			if (strcmp(buf, "conn") == 0) {
				printf("Count: %d\n", Listeners.size());
			}
			if (strcmp(buf, "quit") == 0) {
				printf("Terminating threads...\n");
				//place shutdown task
				printf("%s: Place task -> Quit\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
				TasksMutex->lock();
				Tasks.push_back(new TaskQuit());
				TasksMutex->unlock();
			}
		}
	}

	UserInputThread::~UserInputThread()
	{
		pthread_cancel(threadHandle);
	}
	void * LCDControlThread::ManageLCD(void * ptr_null)
	{
		LCD::Display* disp = new LCD::Display();
		disp->Init();
		disp->Power(1, LCD::Display::Cursor::NoCursor_NoFlashing);
		//char buf[20] = { 'R','a','s' ,'p' ,'b' ,'e' ,'r' ,'r','y' ,0x20 ,'P' ,'i' ,0x20 ,0x01 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 };
		//memset(buf, 0x20, 20);
		//sprintf(buf, "<------->");
		//buf[strlen(buf)] = 0x20;

		LCD::CustomSymbol space = { 0,0,0,0,0,0,0,0 };
		disp->LoadSymbol(space, 0);
		LCD::CustomSymbol face = { 0b01110,0b10001,0b11011,0b10001,0b10101,0b10001,0b01110,0b00000 };
		disp->LoadSymbol(face, 1);

		LCD::Screen* main_menu = new LCD::Screen(disp);
		main_menu->AddLine(new LCD::LCDString(L"Главное меню", LCD::LCDString::Alignment::Center));
		main_menu->AddLine(new LCD::LCDString(L" Частота"));
		main_menu->AddLine(new LCD::LCDString(L" Канал", LCD::LCDString::Alignment::Left));
		main_menu->AddLine(new LCD::LCDString(L"Выкл.", LCD::LCDString::Alignment::Right));
		//disp.SetScreen(main_menu);

		//char text[] = "  1234567890  -+/*!?";
		//LCD::Screen scr;
		//scr.SetLine(0, buf);
		//scr.SetLine(1, text);
		//scr.SetLine(2, buf);
		//scr.SetLine(3, buf);
		//auto start = std::chrono::high_resolution_clock::now();
		//disp.SetScreen(scr);
		//auto end = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<double, std::milli> elapsed = end - start;
		//sprintf(text, "time, ms: %6.3f", elapsed.count());
		//scr.SetLine(3, text);
		//disp.SetScreen(scr);
		//std::this_thread::sleep_for(std::chrono::seconds(1));
		while (1) {
			//char temp = buf[19];
			//for (int i = 19; i > 0; i--)
			//	buf[i] = buf[i - 1];
			//buf[0] = temp;
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	LCDControlThread::~LCDControlThread()
	{
		pthread_cancel(threadHandle);
	}
}

using namespace Stuff;

Threading::Thread::Thread(void*(func)(void*), void* arg)
{
	pthread_create(&threadHandle, NULL, func, arg);
}

Threading::Thread::~Thread()
{
	pthread_cancel(threadHandle);
}

pthread_t Threading::Thread::GetThrdHandle()
{
	return threadHandle;
}

int Threading::Thread::Join()
{
	return pthread_join(threadHandle, NULL);
}

void* Threading::TCPServerThread::SocketServer(void * param)
{
	uint16_t port = (uint16_t)(int)param;
	int listener = 0;
	struct sockaddr_in addr;
	printf("%s: Server start on %d port\n", Stuff::MakeColor("SERVER", Stuff::Red).c_str(), port);
	listener = socket(AF_INET, SOCK_STREAM, 0);
	if (listener < 0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(2);
	}

	listen(listener, 5);
	int tempsock;

	while (1) {
		tempsock = accept(listener, NULL, NULL);
		if (tempsock < 0) {
			perror("accept");
			exit(3);
		}
		printf("%s: Accept connection\n", Stuff::MakeColor("SERVER", Stuff::Red).c_str());
		ListenersMutex->lock();
		Listeners.push_back(new TCPReciverThread(tempsock));
		ListenersMutex->unlock();
	}
}

Threading::TCPServerThread::~TCPServerThread()
{
	pthread_cancel(threadHandle);
}

void * Threading::TCPReciverThread::Recive(void * param)
{
	RTData* data = (RTData*)param;
	int reciver = data->iSocket;
	bool* stopflag = data->bStopFlag;
	char buf[1024];
	int bytes_read;
	printf("%s(%d): Start working\n", Stuff::MakeColor("RECV", Stuff::Yellow).c_str(), reciver);
	while (1) {
		bytes_read = recv(reciver, buf, 1024, 0);
		if (bytes_read <= 0) break;
		//проверка
		if (!Verify(buf)) break;
		//обработка
		switch (buf[1]) {	//type id
		case 1:
		{
			buf[0] = 'e';
			buf[1] = 1;
			buf[2] = 2;
			buf[3] = 0;
			buf[4] = 'O';
			buf[5] = 'K';
			CalcSum(buf, 8);
			send(reciver, buf, 1024, 0);
		}
		break;
		case 2:
		{
			printf("%s(%d): Place task -> Set ch %d att %d\n", Stuff::MakeColor("RECV", Stuff::Yellow).c_str(), reciver, buf[4], buf[5]);
			TasksMutex->lock();
			Tasks.push_back(new TaskSetAttCh(buf[5], buf[4]));
			TasksMutex->unlock();
		}
		break;
		default:
			break;
		}
	}
	printf("%s(%d): Stop working\n", Stuff::MakeColor("RECV", Stuff::Yellow).c_str(), reciver);
	*stopflag = true;
	close(reciver);
	return 0;
}

Threading::TCPReciverThread::~TCPReciverThread()
{
}
