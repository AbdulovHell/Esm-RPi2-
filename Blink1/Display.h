#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define LEFT 0
#define RIGHT 1
//#define MAX_STRING_LENGTH 255
//#define MAX_VISIBLE_LENGTH 20

namespace Display {
	using namespace std;
	class Display;

	struct CustomSymbol {
		char Rows[8];
		CustomSymbol operator= (char* other);
		CustomSymbol operator= (CustomSymbol other);
	};

	class DisplayString {
		char str[20];
		//bool ScrollRequired;
		//int scrollOffset = 0;
		//bool rightScrolling = false;
		//перевод кирилицы в кодировку второй таблицы знакогенератора
		char ToP1(wchar_t symb);
		//uint32_t ScrollSkipCounter = 0;
	public:
		//Выравнивание текста в строке
		enum class Alignment {
			Left,
			Right,
			Center
		};

		DisplayString operator= (DisplayString other);

		//bool isScrollRequired() { return ScrollRequired; };

		DisplayString(const char* txt);
		DisplayString(unsigned char* arr, size_t len);
		DisplayString(const char* txt, Alignment at);
		DisplayString(const wchar_t* txt);
		DisplayString(const wchar_t* txt, std::function<void(Display*, uint32_t)>);
		DisplayString(const wchar_t* txt, Alignment at);
		DisplayString(const wchar_t* txt, Alignment at, std::function<void(Display*, uint32_t)>);
		//Возвращает итоговую строку,форматированную под дисплей
		const char* GetString() const;

		//bool ScrollSting();
		//bool ResetPosition();

		std::function<void(Display*, uint32_t)> ItemPressedCallback;
	};

	class Display {
		int __E = 20;
		int __RS = 16;
		int __D4 = 6;
		int __D5 = 13;
		int __D6 = 19;
		int __D7 = 26;

		void SendByte(uint8_t byte);
		void Send4Bit(uint8_t byte);
		void SetDDRAMAddress(int addr);
		void FunctionSet();

	public:

		enum class Cursor : int {
			NoCursor_NoFlashing = 0,
			NoCursor_SymbolFlashing = 1,
			Cursor_NoFlashing = 2,
			Cursor_Flashing = 3
		};

		int CurrentStr;
		int CurrentCol;
		Cursor CurrentCursorType;

		Display();
		Display(int E, int RS, int D4, int D5, int D6, int D7);
		void Init();
		void Clear();
		void ReturnHome();
		void EntryModeSet(uint8_t ID, uint8_t SH);
		void Power(uint8_t state, Cursor cursor);
		void SetCursorType(Cursor cursor);
		void CursorShift(uint8_t RL);
		void DisplayShift(uint8_t RL);
		void WriteDataToRAM(uint8_t msg);
		void SetCursorPos(int str, int col);
		void SendText(char* txt, size_t size);
		void SendText(char txt);
		void LoadSymbol(CustomSymbol symb, uint8_t pos);
		void SetScreen(std::list<DisplayString*>* strs, size_t topline);
		void SetCGRAMAddr(int addr);
	};
}

#endif