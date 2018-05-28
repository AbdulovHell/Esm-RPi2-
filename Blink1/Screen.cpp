#include "sys_headers.h"

#include "Display.h"
#include "Screen.h"

void Display::Screen::UpdateScrllFlag()
{
	if (Lines->size() > 4) {
		bScrollable = true;
	}
	else {
		bScrollable = false;
	}
}

void Display::Screen::UpdateDisplay()
{
	display->SetScreen(Lines, TopLine);
	DrawScrollBar();
	if (isMenu) {
		display->Power(1, Display::Cursor::NoCursor_SymbolFlashing);
		display->SetCursorPos(SelectedLine - TopLine, 1);
	}
}

void Display::Screen::DrawScrollBar()
{
	if (!bScrollable) return;
#define ONELINE 1
#define TWOLINE 3
	//������ ������ 8*4=32 , 4 ������
	int doubleline = 128 / LinesCount;
	int oneline = 32 - doubleline;
	int parts = oneline / (LinesCount - 4);
	int top = parts * TopLine;
	if (TopLine == LinesCount - 4)
		top = oneline;
	display->SetCGRAMAddr(0x0);
	for (int i = 0; i < top; i++) {
		display->SendText(ONELINE);
	}
	for (int i = top; i < top + doubleline; i++) {
		display->SendText(TWOLINE);
	}
	for (int i = top + doubleline; i < 32; i++) {
		display->SendText(ONELINE);
	}

	for (int i = 0; i < 4; i++) {
		display->SetCursorPos(i + 1, 20);
		display->SendText((char)i);
	}
}

Display::Screen::Screen(Display * disp)
{
	Lines = new list<DisplayString*>;
	LinesCount = Lines->size();
	bScrollable = false;
	TopLine = 0;
	display = disp;
}

Display::DisplayString* Display::Screen::operator[](size_t c)
{
	list<DisplayString*>::iterator it = Lines->begin();
	std::advance(it, c);
	return *it;
}

size_t Display::Screen::AddLine(DisplayString * txt)
{
	Lines->push_back(txt);
	LinesCount = Lines->size();
	UpdateScrllFlag();
	//UpdateDisplay();
	return LinesCount;
}

size_t Display::Screen::AddLine(DisplayString * txt, size_t pos)
{
	std::list<DisplayString*>::iterator it = Lines->begin();
	std::advance(it, pos);
	Lines->emplace(it, txt);
	LinesCount = Lines->size();
	UpdateScrllFlag();
	//UpdateDisplay();
	return LinesCount;
}

int Display::Screen::Count()
{
	return (int)LinesCount;
}

size_t Display::Screen::RemoveLine(size_t num)
{
	std::list<DisplayString*>::iterator it = Lines->begin();
	std::advance(it, num);
	Lines->erase(it);
	LinesCount = Lines->size();
	UpdateScrllFlag();
	//UpdateDisplay();
	return LinesCount;
}

bool Display::Screen::isScrollable()
{
	return bScrollable;
}

void Display::Screen::SetActive()
{
	UpdateDisplay();
}

void Display::Screen::Scroll(int offset)
{
	//if (TopLine + offset >= LinesCount || TopLine + offset < 0) return;
	TopLine += offset;
	if (TopLine + 4 > LinesCount) {
		TopLine -= (TopLine + 4) - LinesCount;
	}
	if (TopLine < 0) TopLine = 0;
	UpdateDisplay();
}

void Display::Screen::EnableMenu(int headerLen, int DefaultCursorPos)
{
	if (DefaultCursorPos > 4 || DefaultCursorPos < 1) {
		DefaultCursorPos = headerLen + 1;
	}

	isMenu = true;
	HeaderLen = headerLen;
	SelectedLine = DefaultCursorPos;
}

void Display::Screen::ScrollMenu(int offset)
{
	if (!isMenu) return;

	int PrevSL = SelectedLine;
	SelectedLine += offset;
	if (SelectedLine < 1 + HeaderLen)
		SelectedLine = 1 + HeaderLen;

	if (SelectedLine > LinesCount)
		SelectedLine = LinesCount;
	int Diff = SelectedLine - PrevSL;

	if (Diff < 0) {
		if (SelectedLine < TopLine + 1) {
			Scroll(Diff);
		}
		if (SelectedLine <= HeaderLen + 1) {
			Scroll(-TopLine);
		}
	}
	else if (Diff > 0) {
		if (SelectedLine > TopLine + 4) {
			Scroll(Diff);
		}
	}
	else {	//Diff==0
		return;
	}
	int CrPos = SelectedLine - TopLine;
	display->SetCursorPos(CrPos, 1);
}

int Display::Screen::GetSelectedIndex()
{
	return SelectedLine;
}

size_t Display::Screen::SetLine(DisplayString* line, int pos)
{
	if (pos > LinesCount - 1) {
		return AddLine(line);
	}
	std::list<DisplayString*>::iterator it = Lines->begin();
	std::advance(it, pos);
	delete *it;
	*it = line;
	LinesCount = Lines->size();
	UpdateScrllFlag();
	return LinesCount;
}
