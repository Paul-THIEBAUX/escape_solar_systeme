#include "Profiler.h"


namespace gcle
{
	std::unordered_map<std::string, Task>* Profiler::mp_Tasks		= GCLE_NEW std::unordered_map<std::string, Task>();
	std::unordered_map<std::string, float32>* Profiler::mp_TasksTime	= GCLE_NEW std::unordered_map<std::string, float32>();

	void Profiler::Clear()
	{
		mp_Tasks->clear();
		delete mp_Tasks;

		mp_TasksTime->clear();
		delete mp_TasksTime;
	}

	void Profiler::NewTask(std::string id, std::string message)
	{
		Task t{ message, std::chrono::high_resolution_clock::now() };
		mp_Tasks->emplace(id, t);

		if (mp_TasksTime->find(id) != mp_TasksTime->end())
			mp_TasksTime->emplace(id, 0.0f);
	}

	void Profiler::EndTask(std::string id)
	{
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - mp_Tasks->at(id).Start).count(); 
		mp_Tasks->erase(id);

		float32 time = duration / 1000000.0f;
		(*mp_TasksTime)[id] = time; 
	}
	float32 Profiler::GetTask(std::string id)
	{
		if (mp_TasksTime->find(id) != mp_TasksTime->end())
		{
			return (*mp_TasksTime)[id];
		}

		return float32();
	}
}
