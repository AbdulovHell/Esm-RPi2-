#ifndef _USART_H_
#define _USART_H_

namespace IO {
	//UART support library
	class Usart {
		int fd = -1;
		std::string error_str;
		bool opened = false;

	public:
		//baud - 9600, 19200, etc. bps
		Usart(int baud);
		//�������� ������ �� uart
		void Write(char* buf, size_t size);
		//��������� ���������� ���� � ������ ������
		int DataAvail();
		//������ ���� �� ������ ������
		void Read(char* buf, size_t size);
		//���������� ��������� �����
		bool IsOpen();
		//���������� ���� ������
		std::string GetError();
		//return Linux file desc for using in read(), write().
		int GetFileDesc();
		//���������� � ��������� �����
		~Usart();
	};
}
#endif	// _USART_H_

