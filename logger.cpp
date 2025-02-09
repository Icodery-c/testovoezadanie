#include "logger.h"
#include <iostream>
#include <ctime>
#include <exception>


void Logger::SetPriority( LogPriority priority )
{
    priority_ = priority;
}

std::string Logger::LogPriorityToString(LogPriority level)
{
    switch (level) {
        case LogPriority::LowPriority: return "Low";
        case LogPriority::MediumPriority: return "Medium";
        case LogPriority::HighPriority: return "High";
        default: return "Unknown";
    }
}

void Logger::Log(LogPriority level, std::string  message)
{
    if ( level < priority_ )
    {
        return;
    }

    std::time_t now = std::time( nullptr );
    char timestamp[20];
    std::strftime( timestamp, sizeof( timestamp ), "%Y-%m-%d %H:%M:%S", std::localtime( &now ) );

    file_ << timestamp << " [" << LogPriorityToString( level ) << "] " << message << "\n";
}

Logger::Logger( std::filesystem::path filePath, LogPriority priority )
{
    std::filesystem::perms p = std::filesystem::status( filePath ).permissions();
    if ( (p & std::filesystem::perms::owner_write ) == std::filesystem::perms::none )
    {
        throw std::runtime_error("no write permissions");
    }

    file_.open( filePath, std::ios::app );
    priority_ = priority;
}
