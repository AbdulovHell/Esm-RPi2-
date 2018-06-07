#include "sys_headers.h"
#include <complex>
#include "DisplayControl.h"
#include "games.h"
#include "LCD.h"
#include "Colorize.h"

namespace Display {
	using namespace std;

	mutex ScreenMutex;
	vector<KeyEvent*> KeyEvents;

	template <typename T> T Pow(T base, int n) {
		if (n == 0) return (T)1;
		T res = base;
		for (int i = 1; i < n; i++)
			res *= base;
		return res;
	}

	void FreqAndAttMenu(Display * disp, uint32_t param)
	{
		Screen* scr = new Screen(disp);
		uint8_t mode = 0;	//select
		int32_t freq = 1000;
		int32_t att = 0;
		wchar_t freqstr[20];
		wchar_t attstr[20];
		swprintf(freqstr, 20, L" Частота: %5d МГц", freq);
		swprintf(attstr, 20, L" Затухание: %3d дБ", att);
		scr->AddLine(new DisplayString(freqstr, [&mode, scr](Display * d, uint32_t p) { mode = 1; scr->ReturnToPrevMenu(d, p); }));	//freq select
		scr->AddLine(new DisplayString(attstr, [&mode, scr](Display * d, uint32_t p) { mode = 2; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(L" "));
		scr->AddLine(new DisplayString(L" Назад", [&mode, scr](Display* d, uint32_t p) { mode = 4; scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(0, 1);

		bool work = true;
		while (work) {
			scr->SetActive();

			switch (mode) {
			case 1:
			{
				bool change = true;
				size_t size = 0;
				uint8_t pos = 11;
				disp->SetCursorPos(mode, pos);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							{
								int mult = 15 - pos;
								freq += Pow(10, mult);
								if (freq > 40000) freq = 40000;
								swprintf(freqstr, 20, L" Частота: %5d МГц", freq);
								scr->SetLine(new DisplayString(freqstr), 0);
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = 15 - pos;
								freq -= Pow(10, mult);
								if (freq < 0) freq = 0;
								swprintf(freqstr, 20, L" Частота: %5d МГц", freq);
								scr->SetLine(new DisplayString(freqstr), 0);
							}
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							pos--;
							if (pos < 11) pos = 11;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos++;
							if (pos > 15) pos = 15;
							break;
						case EventCode::MidKeyPress:
							scr->DispatchMessage();
							change = false;
							break;
						default:
							scr->DispatchMessage();
							break;
						}
						scr->UpdateDisplay();
						disp->SetCursorPos(mode, pos);
					}
				}
			}
			break;
			case 2:
			{
				bool change = true;
				size_t size = 0;
				uint8_t pos = 14;
				disp->SetCursorPos(mode, pos);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							{
								int mult = 15 - pos;
								att += Pow(10, mult);
								if (att > 0) att = 0;
								swprintf(attstr, 20, L" Затухание: %3d дБ", att);
								scr->SetLine(new DisplayString(attstr), 1);
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = 15 - pos;
								att -= Pow(10, mult);
								if (att < -35) att = -35;
								swprintf(attstr, 20, L" Затухание: %3d дБ", att);
								scr->SetLine(new DisplayString(attstr), 1);
							}
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							pos--;
							if (pos < 14) pos = 14;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos++;
							if (pos > 15) pos = 15;
							break;
						case EventCode::MidKeyPress:
							scr->DispatchMessage();
							change = false;
							break;
						default:
							scr->DispatchMessage();
							break;
						}
						scr->UpdateDisplay();
						disp->SetCursorPos(mode, pos);
					}
				}
			}
				break;
			case 4:
				return;
				break;
			}
		}
	}
}

namespace Threading {
	using namespace Display;

	void * LCDControlThread::ManageLCD(void * ptr_null)
	{
		//Инициализация драйвера
		Display::Display* disp = new Display::Display();
		disp->Init();
		disp->Power(1, Display::Display::Cursor::NoCursor_NoFlashing);

		//SandTest(disp);
		//Games::TetrisGame(disp);
		//Display::CustomSymbol face = { 0b01110,0b10001,0b11011,0b10001,0b10101,0b10001,0b01110,0b00000 };
		//disp->LoadSymbol(face, 1);
		//Инициализация меню
		Display::Screen* main_menu = new Display::Screen(disp);
		//Добавление пунктов меню
		main_menu->AddLine(new Display::DisplayString(L"Главное меню", Display::DisplayString::Alignment::Center));
		main_menu->AddLine(new Display::DisplayString(L" Частота и затухание_тест_длинны123.", FreqAndAttMenu));
		main_menu->AddLine(new Display::DisplayString(L" Каналы"));
		main_menu->AddLine(new Display::DisplayString(L" Статус"));
		main_menu->AddLine(new Display::DisplayString(L" Настройки"));
		main_menu->AddLine(new Display::DisplayString(L" Инструкция (?)"));
		main_menu->AddLine(new Display::DisplayString(L" ", Games::LaunchSecretMenu));
		//Включение полосы прокрутки и маркера строки
		main_menu->EnableMenu(1, 2);

		while (true) {
			printf("%s: Launch menu\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str());
			main_menu->SetActive();
		}
	}

	LCDControlThread::~LCDControlThread()
	{
		pthread_cancel(threadHandle);
	}
}