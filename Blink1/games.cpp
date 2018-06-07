#include "sys_headers.h"

#include "DisplayControl.h"
#include "LCD.h"

#include "stuff.h"
#include "Snake.h"
#include "Tetris.h"

#define PART 0x14
#define E_PART1 0x89 
#define E_PART2 0x99
#define WALL 0x0D
#define ROAD 0x20

#define HEAD 0xE5
#define BODY 0xEE
#define EAT 0xB2
#define SPACE 0x20
#define D_UP 1
#define D_RIGHT 2
#define D_DOWN 3
#define D_LEFT 4

#include "games.h"
#include "Colorize.h"

namespace Games {
	using namespace std;
	using namespace Display;
	using namespace Stuff;

	void LaunchSecretMenu(Display::Display * disp, uint32_t param)
	{
		//printf("%s: Secter menu founded\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str());
		Display::Screen* menu = new Display::Screen(disp);
		menu->AddLine(new Display::DisplayString(L"Секретное меню", Display::DisplayString::Alignment::Center));
		menu->AddLine(new Display::DisplayString(L" Самоуничтожение", Selfdestruction));
		menu->AddLine(new Display::DisplayString(L" Гонки", RacingMenu));
		menu->AddLine(new Display::DisplayString(L" Змейка", SnakeMenu));
		menu->AddLine(new Display::DisplayString(L" Тетрис", TetrisGame));
		menu->AddLine(new Display::DisplayString(L" Выход", [menu](Display::Display* d, uint32_t p) { menu->ReturnToPrevMenu(d,p); }));
		menu->EnableMenu(1, 2);
		menu->SetActive();
	}

