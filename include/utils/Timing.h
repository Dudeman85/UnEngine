#pragma once

#include <chrono>
#include <vector>
#include <functional>

#include "ECS.h"
#include "utils/Callback.h"

namespace une
{
	// All CallbackWrappers binded by ScheduleFunction
	inline std::vector<CallbackWrapper*> _CallbackWrappers;

	//Global timers, these are updated in TimerSystem::Update()
	//How many seconds the last frame took
	inline double deltaTime = 0;
	//dt clampled to 20fps to reduce physics impercision
	inline double cappedDeltaTime = 0;
	//How many seconds the program has been running
	inline double programTime = 0;
	//How many frames the program has been through
	inline uint64_t frameCount = 0;

	inline std::chrono::time_point<std::chrono::high_resolution_clock> _lastFrame;

	//Get the time since last call in seconds
	double CalculateDeltaTime();

	//A function to be called sometime in the future
	struct ScheduledFunction
	{
		enum class Type { frames, seconds };
		//Should the duration be treated as frames or seconds
		Type type;
		//How long the delay before calling is
		double duration;
		bool repeat = false;
		CallbackWrapper* function;
		double timePassed;

		bool operator == (const ScheduledFunction comp) const;
	};

	//Timer Component
	ECS_REGISTER_COMPONENT(TimerComponent)
	struct TimerComponent
	{
		//Duration of the timer in seconds
		double duration = 0;
		//Seconds passed
		double timePassed = 0;
		//Restart the timer after completion
		bool repeat = false;
		//Function to call after timer is done
		CallbackWrapper* callback = nullptr;
		//Is the timer currently running
		bool running = false;
	};

	//Timer System, Requires Timer
	ECS_REGISTER_SYSTEM(TimerSystem, TimerComponent)
	class TimerSystem : public ecs::System
	{
	private:
		static std::vector<ScheduledFunction*> schedule;

	public:
		void Init();

		void Update(bool timersActive);

		// Shorthand for starting an entity's timer
		static inline void StartTimer(ecs::Entity entity, double duration, bool repeat = false, CallbackWrapper* callback = nullptr);

		// TODO: make correctly
		void DeleteTimer(ScheduledFunction* deleteFunction);

		// Schedule a function to be executed in n seconds or frames, returns a handle to that event
		template<typename Function, typename... Args>
		static inline ScheduledFunction* ScheduleFunction(Function&& CallbackFunction, double time, bool repeat = false, ScheduledFunction::Type durationType = ScheduledFunction::Type::seconds, Args&&... Arguments);
	};

	// Clean up timer memory
	void UninitializeTimers();

}
