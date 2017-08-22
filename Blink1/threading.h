#ifndef _THREADING_H_
#define _THREADING_H_

namespace Threading {	
	using namespace std;

	class TCPReciverThread;
	class TCPServerThread;

	bool Verify(char* buf, size_t size);
	void CalcSum(char* buf, size_t size);
	void* SocketServer(void* threadID);
	TCPServerThread** GetServerThreadP();

	class Thread {
	protected:
		pthread_mutex_t* start_mutex;
		pthread_t threadHandle;
	public:
		void Start();
		Thread(void*(func)(void*));
		~Thread();
		pthread_t GetThrdHandle();
		int Join();
	};

	class TCPServerThread : virtual public Thread {
	private:
	protected:
	public:
		int listener;
		int port;

		TCPServerThread(int _port) : Thread(Threading::SocketServer) { port = _port; }
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

}
#endif	//#ifndef _THREADING_H_
