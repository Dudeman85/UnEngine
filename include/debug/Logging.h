#pragma once
#include <iostream>
#include <source_location>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
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

	std::string GetTimestamp();

	void Log(const std::string& msg, Verbosity level, std::source_location sl = std::source_location::current());

	void LogSpam(const std::string& msg, std::source_location sl = std::source_location::current());
	void LogInfo(const std::string& msg, std::source_location sl = std::source_location::current());
	void LogWarning(const std::string& msg, std::source_location sl = std::source_location::current());
	void LogError(const std::string& msg, std::source_location sl = std::source_location::current());

	//Logs the last OpenGL error
	void LogGLError(std::source_location sl = std::source_location::current());
}
