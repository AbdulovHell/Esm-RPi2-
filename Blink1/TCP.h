#ifndef _TCP_H_
#define _TCP_H_

#include "threading.h"

namespace Threading {
	class TCPReciverThread;

	extern std::vector<Threading::TCPReciverThread*> Listeners;
	extern std::mutex* ListenersMutex;

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
}

#endif
