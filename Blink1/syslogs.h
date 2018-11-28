#ifndef _SYSLOGS_H_
#define _SYSLOGS_H_

#include <string>
#include <fstream>
#include <mutex>

namespace Threading {

	using namespace std;
	
	class SettingAccessLog;

	extern SettingAccessLog SALog;

	class SettingAccessLog {
		string path = "/esm/sal.txt";
		ofstream* fs;
	public:
		SettingAccessLog();

		void Append(string text);
	};
}

#endif	// _SYSLOGS_H_
