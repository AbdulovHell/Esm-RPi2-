#ifndef _THREADING_H_
#define _THREADING_H_

namespace Threading {	
	using namespace std;

	class TCPReciverThread;

	vector<TCPReciverThread*> Listeners;
	vector<string> outStrs;
	

	class Thread {
	protected:
		pthread_t threadHandle;
	public:
		Thread();
		Thread(void*(func)(void*));
		~Thread();
		pthread_t getHandle();
		void Terminate();
		void Join();
	};

	class TCPServerThread : public Thread {
	private:
		static int listener;
		static struct sockaddr_in addr;
		static void* SocketServer(void* threadID);
		static int port;
	protected:
	public:
		TCPServerThread();
		TCPServerThread(int _port);
	};

	class UARTThread : public Thread {
	private:
		static int read_cnt;
		static void* SendUART(void* threadID);
	protected:
	public:
		UARTThread() :Thread(SendUART) {}
	};

	class TCPReciverThread : public Thread {
	private:
		static int reciver;
		static char buf[1024];
		static int bytes_read;
		static void* Recive(void* threadID);
	protected:
	public:
		TCPReciverThread(int _sock);
	};
}
#endif	//#ifndef _THREADING_H_
