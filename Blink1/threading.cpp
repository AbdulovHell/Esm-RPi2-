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
#include <list>
#include <thread>

#include "Snake.h"
#include "cg.h"
#include "Floodgate.h"
#include "Colorize.h"
#include "../src/stuff.h"
#include "Task.h"
#include "LCD.h"
#include "threading.h"
#include "Screen.h"

namespace Threading {
	vector<Threading::TCPReciverThread*> Listeners;
	vector<Threading::Task*> MainTasks;
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
				MainTasks.push_back(new TaskQuit());
				TasksMutex->unlock();
			}
			if (strcmp(buf, "pkey") == 0) {
				//simulating phys keys
				//printf("%s: Send key press\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
				EmulatingKeyboard();
			}
		}
	}

	UserInputThread::~UserInputThread()
	{
		pthread_cancel(threadHandle);
	}

	void * LCDControlThread::ManageLCD(void * ptr_null)
	{
		using namespace LCD;
		bool SecretMenu = false;
		int size;
		LCD::Display* disp = new LCD::Display();
		disp->Init();
		disp->Power(1, LCD::Display::Cursor::NoCursor_NoFlashing);

		//LCD::CustomSymbol face = { 0b01110,0b10001,0b11011,0b10001,0b10101,0b10001,0b01110,0b00000 };
		//disp->LoadSymbol(face, 1);

		LCD::Screen* main_menu = new LCD::Screen(disp);
		main_menu->AddLine(new LCD::LCDString(L"Главное меню", LCD::LCDString::Alignment::Center));
		main_menu->AddLine(new LCD::LCDString(L" Частота и атт."));
		main_menu->AddLine(new LCD::LCDString(L" Каналы"));
		main_menu->AddLine(new LCD::LCDString(L" Статус"));
		main_menu->AddLine(new LCD::LCDString(L" Настройки"));
		main_menu->AddLine(new LCD::LCDString(L" Инструкция (?)"));
		main_menu->AddLine(new LCD::LCDString(L" Секретное меню"));
		main_menu->EnableMenu(1, 2);
		main_menu->SetActive();

		while (1) {

			LCD::ScreenMutex.lock();
			size = LCDTasks.size();
			if (size > 0) {
				switch (LCDTasks[0]->GetType())
				{
				case TaskType::KeyUp:
					main_menu->ScrollMenu(-1);
					break;
				case TaskType::KeyDown:
					main_menu->ScrollMenu(1);
					break;
				case TaskType::KeyMid:
					if (main_menu->GetSelectedIndex() == 7)
						SecretMenu = true;
					break;
				default:
					break;
				}
				LCDTasks.erase(LCDTasks.begin());
			}
			LCD::ScreenMutex.unlock();

			if (SecretMenu) {
				LaunchSecretMenu(disp);
				main_menu->SetActive();
				SecretMenu = false;
			}
		}
	}

	LCDControlThread::~LCDControlThread()
	{
		pthread_cancel(threadHandle);
	}

	int LaunchSecretMenu(LCD::Display * disp)
	{
		LCD::Screen* menu = new LCD::Screen(disp);
		LCD::Screen* track = new LCD::Screen(disp);

		menu->AddLine(new LCD::LCDString(L"Секретное меню", LCD::LCDString::Alignment::Center));
		menu->AddLine(new LCD::LCDString(L" Самоуничтожение"));
		menu->AddLine(new LCD::LCDString(L" Гонки"));
		menu->AddLine(new LCD::LCDString(L" Змейка"));
		menu->AddLine(new LCD::LCDString(L" Начальник идет!"));

		char border[] = { 0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0,0xA0 };
		char tr1[20] = { 0x20, 0x90,0x0 };
		char tr2[20] = { 0x20 ,0x0 };
		track->AddLine(new LCD::LCDString(border));
		track->AddLine(new LCD::LCDString(tr1));
		track->AddLine(new LCD::LCDString(tr2));
		track->AddLine(new LCD::LCDString(border));

		menu->EnableMenu(1, 2);
		menu->SetActive();

		int size;
		bool snake = false;
		while (1) {
			LCD::ScreenMutex.lock();
			size = LCD::LCDTasks.size();
			if (size > 0) {
				switch (LCD::LCDTasks[0]->GetType())
				{
				case TaskType::KeyUp:
					menu->ScrollMenu(-1);
					break;
				case TaskType::KeyDown:
					menu->ScrollMenu(1);
					break;
				case TaskType::KeyMid:
					switch (menu->GetSelectedIndex()) {
					case 2:
						Selfdestruction(disp);
						menu->SetActive();
						break;
					case 3:

						break;
					case 4:
						snake = true;
						break;
					case 5:
						LCD::LCDTasks.erase(LCD::LCDTasks.begin());
						LCD::ScreenMutex.unlock();
						return 0;
					default:
						break;
					}
					break;
				default:
					break;
				}
				LCD::LCDTasks.erase(LCD::LCDTasks.begin());
			}
			LCD::ScreenMutex.unlock();

			if (snake) {
				SnakeGame(disp);
				menu->SetActive();
				snake = false;
			}
		}

		return 0;
	}

	void EmulatingKeyboard()
	{
		while (1) {
			int ch = Stuff::Conio::getch();
			LCD::ScreenMutex.lock();
			switch (ch)
			{
			case 'w':
				LCD::LCDTasks.push_back(new TaskKeyUp());
				break;
			case 'a':
				LCD::LCDTasks.push_back(new TaskKeyLeft());
				break;
			case 's':
				LCD::LCDTasks.push_back(new TaskKeyDown());
				break;
			case 'd':
				LCD::LCDTasks.push_back(new TaskKeyRight());
				break;
			case 'f':
				LCD::LCDTasks.push_back(new TaskKeyMid());
				break;
			case 'q':
				return;
			default:
				break;
			}
			LCD::ScreenMutex.unlock();
		}
	}

	void Selfdestruction(LCD::Display * disp)
	{
		disp->Power(1, LCD::Display::Cursor::NoCursor_NoFlashing);
		LCD::Screen* scr = new LCD::Screen(disp);
		scr->AddLine(new LCD::LCDString(L"Обратный отсчет:", LCD::LCDString::Alignment::Center));
		scr->AddLine(new LCD::LCDString(L""));
		scr->AddLine(new LCD::LCDString(L"5с", LCD::LCDString::Alignment::Center));
		scr->AddLine(new LCD::LCDString(L""));
		scr->SetActive();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		scr->SetLine(new LCD::LCDString(L"4с", LCD::LCDString::Alignment::Center), 2);
		scr->SetActive();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		int rnd = 100;
		char buf[20];
		srand(time(NULL));
		int cnt = 0;
		while (1) {
			rnd = Stuff::RangeRand(0, 10);
			sprintf(buf, "%dc", rnd);
			cnt++;
			if (!rnd) {
				scr->SetLine(new LCD::LCDString(buf, LCD::LCDString::Alignment::Center), 1);
				scr->SetLine(new LCD::LCDString(L"Вы погибли", LCD::LCDString::Alignment::Center), 2);
				wchar_t txt[20];
				swprintf(txt, 20, L"c %d попытки", cnt);
				scr->SetLine(new LCD::LCDString(txt, LCD::LCDString::Alignment::Center), 3);
				scr->SetActive();
				std::this_thread::sleep_for(std::chrono::seconds(5));
				return;
			}
			else {
				scr->SetLine(new LCD::LCDString(buf, LCD::LCDString::Alignment::Center), 2);
				scr->SetActive();
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	void SnakeGame(LCD::Display * disp)
	{
#define HEAD 0xE5
#define BODY 0xEE
#define BODY_M 0x23
#define EAT 0xB2
#define SPACE 0x20
#define D_UP 1
#define D_RIGHT 2
#define D_DOWN 3
#define D_LEFT 4

		char table[4][20];
		//bool Start = true;
		bool Eating = false;

		LCD::Screen* scr = new LCD::Screen(disp);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 20; j++) {
				table[i][j] = SPACE;
			}
		}
		srand(time(NULL));

		int Direction = 2;
		int Size = 1;

		Stuff::SCoord head, bodypart;
		head.x = 3;
		head.y = 1;
		bodypart.x = 4;
		bodypart.x = 1;
		Stuff::Snake* snake = new Stuff::Snake(&head, &bodypart);

		table[head.y][head.x] = BODY;
		table[bodypart.y][bodypart.x] = HEAD;

		int eatX, eatY;
		do {
			eatX = Stuff::RangeRand(0, 19);
			eatY = Stuff::RangeRand(0, 3);
			if (table[eatY][eatX] == SPACE) {
				table[eatY][eatX] = EAT;
				break;
			}
		} while (1);

		for (int i = 0; i < 4; i++)
			scr->AddLine(new LCD::LCDString(table[i]));
		scr->SetActive();
		disp->Power(1, LCD::Display::Cursor::NoCursor_NoFlashing);

		printf("start	");	//и гдето дальше вылет
		while (1) {
			//обрабатываем нажатие клавиш
			LCD::ScreenMutex.lock();
			int size = LCD::LCDTasks.size();
			if (size > 0) {
				switch (LCD::LCDTasks[0]->GetType())
				{
				case TaskType::KeyUp:
					if (Direction != D_DOWN)
						Direction = D_UP;
					break;
				case TaskType::KeyDown:
					if (Direction != D_UP)
						Direction = D_DOWN;
					break;
				case TaskType::KeyLeft:
					if (Direction != D_RIGHT)
						Direction = D_RIGHT;
					break;
				case TaskType::KeyRight:
					if (Direction != D_LEFT)
						Direction = D_RIGHT;
					break;
				case TaskType::KeyMid:
					break;
				default:
					break;
				}
				LCD::LCDTasks.erase(LCD::LCDTasks.begin());
			}
			LCD::ScreenMutex.unlock();
			///двигаемся
			//находим голову
			int hX = -1, hY = -1;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 20; j++) {
					if (table[i][j] == HEAD) {
						hX = j;
						hY = i;
					}
				}
			}
			//двигаем голову по направлению движения
			switch (Direction)
			{
			case D_LEFT:
				hX--;
				if (hX < 0) {
					return;
				}
				break;
			case D_RIGHT:
				hX++;
				if (hX > 19) {
					return;
				}
				break;
			case D_UP:
				hY--;
				if (hY < 0) {
					return;
				}
				break;
			case D_DOWN:
				hY++;
				if (hY > 3) {
					return;
				}
				break;
			default:
				break;
			}
			//проверяем наличие хавки
			if (table[hY][hX] == EAT) {
				Size++;
				Eating = true;
			}
			//подтягиваем хвост
			if (Eating) {
				snake->MoveAndGrow(hX, hY);
			}
			else {
				snake->Move(hX, hY);
			}
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 20; j++)
					if (table[i][j] == HEAD || table[i][j] == BODY)
						table[i][j] = SPACE;
			for (int i = 0; i < Size+1; i++) {
				Stuff::SCoord temp = snake->Part(i);
				//голова
				if (i == 0) {
					table[temp.y][temp.x] = HEAD;
				}//тело
				else {
					table[temp.y][temp.x] = BODY;
				}
			}
			//генерируем новую хавку, если старую сьели
			if (Eating) {
				do {
					eatX = Stuff::RangeRand(0, 19);
					eatY = Stuff::RangeRand(0, 3);
					if (table[eatY][eatX] == SPACE) {
						table[eatY][eatX] = EAT;
						break;
					}
				} while (1);
				Eating = false;
			}
		}
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
			MainTasks.push_back(new TaskSetAttCh(buf[5], buf[4]));
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
	pthread_cancel(threadHandle);
}
