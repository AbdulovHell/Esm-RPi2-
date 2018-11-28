#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <stdint.h>
#include <fstream>
#include <string.h>
#include <mutex>

namespace Stuff {
	class Settings;

	extern Settings* Storage;

	struct Data {
		uint32_t Freq;
		uint8_t RFatt;
		uint8_t IFatt;
		uint8_t IF;
		uint8_t BrightLvl;
		uint8_t Ref;
		uint64_t WorkTime_m;

		int SnakeRecord;
		int SelfdestructRecord;
		int RacingRecord;
		int TetrisRecord;
	};

	//поставщик файлов настроек
	class Settings {
		Data dt;

		void UpdateFile();

		void CreateNew();

	public:

		Settings();
		void LoadSettings();

		void SetSnakeRecord(int rec);
		int GetSnakeRecord();

		void SetSelfdestructRecord(int rec);
		int GetSelfdestructRecord();

		void SetRacingRecord(int rec);
		int GetRacingRecord();

		void SetTetrisRecord(int rec);
		int GetTetrisRecord();

		void SetFreq(uint32_t freq);
		uint32_t GetFreq();

		void SetAtt(uint8_t _rf, uint8_t _if);
		uint8_t GetRFAtt();
		uint8_t GetIFAtt();

		void SetIF(uint8_t _if);
		uint8_t GetIF();

		void SetBrightLvl(uint8_t lvl);
		uint8_t GetBrightLvl();

		void SetRef(uint8_t ref);
		uint8_t GetRef();

		void SetWorkTime(uint64_t t);
		uint64_t GetWorkTime();
	};

}

#endif	// _SETTINGS_H_
