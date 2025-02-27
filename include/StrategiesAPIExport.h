#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #ifdef STRATEGIES_API_EXPORT
        #define STRATEGIES_API __declspec(dllexport)
    #else
        #define STRATEGIES_API __declspec(dllimport)
    #endif
#elif defined(__linux__) || defined(__unix__)
    #define STRATEGIES_API __attribute__((visibility("default")))
#else
    #define STRATEGIES_API
#endif
