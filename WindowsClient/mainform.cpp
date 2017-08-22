#include <stdint.h>
#include "mainform.h"
#include "../src/stuff.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace Stuff;

[STAThreadAttribute]
void Main(cli::array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	WindowsClient::mainform form;
	Application::Run(%form);
}

void WindowsClient::mainform::WriteLog(String ^ msg)
{
	ProgLog->AppendText(msg + "\n");
}

void WindowsClient::mainform::ReadSocket()
{
	while (isConnected) {
		array<Byte>^ data = gcnew array<Byte>(256);
		String^ response = String::Empty;
		int bytes = stream->Read(data, 0, data->Length);
		response = System::Text::Encoding::ASCII->GetString(data, 0, bytes);

		char* temp = new char[bytes];
		for (int i = 0; i < bytes; i++)
			temp[i] = data[i];
		if (!Verify(temp))
			this->Invoke(gcnew Action<String^>(this, &mainform::WriteLog), response);
		else
			switch (data[1]) {
			case 1:
			{
				initTest = false;
				char txt[3] = { data[4],data[5],0 };
				this->Invoke(gcnew Action<String^>(this, &mainform::WriteLog), gcnew String(txt));
			}
			break;
			default:
				break;
			}
		delete[] temp;
	}
}

void WindowsClient::mainform::SendCMD(uint8_t ch, uint8_t att)
{
	char data[8];
	//header
	data[0] = 'e';
	//message type
	data[1] = 2;
	//len = 2
	data[2] = 2;
	data[3] = 0;
	//ch & att
	data[4] = ch;
	data[5] = att;
	CalcSum(data,8);
	
	array<Byte>^ dt = gcnew array<Byte>(8);	//2+1+1+2+1+1+2  header+mc+mi+len+ch+att+sum
	for (int i = 0; i < 8; i++)
		dt[i] = data[i];
	stream->Write(dt, 0, dt->Length);
}

void WindowsClient::mainform::Test()
{
	char data[6];
	data[0] = 'e';
	//message type
	data[1] = 1;
	//len = 0
	data[2] = 0;
	data[3] = 0;
	CalcSum(data,6);
	
	array<Byte>^ dt = gcnew array<Byte>(6);
	for (int i = 0; i < 6; i++)
		dt[i] = data[i];
	stream->Write(dt, 0, dt->Length);
}

System::Void WindowsClient::mainform::button1_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	int port = 4550;	//default
	if (!int::TryParse(PortEdit->Text, port))
		return;
	client = gcnew TcpClient(IPAddrEdit->Text, port);
	stream = client->GetStream();
	isConnected = true;
	ConnTest->Enabled = true;
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
	try {
		client->Close();
	}
	catch (...) {

	}
}

System::Void WindowsClient::mainform::SendParamBtn_Click(System::Object ^ sender, System::EventArgs ^ e)
{
	uint8_t att, ch;
	if (ChannelNum->SelectedIndex < 0 || ChannelNum->SelectedIndex>40) {
		WriteLog("Неправильный номер канала");
		return;
	}
	else {
		ch = ChannelNum->SelectedIndex;
	}
	att = Decimal::ToByte(AttNum->Value);
	SendCMD(ch, att);
	WriteLog("Ch:" + ch + " At:" + att);
}

System::Void WindowsClient::mainform::ConnTest_Tick(System::Object ^ sender, System::EventArgs ^ e)
{
	if (initTest) {
		isConnected = false;
		ConnTest->Enabled = false;
	}
	else {
		Test();
		initTest = true;
	}
}

