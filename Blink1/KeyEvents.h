#ifndef _KEYEVENTS_H_
#define _KEYEVENTS_H_

namespace Display {

	enum EventCode : int {
		UpKeyPress,
		DownKeyPress,
		LeftKeyPress,
		RightKeyPress,
		MidKeyPress
	};

	class KeyEvent {
	public:
		EventCode eCode;
		int param=0;

		KeyEvent(EventCode c) { eCode = c; }
	};
}

#endif	//_KEYEVENTS_H_