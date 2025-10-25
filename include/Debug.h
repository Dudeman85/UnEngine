#pragma once
#include <iostream>
#include <source_location>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <unordered_map>

#ifndef __gl_h_
#include "glad/gl.h"
#endif

namespace debug
{
	//LOGGING

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
		oss << std::put_time(std::localtime(&time), "%T.") << std::setfill('0') << std::setw(3) << ms;
		return oss.str();
	}

	inline void Log(const std::string& msg, Verbosity level, std::source_location sl = std::source_location::current())
	{
		if (verbosity >= level)
		{
			std::ostringstream header;
			std::string file = sl.file_name();
			//Fix for MSVC
			size_t pos = file.rfind('\\');
			if (pos != std::string::npos)
				file = file.substr(pos + 1);

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

	//Logs the last OpenGL error
	inline void CheckGLError(std::source_location sl = std::source_location::current())
	{
		std::unordered_map<GLenum, std::string> errorMap = {
			{GL_INVALID_ENUM, "GL_INVALID_ENUM"}, {GL_INVALID_VALUE, "GL_INVALID_VALUE"}, {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
			 {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"}
		};

		std::string errors;
		GLenum e = glGetError();
		while (e != GL_NO_ERROR)
		{
			errors += errorMap[e] + ", ";
			e = glGetError();
		}

		if (!errors.empty())
			LogError("Last OpenGL error(s): " + errors, sl);
	}

	//TIMERS

	//Store timers by name
	inline std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timers;

	inline void StartTimer(const std::string& name)
	{
		timers[name] = std::chrono::high_resolution_clock::now();
	}
	//Return the time (default milliseconds) since the start of this timer
	template <typename T = std::chrono::milliseconds>
	inline long SampleTimer(const std::string& name)
	{
		if (!timers.contains(name))
		{
			LogWarning("No timer named " + name);
			return 0;
		}

		return std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - timers[name]).count();
	}
	//Return the time (default milliseconds) since the start of this timer, also resets the timer
	template <typename T = std::chrono::milliseconds>
	inline long ResetTimer(const std::string& name)
	{
		if (!timers.contains(name))
		{
			LogWarning("No timer named " + name);
			return 0;
		}

		const long duration = std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - timers[name]).count();
		timers[name] = std::chrono::high_resolution_clock::now();
		return duration;
	}
	//Return the time (default milliseconds) since the start of this timer, also ends the timer
	template <typename T = std::chrono::milliseconds>
	inline long EndTimer(const std::string& name)
	{
		if (!timers.contains(name))
		{
			LogWarning("No timer named " + name);
			return 0;
		}

		const long duration = std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - timers[name]).count();
		timers.erase(name);
		return duration;
	}
}
