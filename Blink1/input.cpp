#include "sys_headers.h"

#include "threading.h"
#include "Colorize.h"
#include "Task.h"
#include "cg.h"
#include "input.h"
#include "KeyEvents.h"
#include "DisplayControl.h"
#include "TCP.h"
#include "main.h"
#include "stuff.h"


void EmulatingKeyboard()
{
	while (1) {
		int ch = Stuff::Conio::getch();
		Display::ScreenMutex.lock();
		switch (ch)
		{
		case 'w':
			Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::UpKeyPress));
			break;
		case 'a':
			Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::LeftKeyPress));
			break;
		case 's':
			Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::DownKeyPress));
			break;
		case 'd':
			Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::RightKeyPress));
			break;
		case 'f':
			Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::MidKeyPress));
			break;
		case 'q':
			return;
		default:
			break;
		}
		Display::ScreenMutex.unlock();
	}
}

namespace Threading {

	void * ConsoleInputThread::ScanConsole(void * ptr_null)
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
				MainTasks.push_back(Stuff::make_unique<TaskQuit>());
				TasksMutex->unlock();
			}
			if (strcmp(buf, "pkey") == 0) {
				//simulating phys keys
				//printf("%s: Send key press\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
				EmulatingKeyboard();
			}
		}
		return nullptr;
	}

	ConsoleInputThread::~ConsoleInputThread()
	{
		pthread_cancel(threadHandle);
	}

	void * ButtonsInputThread::ScanButtons(void * ptr_null)
	{
		int Pressed[5] = { 0,0,0,0,0 };
		int ButtonsPin[5] = { 4,17,27,22,23 };
		int TempState[5] = { 0,0,0,0,0 };

		for (int i = 0; i < 5; i++) {
			digitalWrite(ButtonsPin[i], 0);
			pinMode(ButtonsPin[i], INPUT);
		}

		while (1) {
			this_thread::sleep_for(std::chrono::microseconds(1));
			for (int i = 0; i < 5; i++)
				TempState[i] = digitalRead(ButtonsPin[i]);

			for (int i = 0; i < 5; i++) {
				if (Pressed[i] != TempState[i] && Pressed[i] == 1) {	//состояние изменилось, кнопку отпустили
					//printf("INPUT: Button %d Now: %d Last: %d\n",ButtonsPin[i],TempState[i],Pressed[i]);
					Display::ScreenMutex.lock();
					switch (ButtonsPin[i])
					{
					case 17:
						Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::UpKeyPress));
						//printf("%s: Up key\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
						break;
					case 23:
						Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::LeftKeyPress));
						//printf("%s: Left key\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
						break;
					case 22:
						Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::DownKeyPress));
						//printf("%s: Down key\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
						break;
					case 27:
						Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::RightKeyPress));
						//printf("%s: Right key\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
						break;
					case 4:
						Display::KeyEvents.push_back(new Display::KeyEvent(Display::EventCode::MidKeyPress));
						//printf("%s: OK key\n", Stuff::MakeColor("INPUT", Stuff::Blue).c_str());
						break;
					default:
						break;
					}
					Display::ScreenMutex.unlock();
				}
				Pressed[i] = TempState[i];
			}
		}
		return nullptr;
	}

	ButtonsInputThread::~ButtonsInputThread()
	{
		pthread_cancel(threadHandle);
	}

}
