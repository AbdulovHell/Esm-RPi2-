#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>

#include "usart.h"

IO::Usart::Usart(int baud)
{
	fd = serialOpen("/dev/ttyAMA0", baud);
	if (fd == -1) {
		error_str = "UART Open error";
	}
	else {
		opened = true;
	}
}

void IO::Usart::Write(char * buf, size_t size)
{
	if (!opened) return;
	for (int i = 0; i < size; i++)
		serialPutchar(fd, buf[i]);
}

int IO::Usart::DataAvail()
{
	if (!opened) return 0;
	return serialDataAvail(fd);
}

void IO::Usart::Read(char * buf, size_t size)
{
	if (!opened) return;
	for (int i = 0; i < size; i++) {
		buf[i] = serialGetchar(fd);
	}
}

bool IO::Usart::IsOpen()
{
	return opened;
}

std::string IO::Usart::GetError()
{
	return error_str;
}

int IO::Usart::GetFileDesc()
{
	return fd;
}

IO::Usart::~Usart()
{
	if (opened)
		serialClose(fd);
}
