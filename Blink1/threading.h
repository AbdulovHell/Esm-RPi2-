#ifndef _THREADING_H_
#define _THREADING_H_

namespace Threading {	
	using namespace std;

	class Thread {
	protected:
		pthread_t threadHandle;
	public:
		Thread(void*(func)(void*));
		~Thread();
		pthread_t getHandle();
		int Join();
	};

	class TCPServerThread : virtual public Thread {
	private:
		static int listener;
		static int port;
		static void* SocketServer(void* threadID);
	protected:
	public:
		TCPServerThread(int _port) : Thread(SocketServer) { port = _port; }
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
		static int reciver;
		static char buf[1024];
		static int bytes_read;
		static void* Recive(void* threadID);
	protected:
	public:
		TCPReciverThread(int _sock) :Thread(Recive) { reciver = _sock; }
		~TCPReciverThread();
	};

	//vector<string> outStrs;
	bool Verify(char* buf, size_t size);
	void CalcSum(char* buf, size_t size);
	
	//typedef int SOCK_HANDLE;
}
#endif	//#ifndef _THREADING_H_
