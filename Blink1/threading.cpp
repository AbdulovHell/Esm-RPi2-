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

Threading::Thread::Thread()
{
}

Threading::Thread::Thread(void*(func)(void*))
{
	pthread_create(&threadHandle, NULL, func, NULL);
}

Threading::Thread::~Thread()
{
}

pthread_t Threading::Thread::getHandle()
{
	return threadHandle;
}

void Threading::Thread::Terminate()
{
	//pthread_exit();
}

void Threading::Thread::Join()
{
	pthread_join(threadHandle, NULL);
}

void* Threading::TCPServerThread::SocketServer(void * threadID)
{
	printf("Server start on %d port\n",port);
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
		Threading::Listeners.push_back(new TCPReciverThread(tempsock));
	}
}

Threading::TCPServerThread::TCPServerThread()
{
	port = 4550;
	pthread_create(&threadHandle, NULL, SocketServer, NULL);
}

Threading::TCPServerThread::TCPServerThread(int _port)
{
	port = _port;
	pthread_create(&threadHandle, NULL, SocketServer, NULL);
}

void * Threading::TCPReciverThread::Recive(void * threadID)
{
	while (1) {
		bytes_read = recv(reciver, buf, 1024, 0);
		if (bytes_read <= 0) break;
		//проверка, ну пока так
		if (buf[0] != 45 || buf[1] != 50) break;
		//обработка
		switch (buf[2]) {	//class id
		case 0:
		{
			switch (buf[3]) {	//message id
			case 0:
			{

			}
			break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
	}
	close(reciver);
}

Threading::TCPReciverThread::TCPReciverThread(int _sock)
{
	reciver = _sock;
	pthread_create(&threadHandle, NULL, Recive, NULL);
}

void * Threading::UARTThread::SendUART(void * threadID)
{
	int fd = serialOpen("/dev/ttyAMA0", 9600);
	if (fd == -1)
		return 0;

	while (1) {
		read_cnt = serialDataAvail(fd);
		if (read_cnt > 0) {
			char* buf = new char[read_cnt + 1];
			for (int i = 0; i < read_cnt; i++)
				buf[i] = serialGetchar(fd);
			buf[read_cnt] = 0;
			serialPuts(fd, buf);
		}
	}
	serialClose(fd);
}
