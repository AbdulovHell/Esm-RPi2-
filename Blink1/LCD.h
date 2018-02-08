#ifndef _LCD_H_
#define _LCD_H_

#define LEFT 0
#define RIGHT 1

namespace LCD {
	using namespace std;

	struct CustomSymbol {
		char Rows[8];
		CustomSymbol operator= (char* other);
		CustomSymbol operator= (CustomSymbol other);
	};

	class LCDString {
		char str[20];

		//перевод кирилицы в кодировку второй таблицы знакогенератора
		char ToP1(wchar_t symb);
	public:
		//Выравнивание текста в строке
		enum class Alignment {
			Left,
			Right,
			Center
		};

		LCDString(const char* txt);
		LCDString(const char* txt, Alignment at);
		LCDString(const wchar_t* txt);
		LCDString(const wchar_t* txt, Alignment at);
		//Возвращает итоговую строку,форматированную под дисплей
		const char* GetString() const;
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
		void SetCGRAMAddr(int addr);

	public:
		enum class Cursor : int {
			NoCursor_NoFlashing = 0,
			NoCursor_SymbolFlashing = 1,
			Cursor_NoFlashing = 2,
			Cursor_Flashing = 3
		};

		Display();
		Display(int E, int RS, int D4, int D5, int D6, int D7);
		void Init();
		void Clear();
		void ReturnHome();
		void EntryModeSet(uint8_t ID, uint8_t SH);
		void Power(uint8_t state, Cursor cursor);
		void CursorShift(uint8_t RL);
		void DisplayShift(uint8_t RL);
		void WriteDataToRAM(uint8_t msg);
		void SetCursorPos(int str, int col);
		void SendText(char* txt, size_t size);
		void SendText(char txt);
		void LoadSymbol(CustomSymbol symb, uint8_t pos);
		void SetScreen(list<LCDString*>* strs,size_t topline);
	};
}

#endif