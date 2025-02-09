#pragma once

#include <fstream>
#include <string>
#include <filesystem>

enum class LogPriority {
    LowPriority,
    MediumPriority,
    HighPriority
};

class Logger {
    LogPriority priority_;
    std::ofstream file_;

    std::string LogPriorityToString(LogPriority priority);

public:
    
    // При отсутствии прав на запись выбрасывает исключение runtime_error
    Logger(std::filesystem::path filepath, LogPriority priority);

    void SetPriority(LogPriority priority);

    void Log(LogPriority priority,  std::string mes);
};