	void Selfdestruction(Display::Display * disp, uint32_t param)
	{
		disp->SetCursorType(Display::Display::Cursor::NoCursor_NoFlashing);
		Display::Screen* scr = new Display::Screen(disp);
		scr->AddLine(new Display::DisplayString(L"Обратный отсчет:", Display::DisplayString::Alignment::Center));
		scr->AddLine(new Display::DisplayString(L""));
		scr->AddLine(new Display::DisplayString(L"5с", Display::DisplayString::Alignment::Center));
		scr->AddLine(new Display::DisplayString(L""));
		scr->UpdateDisplay();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		scr->SetLine(new Display::DisplayString(L"4с", Display::DisplayString::Alignment::Center), 2);
		scr->UpdateDisplay();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		int rnd = 100;
		char buf[20];
		srand(time(NULL));
		int cnt = 0;
		while (1) {
			rnd = Stuff::RangeRand(0, 9);
			sprintf(buf, "%dc", rnd);
			cnt++;
			if (!rnd) {
				scr->SetLine(new Display::DisplayString(buf, Display::DisplayString::Alignment::Center), 0);
				scr->SetLine(new Display::DisplayString(L"Вы погибли", Display::DisplayString::Alignment::Center), 1);
				wchar_t txt[20];
				swprintf(txt, 20, L"c %d попытки", cnt);
				scr->SetLine(new Display::DisplayString(txt, Display::DisplayString::Alignment::Center), 2);
				if (cnt > Stuff::RecordsStorage->GetSelfdestructRecord()) {
					Stuff::RecordsStorage->SetSelfdestructRecord(cnt);
					scr->SetLine(new Display::DisplayString(L"Новый рекорд", Display::DisplayString::Alignment::Center), 3);
				}
				else {
					swprintf(txt, 20, L"Рекорд: %d", Stuff::RecordsStorage->GetSelfdestructRecord());
					scr->SetLine(new Display::DisplayString(txt, Display::DisplayString::Alignment::Center), 3);
				}
				scr->UpdateDisplay();
				std::this_thread::sleep_for(std::chrono::seconds(5));
				return;
			}
			else {
				scr->SetLine(new Display::DisplayString(buf, Display::DisplayString::Alignment::Center), 2);
				scr->UpdateDisplay();
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	void SnakeMenu(Display::Display * disp, uint32_t param)
	{
		int size, speed = 5;
		bool start = false, isBordless = true;
		wchar_t spdtext[20] = L" Скорость: 0";
		spdtext[11] = speed + 48;

		disp->Power(1, Display::Display::Cursor::NoCursor_SymbolFlashing);
		Display::Screen* scr = new Display::Screen(disp);
		scr->AddLine(new Display::DisplayString(L" Старт"));
		scr->AddLine(new Display::DisplayString(spdtext));
		scr->AddLine(new Display::DisplayString(L" Без границ: Д"));
		scr->AddLine(new Display::DisplayString(L" Выход"));
		scr->EnableMenu(0, 1);
		scr->UpdateDisplay();

		while (1) {
			Display::ScreenMutex.lock();
			size = Display::KeyEvents.size();
			if (size > 0) {
				switch (Display::KeyEvents[0]->eCode)
				{
				case EventCode::UpKeyPress:
					scr->ScrollMenu(-1);
					break;
				case EventCode::DownKeyPress:
					scr->ScrollMenu(1);
					break;
				case EventCode::MidKeyPress:
					switch (scr->GetSelectedIndex()) {
					case 1:
						start = true;
						break;
					case 2:
						speed++;
						if (speed > 9)
							speed = 1;
						spdtext[11] = speed + 48;
						scr->SetLine(new Display::DisplayString(spdtext), 1);
						scr->UpdateDisplay();
						break;
					case 3:
						isBordless = !isBordless;
						scr->SetLine(new Display::DisplayString((isBordless) ? L" Без границ: Д" : L" Без границ: Н"), 2);
						scr->UpdateDisplay();
						break;
					case 4:
						Display::KeyEvents.erase(Display::KeyEvents.begin());
						Display::ScreenMutex.unlock();
						return;
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
				Display::KeyEvents.erase(Display::KeyEvents.begin());
			}
			Display::ScreenMutex.unlock();

			if (start) {
				bool record = false;
				int res = SnakeGame(disp, isBordless, speed);
				if (res > RecordsStorage->GetSnakeRecord()) {
					RecordsStorage->SetSnakeRecord(res);
					record = true;
				}

				Display::Screen resscr(disp);
				resscr.AddLine(new Display::DisplayString(L"Конец", Display::DisplayString::Alignment::Center));
				wchar_t buf[20];
				swprintf(buf, 20, L"Очки: %d", res);
				resscr.AddLine(new Display::DisplayString(buf, Display::DisplayString::Alignment::Center));
				swprintf(buf, 20, L"Рекорд: %d", RecordsStorage->GetSnakeRecord());
				resscr.AddLine(new Display::DisplayString(buf, Display::DisplayString::Alignment::Center));
				if (record) {
					resscr.AddLine(new Display::DisplayString(L"Новый рекорд", Display::DisplayString::Alignment::Center));
				}
				else {
					resscr.AddLine(new Display::DisplayString(" "));
				}
				resscr.UpdateDisplay();
				this_thread::sleep_for(chrono::seconds(3));
				scr->UpdateDisplay();
				start = false;
			}
		}
	}

	int SnakeGame(Display::Display * disp, bool isBordless, int speed_ms)
	{
		speed_ms = (10 - speed_ms) * 50;

		char table[4][20];
		bool Start = true;
		bool Eating = false;

		Display::Screen* scr = new Display::Screen(disp);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 20; j++) {
				table[i][j] = SPACE;
			}
		}
		srand(time(NULL));

		int Direction = 0;
		int Size = 1;

		SCoord head, bodypart;
		head.x = 5;
		head.y = 1;
		bodypart.x = 5;
		bodypart.y = 1;
		Snake* snake = new Snake(&head, &bodypart);

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
			scr->AddLine(new Display::DisplayString(table[i]));
		scr->UpdateDisplay();
		disp->Power(1, Display::Display::Cursor::NoCursor_NoFlashing);

		while (1) {
			//обрабатываем нажатие клавиш
			do {
				Display::ScreenMutex.lock();
				int size = Display::KeyEvents.size();
				if (size > 0) {
					switch (Display::KeyEvents[0]->eCode)
					{
					case EventCode::UpKeyPress:
						if (Direction != D_DOWN)
							Direction = D_UP;
						Start = false;
						break;
					case EventCode::DownKeyPress:
						if (Direction != D_UP)
							Direction = D_DOWN;
						Start = false;
						break;
					case EventCode::LeftKeyPress:
						if (Direction != D_RIGHT)
							Direction = D_LEFT;
						Start = false;
						break;
					case EventCode::RightKeyPress:
						if (Direction != D_LEFT)
							Direction = D_RIGHT;
						Start = false;
						break;
					case EventCode::MidKeyPress:
						Display::KeyEvents.erase(Display::KeyEvents.begin());
						Display::ScreenMutex.unlock();
						return Size;
						break;
					default:
						break;
					}
					Display::KeyEvents.erase(Display::KeyEvents.begin());
				}
				Display::ScreenMutex.unlock();
			} while (Start);
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
				if (hX < 0 && !isBordless) {
					return Size;
				}
				else if (hX < 0 && isBordless) {
					hX = 19;
				}
				break;
			case D_RIGHT:
				hX++;
				if (hX > 19 && !isBordless) {
					return Size;
				}
				else if (hX > 19 && isBordless) {
					hX = 0;
				}
				break;
			case D_UP:
				hY--;
				if (hY < 0 && !isBordless) {
					return Size;
				}
				else if (hY < 0 && isBordless) {
					hY = 3;
				}
				break;
			case D_DOWN:
				hY++;
				if (hY > 3 && !isBordless) {
					return Size;
				}
				else if (hY > 3 && isBordless) {
					hY = 0;
				}
				break;
			default:
				break;
			}

			if (table[hY][hX] == BODY) {
				return Size;
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
			for (int i = 0; i < Size + 1; i++) {
				SCoord temp = snake->Part(i);
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
					if (table[eatY][eatX] == SPACE && eatY != hY) {
						table[eatY][eatX] = EAT;
						break;
					}
				} while (1);
				Eating = false;
			}
			//обновляем дисплей
			for (int i = 0; i < 4; i++)
				scr->SetLine(new Display::DisplayString(table[i]), i);
			scr->UpdateDisplay();
			this_thread::sleep_for(chrono::milliseconds(speed_ms));
		}
	}

	void RacingMenu(Display::Display * disp, uint32_t param)
	{
		int size, speed = 5;
		bool start = false;
		wchar_t spdtext[20] = L" Скорость: 0";
		spdtext[11] = speed + 48;

		disp->Power(1, Display::Display::Cursor::NoCursor_SymbolFlashing);
		Display::Screen* scr = new Display::Screen(disp);
		scr->AddLine(new Display::DisplayString(L" Старт"));
		scr->AddLine(new Display::DisplayString(spdtext));
		scr->AddLine(new Display::DisplayString(L" "));
		scr->AddLine(new Display::DisplayString(L" Выход"));
		scr->EnableMenu(0, 1);
		scr->UpdateDisplay();

		while (1) {
			Display::ScreenMutex.lock();
			size = Display::KeyEvents.size();
			if (size > 0) {
				switch (Display::KeyEvents[0]->eCode)
				{
				case EventCode::UpKeyPress:
					scr->ScrollMenu(-1);
					break;
				case EventCode::DownKeyPress:
					scr->ScrollMenu(1);
					break;
				case EventCode::MidKeyPress:
					switch (scr->GetSelectedIndex()) {
					case 1:
						start = true;
						break;
					case 2:
						speed++;
						if (speed > 9)
							speed = 1;
						spdtext[11] = speed + 48;
						scr->SetLine(new Display::DisplayString(spdtext), 1);
						scr->UpdateDisplay();
						break;
					case 3:
						break;
					case 4:
						Display::KeyEvents.erase(Display::KeyEvents.begin());
						Display::ScreenMutex.unlock();
						return;
						break;
					default:
						break;
					}
					break;
				default:
					break;
				}
				Display::KeyEvents.erase(Display::KeyEvents.begin());
			}
			Display::ScreenMutex.unlock();

			if (start) {
				bool record = false;
				int res = RacingGame2(disp, speed);
				if (res > RecordsStorage->GetRacingRecord()) {
					RecordsStorage->SetRacingRecord(res);
					record = true;
				}

				Display::Screen resscr(disp);
				resscr.AddLine(new Display::DisplayString(L"Конец", Display::DisplayString::Alignment::Center));
				wchar_t buf[20];
				swprintf(buf, 20, L"Очки: %d", res);
				resscr.AddLine(new Display::DisplayString(buf, Display::DisplayString::Alignment::Center));
				swprintf(buf, 20, L"Рекорд: %d", RecordsStorage->GetRacingRecord());
				resscr.AddLine(new Display::DisplayString(buf, Display::DisplayString::Alignment::Center));
				if (record) {
					resscr.AddLine(new Display::DisplayString(L"Новый рекорд", Display::DisplayString::Alignment::Center));
				}
				else {
					resscr.AddLine(new Display::DisplayString(" "));
				}
				resscr.UpdateDisplay();
				this_thread::sleep_for(chrono::seconds(3));
				scr->UpdateDisplay();
				start = false;
			}
		}
	}

	int RacingGame2(Display::Display * disp, int speed)
	{
		int speed_ms = (10 - speed) * 50;
		Display::Screen* track = new Display::Screen(disp);
		bool Start = true;
		int size;
		int Line = 0;
		int Score = 0;
		char border[20];
		for (int i = 0; i < 20; i++) border[i] = WALL;
		char** tr = new char*[2];
		tr[0] = new char[20];
		tr[1] = new char[20];
		memset(tr[0], ROAD, 20);
		memset(tr[1], ROAD, 20);
		tr[0][1] = PART;
		tr[0][2] = PART;

		disp->Power(1, Display::Display::Cursor::NoCursor_NoFlashing);
		PlaceCars(tr);

		track->AddLine(new Display::DisplayString(border));
		track->AddLine(new Display::DisplayString(tr[0]));
		track->AddLine(new Display::DisplayString(tr[1]));
		track->AddLine(new Display::DisplayString(border));
		track->UpdateDisplay();

		while (1) {
			do {
				Display::ScreenMutex.lock();
				size = Display::KeyEvents.size();
				if (size > 0) {
					switch (Display::KeyEvents[0]->eCode)
					{
					case EventCode::UpKeyPress:
						if (Line == 1) {
							tr[Line][1] = ROAD;
							tr[Line][2] = ROAD;
						}
						Line = 0;
						Start = false;
						break;
					case EventCode::DownKeyPress:
						if (Line == 0) {
							tr[Line][1] = ROAD;
							tr[Line][2] = ROAD;
						}
						Line = 1;
						Start = false;
						break;
					case EventCode::MidKeyPress:
						delete[] tr[0];
						delete[] tr[1];
						delete[] tr;
						Display::KeyEvents.erase(Display::KeyEvents.begin());
						Display::ScreenMutex.unlock();
						return Score;
						break;
					default:
						break;
					}
					Display::KeyEvents.erase(Display::KeyEvents.begin());
				}
				Display::ScreenMutex.unlock();
			} while (Start);
			if (tr[Line][1] == E_PART1 || tr[Line][2] == E_PART1 || tr[Line][1] == E_PART2 || tr[Line][2] == E_PART2) {
				delete[] tr[0];
				delete[] tr[1];
				delete[] tr;
				return Score;
			}

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 19; j++) {
					tr[i][j] = tr[i][j + 1];
				}
				tr[i][19] = ROAD;
			}

			if (tr[Line][1] == E_PART1 || tr[Line][2] == E_PART1 || tr[Line][1] == E_PART2 || tr[Line][2] == E_PART2) {
				delete[] tr[0];
				delete[] tr[1];
				delete[] tr;
				return Score;
			}

			tr[Line][0] = ROAD;
			tr[Line][1] = PART;
			tr[Line][2] = PART;

			PlaceCars(tr);

			track->SetLine(new Display::DisplayString(tr[0]), 1);
			track->SetLine(new Display::DisplayString(tr[1]), 2);
			track->UpdateDisplay();
			Score++;
			this_thread::sleep_for(chrono::milliseconds(speed_ms));
		}
		delete[] tr[0];
		delete[] tr[1];
		delete[] tr;
		return Score;
	}

