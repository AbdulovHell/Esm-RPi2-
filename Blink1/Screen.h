#ifndef _SCREEN_H_
#define _SCREEN_H_

namespace LCD {
	using namespace std;

	extern mutex ScreenMutex;
	extern vector<Threading::Task*> LCDTasks;

	class Screen {
		//��������� �� ����������������� �������
		LCD::Display* display;
		//������� �������� ������ �������
		list<LCDString*>* Lines;
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
		Screen(LCD::Display* disp);

		LCDString* operator[] (size_t c);

		size_t AddLine(LCDString* txt);
		size_t AddLine(LCDString* txt, size_t pos);
		size_t SetLine(LCDString* line, int pos);
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
