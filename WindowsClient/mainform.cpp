#include "mainform.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Net;
using namespace System::Net::Sockets;

[STAThreadAttribute]
void Main(cli::array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	WindowsClient::mainform form;
	Application::Run(%form);
}

void WindowsClient::mainform::WriteLog(String ^ msg)
{
	textBox4->AppendText(msg + "\n");
}

void WindowsClient::mainform::ReadSocket()
{
	while (1) {
		array<Byte>^ data = gcnew array<Byte>(256);
		String^ response = String::Empty;
		int bytes = stream->Read(data, 0, data->Length);
		response = System::Text::Encoding::ASCII->GetString(data, 0, bytes);
		this->Invoke(gcnew Action<String^>(this,&mainform::WriteLog), response);
	}
}

System::Void WindowsClient::mainform::button1_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	client = gcnew TcpClient(textBox1->Text, int::Parse(textBox2->Text));
	stream = client->GetStream();
	readThrd = gcnew Thread(gcnew ThreadStart(this, &mainform::ReadSocket));
	readThrd->Start();
}

System::Void WindowsClient::mainform::button2_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	array<Byte>^ data = System::Text::Encoding::ASCII->GetBytes(textBox3->Text);
	stream->Write(data, 0, data->Length);
}

System::Void WindowsClient::mainform::mainform_FormClosed(System::Object ^ sender, System::Windows::Forms::FormClosedEventArgs ^ e)
{
	client->Close();
}

System::Void WindowsClient::mainform::timer1_Tick(System::Object ^ sender, System::EventArgs ^ e)
{
}