	int RacingGame4(Display::Display * disp, int speed)
	{
		int speed_ms = (10 - speed) * 50;
		Display::Screen* track = new Display::Screen(disp);
		bool Start = true;
		int size;
		int Line = 1;
		int Score = 0;
		//char border[20];
		//for (int i = 0; i < 20; i++) border[i] = WALL;
		char** tr = new char*[4];
		tr[0] = new char[20];
		tr[1] = new char[20];
		tr[2] = new char[20];
		tr[3] = new char[20];
		memset(tr[0], ROAD, 20);
		memset(tr[1], ROAD, 20);
		memset(tr[2], ROAD, 20);
		memset(tr[3], ROAD, 20);
		tr[1][1] = PART;
		tr[1][2] = PART;

		disp->Power(1, Display::Display::Cursor::NoCursor_NoFlashing);
		PlaceCars(tr);

		track->AddLine(new Display::DisplayString(tr[0]));
		track->AddLine(new Display::DisplayString(tr[1]));
		track->AddLine(new Display::DisplayString(tr[2]));
		track->AddLine(new Display::DisplayString(tr[3]));
		track->SetActive();

		while (1) {
			do {
				Display::ScreenMutex.lock();
				size = Display::KeyEvents.size();
				if (size > 0) {
					switch (Display::KeyEvents[0]->eCode)
					{
					case EventCode::UpKeyPress:
						Line--;
						if (Line < 0) {
							tr[Line][1] = ROAD;
							tr[Line][2] = ROAD;
							Line = 0;
						}
						Start = false;
						break;
					case EventCode::DownKeyPress:
						Line++;
						if (Line > 3) {
							Line = 3;
						}
						Start = false;
						break;
					case EventCode::MidKeyPress:
						delete[] tr[0];
						delete[] tr[1];
						delete[] tr;
						Display::KeyEvents.erase(Display::KeyEvents.begin());
						Display::ScreenMutex.unlock();
						return Score;
						break;
					default:
						break;
					}
					Display::KeyEvents.erase(Display::KeyEvents.begin());
				}
				Display::ScreenMutex.unlock();
			} while (Start);

			if (tr[Line][1] == E_PART1 || tr[Line][2] == E_PART1 || tr[Line][1] == E_PART2 || tr[Line][2] == E_PART2) {
				delete[] tr[0];
				delete[] tr[1];
				delete[] tr;
				return Score;
			}

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 19; j++) {
					tr[i][j] = tr[i][j + 1];
				}
				tr[i][19] = ROAD;
			}

