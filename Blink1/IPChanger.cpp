#include "IPChanger.h"
#include <sys/stat.h>
#include <list>

namespace Threading {

	string IPChanger::GetCurrentIP()
	{
		system("ifconfig eth0 > eth0inf.txt");

		FILE* log = fopen("eth0inf.txt", "r");
		if (log == nullptr) return "0.0.0.0";
		fseek(log, 0, SEEK_END);
		long pos = ftell(log);
		if (pos < 1) return "0.0.0.0";
		fseek(log, 0, SEEK_SET);
		char* buf = new char[pos + 1];
		buf[pos] = 0;
		auto readed = fread(buf, pos, pos, log);
		string info = buf;
		fclose(log);
		//delete[] buf;
		auto param_pos = info.find("inet addr:");
		int len = 0;
		for (int i = param_pos + 10; i < info.length(); i++) {
			if (info[i] == ' ') {
				len = i - param_pos - 10;
				break;
			}
		}

		string ip_str = info.substr(param_pos + 10, len);

		//cout << "IP: " << ip_str << endl;
		delete[] buf;
		system("rm eth0inf.txt");
		return ip_str;
	}

	IPChanger::IPChanger()
	{
		ReadIPConf();
		if (usingDHCP) {
			CurrentIP = GetCurrentIP();
		}
	}

	void IPChanger::Save()
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

		//cout << input << endl;

		string text = input;
		int StartIndex = 0;
		list<string> strs;

		//Поделим на строки
		for (int i = 0; i < text.length(); i++) {
			if (text[i] == '\n') {
				strs.push_back(text.substr(StartIndex, i - StartIndex + 1));
				StartIndex = i + 1;
			}
		}

		std::list<string>::iterator it = strs.begin();
		for (int i = 0; i < strs.size(); i++, it++) {
			auto pos = (*it).find("p_address=");
			if (pos > 0 && pos < (*it).length()) {	//строка с ip
				if (usingDHCP) {
					(*it)[0] = '#';
				}
				else {
					(*it) = " static ip_address=" + CurrentIP + "/24\n";
				}
			}
		}

		it = strs.begin();
		for (int i = 0; i < strs.size(); i++, it++) {
			auto pos = (*it).find("omain_name_servers=");
			if (pos > 0 && pos < (*it).length()) {
				if (usingDHCP) {
					(*it)[0] = '#';
				}
				else {
					(*it) = " static domain_name_servers=" + RouterIP(CurrentIP)+"\n";
				}
			}
		}

		it = strs.begin();
		for (int i = 0; i < strs.size(); i++, it++) {
			auto pos = (*it).find("routers=");
			if (pos > 0 && pos < (*it).length()) {
				if (usingDHCP) {
					(*it)[0] = '#';
				}
				else {
					(*it) = " static routers=" + RouterIP(CurrentIP)+"\n";
				}
			}
		}

		string outtxt = "";
		it = strs.begin();
		for (int i = 0; i < strs.size(); i++, it++) {
			outtxt += (*it);
		}

		ofstream ofs(path, ios::out);

		ofs.write(outtxt.c_str(), outtxt.length());

		ofs.close();

		delete[] input;
	}

	void IPChanger::SetIP(int nums[])
	{
		char buf[20];
		std::sprintf(buf, "%d.%d.%d.%d", nums[0], nums[1], nums[2], nums[3]);
		CurrentIP = buf;
	}

	void IPChanger::ReadIPConf()
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

		//cout << input << endl;

		string text = input;
		int StartIndex = 0;
		list<string> strs;

		//Поделим на строки
		for (int i = 0; i < text.length(); i++) {
			if (text[i] == '\n') {
				strs.push_back(text.substr(StartIndex, i - StartIndex + 1));
				StartIndex = i + 1;
			}
		}

		std::list<string>::iterator it = strs.begin();
		for (int i = 0; i < strs.size(); i++, it++) {
			auto pos = (*it).find("p_address=");
			if (pos > 0 && pos < (*it).length()) {	//строка с ip
				if ((*it)[0] == '#') {	//dhcp
					usingDHCP = true;
				}
				else if ((*it)[0] == ' ') {
					usingDHCP = false;
					CurrentIP = (*it).substr(pos + 10, (*it).length() - pos - 11);
				}
				//(*it)[0] = '#';
			}
		}
		delete[] input;
	}

	string IPChanger::RouterIP(string ip)
	{
		int cnt = 0;
		for (int i = 0; i < ip.length(); i++) {
			if (ip[i] == '.') {
				cnt++;
			}
			if (cnt == 3) {
				cnt = ++i;
				break;
			}
		}
		return ip.substr(0,cnt)+"1";
	}
}