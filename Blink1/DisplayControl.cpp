#include "sys_headers.h"
#include <complex>
#include "DisplayControl.h"
#include "games.h"
#include "LCD.h"
#include "Colorize.h"
#include "main.h"
#include "Task.h"
#include "settings.h"
#include "IPChanger.h"
#include <inttypes.h>

namespace Display {
	using namespace std;

	mutex ScreenMutex;
	vector<KeyEvent*> KeyEvents;
	const float SoftwareVersion = 1.57;

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
		int32_t Freq = Stuff::Storage->GetFreq();
		int32_t prevFreq = Freq;
		uint8_t RFinAtt = Stuff::Storage->GetRFAtt();
		uint8_t IFAtt = Stuff::Storage->GetIFAtt();
		const uint8_t pos1485 = 7;
		const uint8_t pos140 = 13;
		wchar_t freqstr[20];
		wchar_t AttRFinStr[20];
		wchar_t AttIFStr[20];
		wchar_t OutSelectStr[20];

		swprintf(freqstr, 20, L"  Freq: %5d MHz", Freq);
		swprintf(AttRFinStr, 20, L"RF Att: %2d dB", RFinAtt);
		swprintf(AttIFStr, 20, L"IF Att: %2d dB", IFAtt);
		swprintf(OutSelectStr, 20, L"    IF:>1485  140");
		if (Stuff::Storage->GetIF()) {
			OutSelectStr[pos1485] = '>';
			OutSelectStr[pos140] = ' ';
		}
		else {
			OutSelectStr[pos1485] = ' ';
			OutSelectStr[pos140] = '>';
		}
		scr->AddLine(new DisplayString(L"Frequency & Att.", DisplayString::Alignment::Center));
		scr->AddLine(new DisplayString(freqstr, [&mode, scr](Display * d, uint32_t p) { mode = 1; scr->ReturnToPrevMenu(d, p); }));	//freq select
		scr->AddLine(new DisplayString(AttRFinStr, [&mode, scr](Display * d, uint32_t p) { mode = 2; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(AttIFStr, [&mode, scr](Display * d, uint32_t p) { mode = 5; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(OutSelectStr, [&mode, scr](Display * d, uint32_t p) { mode = 3; scr->ReturnToPrevMenu(d, p); }));

		scr->AddLine(new DisplayString(L" Back", [&mode, scr](Display* d, uint32_t p) { mode = 4; scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(1, 2);

		bool work = true;
		while (work) {
			scr->SetActive();

			switch (mode) {
			case 1:
			{
				const int CursorPos = 2;
				const int LoPos = 13;
				const int HiPos = 9;
				bool change = true;
				size_t size = 0;
				uint8_t pos = LoPos;
				disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);

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
								//swprintf(freqstr, 20, L"  Freq: %5d MHz", Freq);
								//scr->SetLine(new DisplayString(freqstr), 2);
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = LoPos - pos;
								Freq -= Pow(10, mult);
								if (Freq < 3000) Freq = 3000;
								//swprintf(freqstr, 20, L"  Freq: %5d MHz", Freq);
								//scr->SetLine(new DisplayString(freqstr), 2);
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
							swprintf(freqstr, 20, L"  Freq: %5d MHz", Freq);
							scr->SetLine(new DisplayString(freqstr), 1);
						}
						scr->UpdateDisplay();
						disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);
					}
				}
			}
			break;
			case 2:
			{
				const int CursorPos = 3;
				bool change = true;
				uint8_t PrevAtt = RFinAtt;
				size_t size = 0;
				uint8_t pos = 9;
				disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);

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
								if (RFinAtt > 15) RFinAtt = 15;
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = 10 - pos;
								RFinAtt -= Pow(10, mult);
								if (std::abs(RFinAtt - PrevAtt) > 30) RFinAtt = 0;
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
						if (RFinAtt != PrevAtt) {	//произошло изменение
							PrevAtt = RFinAtt;
							swprintf(AttRFinStr, 20, L"RF Att: %2d dB", RFinAtt);
							scr->SetLine(new DisplayString(AttRFinStr), 2);
							Threading::AddTask(new Threading::TaskSetAtt(RFinAtt, IFAtt));
						}
						scr->UpdateDisplay();
						disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);
					}
				}
			}
			break;
			case 5:
			{
				const int CursorPos = 4;
				bool change = true;
				uint8_t PrevAtt = IFAtt;
				size_t size = 0;
				uint8_t pos = 9;
				disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);

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
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = 10 - pos;
								IFAtt -= Pow(10, mult);
								if (std::abs(IFAtt - PrevAtt) > 30) IFAtt = 0;
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
						if (IFAtt != PrevAtt) {	//произошло изменение
							PrevAtt = IFAtt;
							swprintf(AttIFStr, 20, L"IF Att: %2d dB", IFAtt);
							scr->SetLine(new DisplayString(AttIFStr), 3);
							Threading::AddTask(new Threading::TaskSetAtt(RFinAtt, IFAtt));
						}
						scr->UpdateDisplay();
						disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);
					}
				}
			}
			break;
			case 3:
			{
				bool change = true;
				size_t size = 0;
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
							scr->SetLine(new DisplayString(OutSelectStr), 4);
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

	string AskSystem(string cmd) {
		//redirect
		cmd += " > as.txt";
		system(cmd.c_str());
		//open
		FILE* log = fopen("as.txt", "r");
		//get output size
		fseek(log, 0, SEEK_END);
		long pos = ftell(log);
		fseek(log, 0, SEEK_SET);
		//create buffer
		char* buf = new char[pos + 1];
		buf[pos] = 0;
		//read
		auto readed = fread(buf, pos, pos, log);
		string info = buf;

		fclose(log);
		delete[] buf;
		system("rm as.txt");
		return info;
	}

	void StartUpdate(Display * disp, uint32_t param) {
		Screen* scr = new Screen(disp);

		scr->AddLine(new DisplayString(L"Updating..."));
		scr->AddLine(new DisplayString(L""));
		scr->AddLine(new DisplayString(L""));
		scr->AddLine(new DisplayString(L""));

		scr->UpdateDisplay();

		const string dev = "/dev/sda1";
		string ans = AskSystem("ls " + dev);

		if (ans == (dev + "\n")) {
			scr->SetLine(new DisplayString(L"Reboot"), 1);
			scr->UpdateDisplay();
			cout << "start update" << endl;
			system("systemctl start esmupdate.service");
		}
		else {
			scr->SetLine(new DisplayString(L"USB flash not found"), 1);
			scr->UpdateDisplay();
			cout << "update failed" << endl;
			this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	void StatusMenu(Display * disp, uint32_t param)
	{
		Screen* scr = new Screen(disp);
		uint8_t mode = 0;	//select
		wchar_t temp_str[20];
		float temp = 0;
		swprintf(temp_str, 20, L"   %2.2f °C", temp);

		auto sTime = __TIME__;
		/*wchar_t* wtime = new wchar_t[strlen(time)+1];
		memset(wtime, 0, strlen(time) + 1);
		for (int i = 0; i < strlen(time) + 1; i++) wtime[i] = time[i];*/

		auto sDate = __DATE__;
		wchar_t* wdate = new wchar_t[strlen(sDate) + 1];
		memset(wdate, 0, strlen(sDate) + 1);
		for (int i = 0; i < strlen(sDate) + 1; i++) wdate[i] = sDate[i];
		wstring wdatestr = wdate;
		wdatestr = wdatestr + L" ";
		delete[] wdate;
		//time_t time = lts;
		//auto sysTime = std::chrono::system_clock::from_time_t(time);
		//auto date = year
		wchar_t verstr1[20];
		swprintf(verstr1, 20, L" %1.2f      11:11:11 ", SoftwareVersion);
		for (int i = 0; i < 8; i++) verstr1[i + 11] = sTime[i];

		wchar_t uptimestr[20];
		wchar_t uptime_template[] = L" Uptime: %8llu h";
		swprintf(uptimestr, 20, uptime_template, Stuff::Storage->GetWorkTime());

		scr->AddLine(new DisplayString(L" Temperature:"));	//freq select
		scr->AddLine(new DisplayString(temp_str));
		scr->AddLine(new DisplayString(uptimestr));
		scr->AddLine(new DisplayString(L" Software version:"));
		scr->AddLine(new DisplayString(verstr1));
		scr->AddLine(new DisplayString(wdatestr.c_str(), DisplayString::Alignment::Right));
		scr->AddLine(new DisplayString(L" Update", StartUpdate));
		scr->AddLine(new DisplayString(L" Back", [&mode, scr](Display* d, uint32_t p) { scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(0, 1);

		scr->SetActive([&uptime_template, scr](uint32_t param) {
			if (Threading::UpdateNow) {
				Threading::UpdateNow = false;
				wchar_t uptimestr[20];
				swprintf(uptimestr, 20, uptime_template, Stuff::Storage->GetWorkTime() / 3600);
				scr->SetLine(new DisplayString(uptimestr), 2);

				wchar_t temp_str[20];
				swprintf(temp_str, 20, L"   %2.2f °C", Threading::Temperature);
				scr->SetLine(new DisplayString(temp_str), 1);
				return true;
			}
			else
				return false;
		});
	}

	void EthernetSettings(Display * disp, uint32_t param)
	{
		Screen* scr = new Screen(disp);
		uint8_t mode = 0;	//select

		Threading::IPChanger ipchanger;
		string IP = ipchanger.GetIP();

		wchar_t ModeStr[] = L" >Static  Dynamic";
		const uint8_t posStatic = 1;
		const uint8_t posDynamic = 9;
		if (!ipchanger.UsingDHCP()) {
			ModeStr[posStatic] = '>';
			ModeStr[posDynamic] = ' ';
		}
		else {
			ModeStr[posStatic] = ' ';
			ModeStr[posDynamic] = '>';
		}

		wchar_t IPstr[20] = { 0, };
		int dotpos[3] = { 0, };
		int dotcnt = 0;
		for (int i = 0; i < IP.length(); i++) {
			if (IP[i] == '.') {
				dotpos[dotcnt++] = ++i;
			}
		}
		int nums[4] = { 0, };
		for (int i = 0; i < 4; i++) {
			if (i == 0)
				nums[i] = atoi(IP.c_str());
			else
				nums[i] = atoi(IP.c_str() + dotpos[i - 1]);
		}
		swprintf(IPstr, 20, L" %3d.%3d.%3d.%3d", nums[0], nums[1], nums[2], nums[3]);
		int Lastnums[4] = { nums[0],nums[1],nums[2],nums[3] };

		scr->AddLine(new DisplayString(L"Ethernet settings", DisplayString::Alignment::Center));
		scr->AddLine(new DisplayString(ModeStr, [&mode, scr](Display* d, uint32_t p) { mode = 2; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(IPstr, [&mode, scr](Display* d, uint32_t p) { mode = 3; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(L" Save and reboot", [&mode, scr](Display* d, uint32_t p) { mode = 1; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(L" Back", [&mode, scr](Display* d, uint32_t p) { mode = 0; scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(1, 2);

		bool work = true;
		while (work) {
			scr->SetActive();

			switch (mode) {
			case 1:
				ipchanger.Save();
				system("sudo reboot");
				break;
			case 2:
			{
				bool change = true;
				size_t size = 0;
				uint8_t pos = ModeStr[posStatic] == '>' ? posStatic : posDynamic;
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
							pos = posStatic;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos = posDynamic;
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
							printf("%s: IP work mode set to %s\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str(), pos == posStatic ? "Static" : "Dynamic");
							ipchanger.SetDHCPState(pos == posDynamic);
							if (pos == posStatic) {
								ModeStr[posStatic] = '>';
								ModeStr[posDynamic] = ' ';
							}
							else {
								ModeStr[posStatic] = ' ';
								ModeStr[posDynamic] = '>';
							}
							scr->SetLine(new DisplayString(ModeStr), 1);
						}
						scr->UpdateDisplay();
						disp->SetCursorType(Display::Cursor::NoCursor_NoFlashing);
					}
				}
			}
			break;
			case 3:
			{
				const int CursorPos = 3;	//Line index
				const int LoPos[4] = { 4,8,12, 16 };
				const int HiPos[4] = { 2,6,10, 14 };
				int Num = 0;
				bool change = true;
				size_t size = 0;
				uint8_t pos = LoPos[Num];
				disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);

				while (change) {
					size = KeyEvents.size();
					if (size > 0) {
						ScreenMutex.lock();
						switch (KeyEvents[0]->eCode) {
						case EventCode::UpKeyPress:
							scr->DispatchMessage();
							{
								int mult = LoPos[Num] - pos;
								nums[Num] += Pow(10, mult);
								if (nums[Num] > 255) nums[Num] = 255;
							}
							break;
						case EventCode::DownKeyPress:
							scr->DispatchMessage();
							{
								int mult = LoPos[Num] - pos;
								nums[Num] -= Pow(10, mult);
								if (nums[Num] < 0) nums[Num] = 0;
							}
							break;
						case EventCode::LeftKeyPress:
							scr->DispatchMessage();
							pos--;
							if (pos < HiPos[Num]) {
								Num--;
								if (Num < 0) {
									Num = 0;
									pos = HiPos[Num];
								}
								else {
									pos = LoPos[Num];
								}
							}
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos++;
							if (pos > LoPos[Num]) {
								Num++;
								if (Num > 3) {
									Num = 3;
									pos = LoPos[Num];
								}
								else {
									pos = HiPos[Num];
								}
							}
							break;
						case EventCode::MidKeyPress:
							scr->DispatchMessage();
							change = false;
							break;
						default:
							scr->DispatchMessage();
							break;
						}
						if (Lastnums[Num] != nums[Num]) {	//произошло изменение
							Lastnums[Num] = nums[Num];
							swprintf(IPstr, 20, L" %3d.%3d.%3d.%3d", nums[0], nums[1], nums[2], nums[3]);
							scr->SetLine(new DisplayString(IPstr), 2);
							ipchanger.SetIP(nums);
						}
						scr->UpdateDisplay();
						disp->SetCursorPos(CursorPos - scr->TopLineIndex(), pos);
					}
				}
			}
			break;
			case 0:
				return;
			}
		}
	}

	void SetupMenu(Display * disp, uint32_t param)
	{
		Screen* scr = new Screen(disp);
		uint8_t mode = 0;	//select
		wchar_t temp_str[20];
		wchar_t PWMstr[20];
		const uint8_t posInt = 7;
		const uint8_t posExt = 12;
		int Step = Stuff::Storage->GetBrightLvl(), PrevStep = -1;
		int PWMStep = 0, PWMPrevStep = -1;

		wchar_t RefSelectStr[20];
		swprintf(RefSelectStr, 20, L"  Ref.:>Int  Ext");
		if (Stuff::Storage->GetRef()) {
			RefSelectStr[posInt] = '>';
			RefSelectStr[posExt] = ' ';
		}
		else {
			RefSelectStr[posInt] = ' ';
			RefSelectStr[posExt] = '>';
		}

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

		scr->AddLine(new DisplayString(L"Settings", DisplayString::Alignment::Center));
		scr->AddLine(new DisplayString(L" Brightness:", DisplayString::Alignment::Center));
		scr->AddLine(new DisplayString(temp_str, [&mode, scr](Display * d, uint32_t p) { mode = 2; scr->ReturnToPrevMenu(d, p); }));
		//scr->AddLine(new DisplayString(L" PWM Test, /%:"));	//freq select
		//scr->AddLine(new DisplayString(PWMstr, [&mode, scr](Display * d, uint32_t p) { mode = 3; scr->ReturnToPrevMenu(d, p); }));
		scr->AddLine(new DisplayString(RefSelectStr, [&mode, scr](Display * d, uint32_t p) { mode = 6; scr->ReturnToPrevMenu(d, p); }));

		scr->AddLine(new DisplayString(L" Ethernet settings", EthernetSettings));
		scr->AddLine(new DisplayString(L" Back", [&mode, scr](Display* d, uint32_t p) { mode = 4; scr->ReturnToPrevMenu(d, p); }));	//exit
		scr->EnableMenu(1, 2);

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
							scr->SetLine(new DisplayString(temp_str), 2);
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
							scr->SetLine(new DisplayString(PWMstr), 4);
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
			case 6:
			{
				bool change = true;
				size_t size = 0;
				uint8_t pos = RefSelectStr[posInt] == '>' ? posInt : posExt;
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
							pos = posInt;
							break;
						case EventCode::RightKeyPress:
							scr->DispatchMessage();
							pos = posExt;
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
							printf("%s: Add ChangeRef to %s task\n", Stuff::MakeColor("DISPLAY", Stuff::Yellow).c_str(), pos == posInt ? "internal" : "external");
							Threading::AddTask(new Threading::TaskChangeRef(pos == posInt ? 1 : 0));
							if (pos == posInt) {
								RefSelectStr[posInt] = '>';
								RefSelectStr[posExt] = ' ';
							}
							else {
								RefSelectStr[posInt] = ' ';
								RefSelectStr[posExt] = '>';
							}
							scr->SetLine(new DisplayString(RefSelectStr), 3);
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
		main_menu->AddLine(new Display::DisplayString(L"Main menu", Display::DisplayString::Alignment::Center));
		main_menu->AddLine(new Display::DisplayString(L"Frequency & Att.", Display::DisplayString::Alignment::Center, FreqAndAttMenu));
		main_menu->AddLine(new Display::DisplayString(L"Settings", Display::DisplayString::Alignment::Center, SetupMenu));
		main_menu->AddLine(new Display::DisplayString(L"Status", Display::DisplayString::Alignment::Center, StatusMenu));
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