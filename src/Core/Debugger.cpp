#include "Debugger.h"

#include "include.h"
#include <iostream>

#include <sstream>
#include <iomanip>

#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>

#include <d3d11.h>
#include <dxgi.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace GCLE
{
	std::string WideToUtf8(const std::wstring& wide) {
		if (wide.empty()) return{};

		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int32>(wide.size()), nullptr, 0, nullptr, nullptr);
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int32>(wide.size()), result.data(), sizeNeeded, nullptr, nullptr);

		return result;
	}

	Debugger& Debugger::SelfRef()
	{
		static Debugger dbg;
		return dbg;
	}

	void Debugger::Output(const std::string& str)
	{
		if (m_IsInit == false) return;
		if ((m_CurrentInputMode & m_AllowedInputMode) == false) return;
		if (m_Output & DebuggerOutput::CONSOLE)
		{
			std::cout << str;
		}
		if (m_CurrentInputMode & DebuggerInputMode::ERR) m_Errors.back().msg += str;
		if (m_Output & DebuggerOutput::LOGS)
		{
			m_LogFile << str;
		}
	}

	void Debugger::OutputError(const ErrorReport& report)
	{
		if (m_IsInit == false) return;
		StartWithInputMode(DebuggerInputMode::ERR);
		SelfRef() << "=================================\n";
		SelfRef() << "= Line     : " << report.line << '\n';
		SelfRef() << "= Filename : " << report.file << '\n';
		SelfRef() << "= Funcname : " << report.funcname << '\n';
		SelfRef() << "= msg      : \n" << report.msg;
		SelfRef() << "==================================" << ENDL;
	}

	void Debugger::OutputSysConfig()
	{
		Debugger& ref = Debugger::SelfRef();
		GCLE_INFO << "Hardware information:\n";

#ifdef WIN32


		// CPU name
		char cpuName[256]{};
		DWORD size = sizeof(cpuName);

		RegGetValueA(
			HKEY_LOCAL_MACHINE,
			"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
			"ProcessorNameString",
			RRF_RT_REG_SZ,
			nullptr,
			cpuName,
			&size
		);

		GCLE_INFO << "  CPU name: " << cpuName << ENDL;
		 
		// Processor number
		SYSTEM_INFO siSysInfo{};
		GetSystemInfo(&siSysInfo);
		GCLE_INFO << "  Number of logical core: " << siSysInfo.dwNumberOfProcessors << ENDL;
		
		DWORD length = 0;
		 
		GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &length);

		std::vector<BYTE> buffer(length);

		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX info =
			reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(buffer.data());

		if (GetLogicalProcessorInformationEx(RelationProcessorCore, info, &length))
		{
			DWORD physicalCores = 0;

			BYTE* ptr = buffer.data();
			BYTE* end = ptr + length;

			while (ptr < end)
			{
				auto* entry = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(ptr);

				++physicalCores;

				ptr += entry->Size;
			}

			GCLE_INFO << "  Number of physical cores : " << physicalCores << ENDL;
		}
		 
		// MAC Address
		GCLE_INFO << "  MAC Address: " << ENDL;
		ULONG bufferSize = 0;
		 
		if (GetAdaptersInfo(nullptr, &bufferSize) == ERROR_BUFFER_OVERFLOW)
		{
			PIP_ADAPTER_INFO adapterInfo = (PIP_ADAPTER_INFO)malloc(bufferSize);

			if (adapterInfo)
			{
				if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR)
				{
					for (PIP_ADAPTER_INFO adapter = adapterInfo; adapter != nullptr; adapter = adapter->Next)
					{
						GCLE_INFO << "		Name : " << adapter->Description << ENDL;

						std::ostringstream mac;

						for (UINT i = 0; i < adapter->AddressLength; ++i)
						{
							if (i != 0)
								mac << ":";

							mac << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(adapter->Address[i]);
						}

						std::string macAddress = mac.str();

						GCLE_INFO << "		MAC : " << macAddress << ENDL;
					}
				}

				free(adapterInfo);
			}
		}  

		// GPU
		IDXGIFactory1* factory = nullptr;
		CreateDXGIFactory1(IID_PPV_ARGS(&factory));

		IDXGIAdapter1* adapter = nullptr;


		for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			std::wstring ws(desc.Description);
			std::string name = WideToUtf8(ws);

			if (strcmp(name.c_str(), "Microsoft Basic Render Driver") == 0)
			{
				GCLE_INFO << "	GPU : Couldn't be found !" << ENDL;
				GCLE_INFO << "	Look if your GPU drivers are installed or if a graphic card is detected in the task manager" << ENDL;
			}
			else
			{
				GCLE_INFO << "	GPU : " << name << ENDL;
				GCLE_INFO << "	VRAM : " << desc.DedicatedVideoMemory / (static_cast<uint64>(1024) * 1024) << " MB" << ENDL;
				GCLE_INFO << "	Flags : " << desc.Flags << ENDL;
			}

			adapter->Release();
			break;
		}

		factory->Release();

