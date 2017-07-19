#include "mainform.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;

[STAThreadAttribute]
void Main(cli::array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	WindowsClient::mainform form;
	Application::Run(%form);
}