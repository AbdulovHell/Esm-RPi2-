#ifndef _THREADING_H_
#define _THREADING_H_

namespace Threading {	
	using namespace std;

	class TCPReciverThread;
	class TCPServerThread;

	extern vector<Threading::TCPReciverThread*> Listeners;

	bool Verify(char* buf, size_t size);
	void CalcSum(char* buf, size_t size);
	
	class Thread {
	protected:
		pthread_t threadHandle;
	public:
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
		TCPServerThread(int _port) : Thread(SocketServer,(void*)_port) {}
		~TCPServerThread();
	};

	/*class UARTThread : public Thread {
	private:
		static int read_cnt;
		static void* SendUART(void* threadID);
	protected:
	public:
		UARTThread() :Thread(SendUART) {}
	};*/

	class TCPReciverThread : virtual public Thread {
	private:
		static void* Recive(void* threadID);
	protected:
	public:
		TCPReciverThread(int _sock) :Thread(Recive,(void*)_sock) { }
		~TCPReciverThread();
	};

}
#endif	//#ifndef _THREADING_H_
