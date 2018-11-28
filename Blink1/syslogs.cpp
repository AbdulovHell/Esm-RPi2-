#include "syslogs.h"

using namespace std;

namespace Threading {
	SettingAccessLog SALog;
	mutex SALMutex;
}

Threading::SettingAccessLog::SettingAccessLog()
{
	SALMutex.lock();
	fs = new ofstream(path, ios::out);
	string temp = "-------------Start new-------------";
	fs->write(temp.c_str(), temp.length() + 1);
	fs->close();
	SALMutex.unlock();
}

void Threading::SettingAccessLog::Append(string text)
{
	SALMutex.lock();
	text += "\n";
	fs->open(path, ios::out | ios::app);
	fs->write(text.c_str(), text.length() + 1);
	fs->close();
	SALMutex.unlock();
}
