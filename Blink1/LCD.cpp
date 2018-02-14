#include <stdint.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>
#include <string.h>
#include <list>

#include "LCD.h"

void LCD::Display::SendByte(uint8_t byte)
{
	digitalWrite(__E, 1);

	digitalWrite(__D7, byte & 0b10000000);
	digitalWrite(__D6, byte & 0b01000000);
	digitalWrite(__D5, byte & 0b00100000);
	digitalWrite(__D4, byte & 0b00010000);

	std::this_thread::sleep_for(std::chrono::nanoseconds(500));
	digitalWrite(__E, 0);
	std::this_thread::sleep_for(std::chrono::nanoseconds(500));

	digitalWrite(__E, 1);

	digitalWrite(__D7, byte & 0b00001000);
	digitalWrite(__D6, byte & 0b00000100);
	digitalWrite(__D5, byte & 0b00000010);
	digitalWrite(__D4, byte & 0b00000001);

	std::this_thread::sleep_for(std::chrono::nanoseconds(500));
	digitalWrite(__E, 0);
	std::this_thread::sleep_for(std::chrono::nanoseconds(500));
}

void LCD::Display::Send4Bit(uint8_t byte)
{
	digitalWrite(__E, 1);

	digitalWrite(__D7, byte & 0b00001000);
	digitalWrite(__D6, byte & 0b00000100);
	digitalWrite(__D5, byte & 0b00000010);
	digitalWrite(__D4, byte & 0b00000001);

	std::this_thread::sleep_for(std::chrono::nanoseconds(500));
	digitalWrite(__E, 0);
	std::this_thread::sleep_for(std::chrono::nanoseconds(500));
}

