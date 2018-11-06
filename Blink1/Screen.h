#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "KeyEvents.h"

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
		bool isMenu = false;
		//Активность обработчика
		bool isActive = true;
		//Длинна шапки меню
		int HeaderLen = 0;
		//Текущая позиция указателя для меню
		int SelectedLine = 1;

		//Обновление флага наличия прокрутки, вызывать при любом изменении Lines
		void UpdateScrllFlag();
		//Отрисовка полосы прокрутки, если количество строк превышает 4
		void DrawScrollBar();

		Display::Cursor cursor;

		

		std::function<void(Display*, uint32_t)> UpKeyCallback;
		std::function<void(Display*, uint32_t)> DownKeyCallback;
		std::function<void(Display*, uint32_t)> LeftKeyCallback;
		std::function<void(Display*, uint32_t)> RightKeyCallback;

	public:
		Screen(Display* disp);
		Screen(Display* disp,Display::Cursor curs);

		DisplayString* operator[] (size_t c);

		size_t AddLine(DisplayString* txt);
		size_t AddLine(DisplayString* txt, size_t pos);
		//отсчет от 0!
		size_t SetLine(DisplayString* line, int pos);
		size_t RemoveLine(size_t num);
		int Count();

		bool isScrollable();

		void SetActive();

		void SetActive(std::function<bool(uint32_t)>);

		void ProceedMessage(KeyEvent* ev);
		//Прокрутка содержимого дисплея
		void Scroll(int offset);
		void EnableMenu(int HeaderLen, int DefaultCursorPos);
		//Прокрутка пункта меню
		void ScrollMenu(int offset);
		//Индекс выбранного пункта
		int GetSelectedIndex();
		//Функция выхода в предыдущее меню
		void ReturnToPrevMenu(Display* disp, uint32_t param);
		//Обновление изображения на дисплее, каждый раз при изменении
		void UpdateDisplay();
		void DispatchMessage();
	};
}

#endif	//_SCREEN_H_
