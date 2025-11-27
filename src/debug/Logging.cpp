#include "debug/Logging.h"

#include "glad/gl.h"

namespace debug
{
	std::string GetTimestamp()
	{
		std::time_t time = std::time(0);
		auto ms = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000;
		std::ostringstream oss;
		oss << std::put_time(std::localtime(&time), "%T.") << std::setfill('0') << std::setw(3) << ms;
		return oss.str();
	}

	void Log(const std::string& msg, Verbosity level, std::source_location sl)
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

	void LogSpam(const std::string& msg, std::source_location sl)
	{
		Log(msg, Spam, sl);
	}
	void LogInfo(const std::string& msg, std::source_location sl)
	{
		Log(msg, Info, sl);
	}
	void LogWarning(const std::string& msg, std::source_location sl)
	{
		Log(msg, Warning, sl);
	}
	void LogError(const std::string& msg, std::source_location sl)
	{
		Log(msg, Error, sl);
	}

	//Logs the last OpenGL error
	void LogGLError(std::source_location sl)
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
}
