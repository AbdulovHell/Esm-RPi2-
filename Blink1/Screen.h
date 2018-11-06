#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "KeyEvents.h"

namespace Display {
	using namespace std;

	class Screen {
		//��������� �� ����������������� �������
		Display* display;
		//������� �������� ������ �������
		list<DisplayString*>* Lines;
		//���������� �����
		int LinesCount;
		//����� �� ���������
		bool bScrollable;
		//������ ������� ������, ������������ �� �������
		int TopLine;
		//����?
		bool isMenu = false;
		//���������� �����������
		bool isActive = true;
		//������ ����� ����
		int HeaderLen = 0;
		//������� ������� ��������� ��� ����
		int SelectedLine = 1;

		//���������� ����� ������� ���������, �������� ��� ����� ��������� Lines
		void UpdateScrllFlag();
		//��������� ������ ���������, ���� ���������� ����� ��������� 4
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
		//������ �� 0!
		size_t SetLine(DisplayString* line, int pos);
		size_t RemoveLine(size_t num);
		int Count();

		bool isScrollable();

		void SetActive();

		void SetActive(std::function<bool(uint32_t)>);

		void ProceedMessage(KeyEvent* ev);
		//��������� ����������� �������
		void Scroll(int offset);
		void EnableMenu(int HeaderLen, int DefaultCursorPos);
		//��������� ������ ����
		void ScrollMenu(int offset);
		//������ ���������� ������
		int GetSelectedIndex();
		//������� ������ � ���������� ����
		void ReturnToPrevMenu(Display* disp, uint32_t param);
		//���������� ����������� �� �������, ������ ��� ��� ���������
		void UpdateDisplay();
		void DispatchMessage();
	};
}

#endif	//_SCREEN_H_
