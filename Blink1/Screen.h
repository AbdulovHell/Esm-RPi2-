#ifndef _SCREEN_H_
#define _SCREEN_H_

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
		bool isMenu=false;
		//������ ����� ����
		int HeaderLen = 0;
		//������� ������� ��������� ��� ����
		int SelectedLine = 1;

		//���������� ����� ������� ���������, �������� ��� ����� ��������� Lines
		void UpdateScrllFlag();
		//���������� ����������� �� �������, ������ ��� ��� ���������
		void UpdateDisplay();
		//��������� ������ ���������, ���� ���������� ����� ��������� 4
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
		//��������� ����������� �������
		void Scroll(int offset);
		void EnableMenu(int HeaderLen,int DefaultCursorPos);
		//��������� ������ ����
		void ScrollMenu(int offset);
		//
		int GetSelectedIndex();
	};
}

#endif	//_SCREEN_H_
