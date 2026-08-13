#pragma once


#include <chrono>
#include <string>
#include <unordered_map> 
#include "Utils.h"
#include "Debugger.h"

#define PROFILER

#if defined(_DEBUG) && defined(PROFILER)

#define PROFILER_START(name, message) gcle::Profiler::NewTask(name, message)
#define PROFILER_END(name) gcle::Profiler::EndTask(name)
#define PROFILER_CLEAR() gcle::Profiler::Clear()
#define PROFILER_GET(name) gcle::Profiler::GetTask(name)
#else
#define PROFILER_START(name, message) {}
#define PROFILER_END(name) {}
#define PROFILER_CLEAR() {}
#define PROFILER_GET(name) {0}
#endif // _DEBUG

namespace gcle
{
    struct Task
    {
        std::string Name;
        std::chrono::high_resolution_clock::time_point Start;

        Task(std::string name, std::chrono::high_resolution_clock::time_point start) : Name(name), Start(start) {}
    };

    struct Profiler
    {
        static void Clear();

        static void NewTask(std::string id, std::string message = "");
        static void EndTask(std::string id);
        static float32 GetTask(std::string id);

    private:
        static std::unordered_map<std::string, Task>* mp_Tasks;
        static std::unordered_map<std::string, float32>* mp_TasksTime;
    };
}