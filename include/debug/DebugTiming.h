#pragma once
#include <string>
#include <chrono>
#include <unordered_map>

namespace debug
{
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
