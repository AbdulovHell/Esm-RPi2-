#include "sys_headers.h"

#include "Colorize.h"
#include "stuff.h"
#include "TCP.h"
#include "main.h"
#include "Task.h"

namespace Threading {
	std::vector<Threading::TCPReciverThread*> Listeners;
	std::mutex* ListenersMutex;

	void* TCPServerThread::SocketServer(void * param)
	{
		uint16_t port = (uint16_t)(int)param;
		int listener = 0;
		struct sockaddr_in addr;
		printf("%s: Server start on %d port\n", Stuff::MakeColor("SERVER", Stuff::Red).c_str(), port);
		listener = socket(AF_INET, SOCK_STREAM, 0);
		if (listener < 0)
		{
			perror("socket");
			exit(1);
		}

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			perror("bind");
			exit(2);
		}

		listen(listener, 5);
		int tempsock;

		while (1) {
			tempsock = accept(listener, NULL, NULL);
			if (tempsock < 0) {
				perror("accept");
				exit(3);
			}
			printf("%s: Accept connection\n", Stuff::MakeColor("SERVER", Stuff::Red).c_str());
			ListenersMutex->lock();
			Listeners.push_back(new TCPReciverThread(tempsock));
			ListenersMutex->unlock();
		}
	}

	TCPServerThread::~TCPServerThread()
	{
		pthread_cancel(threadHandle);
	}

	void * TCPReciverThread::Recive(void * param)
	{
		RTData* data = (RTData*)param;
		int reciver = data->iSocket;
		bool* stopflag = data->bStopFlag;
		char buf[1024];
		int bytes_read;
		printf("%s(%d): Start working\n", Stuff::MakeColor("RECV", Stuff::Yellow).c_str(), reciver);
		while (1) {
			bytes_read = recv(reciver, buf, 1024, 0);
			if (bytes_read <= 0) break;
			//проверка
			if (!Stuff::Verify(buf)) break;
			//обработка
			switch (buf[1]) {	//type id
			case 1:
			{
				buf[0] = 'e';
				buf[1] = 1;
				buf[2] = 2;
				buf[3] = 0;
				buf[4] = 'O';
				buf[5] = 'K';
				Stuff::CalcSum(buf, 8);
				send(reciver, buf, 1024, 0);
			}
			break;
			case 2:
			{
				printf("%s(%d): Place task -> Set ch %d att %d\n", Stuff::MakeColor("RECV", Stuff::Yellow).c_str(), reciver, buf[4], buf[5]);
				TasksMutex->lock();
				MainTasks.push_back(new TaskSetAttCh(buf[5], buf[4]));
				TasksMutex->unlock();
			}
			break;
			default:
				break;
			}
		}
		printf("%s(%d): Stop working\n", Stuff::MakeColor("RECV", Stuff::Yellow).c_str(), reciver);
		*stopflag = true;
		close(reciver);
		return 0;
	}

	TCPReciverThread::~TCPReciverThread()
	{
		pthread_cancel(threadHandle);
	}
}