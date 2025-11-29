#ifndef MEMORY_MONITOR_HPP
#define MEMORY_MONITOR_HPP
#ifdef __linux__
#include <sys/resource.h>
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#endif

class MemoryMonitor {
public:
    static size_t getCurrentMemoryUsage() {
#ifdef __linux__
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            return usage.ru_maxrss; 
        }
#elif defined(_WIN32)
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize / 1024; 
        }
#endif
        return 0;
    }
};

#endif