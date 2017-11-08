#ifndef _THREADING_H_
#define _THREADING_H_

namespace Threading {
	using namespace std;

	class TCPReciverThread;
	class TCPServerThread;
	class Task;

	extern vector<Threading::TCPReciverThread*> Listeners;
	extern vector<Task*> Tasks;
	extern std::mutex* ListenersMutex;
	extern std::mutex* TasksMutex;

	class Task {
		uint8_t att;
		uint8_t ch;
	public:
		Task(uint8_t _att, uint8_t _ch) { att = _att; ch = _ch; }

		uint8_t Att() { return att; }
		uint8_t Ch() { return ch; }
	};

	class Thread {
	protected:
		pthread_t threadHandle;
		struct RTData {
			bool* bStopFlag;
			int iSocket;
		};
	public:
		Thread(){}
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
}
#endif	//#ifndef _THREADING_H_
