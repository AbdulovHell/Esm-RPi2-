#pragma once

namespace WindowsClient {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Net;
	using namespace System::Net::Sockets;
	using namespace System::Threading;
	/// <summary>
	/// Сводка для mainform
	/// </summary>
	public ref class mainform : public System::Windows::Forms::Form
	{
	public:
		mainform(void)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~mainform()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  IPAddrEdit;
	private: System::Windows::Forms::TextBox^  PortEdit;
	private: System::Windows::Forms::Button^  ConnectBtn;
	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::ComboBox^  ChannelNum;
	private: System::Windows::Forms::NumericUpDown^  AttNum;
	private: System::Windows::Forms::Button^  SendParamBtn;
	private: System::Windows::Forms::Timer^  ConnTest;
	private: System::Windows::Forms::TextBox^  ProgLog;

	private:
		void WriteLog(String^ msg);
		void ReadSocket();
		//Комманда на установку канала и значения затухания
		void SendCMD(uint8_t ch, uint8_t att);
		//Комманда проверки соединения
		void Test();
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		TcpClient^ client;
		NetworkStream^ stream;
		Thread^ readThrd;
		bool isConnected = false;
		bool initTest = false;

	private: System::ComponentModel::IContainer^  components;


#pragma region Windows Form Designer generated code
			 /// <summary>
			 /// Требуемый метод для поддержки конструктора — не изменяйте 
			 /// содержимое этого метода с помощью редактора кода.
			 /// </summary>
			 void InitializeComponent(void)
			 {
				 this->components = (gcnew System::ComponentModel::Container());
				 this->IPAddrEdit = (gcnew System::Windows::Forms::TextBox());
				 this->PortEdit = (gcnew System::Windows::Forms::TextBox());
				 this->ConnectBtn = (gcnew System::Windows::Forms::Button());
				 this->textBox3 = (gcnew System::Windows::Forms::TextBox());
				 this->button2 = (gcnew System::Windows::Forms::Button());
				 this->ProgLog = (gcnew System::Windows::Forms::TextBox());
				 this->ChannelNum = (gcnew System::Windows::Forms::ComboBox());
				 this->AttNum = (gcnew System::Windows::Forms::NumericUpDown());
				 this->SendParamBtn = (gcnew System::Windows::Forms::Button());
				 this->ConnTest = (gcnew System::Windows::Forms::Timer(this->components));
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AttNum))->BeginInit();
				 this->SuspendLayout();
				 // 
				 // IPAddrEdit
				 // 
				 this->IPAddrEdit->Location = System::Drawing::Point(12, 12);
				 this->IPAddrEdit->Name = L"IPAddrEdit";
				 this->IPAddrEdit->Size = System::Drawing::Size(105, 20);
				 this->IPAddrEdit->TabIndex = 0;
				 this->IPAddrEdit->Text = L"192.168.1.44";
				 // 
				 // PortEdit
				 // 
				 this->PortEdit->Location = System::Drawing::Point(123, 12);
				 this->PortEdit->Name = L"PortEdit";
				 this->PortEdit->Size = System::Drawing::Size(73, 20);
				 this->PortEdit->TabIndex = 1;
				 this->PortEdit->Text = L"4550";
				 // 
				 // ConnectBtn
				 // 
				 this->ConnectBtn->Location = System::Drawing::Point(202, 12);
				 this->ConnectBtn->Name = L"ConnectBtn";
				 this->ConnectBtn->Size = System::Drawing::Size(83, 20);
				 this->ConnectBtn->TabIndex = 2;
				 this->ConnectBtn->Text = L"Соединение";
				 this->ConnectBtn->UseVisualStyleBackColor = true;
				 this->ConnectBtn->Click += gcnew System::EventHandler(this, &mainform::button1_Click);
				 // 
				 // textBox3
				 // 
				 this->textBox3->Location = System::Drawing::Point(487, 38);
				 this->textBox3->Multiline = true;
				 this->textBox3->Name = L"textBox3";
				 this->textBox3->Size = System::Drawing::Size(184, 78);
				 this->textBox3->TabIndex = 3;
				 // 
				 // button2
				 // 
				 this->button2->Location = System::Drawing::Point(487, 9);
				 this->button2->Name = L"button2";
				 this->button2->Size = System::Drawing::Size(75, 23);
				 this->button2->TabIndex = 4;
				 this->button2->Text = L"button2";
				 this->button2->UseVisualStyleBackColor = true;
				 this->button2->Click += gcnew System::EventHandler(this, &mainform::button2_Click);
				 // 
				 // ProgLog
				 // 
				 this->ProgLog->Location = System::Drawing::Point(12, 110);
				 this->ProgLog->Multiline = true;
				 this->ProgLog->Name = L"ProgLog";
				 this->ProgLog->Size = System::Drawing::Size(446, 165);
				 this->ProgLog->TabIndex = 5;
				 // 
				 // ChannelNum
				 // 
				 this->ChannelNum->FormattingEnabled = true;
				 this->ChannelNum->Items->AddRange(gcnew cli::array< System::Object^  >(10) {
					 L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8",
						 L"9", L"10"
				 });
				 this->ChannelNum->Location = System::Drawing::Point(12, 57);
				 this->ChannelNum->Name = L"ChannelNum";
				 this->ChannelNum->Size = System::Drawing::Size(135, 21);
				 this->ChannelNum->TabIndex = 6;
				 // 
				 // AttNum
				 // 
				 this->AttNum->Location = System::Drawing::Point(12, 84);
				 this->AttNum->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 35, 0, 0, 0 });
				 this->AttNum->Name = L"AttNum";
				 this->AttNum->Size = System::Drawing::Size(135, 20);
				 this->AttNum->TabIndex = 7;
				 // 
				 // SendParamBtn
				 // 
				 this->SendParamBtn->Location = System::Drawing::Point(153, 57);
				 this->SendParamBtn->Name = L"SendParamBtn";
				 this->SendParamBtn->Size = System::Drawing::Size(75, 47);
				 this->SendParamBtn->TabIndex = 8;
				 this->SendParamBtn->Text = L"Установить";
				 this->SendParamBtn->UseVisualStyleBackColor = true;
				 this->SendParamBtn->Click += gcnew System::EventHandler(this, &mainform::SendParamBtn_Click);
				 // 
				 // ConnTest
				 // 
				 this->ConnTest->Interval = 1000;
				 this->ConnTest->Tick += gcnew System::EventHandler(this, &mainform::ConnTest_Tick);
				 // 
				 // mainform
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(679, 289);
				 this->Controls->Add(this->SendParamBtn);
				 this->Controls->Add(this->AttNum);
				 this->Controls->Add(this->ChannelNum);
				 this->Controls->Add(this->ProgLog);
				 this->Controls->Add(this->button2);
				 this->Controls->Add(this->textBox3);
				 this->Controls->Add(this->ConnectBtn);
				 this->Controls->Add(this->PortEdit);
				 this->Controls->Add(this->IPAddrEdit);
				 this->Name = L"mainform";
				 this->Text = L"mainform";
				 this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &mainform::mainform_FormClosed);
				 (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->AttNum))->EndInit();
				 this->ResumeLayout(false);
				 this->PerformLayout();

			 }
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void mainform_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
	private: System::Void SendParamBtn_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ConnTest_Tick(System::Object^  sender, System::EventArgs^  e);
	};
}
