#pragma once

#include <fstream>
#include <queue>
#include <string>

#define GCLE_SPACE GCLE_INFO << ENDL

#ifdef NDEBUG
#define GCLE_INFO	if(true) {} else GCLE::Debugger::StartWithInputMode(GCLE::DebuggerInputMode::INFO) << "[INFO ] "
#define GCLE_WARN if(true) {} else GCLE::Debugger::StartWithInputMode(GCLE::DebuggerInputMode::WARNING) << "[WARN ] "
#define GCLE_ERROR	if(true) {} else GCLE::Debugger::StartWithInputMode(GCLE::DebuggerInputMode::ERR).ReportError(__LINE__, __FILE__, __FUNCTION__) << "[ERROR] "
#else
#define GCLE_INFO	GCLE::Debugger::StartWithInputMode(GCLE::DebuggerInputMode::INFO) << "[INFO] "
#define GCLE_WARN GCLE::Debugger::StartWithInputMode(GCLE::DebuggerInputMode::WARNING) << "[WARN] "
#define GCLE_ERROR	GCLE::Debugger::StartWithInputMode(GCLE::DebuggerInputMode::ERR).ReportError(__LINE__, __FILE__, __FUNCTION__) << "[ERROR] "
#endif // NDEBUG

#define ENDL GCLE::DBG_END_OF_LINE()


namespace GCLE
{
	enum DebuggerOutput : int
	{
		CONSOLE = 1,
		LOGS = 2,
	};

	enum DebuggerInputMode : int
	{
		INFO = 1,
		WARNING = 2,
		ERR = 4,
	};

	struct DebuggerDesc
	{
		int allowedInputMode = DebuggerInputMode::INFO | DebuggerInputMode::WARNING | DebuggerInputMode::ERR;
		int output = DebuggerOutput::CONSOLE | DebuggerOutput::LOGS;
	};

	struct DBG_END_OF_LINE {
		inline static constexpr char c = '\n';
	};

	class Debugger
	{
		inline static int m_Output;
		inline static int m_AllowedInputMode;
		inline static DebuggerInputMode m_CurrentInputMode;
		inline static std::ofstream m_LogFile;
		inline static bool m_IsInit = false;
		const std::string BOOL_CHARACTERS[2] = { "TRUE", "FALSE" };

		struct ErrorReport
		{
			unsigned int line;
			const char* file;
			const char* funcname;
			std::string msg;
		};
		inline static std::queue<ErrorReport> m_Errors;

		static Debugger& SelfRef();

		void Output(const std::string& str);

		void OutputError(const ErrorReport& report);

		void OutputSysConfig();

	public:

		static void Init(DebuggerDesc* pDesc);
		static Debugger& StartWithInputMode(DebuggerInputMode inputMode);
		static void Destroy();

		Debugger& ReportError(unsigned int line, const char* file, const char* funcname);

		Debugger& operator << (char c);
		Debugger& operator << (const char* str);
		Debugger& operator << (const std::string& str);
		Debugger& operator << (const std::string_view& str);
		Debugger& operator<<(int value);
		Debugger& operator<<(unsigned int value);
		Debugger& operator<<(long value);
		Debugger& operator<<(unsigned long value);
		Debugger& operator<<(long long value);
		Debugger& operator<<(unsigned long long value);
		Debugger& operator<<(void* ptr);

		Debugger& operator << (bool b);

		void operator << (const DBG_END_OF_LINE& end);

		template<typename T> requires(!std::is_same_v<T, char> && (std::is_floating_point_v<T> || std::is_integral_v<T>))
			Debugger& operator << (T number) {
			Output(std::to_string(number));
			return *this;
		}

	};
}
