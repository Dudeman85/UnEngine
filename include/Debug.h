#pragma once
#include <iostream>
#include <source_location>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <unordered_map>

namespace debug
{
	//Define one or more ostreams as logging outputs, and specify if they should use colors
	inline std::vector<std::pair<std::ostream*, bool>> logOutputs = {{&std::cout, true}};

	enum Verbosity { Spam = 4, Info = 3, Warning = 2, Error = 1, None = 0 };
	//What level of logs should be logged
	inline Verbosity verbosity = Spam;
	inline std::unordered_map<Verbosity, std::string> nameMap = {
		{None, "None"}, {Spam, "Spam"}, {Info, "Info"}, {Warning, "Warning"}, {Error, "Error"}
	};
	inline std::unordered_map<Verbosity, std::string> colorMap = {
		{None, "\033[37m"}, {Spam, "\033[37m"}, {Info, "\033[37m"}, {Warning, "\033[33m"}, {Error, "\033[31m"}
	};

	inline std::string GetTimestamp()
	{
		std::time_t time = std::time(0);
		auto ms = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000;
		std::ostringstream oss;
		oss << std::put_time(std::localtime(&time), "%T.") << ms;
		return oss.str();
	}

	inline void Log(const std::string& msg, Verbosity level, std::source_location sl = std::source_location::current())
	{
		if (verbosity >= level)
		{
			std::ostringstream header;
			std::string file = sl.file_name();
			header << "[" << GetTimestamp() << "][" << nameMap[level] << "] " << file.substr(file.find_last_of('/') + 1) << ":" << sl.line() << " ";

			for (const auto os : logOutputs)
			{
				//Change color
				if (os.second)
					*os.first << colorMap[level];

				*os.first << header.str() << msg << std::endl;

				//Change color back to default
				if (os.second)
					*os.first << colorMap[None];
			}
		}
	}

	inline void LogSpam(const std::string& msg, std::source_location sl = std::source_location::current())
	{
		Log(msg, Spam, sl);
	}
	inline void LogInfo(const std::string& msg, std::source_location sl = std::source_location::current())
	{
		Log(msg, Info, sl);
	}
	inline void LogWarning(const std::string& msg, std::source_location sl = std::source_location::current())
	{
		Log(msg, Warning, sl);
	}
	inline void LogError(const std::string& msg, std::source_location sl = std::source_location::current())
	{
		Log(msg, Error, sl);
	}
}
