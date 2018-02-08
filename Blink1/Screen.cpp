#include <stdint.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>
#include <string.h>
#include <list>

#include "LCD.h"
#include "Screen.h"

void LCD::Screen::UpdateScrllFlag()
{
	if (Lines->size() > 4) {
		bScrollable = true;
	}
	else {
		bScrollable = false;
	}
}

void LCD::Screen::UpdateDisplay()
{
	display->SetScreen(Lines, TopLine);
}

LCD::Screen::Screen(LCD::Display * disp)
{
	Lines = new list<LCDString*>;
	LinesCount = Lines->size();
	bScrollable = false;
	TopLine = 0;
	display = disp;
}

LCD::LCDString* LCD::Screen::operator[](size_t c)
{
	list<LCDString*>::iterator it=Lines->begin();
	std::advance(it, c);
	return *it;
}

size_t LCD::Screen::AddLine(LCDString * txt)
{
	Lines->push_back(txt);
	UpdateScrllFlag();
	return Lines->size();
}

size_t LCD::Screen::AddLine(LCDString * txt, size_t pos)
{
	std::list<LCDString*>::iterator it = Lines->begin();
	std::advance(it, pos);
	Lines->emplace(it, txt);
	UpdateScrllFlag();
	return Lines->size();
}

size_t LCD::Screen::Count()
{
	return LinesCount;
}

size_t LCD::Screen::RemoveLine(size_t num)
{
	std::list<LCDString*>::iterator it = Lines->begin();
	std::advance(it, num);
	Lines->erase(it);
	UpdateScrllFlag();
	return Lines->size();
}

bool LCD::Screen::isScrollable()
{
	return bScrollable;
}

void LCD::Screen::SetLine(uint8_t line, char * str)
{
	
}

void LCD::Screen::SetLine(uint8_t line, const char * str)
{
	
}