#else
GCLE_INFO << "	Debug System was only defined for windows" << ENDL;
#endif
	}

	void Debugger::Init(DebuggerDesc* pDesc)
	{
		if (m_IsInit) return;

		m_Output = pDesc->output;
		m_AllowedInputMode = pDesc->allowedInputMode;

		if (m_Output & DebuggerOutput::LOGS)
		{
			m_LogFile.open("Logs.txt", std::ios::binary);
		}

		m_IsInit = true;
		 
		Debugger& ref = Debugger::SelfRef();
		ref.OutputSysConfig();
	}

	Debugger& Debugger::StartWithInputMode(DebuggerInputMode inputMode)
	{
		Debugger& ref = Debugger::SelfRef();
		ref.m_CurrentInputMode = inputMode;
		return ref;
	}

	void Debugger::Destroy()
	{
		if (m_IsInit == false) return;

		while (m_Errors.size() > 0)
		{
			SelfRef().OutputError(m_Errors.front());
			m_Errors.pop();
		}

		if (m_Output & DebuggerOutput::LOGS)
		{
			m_LogFile.close();
		}

		m_IsInit = false;
	}

	Debugger& Debugger::ReportError(unsigned int line, const char* file, const char* funcname)
	{
		if (m_IsInit == false) return *this;
		m_Errors.push({
			.line = line,
			.file = file,
			.funcname = funcname
			});
		return *this;
	}

	Debugger& Debugger::operator<<(char c)
	{
		char str[2]{ c, '\0' };
		Output(str);
		return *this;
	}

	Debugger& Debugger::operator<<(const char* str)
	{
		Output(str);
		return *this;
	}

	Debugger& Debugger::operator<<(const std::string& str)
	{
		Output(str);
		return *this;
	}

	Debugger& Debugger::operator<<(const std::string_view& str) {
		Output(str.data());
		return *this;
	}

	Debugger& Debugger::operator<<(int value)
	{
		Output(std::to_string(value));
		return *this;
	}

	Debugger& Debugger::operator<<(unsigned int value)
	{
		Output(std::to_string(value));
		return *this;
	}

	Debugger& Debugger::operator<<(long value)
	{
		Output(std::to_string(value));
		return *this;
	}

	Debugger& Debugger::operator<<(unsigned long value)
	{
		Output(std::to_string(value));
		return *this;
	}

	Debugger& Debugger::operator<<(long long value)
	{
		Output(std::to_string(value));
		return *this;
	}

	Debugger& Debugger::operator<<(unsigned long long value)
	{
		Output(std::to_string(value));
		return *this;
	}

	Debugger& Debugger::operator<<(void* ptr)
	{
		std::ostringstream oss;
		oss << ptr;
		Output(oss.str());
		return *this;
	}

	Debugger& Debugger::operator<<(bool b) 
	{ 
		Output(BOOL_CHARACTERS[b ? 0 : 1]);
		return *this;
	}

	void Debugger::operator<<(const DBG_END_OF_LINE& end) 
	{
		operator<<(end.c);
		if (m_Output & DebuggerOutput::CONSOLE)
			std::cout.flush();
		if (m_Output & DebuggerOutput::LOGS)
			m_LogFile.flush();
	}
}