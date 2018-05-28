#include "sys_headers.h"

#include "DisplayControl.h"
#include "games.h"
#include "LCD.h"

namespace Display {
	using namespace std;

	mutex ScreenMutex;
	vector<KeyEvent*> KeyEvents;
}

namespace Threading {
	using namespace Display;

	void * LCDControlThread::ManageLCD(void * ptr_null)
	{

		bool SecretMenu = false;
		int size;
		Display::Display* disp = new Display::Display();
		disp->Init();
		disp->Power(1, Display::Display::Cursor::NoCursor_NoFlashing);

		//SandTest(disp);
		//Games::TetrisGame(disp);
		//Display::CustomSymbol face = { 0b01110,0b10001,0b11011,0b10001,0b10101,0b10001,0b01110,0b00000 };
		//disp->LoadSymbol(face, 1);

		Display::Screen* main_menu = new Display::Screen(disp);
		main_menu->AddLine(new Display::DisplayString(L"Главное меню", Display::DisplayString::Alignment::Center));
		main_menu->AddLine(new Display::DisplayString(L" Частота и атт."));
		main_menu->AddLine(new Display::DisplayString(L" Каналы"));
		main_menu->AddLine(new Display::DisplayString(L" Статус"));
		main_menu->AddLine(new Display::DisplayString(L" Настройки"));
		main_menu->AddLine(new Display::DisplayString(L" Инструкция (?)"));
		main_menu->AddLine(new Display::DisplayString(" "));
		main_menu->EnableMenu(1, 2);
		main_menu->SetActive();
		int cnt = 1;
		char txt[20];
		while (1) {

			Display::ScreenMutex.lock();
			size = KeyEvents.size();
			if (size > 0) {
				switch (KeyEvents[0]->eCode)
				{
				case EventCode::UpKeyPress:
					main_menu->ScrollMenu(-1);
					break;
				case EventCode::DownKeyPress:
					main_menu->ScrollMenu(1);
					break;
				case EventCode::MidKeyPress:
					switch (main_menu->GetSelectedIndex()) {
					case 2:	//Частота и атт.
						
						break;
					case 3:	//Каналы

						break;
					case 4:	//Статус

						break;
					case 5:	//Настройки

						break;
					case 6:	//Настройки

						break;
					case 7:	//Секретное меню
						if (cnt > 3) {
							SecretMenu = true;
						}
						sprintf(txt, " %d", cnt);
						main_menu->SetLine(new Display::DisplayString(txt), 6);
						main_menu->SetActive();
						cnt++;
						break;
					}
					break;
				default:
					break;
				}
				KeyEvents.erase(KeyEvents.begin());
			}
			Display::ScreenMutex.unlock();

			if (SecretMenu) {
				if (Games::LaunchSecretMenu(disp)) {
					main_menu->SetLine(new Display::DisplayString(" "), 6);
					main_menu->ScrollMenu(-10);
					cnt = 0;
				}
				main_menu->SetActive();
				SecretMenu = false;
			}
		}
	}

	LCDControlThread::~LCDControlThread()
	{
		pthread_cancel(threadHandle);
	}
}