#include "sys_headers.h"

#include "Task.h"
#include "Colorize.h"
#include "main.h"

using namespace std;

void Threading::TaskSetAttCh::Run()
{
	char buf[128];
	memset(buf, 0, 128);
	sprintf(buf, "%s: Set channel: %d Att: %d", Stuff::MakeColor("MAIN", Stuff::Green).c_str(), ch, att);
	cout << buf << endl;
}

void Threading::TaskQuit::Run()
{
	InitShutdown();
}
