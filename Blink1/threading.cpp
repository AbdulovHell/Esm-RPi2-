#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <wiringSerial.h>
#include "threading.h"
#include "../src/stuff.h"

namespace Threading {
	vector<Threading::TCPReciverThread*> Listeners;
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
	printf("Server start on %d port\n", port);
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
		Listeners.push_back(new TCPReciverThread(tempsock));
	}
}

Threading::TCPServerThread::~TCPServerThread()
{
	pthread_cancel(threadHandle);
}

//Threading::TCPServerThread::TCPServerThread(int _port)
//{
//	port = _port;
//	pthread_create(&threadHandle, NULL, SocketServer, NULL);
//}

void * Threading::TCPReciverThread::Recive(void * param)
{
	int reciver = (int)param;
	char buf[1024];
	int bytes_read;
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
		default:
			break;
		}
	}
	close(reciver);
}

//Threading::TCPReciverThread::TCPReciverThread(int _sock)
//{
//	reciver = _sock;
//	pthread_create(&threadHandle, NULL, Recive, NULL);
//}

//void * Threading::UARTThread::SendUART(void * threadID)
//{
//	int fd = serialOpen("/dev/ttyAMA0", 9600);
//	if (fd == -1)
//		return 0;
//
//	while (1) {
//		read_cnt = serialDataAvail(fd);
//		if (read_cnt > 0) {
//			char* buf = new char[read_cnt + 1];
//			for (int i = 0; i < read_cnt; i++)
//				buf[i] = serialGetchar(fd);
//			buf[read_cnt] = 0;
//			serialPuts(fd, buf);
//		}
//	}
//	serialClose(fd);
//}

Threading::TCPReciverThread::~TCPReciverThread()
{
}
