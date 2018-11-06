#include "IPChanger.h"
#include <sys/stat.h>
#include <list>

namespace Threading {
	void SetIP(wstring ip)
	{

	}

	void ReadIP()
	{
		string path = "//etc//dhcpcd.conf";
		//string path = "backup_dhcpcd.conf";
		ifstream ifs(path, ios::in);

		//парсим файлик

		struct stat fi;
		stat(path.c_str(), &fi);
		int filesize = fi.st_size;

		char* input = new char[filesize];
		ifs.read(input, filesize);

		ifs.close();

		cout << input << endl;

		string text = input;
		int StartIndex = 0;
		list<string> strs;

		for (int i = 0; i < text.length(); i++) {
			if (text[i] == '\n') {
				strs.push_back(text.substr(StartIndex, i - StartIndex + 1));
				StartIndex = i + 1;
			}
		}
		
		for (int i = 42; i < strs.size(); i++) {
			std::list<string>::iterator it = strs.begin();
			std::advance(it, i);
			auto pos=(*it).find("ip_address=");
			if (pos > 0 && pos < (*it).length()) {
				(*it)[0]='#';
			}
		}
		/*for (int i = 42; i < strs.size(); i++) {
			std::list<string>::iterator it = strs.begin();
			std::advance(it, i);
			auto pos = (*it).find("nterface eth0");
			if (pos > 0 && pos < (*it).length()) {
				(*it)[0] = '#';
			}
		}*/
		for (int i = 42; i < strs.size(); i++) {
			std::list<string>::iterator it = strs.begin();
			std::advance(it, i);
			auto pos = (*it).find("domain_name_servers=");
			if (pos > 0 && pos < (*it).length()) {
				(*it)[0] = '#';
			}
		}
		for (int i = 42; i < strs.size(); i++) {
			std::list<string>::iterator it = strs.begin();
			std::advance(it, i);
			auto pos = (*it).find("routers=");
			if (pos > 0 && pos < (*it).length()) {
				(*it)[0] = '#';
			}
		}
		string outtxt = "";
		std::list<string>::iterator it = strs.begin();

		for (int i = 0; i < strs.size(); i++,it++) {
			outtxt += (*it);
		}
		ofstream ofs(path, ios::out);

		ofs.write(outtxt.c_str(), outtxt.length());

		ofs.close();

		delete[] input;
	}

}