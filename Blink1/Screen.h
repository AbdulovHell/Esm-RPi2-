#ifndef _SCREEN_H_
#define _SCREEN_H_

namespace LCD {
	using namespace std;

	class Screen {
		//Указатель на инициализированый дисплей
		LCD::Display* display;
		//Текущие активные строки дисплея
		list<LCDString*>* Lines;
		//Количество строк
		size_t LinesCount;
		//Нужна ли прокрутка
		bool bScrollable;
		//Индекс верхней строки, отображаемой на дисплее
		size_t TopLine;

		//Обновление флага наличия прокрутки, вызывать при любом изменении Lines
		void UpdateScrllFlag();
		//Обновление изображения на дисплее, каждый раз при изменении
		void UpdateDisplay();

	public:
		Screen(LCD::Display* disp);

		LCDString* operator[] (size_t c);

		size_t AddLine(LCDString* txt);
		size_t AddLine(LCDString* txt, size_t pos);
		size_t Count();
		size_t RemoveLine(size_t num);
		bool isScrollable();

		void SetLine(uint8_t line, char* str);
		void SetLine(uint8_t line, const char* str);
	};

}

#endif	//_SCREEN_H_
