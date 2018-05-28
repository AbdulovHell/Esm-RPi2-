#ifndef _INPUT_H_
#define _INPUT_H_

namespace Threading {

	class ConsoleInputThread : virtual public Thread {
	private:
		static void* ScanConsole(void* ptr_null);
	protected:
	public:
		ConsoleInputThread() :Thread(ScanConsole, (void*)NULL) { }
		~ConsoleInputThread();
	};

	class ButtonsInputThread : virtual public Thread {
	private:
		static void* ScanButtons(void* ptr_null);
	protected:
	public:
		ButtonsInputThread() :Thread(ScanButtons, (void*)NULL) { }
		~ButtonsInputThread();
	};
}

#endif //_INPUT_H_