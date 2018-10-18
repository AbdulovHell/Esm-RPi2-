#include "sys_headers.h"
#include <complex>
#include "DisplayControl.h"
#include "games.h"
#include "LCD.h"
#include "Colorize.h"
#include "main.h"
#include "Task.h"

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
		int32_t Freq = 1000;
		int32_t prevFreq = Freq;
		uint8_t RFinAtt = 0;
		uint8_t IFAtt = 0;
		wchar_t freqstr[20];
		wchar_t AttRFinStr[20];
		wchar_t AttIFStr[20];
		wchar_t OutSelectStr[20];
		swprintf(freqstr, 20, L"  FREQ: %5d MHz", Freq);
		swprintf(AttRFinStr, 20, L"RF ATT: %2d dB", RFinAtt);
		swprintf(AttIFStr, 20, L"IF ATT: %2d dB", IFAtt);
		swprintf(OutSelectStr, 20, L"    IF:>1485  140");
		scr->AddLine(new DisplayString(freqstr, [&mode, scr](Display * d, uint32_t p) { mode = 1; scr->ReturnToPrevMenu(d, p); }));	//freq select
		scr->AddLine(new DisplayString(AttRFinStr, [&mode, scr](Display * d, uint32_t p) { mode = 2; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(AttIFStr, [&mode, scr](Display * d, uint32_t p) { mode = 5; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(OutSelectStr, [&mode, scr](Display * d, uint32_t p) { mode = 3; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(L" Back", [&mode, scr](Display* d, uint32_t p) { mode = 4; scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(0, 1);

		bool work = true;
		while (work) {
			scr->SetActive();

			switch (mode) {
			case 1:
			{
				const int LoPos = 13;
				const int HiPos = 9;
				bool change = true;
				size_t size = 0;
				uint8_t pos = LoPos;
				disp->SetCursorPos(mode, pos);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							{
								int mult = LoPos - pos;
								Freq += Pow(10, mult);
								if (Freq > 40000) Freq = 40000;
								swprintf(freqstr, 20, L"  FREQ: %5d MHz", Freq);
								scr->SetLine(new DisplayString(freqstr), 0);
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = LoPos - pos;
								Freq -= Pow(10, mult);
								if (Freq < 3000) Freq = 3000;
								swprintf(freqstr, 20, L"  FREQ: %5d MHz", Freq);
								scr->SetLine(new DisplayString(freqstr), 0);
							}
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							pos--;
							if (pos < HiPos) pos = HiPos;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos++;
							if (pos > LoPos) pos = LoPos;
							break;
						case EventCode::MidKeyPress:
							scr->DispatchMessage();
							change = false;
							break;
						default:
							scr->DispatchMessage();
							break;
						}
						if (prevFreq != Freq) {	//произошло изменение
							prevFreq = Freq;
							printf("%s: Add SetFreq Task %d\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str(), Freq);
							Threading::AddTask(new Threading::TaskSetFreq(Freq));
							swprintf(freqstr, 20, L"  FREQ: %5d MHz", Freq);
							scr->SetLine(new DisplayString(freqstr), 0);
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
				uint8_t pos = 9;
				disp->SetCursorPos(2, pos);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							{
								int mult = 10 - pos;
								RFinAtt += Pow(10, mult);
								if (RFinAtt > 25) RFinAtt = 25;
								swprintf(AttRFinStr, 20, L"RF ATT: %2d dB", RFinAtt);
								scr->SetLine(new DisplayString(AttRFinStr), 1);
								Threading::AddTask(new Threading::TaskSetAtt(RFinAtt,IFAtt));
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = 10 - pos;
								RFinAtt -= Pow(10, mult);
								if (RFinAtt > 25) RFinAtt = 25;
								swprintf(AttRFinStr, 20, L"RF ATT: %2d dB", RFinAtt);
								scr->SetLine(new DisplayString(AttRFinStr), 1);
								Threading::AddTask(new Threading::TaskSetAtt(RFinAtt, IFAtt));
							}
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							pos--;
							if (pos < 9) pos = 9;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos++;
							if (pos > 10) pos = 10;
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
						disp->SetCursorPos(2, pos);
					}
				}
			}
			break;
			case 5:
			{
				bool change = true;
				size_t size = 0;
				uint8_t pos = 9;
				disp->SetCursorPos(3, pos);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							{
								int mult = 10 - pos;
								IFAtt += Pow(10, mult);
								if (IFAtt > 15) IFAtt = 15;
								swprintf(AttIFStr, 20, L"IF ATT: %2d dB", IFAtt);
								scr->SetLine(new DisplayString(AttIFStr), 2);
								Threading::AddTask(new Threading::TaskSetAtt(RFinAtt, IFAtt));
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = 10 - pos;
								IFAtt -= Pow(10, mult);
								if (IFAtt > 15) IFAtt = 15;
								swprintf(AttIFStr, 20, L"IF ATT: %2d dB", IFAtt);
								scr->SetLine(new DisplayString(AttIFStr), 2);
								Threading::AddTask(new Threading::TaskSetAtt(RFinAtt, IFAtt));
							}
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							pos--;
							if (pos < 9) pos = 9;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos++;
							if (pos > 10) pos = 10;
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
						disp->SetCursorPos(3, pos);
					}
				}
			}
			break;
			case 3:
			{
				bool change = true;
				size_t size = 0;
				const uint8_t pos1485 = 7;
				const uint8_t pos140 = 13;
				uint8_t pos = OutSelectStr[pos1485] == '>' ? pos1485 : pos140;
				uint8_t prevpos = pos;
				disp->SetCursorType(Display::Cursor::NoCursor_NoFlashing);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							pos = pos1485;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos = pos140;
							break;
						case EventCode::MidKeyPress:
							scr->DispatchMessage();
							change = false;
							break;
						default:
							scr->DispatchMessage();
							break;
						}
						if (prevpos != pos) {	//произошло изменение
							prevpos = pos;
							printf("%s: Add SetOutput to %d task\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str(), pos == pos1485 ? 1485 : 140);
							Threading::AddTask(new Threading::TaskSetOutput(pos == pos1485 ? 1 : 0));
							if (pos == pos1485) {
								OutSelectStr[pos1485] = '>';
								OutSelectStr[pos140] = ' ';
							}
							else {
								OutSelectStr[pos1485] = ' ';
								OutSelectStr[pos140] = '>';
							}
							scr->SetLine(new DisplayString(OutSelectStr), 3);
						}
						scr->UpdateDisplay();
						disp->SetCursorType(Display::Cursor::NoCursor_NoFlashing);
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

	void StatusMenu(Display * disp, uint32_t param)
	{
		Screen* scr = new Screen(disp);
		uint8_t mode = 0;	//select
		wchar_t temp_str[20];
		float temp = 0;
		swprintf(temp_str, 20, L"   %2.2f °C", temp);
		scr->AddLine(new DisplayString(L" Temperature 1:"));	//freq select
		scr->AddLine(new DisplayString(temp_str));
		scr->AddLine(new DisplayString(L" "));
		scr->AddLine(new DisplayString(L" Temperature 2:"));
		scr->AddLine(new DisplayString(L""));
		scr->AddLine(new DisplayString(L" Back", [&mode, scr](Display* d, uint32_t p) { scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(0, 1);

		//bool work = true;
		//while (work) {
		int cnt = 0;
		scr->SetActive([&cnt, scr, &mode, &temp](uint32_t param) {
			if (cnt++ % 7500000 == 0) {
				wchar_t temp_str[20];
				switch (mode) {
				case 0:
					mode = 1;	//запрос
					Threading::AddTask(new Threading::TaskRequestTemp(&temp));
					//printf("%s: Read temperature\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str());
					break;
				case 1:
					mode = 0;
					swprintf(temp_str, 20, L"   %2.2f °C", temp);
					scr->SetLine(new DisplayString(temp_str), 1);
					scr->SetLine(new DisplayString(temp_str), 4);
					//printf("%s: Show temperature\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str());
					break;
				}
				return true;
			}
			else
				return false;
		});
		//}
	}

	void SetupMenu(Display * disp, uint32_t param)
	{
		Screen* scr = new Screen(disp);
		uint8_t mode = 0;	//select
		wchar_t temp_str[20];
		wchar_t PWMstr[20];
		int Step = 0, PrevStep = -1;
		int PWMStep = 0, PWMPrevStep = -1;

		temp_str[0] = ' ';
		temp_str[1] = 0x91;
		temp_str[19] = ' ';
		temp_str[18] = 0x90;

		for (int i = 2; i < 18; i++) {
			if (i < Step + 2) {
				temp_str[i] = 0xA0;
			}
			else {
				temp_str[i] = 0x94;
			}
		}

		PWMstr[0] = ' ';
		PWMstr[1] = ' ';
		PWMstr[2] = ' ';
		PWMstr[3] = ' ';
		PWMstr[4] = 0x91;
		PWMstr[19] = ' ';
		PWMstr[18] = ' ';
		PWMstr[17] = ' ';
		PWMstr[16] = ' ';
		PWMstr[15] = 0x90;

		for (int i = 5; i < 15; i++) {
			if (i < PWMStep + 5) {
				PWMstr[i] = 0xA0;
			}
			else {
				PWMstr[i] = 0x94;
			}
		}

		scr->AddLine(new DisplayString(L" Brightness:"));	//freq select
		scr->AddLine(new DisplayString(temp_str, [&mode, scr](Display * d, uint32_t p) { mode = 2; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(L" PWM Test, /%:"));	//freq select
		scr->AddLine(new DisplayString(PWMstr, [&mode, scr](Display * d, uint32_t p) { mode = 3; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(L" Back", [&mode, scr](Display* d, uint32_t p) { mode = 4; scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(1, 1);

		bool work = true;
		while (work) {
			scr->SetActive();

			switch (mode) {
			case 2:
			{
				bool change = true;
				size_t size = 0;
				//uint8_t pos = 6;
				disp->SetCursorType(Display::Cursor::NoCursor_NoFlashing);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							Step = 16;
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							Step = 0;
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							Step--;
							if (Step < 0) Step = 0;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							Step++;
							if (Step > 16) Step = 16;
							break;
						case EventCode::MidKeyPress:
							scr->DispatchMessage();
							change = false;
							break;
						default:
							scr->DispatchMessage();
							break;
						}
						if (PrevStep != Step) {	//произошло изменение
							PrevStep = Step;
							for (int i = 2; i < 18; i++) {
								if (i < Step + 2) {
									temp_str[i] = 0xA0;
								}
								else {
									temp_str[i] = 0x94;
								}
							}
							scr->SetLine(new DisplayString(temp_str), 1);
							printf("%s: Add AdjustBL Task %d %\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str(), Step);
							Threading::AddTask(new Threading::TaskAdjustBL(Step));
						}
						scr->UpdateDisplay();
						//disp->SetCursorPos(mode, pos);
						disp->SetCursorType(Display::Cursor::NoCursor_NoFlashing);
					}
				}
			}
			break;
			case 3:
			{
				bool change = true;
				size_t size = 0;
				//uint8_t pos = 6;
				disp->SetCursorType(Display::Cursor::NoCursor_NoFlashing);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							PWMStep = 10;
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							PWMStep = 0;
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							PWMStep--;
							if (PWMStep < 0) PWMStep = 0;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							PWMStep++;
							if (PWMStep > 10) PWMStep = 10;
							break;
						case EventCode::MidKeyPress:
							scr->DispatchMessage();
							change = false;
							break;
						default:
							scr->DispatchMessage();
							break;
						}
						if (PWMPrevStep != PWMStep) {	//произошло изменение
							PWMPrevStep = PWMStep;
							for (int i = 5; i < 15; i++) {
								if (i < PWMStep + 5) {
									PWMstr[i] = 0xA0;
								}
								else {
									PWMstr[i] = 0x94;
								}
							}
							scr->SetLine(new DisplayString(PWMstr), 3);
							printf("%s: Add SetPWM Task %d %\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str(), PWMStep);
							Threading::AddTask(new Threading::TaskSetPWM(PWMStep));
						}
						scr->UpdateDisplay();
						//disp->SetCursorPos(mode, pos);
						disp->SetCursorType(Display::Cursor::NoCursor_NoFlashing);
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
		main_menu->AddLine(new Display::DisplayString(L"MAIN MENU", Display::DisplayString::Alignment::Center));
		main_menu->AddLine(new Display::DisplayString(L"FREQUENCY & ATT.", Display::DisplayString::Alignment::Center, FreqAndAttMenu));
		main_menu->AddLine(new Display::DisplayString(L"SETTINGS", Display::DisplayString::Alignment::Center, SetupMenu));
		main_menu->AddLine(new Display::DisplayString(L"STATUS", Display::DisplayString::Alignment::Center, StatusMenu));
		main_menu->AddLine(new Display::DisplayString(L" ", Display::DisplayString::Alignment::Center, Games::LaunchSecretMenu));
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