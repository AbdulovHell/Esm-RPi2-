#ifndef _THREADING_H_
#define _THREADING_H_

namespace Threading {
	using namespace std;

	class TCPReciverThread;
	class TCPServerThread;

	int LaunchSecretMenu(LCD::Display * disp);
	void EmulatingKeyboard();
	void Selfdestruction(LCD::Display * disp);
	void SnakeGame(LCD::Display * disp);

	extern vector<Threading::TCPReciverThread*> Listeners;
	extern vector<Threading::Task*> MainTasks;
	extern std::mutex* ListenersMutex;
	extern std::mutex* TasksMutex;

	class Thread {
	protected:
		pthread_t threadHandle;
		struct RTData {
			bool* bStopFlag;
			int iSocket;
		};
	public:
		Thread() {}
		Thread(void*(func)(void*), void* arg);
		~Thread();
		pthread_t GetThrdHandle();
		int Join();
	};

	class TCPServerThread : virtual public Thread {
	private:
		static void* SocketServer(void* threadID);
	protected:
	public:
		TCPServerThread(int _port) : Thread(SocketServer, (void*)_port) {}
		~TCPServerThread();
	};

	class TCPReciverThread : virtual public Thread {
	private:
		static void* Recive(void* threadID);
		RTData dt;
	protected:

	public:
		bool Stoped = false;
		TCPReciverThread(int _sock) {
			dt.bStopFlag = &Stoped;
			dt.iSocket = _sock;
			pthread_create(&threadHandle, NULL, Recive, (void*)&dt);
		}
		~TCPReciverThread();
	};

	class UserInputThread : virtual public Thread {
	private:
		static void* ProceedInput(void* ptr_null);
	protected:
	public:
		UserInputThread() :Thread(ProceedInput, (void*)NULL) { }
		~UserInputThread();
	};

	class LCDControlThread : virtual public Thread {
	private:
		static void* ManageLCD(void* ptr_null);
	public:
		LCDControlThread() :Thread(ManageLCD, (void*)NULL) {}
		~LCDControlThread();
	};
}
#endif	//#ifndef _THREADING_H_
