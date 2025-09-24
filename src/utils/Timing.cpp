#include "utils/Timing.h"

#include <chrono>
#include <vector>
#include <functional>

#include "ECS.h"
#include "utils/Callback.h"

namespace engine
{
	//Get the time since last call in seconds
	double CalculateDeltaTime()
	{
		std::chrono::time_point thisFrame = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> duration = thisFrame - _lastFrame;
		deltaTime = duration.count();
		_lastFrame = thisFrame;
		return deltaTime;
	}

	bool ScheduledFunction::operator==(const ScheduledFunction comp) const
	{
		return
			type == comp.type
			&& duration == comp.duration
			&& repeat == comp.repeat
			&& function == comp.function
			&& timePassed == comp.timePassed;
	}

	void TimerSystem::Init()
	{
		// Start Time
		_lastFrame = std::chrono::high_resolution_clock::now();
	}

	void TimerSystem::Update(bool timersActive)
	{
		if (timersActive)
		{
			// Iterate through scheduled functions
			for (auto itr = schedule.begin(); itr != schedule.end();)
			{
				// Get the entity and increment the iterator
				ScheduledFunction* future = *itr;

				if (future->type == ScheduledFunction::Type::seconds)
				{
					// Timer is realtime, add deltaTime
					future->timePassed += deltaTime;
				}
				else
				{
					// Timer is in frames, add one frame
					future->timePassed++;
				}

				// Whether timer is done
				while (future->timePassed >= future->duration)
				{
					future->timePassed -= future->duration;

					future->function->Call();

					// If not repeating, delete the event
					if (!future->repeat)
					{
						// Remove function from _CallbackWrappers
						auto it = std::find(_CallbackWrappers.begin(), _CallbackWrappers.end(), future->function);
						if (it != _CallbackWrappers.end())
						{
							_CallbackWrappers.erase(it);
						}

						delete future->function;
						future->function = nullptr;

						itr = schedule.erase(itr);
						break;
					}
				}

				// Iterator can get a new value so make sure it is not at end
				if (itr != schedule.end())
				{
					itr++;
				}
			}

			// Iterate through entities
			for (ecs::Entity entity : entities)
			{
				TimerComponent& timer = ecs::GetComponent<TimerComponent>(entity);

				// If timer is not done
				if (timer.timePassed < timer.duration)
				{
					timer.timePassed += deltaTime;
				}
				else
				{
					timer.timePassed = 0;

					// Call callback if applicable
					if (timer.callback)
					{
						timer.callback->Call();
					}

					// Stop timer if not repeating
					if (!timer.repeat)
					{
						timer.running = false;
					}
				}
			}
		}

		// Update all the time stuff
		CalculateDeltaTime();
		frameCount++;
		programTime += deltaTime;
		cappedDeltaTime = std::min(deltaTime, 1.0 / 20.0);
	}

	// Shorthand for starting an entity's timer
	inline void TimerSystem::StartTimer(ecs::Entity entity, double duration, bool repeat, CallbackWrapper* callback)
	{
		TimerComponent& timer = ecs::GetComponent<TimerComponent>(entity);

		timer.duration = duration;
		timer.timePassed = 0;
		timer.repeat = repeat;
		timer.callback = callback;
		timer.running = true;
	}

	// TODO: make correctly
	void TimerSystem::DeleteTimer(ScheduledFunction* deleteFunction)
	{
		auto it = std::find(schedule.begin(), schedule.end(), deleteFunction);

		// If the pointer is found, erase it from the vector
		if (it != schedule.end())
		{
			schedule.erase(it);
			delete deleteFunction;
		}
	}

	// Schedule a function to be executed in n seconds or frames, returns a handle to that event
	template<typename Function, typename... Args>
	inline ScheduledFunction* TimerSystem::ScheduleFunction(Function&& CallbackFunction, double time, bool repeat, ScheduledFunction::Type durationType, Args&&... Arguments) {
		// Deleted when ending engine TODO: delete at ScheduleFunction deconstructor
		CallbackWrapper* wrapper = new CallbackWrapper(std::bind(std::forward<Function>(CallbackFunction), std::forward<Args>(Arguments)...));
		_CallbackWrappers.push_back(wrapper);

		// Deleted at DeleteTimer or when hits timer and isnt repeating TODO: This is currently a memory leak
		ScheduledFunction* future = new ScheduledFunction{ .type = durationType, .duration = time, .repeat = repeat, .function = wrapper };
		schedule.push_back(future);

		return future;
	}

	std::vector<ScheduledFunction*> TimerSystem::schedule = schedule;

	// Clean up timer memory
	void UninitializeTimers()
	{
		for (auto CallbackWrapperToDelete : _CallbackWrappers)
		{
			delete CallbackWrapperToDelete;
		}
	}
}
