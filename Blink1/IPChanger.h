#ifndef _IPCHANGER_H_
#define _IPCHANGER_H_

#include <iostream>
#include <fstream>

using namespace std;

namespace Threading {

	class IPChanger {
		bool usingDHCP = false;
		string CurrentIP = "";

		string GetCurrentIP();

		void ReadIPConf();

		string RouterIP(string ip);

	public:
		IPChanger();

		void Save();

		string GetIP() { return CurrentIP; }
		bool UsingDHCP() { return usingDHCP; }

		void SetIP(string ip) { CurrentIP = ip; }
		void SetIP(int nums[]);
		void SetDHCPState(bool state) { usingDHCP = state; }
	};
}

#endif