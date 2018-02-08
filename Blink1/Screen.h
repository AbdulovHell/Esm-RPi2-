#ifndef _SCREEN_H_
#define _SCREEN_H_

namespace LCD {
	using namespace std;

	class Screen {
		//��������� �� ����������������� �������
		LCD::Display* display;
		//������� �������� ������ �������
		list<LCDString*>* Lines;
		//���������� �����
		size_t LinesCount;
		//����� �� ���������
		bool bScrollable;
		//������ ������� ������, ������������ �� �������
		size_t TopLine;

		//���������� ����� ������� ���������, �������� ��� ����� ��������� Lines
		void UpdateScrllFlag();
		//���������� ����������� �� �������, ������ ��� ��� ���������
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
