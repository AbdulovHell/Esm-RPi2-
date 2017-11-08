#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <mutex>
#include "threading.h"
#include "../src/stuff.h"

namespace Threading {
	vector<Threading::TCPReciverThread*> Listeners;
	vector<Task*> Tasks;
	std::mutex* ListenersMutex;
	std::mutex* TasksMutex;

	void * UserInputThread::ProceedInput(void * ptr_null)
	{
		char buf[512];
		printf("INPUT: Start working\n");
		while (1) {
			scanf("%s", buf);
			if (strcmp(buf, "conn") == 0) {
				printf("Count: %d\n", Listeners.size());
			}
			if (strcmp(buf, "quit") == 0) {
				printf("Terminating threads...\n");
				//place shutdown task
			}
		}
	}

	UserInputThread::~UserInputThread()
	{
		pthread_cancel(threadHandle);
	}
}

using namespace Stuff;

Threading::Thread::Thread(void*(func)(void*), void* arg)
{
	pthread_create(&threadHandle, NULL, func, arg);
}

Threading::Thread::~Thread()
{
	pthread_cancel(threadHandle);
}

pthread_t Threading::Thread::GetThrdHandle()
{
	return threadHandle;
}

int Threading::Thread::Join()
{
	return pthread_join(threadHandle, NULL);
}

void* Threading::TCPServerThread::SocketServer(void * param)
{
	int port = (int)param;
	int listener = 0;
	struct sockaddr_in addr;
	printf("SERVER: Server start on %d port\n", port);
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
		printf("SERVER: Accept connection\n");
		ListenersMutex->lock();
		Listeners.push_back(new TCPReciverThread(tempsock));
		ListenersMutex->unlock();
	}
}

Threading::TCPServerThread::~TCPServerThread()
{
	pthread_cancel(threadHandle);
}

void * Threading::TCPReciverThread::Recive(void * param)
{
	RTData* data = (RTData*)param;
	int reciver = data->iSocket;
	bool* stopflag = data->bStopFlag;
	char buf[1024];
	int bytes_read;
	printf("RECV(%d): Start working\n", reciver);
	while (1) {
		bytes_read = recv(reciver, buf, 1024, 0);
		if (bytes_read <= 0) break;
		//проверка
		if (!Verify(buf)) break;
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
			CalcSum(buf, 8);
			send(reciver, buf, 1024, 0);
		}
		break;
		case 2:
		{
			printf("RECV(%d): Place task -> Set ch %d att %d\n", reciver, buf[4], buf[5]);
			TasksMutex->lock();
			Tasks.push_back(new Task(buf[5], buf[4]));
			TasksMutex->unlock();
		}
		break;
		default:
			break;
		}
	}
	printf("RECV(%d): Stop working\n", reciver);
	*stopflag = true;
	close(reciver);
}

Threading::TCPReciverThread::~TCPReciverThread()
{
}
