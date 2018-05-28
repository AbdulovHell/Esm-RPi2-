#ifndef _DISPLAYCONTROL_H_
#define _DISPLAYCONTROL_H_

#include "KeyEvents.h"
#include "Display.h"
#include "Screen.h"
#include "threading.h"

namespace Display {
	extern std::mutex ScreenMutex;
	extern std::vector<KeyEvent*> KeyEvents;
}

namespace Threading {
	class LCDControlThread : virtual public Thread {
	private:
		static void* ManageLCD(void* ptr_null);
	public:
		LCDControlThread() :Thread(ManageLCD, (void*)NULL) {}
		~LCDControlThread();
	};
}

#endif
