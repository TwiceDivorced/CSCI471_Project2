//
// Created by Phillip Romig on 7/15/24.
//

#ifndef LOGGING_H
#define LOGGING_H
#include <iostream>
#include <filesystem>
#include <string>

#ifndef  __FILE_NAME__
#define __FILE_NAME__ std::filesystem::path(__FILE__).filename().string()
#endif

inline int LOG_LEVEL = 3;
#define TRACE   if (LOG_LEVEL > 5) { std::cerr << "TRACE: "
#define DEBUG   if (LOG_LEVEL > 4) { std::cerr << "DEBUG: "
#define INFO    if (LOG_LEVEL > 3) { std::cerr << "INFO: "
#define WARNING if (LOG_LEVEL > 2) { std::cerr << "WARNING: "
#define ERROR   if (LOG_LEVEL > 1) { std::cerr << "ERROR: "
#define FATAL   if (LOG_LEVEL > 0) { std::cerr << "FATAL: "
#define ENDL  " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl; } 1==1


#endif //LOGGING_H
