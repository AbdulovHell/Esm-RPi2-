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
		static struct sockaddr_in addr;
		static int port;
	protected:
	public:
		static void* SocketServer(void* threadID);
		TCPServerThread(int _port) : Thread(SocketServer) { this->port = _port; }
		//~TCPServerThread() : ~Thread() {}
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
	protected:
	public:
		static void* Recive(void* threadID);
		TCPReciverThread(int _sock) :Thread(Recive) { reciver = _sock; }
	};

	//vector<string> outStrs;
	bool Verify(char* buf, size_t size);
	void CalcSum(char* buf, size_t size);
}
#endif	//#ifndef _THREADING_H_
