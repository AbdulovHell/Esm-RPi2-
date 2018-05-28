#ifndef _SCREEN_H_
#define _SCREEN_H_

namespace Display {
	using namespace std;
	
	class Screen {
		//Указатель на инициализированый дисплей
		Display* display;
		//Текущие активные строки дисплея
		list<DisplayString*>* Lines;
		//Количество строк
		int LinesCount;
		//Нужна ли прокрутка
		bool bScrollable;
		//Индекс верхней строки, отображаемой на дисплее
		int TopLine;
		//Меню?
		bool isMenu=false;
		//Длинна шапки меню
		int HeaderLen = 0;
		//Текущая позиция указателя для меню
		int SelectedLine = 1;

		//Обновление флага наличия прокрутки, вызывать при любом изменении Lines
		void UpdateScrllFlag();
		//Обновление изображения на дисплее, каждый раз при изменении
		void UpdateDisplay();
		//Отрисовка полосы прокрутки, если количество строк превышает 4
		void DrawScrollBar();

	public:
		Screen(Display* disp);

		DisplayString* operator[] (size_t c);

		size_t AddLine(DisplayString* txt);
		size_t AddLine(DisplayString* txt, size_t pos);
		size_t SetLine(DisplayString* line, int pos);
		size_t RemoveLine(size_t num);
		int Count();
		bool isScrollable();
		void SetActive();
		//Прокрутка содержимого дисплея
		void Scroll(int offset);
		void EnableMenu(int HeaderLen,int DefaultCursorPos);
		//Прокрутка пункта меню
		void ScrollMenu(int offset);
		//
		int GetSelectedIndex();
	};
}

#endif	//_SCREEN_H_
