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

Threading::TCPReciverThread* Listeners;

//Threading::Thread::Thread()
//{
//}

Threading::Thread::Thread(void*(func)(void*))
{
	pthread_create(&threadHandle, NULL, func, NULL);
}

Threading::Thread::~Thread()
{
	pthread_cancel(threadHandle);
}

pthread_t Threading::Thread::getHandle()
{
	return threadHandle;
}

int Threading::Thread::Join()
{
	return pthread_join(threadHandle, NULL);
}

void* Threading::TCPServerThread::SocketServer(void * threadID)
{
	struct sockaddr_in addr;
	printf("Server start on %d port\n", Threading::TCPServerThread::port);
	Threading::TCPServerThread::listener = socket(AF_INET, SOCK_STREAM, 0);
	if (Threading::TCPServerThread::listener < 0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(Threading::TCPServerThread::port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(Threading::TCPServerThread::listener, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(2);
	}

	listen(Threading::TCPServerThread::listener, 5);
	int tempsock;

	while (1) {
		tempsock = accept(Threading::TCPServerThread::listener, NULL, NULL);
		if (tempsock < 0) {
			perror("accept");
			exit(3);
		}
		//Threading::Listeners.push_back(new TCPReciverThread(tempsock));
		Listeners = new TCPReciverThread(tempsock);
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

void * Threading::TCPReciverThread::Recive(void * threadID)
{
	while (1) {
		bytes_read = recv(reciver, buf, 1024, 0);
		if (bytes_read <= 0) break;
		//проверка
		if (!Verify(buf, 1024)) break;
		//обработка
		switch (buf[1]) {	//type id
		case 0:
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

bool Threading::Verify(char * buf, size_t size)
{
	if (buf[0] != 'e')  return false;	//базовая проверка
	short len = buf[2] + (buf[3] << 8);	//получаем полезную длинну сообщения
	if (len > size - 6) return false;	//если длинна больше размера буфера (херня в len)
	uint8_t mCK_A = buf[3 + len + 1], mCK_B = buf[3 + len + 2];	//суммы, поставляемые клиентом
	uint8_t cCK_A = 0, cCK_B = 0;	//наши суммы
	for (int i = 1; i < (3 + len); i++) {//считаем
		cCK_A += (uint8_t)buf[i];
		cCK_B += cCK_A;
	}
	if ((cCK_A == mCK_A) && (cCK_B == mCK_B)) //сравниваем
		return true;	//совпали
	return false;	//облом
}

void Threading::CalcSum(char * buf, size_t size_of_msg)
{
	uint8_t CK_A = 0, CK_B = 0;
	for (size_t i = 1; i < size_of_msg - 2; i++) {
		CK_A += buf[i];
		CK_B += CK_A;
	}
	buf[size_of_msg - 1] = CK_B;
	buf[size_of_msg - 2] = CK_A;
}

Threading::TCPReciverThread::~TCPReciverThread()
{
}
