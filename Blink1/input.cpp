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
				MainTasks.push_back(new TaskQuit());
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
		while (1) {

		}
		return nullptr;
	}

	ButtonsInputThread::~ButtonsInputThread()
	{
		pthread_cancel(threadHandle);
	}

}