			if (tr[Line][1] == E_PART1 || tr[Line][2] == E_PART1 || tr[Line][1] == E_PART2 || tr[Line][2] == E_PART2) {
				delete[] tr[0];
				delete[] tr[1];
				delete[] tr;
				return Score;
			}

			tr[Line][0] = ROAD;
			tr[Line][1] = PART;
			tr[Line][2] = PART;

			PlaceCars(tr);

			track->SetLine(new Display::DisplayString(tr[0]), 1);
			track->SetLine(new Display::DisplayString(tr[1]), 2);
			track->SetActive();
			Score++;
			this_thread::sleep_for(chrono::milliseconds(speed_ms));
		}
		delete[] tr[0];
		delete[] tr[1];
		delete[] tr;
		return Score;
	}

	void PlaceCars(char** tracks)
	{
		srand(time(NULL));
		int empty = 0;
		for (int i = 2; i < 20; i++) {
			if (tracks[0][i] == ROAD && tracks[1][i] == ROAD) {
				empty++;
			}
			else {
				empty = 0;
			}
			if (empty > 3) {
				empty = 0;
				if (i < 19) {
					int track = Stuff::RangeRand(0, 1);
					tracks[track][i] = E_PART1;
					tracks[track][i + 1] = E_PART2;
					i++;
				}
			}
		}
	}
	void TetrisGame(Display::Display * disp, uint32_t param)
	{
		int Score = 0;
		char scorebuf[15];
		int Delay = 150;

		Display::Screen* scr = new Display::Screen(disp);
		disp->Power(1, Display::Display::Cursor::NoCursor_NoFlashing);

		//Game area, pixels
		unsigned char** area = new unsigned char*[32];
		for (int i = 0; i < 32; i++) {
			area[i] = new unsigned char[10];
			memset(area[i], EMPTY, 10);
		}
		LCDScreen* disp_area = new LCDScreen(disp, area);
		disp_area->UpdateMemory(LCDScreen::Rectangle_2x4);

		//display rows
		unsigned char** r = new unsigned char*[4];
		for (int i = 0; i < 4; i++) {
			r[i] = new unsigned char[20];
			memset(r[i], 0x20, 20);
		}

		/*r[0][10] = 0xA0;*/  r[0][0] = 0; r[0][1] = 4; r[0][2] = 0xA0;
		/*r[1][10] = 0xA0;*/ r[1][0] = 1; r[1][1] = 5; r[1][2] = 0xA0;
		/*r[2][10] = 0xA0;*/ r[2][0] = 2; r[2][1] = 6; r[2][2] = 0xA0;
		/*r[3][10] = 0xA0;*/ r[3][0] = 3; r[3][1] = 7; r[3][2] = 0xA0;

		sprintf(scorebuf, "Score: %d", Score);
		for (size_t i = 3; i < strlen(scorebuf) + 3 && i < 20; i++) {
			r[0][i] = scorebuf[i - 3];
		}

		scr->AddLine(new Display::DisplayString(r[0], 20));
		scr->AddLine(new Display::DisplayString(r[1], 20));
		scr->AddLine(new Display::DisplayString(r[2], 20));
		scr->AddLine(new Display::DisplayString(r[3], 20));
		scr->UpdateDisplay();

		bool InAction = false;
		Figure* figure = nullptr;
		int CurrentPos = 0;
		int UserInputCounterTicks = 0;
		while (1) {
			bool Update = false;
			Delay = 150;
			//Ввод
			Display::ScreenMutex.lock();
			size_t size = Display::KeyEvents.size();
			if (size > 0) {
				switch (Display::KeyEvents[0]->eCode)
				{
				case EventCode::LeftKeyPress:
					Update = figure->TurnLeft(area, CurrentPos);
					break;
				case EventCode::RightKeyPress:
					Update = figure->TurnRight(area, CurrentPos);
					break;
				case EventCode::MidKeyPress:
					Update = figure->Rotate(area, CurrentPos);
					break;
				case EventCode::UpKeyPress:
					for (int i = 0; i < 32; i++) {
						delete[] area[i];
					}
					delete[] area;

					for (int i = 0; i < 4; i++) {
						delete[] r[i];
					}
					delete[] r;
					Display::KeyEvents.erase(Display::KeyEvents.begin());
					Display::ScreenMutex.unlock();
					return ;
				case EventCode::DownKeyPress:
					Delay = 1;
					UserInputCounterTicks = 10;
					break;
				default:
					break;
				}
				Display::KeyEvents.erase(Display::KeyEvents.begin());
			}
			Display::ScreenMutex.unlock();
			if (Update) {

				Update = false;

				for (int m = 0; m < 32; m++)
					for (int n = 0; n < 10; n++)
						if (area[m][n] == ACTIONFIGURE)
							area[m][n] = EMPTY;

				for (int i = CurrentPos; i < CurrentPos + 4; i++) {
					if (i < 0 || i>31) continue;
					for (int j = 0; j < 10; j++) {
						if (figure->FigureSpace[i - CurrentPos][j])
							area[i][j] = figure->FigureSpace[i - CurrentPos][j];
					}
				}
			}
			UserInputCounterTicks++;
			if (UserInputCounterTicks < 5) continue;
			UserInputCounterTicks = 0;
			//
			if (InAction) {
				//Проверка на наличие опоры
				bool stable = false;
				for (int j = 0; j < 10; j++) {
					for (int i = 0; i < 32; i++) {
						if (area[i][j] == ACTIONFIGURE) {
							if (i == 31 || ((i + 1 < 32) && (area[i + 1][j] == STATIC))) {
								//Есть опора
								stable = true;
								break;
							}
						}
					}
					if (stable) break;
				}
				if (stable) {
					//Фиксируем
					for (int m = 0; m < 32; m++)
						for (int n = 0; n < 10; n++)
							if (area[m][n] == ACTIONFIGURE)
								area[m][n] = STATIC;
					InAction = false;
					//Проверка на заполнение
					for (int i = 31; i >= 0; i--) {
						int sum = 0;
						for (int j = 0; j < 10; j++)
							sum += area[i][j];
						if (sum == 20) {	//Целая линия
							for (int m = i - 1; m >= 0; m--) {
								for (int n = 0; n < 10; n++)
									area[m + 1][n] = area[m][n];
							}
							i++;
							Score += 10;
							sprintf(scorebuf, "Score: %d", Score);
							for (size_t i = 3; i < strlen(scorebuf) + 3 && i < 20; i++) {
								r[0][i] = scorebuf[i - 3];
							}
							scr->SetLine(new Display::DisplayString(r[0], 20), 0);
							scr->UpdateDisplay();
						}
					}
				}
				else {	//Иначе двигаем вниз
					CurrentPos++;

					for (int m = 0; m < 32; m++)
						for (int n = 0; n < 10; n++)
							if (area[m][n] == ACTIONFIGURE)
								area[m][n] = EMPTY;

					for (int i = CurrentPos; i < CurrentPos + 4; i++) {
						if (i < 0 || i>31) continue;
						for (int j = 0; j < 10; j++) {
							if (figure->FigureSpace[i - CurrentPos][j])
								area[i][j] = figure->FigureSpace[i - CurrentPos][j];
						}
					}
				}
			}
			//Проверка на завершение игры
			for (int i = 0; i < 10; i++)
				if (area[0][i] == STATIC) {
					for (int i = 0; i < 32; i++) {
						delete[] area[i];
					}
					delete[] area;

					for (int i = 0; i < 4; i++) {
						delete[] r[i];
					}
					delete[] r;
					//Losing
					this_thread::sleep_for(chrono::seconds(3));
					return ;
				}
			//Создание новой фигурки
			if (!InAction) {
				InAction = true;
				if (figure != nullptr) delete figure;
				figure = CreateRandom();

				int pos = figure->InitPos();

				for (int i = pos; i < pos + 4; i++) {
					if (i < 0 || i>31) continue;
					for (int j = 0; j < 10; j++) {
						if (figure->FigureSpace[i - pos][j])
							area[i][j] = figure->FigureSpace[i - pos][j];
					}
				}
				CurrentPos = pos;
			}

			//Обновление экрана
			disp_area->UpdateMemory(LCDScreen::Rectangle_2x4);
			this_thread::sleep_for(chrono::milliseconds(Delay));
		}

		for (int i = 0; i < 32; i++) {
			delete[] area[i];
		}
		delete[] area;

		for (int i = 0; i < 4; i++) {
			delete[] r[i];
		}
		delete[] r;
		return ;
	}
}
