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
		//отправка данных по uart
		void Write(char* buf, size_t size);
		//получение количества байт в буфере приема
		int DataAvail();
		//чтение байт из буфера приема
		void Read(char* buf, size_t size);
		//возвращает состояние порта
		bool IsOpen();
		//возвращает текс ошибки
		std::string GetError();
		//return Linux file desc for using in read(), write().
		int GetFileDesc();
		//деструктор с закрытием порта
		~Usart();
	};
}
#endif	// _USART_H_

