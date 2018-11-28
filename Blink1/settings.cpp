#include "settings.h"
#include <iostream>
#include <sys/stat.h>
#include "syslogs.h"

namespace Stuff {
	Settings* Storage;
}

using namespace std;
const string SettingsPath = "/esm/settings.bin";
std::mutex* SettingsMutex;

void Stuff::Settings::UpdateFile()
{
	SettingsMutex->lock();
	Threading::SALog.Append("Update file");
	ofstream fs(SettingsPath, ios::out | ios::trunc | ios::binary);
	//cout << "Settings file updated" << endl;
	fs.write((char*)&dt, sizeof(Data));
	fs.flush();
	fs.close();
	SettingsMutex->unlock();
}

void Stuff::Settings::CreateNew()
{
	dt.Freq = 3000;
	dt.RFatt = 0;
	dt.IFatt = 0;
	dt.IF = 1;
	dt.BrightLvl = 16;
	dt.Ref = 1;
	dt.WorkTime_m = 0;

	dt.SnakeRecord = 0;
	dt.SelfdestructRecord = 0;
	dt.RacingRecord = 0;
	dt.TetrisRecord = 0;
	Threading::SALog.Append("Set default params");
	UpdateFile();
}

Stuff::Settings::Settings()
{
	SettingsMutex = new std::mutex();
}

string ToString(uint32_t num) {
	string ret = "";
	char buf[100];
	sprintf(buf, "%u", num);
	ret = buf;
	return ret;
}

void Stuff::Settings::LoadSettings()
{
	int filesize = -1;
	//попытка загрузки из файла
	ifstream ifs(SettingsPath, ios::in | ios::binary);
	//если файла нет, то установка по умолчанию
	if (!ifs) {
		Threading::SALog.Append("Create new, not exist. err:" + ToString(errno));
		CreateNew();
	}
	else {//иначе парсим файлик
		struct stat fi;
		stat(SettingsPath.c_str(), &fi);
		filesize = fi.st_size;

		if (filesize > 0) {
			ifs.read((char*)&dt, sizeof(Data));
			ifs.close();
			Threading::SALog.Append("Readed, F:" + ToString(dt.Freq));
		}
		else {
			ifs.close();
			Threading::SALog.Append("Create new, filesize 0. err:" + ToString(errno));
			CreateNew();
		}
	}
}

void Stuff::Settings::SetSnakeRecord(int rec)
{
	dt.SnakeRecord = rec;
	UpdateFile();
}

int Stuff::Settings::GetSnakeRecord()
{
	return dt.SnakeRecord;
}

void Stuff::Settings::SetSelfdestructRecord(int rec)
{
	dt.SelfdestructRecord = rec;
	UpdateFile();
}

int Stuff::Settings::GetSelfdestructRecord()
{
	return dt.SelfdestructRecord;
}

void Stuff::Settings::SetRacingRecord(int rec)
{
	dt.RacingRecord = rec;
	UpdateFile();
}

int Stuff::Settings::GetRacingRecord()
{
	return dt.RacingRecord;
}

void Stuff::Settings::SetTetrisRecord(int rec)
{
	dt.TetrisRecord = rec;
	UpdateFile();
}

int Stuff::Settings::GetTetrisRecord()
{
	return dt.TetrisRecord;
}

void Stuff::Settings::SetFreq(uint32_t freq)
{
	dt.Freq = freq;
	UpdateFile();
}

uint32_t Stuff::Settings::GetFreq()
{
	return dt.Freq;
}

void Stuff::Settings::SetAtt(uint8_t _rf, uint8_t _if)
{
	dt.RFatt = _rf;
	dt.IFatt = _if;
	UpdateFile();
}

uint8_t Stuff::Settings::GetRFAtt()
{
	return dt.RFatt;
}

uint8_t Stuff::Settings::GetIFAtt()
{
	return dt.IFatt;
}

void Stuff::Settings::SetIF(uint8_t _if)
{
	dt.IF = _if;
	UpdateFile();
}

uint8_t Stuff::Settings::GetIF()
{
	return dt.IF;
}

void Stuff::Settings::SetBrightLvl(uint8_t lvl)
{
	dt.BrightLvl = lvl;
	UpdateFile();
}

uint8_t Stuff::Settings::GetBrightLvl()
{
	return dt.BrightLvl;
}

void Stuff::Settings::SetRef(uint8_t ref)
{
	dt.Ref = ref;
	UpdateFile();
}

uint8_t Stuff::Settings::GetRef()
{
	return dt.Ref;
}

void Stuff::Settings::SetWorkTime(uint64_t t)
{
	dt.WorkTime_m = t;
	UpdateFile();
}

uint64_t Stuff::Settings::GetWorkTime()
{
	return dt.WorkTime_m;
}