void LCD::Display::SetDDRAMAddress(int addr)
{
	uint8_t msg = 0b10000000;
	msg |= (uint8_t)addr;
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void LCD::Display::FunctionSet()
{
	uint8_t msg = 0b00101010;
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

LCD::Display::Display()
{
	char buf[200];
	sprintf(buf, "gpio export %d output && gpio export %d output && gpio export %d output && gpio export %d output && gpio export %d output && gpio export %d output", __RS, __E, __D4, __D5, __D6, __D7);
	system(buf);
	wiringPiSetupSys();

	digitalWrite(__RS, 0);
	digitalWrite(__E, 0);
	digitalWrite(__D4, 0);
	digitalWrite(__D5, 0);
	digitalWrite(__D6, 0);
	digitalWrite(__D7, 0);
}

LCD::Display::Display(int E, int RS, int D4, int D5, int D6, int D7)
{
	__E = E;
	__RS = RS;
	__D4 = D4;
	__D5 = D5;
	__D6 = D6;
	__D7 = D7;

	char buf[200];
	sprintf(buf, "gpio export %d output && gpio export %d output && gpio export %d output && gpio export %d output && gpio export %d output && gpio export %d output", __RS, __E, __D4, __D5, __D6, __D7);
	system(buf);
	wiringPiSetupSys();

	digitalWrite(__RS, 0);
	digitalWrite(__E, 0);
	digitalWrite(__D4, 0);
	digitalWrite(__D5, 0);
	digitalWrite(__D6, 0);
	digitalWrite(__D7, 0);
}

void LCD::Display::Init()
{
	//начало ритуала
	Send4Bit(0b0011);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
	Send4Bit(0b0011);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
	Send4Bit(0b0011);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
	Send4Bit(0b0010);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
	//Общие команды
	FunctionSet();
	Power(1, Cursor::Cursor_Flashing);
	Clear();
	EntryModeSet(1, 0);
}

void LCD::Display::Clear()
{
	SendByte(0b00000001);
	std::this_thread::sleep_for(std::chrono::microseconds(1500));
}

void LCD::Display::ReturnHome()
{
	SendByte(0b00000010);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void LCD::Display::EntryModeSet(uint8_t ID, uint8_t SH)
{
	uint8_t msg = 0b00000100;
	msg |= (uint8_t)(ID << 1);
	msg |= SH;
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void LCD::Display::Power(uint8_t state, Cursor cursor)
{
	CurrentCursorType = cursor;
	uint8_t msg = 0b00001000;
	msg |= (uint8_t)(state << 2);
	msg |= (uint8_t)cursor;
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void LCD::Display::CursorShift(uint8_t RL)
{
	uint8_t msg = 0b00010000;
	msg |= (uint8_t)(RL << 2);
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void LCD::Display::DisplayShift(uint8_t RL)
{
	uint8_t msg = 0b00011000;
	msg |= (uint8_t)(RL << 2);
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

void LCD::Display::WriteDataToRAM(uint8_t msg)
{
	digitalWrite(__RS, 1);
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
	digitalWrite(__RS, 0);
}

void LCD::Display::SetCursorPos(int str, int col)
{
	if (str < 1 || str>4 || col < 1 || col>20) return;
	CurrentCol = col;
	CurrentStr = str;
	col--;
	switch (str) {
	case 1:
		SetDDRAMAddress(col);
		break;
	case 2:
		SetDDRAMAddress(col + 0x40);
		break;
	case 3:
		SetDDRAMAddress(col + 0x14);
		break;
	case 4:
		SetDDRAMAddress(col + 0x54);
		break;
	default:
		break;
	}
}

void LCD::Display::SendText(char * txt, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		WriteDataToRAM(txt[i]);
	}
}

void LCD::Display::SendText(char txt)
{
	WriteDataToRAM(txt);
}

void LCD::Display::LoadSymbol(CustomSymbol symb, uint8_t pos)
{
	if (pos < 0 || pos>7) return;
	SetCGRAMAddr(pos << (uint8_t)3);
	WriteDataToRAM(symb.Rows[0]);
	WriteDataToRAM(symb.Rows[1]);
	WriteDataToRAM(symb.Rows[2]);
	WriteDataToRAM(symb.Rows[3]);
	WriteDataToRAM(symb.Rows[4]);
	WriteDataToRAM(symb.Rows[5]);
	WriteDataToRAM(symb.Rows[6]);
	WriteDataToRAM(symb.Rows[7]);
}

void LCD::Display::SetScreen(list<LCDString*>* strs, size_t topline)
{
	list<LCDString*>::iterator it = strs->begin();
	if (topline > 0) std::advance(it, topline);
	
	for (size_t i = topline; i < topline + 4 && i < (strs->size()); i++, it++) {
		SetCursorPos(i - topline + 1, 1);
		LCDString* str = *it;
		const char* temp = str->GetString();
		for (int j = 0; j < 20; j++) {
			WriteDataToRAM(temp[j]);
		}
	}

	/*for (int i = 1; i < 5; i++) {
		SetCursorPos(i, 1);
		list<LCDString*>::iterator it = strs->begin();

		LCDString* str;
		if (*it != 0) {
			str = *it;
			const char* temp = str->GetString();
			for (int j = 0; j < 20; j++) {
				WriteDataToRAM(temp[j]);
			}
		}
		else {

		}
	}*/
}

void LCD::Display::SetCGRAMAddr(int addr)
{
	uint8_t msg = 0b01000000;
	msg |= (uint8_t)addr;
	SendByte(msg);
	std::this_thread::sleep_for(std::chrono::microseconds(50));
}

LCD::CustomSymbol LCD::CustomSymbol::operator=(char * other)
{
	for (int i = 0; i < 8; i++)
		Rows[i] = *(other + i);
	return *this;
}

LCD::CustomSymbol LCD::CustomSymbol::operator=(CustomSymbol other)
{
	Rows[0] = other.Rows[1];
	Rows[1] = other.Rows[1];
	Rows[2] = other.Rows[2];
	Rows[3] = other.Rows[3];
	Rows[4] = other.Rows[4];
	Rows[5] = other.Rows[5];
	Rows[6] = other.Rows[6];
	Rows[7] = other.Rows[7];
	return *this;
}

char LCD::LCDString::ToP1(wchar_t symb)
{
	if (symb >= 0x410 && symb <= 0x044F) {
		return (char)(symb - 0x410 + 0xC0);
	}
	else {
		return (char)symb;
	}
}

LCD::LCDString::LCDString(const char * txt)
{
	//default align = left
	int len = (int)strlen(txt);
	if (len > 20) len = 20;
	for (int i = 0; i < len; i++)
		str[i] = txt[i];
	for (int i = len; i < 20; i++) {
		str[i] = ' ';
	}
}

LCD::LCDString::LCDString(const char * txt, Alignment at)
{
	int len = (int)strlen(txt);
	if (len > 20) len = 20;
	switch (at)
	{
	case LCD::LCDString::Alignment::Left:
		for (int i = 0; i < len; i++)
			str[i] = txt[i];
		for (int i = len; i < 20; i++) {
			str[i] = ' ';
		}
		break;
	case LCD::LCDString::Alignment::Right:
		for (int i = 0; i < 20 - len; i++)
			str[i] = ' ';
		for (int i = 20 - len; i < 20; i++) {
			str[i] = txt[i - (20 - len)];
		}
		break;
	case LCD::LCDString::Alignment::Center:
	{
		int leftspace = (20 - len) / 2;
		//int rightspace = (20 - len) / 2 + (20 - len) % 2;
		for (int i = 0; i < leftspace; i++) {
			str[i] = ' ';
		}
		for (int i = leftspace; i < leftspace + len; i++) {
			str[i] = txt[i - leftspace];
		}
		for (int i = leftspace + len; i < 20; i++) {
			str[i] = ' ';
		}
	}
	break;
	default:
		break;
	}
}

LCD::LCDString::LCDString(const wchar_t * txt)
{
	//default align = left
	int len = (int)wcslen(txt);
	if (len > 20) len = 20;
	for (int i = 0; i < len; i++)
		str[i] = ToP1(txt[i]);
	for (int i = len; i < 20; i++) {
		str[i] = ' ';
	}
}

LCD::LCDString::LCDString(const wchar_t * txt, Alignment at)
{
	int len = (int)wcslen(txt);
	if (len > 20) len = 20;
	switch (at)
	{
	case LCD::LCDString::Alignment::Left:
		for (int i = 0; i < len; i++)
			str[i] = ToP1(txt[i]);
		for (int i = len; i < 20; i++) {
			str[i] = ' ';
		}
		break;
	case LCD::LCDString::Alignment::Right:
		for (int i = 0; i < 20 - len; i++)
			str[i] = ' ';
		for (int i = 20 - len; i < 20; i++) {
			str[i] = ToP1(txt[i - (20 - len)]);
		}
		break;
	case LCD::LCDString::Alignment::Center:
	{
		int leftspace = (20 - len) / 2;
		//int rightspace = (20 - len) / 2 + (20 - len) % 2;
		for (int i = 0; i < leftspace; i++) {
			str[i] = ' ';
		}
		for (int i = leftspace; i < leftspace + len; i++) {
			str[i] = ToP1(txt[i - leftspace]);
		}
		for (int i = leftspace + len; i < 20; i++) {
			str[i] = ' ';
		}
	}
	break;
	default:
		break;
	}
}

const char * LCD::LCDString::GetString() const
{
	return str;
}
