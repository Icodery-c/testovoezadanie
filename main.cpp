#include "logger.h"
#include <iostream>
#include <thread>
#include <condition_variable>
#include <queue>
#include <utility>
#include <atomic>

std::mutex queueMutex;
std::condition_variable queueCond;
std::atomic< bool > run = false;

void logTask( std::queue< std::pair< std::string, LogPriority > >& queue, Logger& logger )
{
    while( run || !queue.empty() )
    {
        std::unique_lock lock( queueMutex );
        queueCond.wait( lock, [ & ] { return !queue.empty() || !run; } );
        while( !queue.empty() )
        {
            std::pair< std::string, LogPriority > item = queue.front();
            logger.Log( item.second, item.first );
            queue.pop();
        }
    }
}
 

int main( int argc, char* argv[] )
{
    if( argc < 2 )
    {
        std::cerr << "Usage: log_file_name [priority]" << "\n" << "[priority] - low, medium, high" << "\n";
        return 1;
    }
    
    // Парсинг приоритета
    LogPriority defPriority;
    std::string levelStr = argv[ 2 ];
    
    if( levelStr == "low" ) defPriority = LogPriority::LowPriority;
    else if( levelStr == "medium" ) defPriority = LogPriority::MediumPriority;
    else if( levelStr == "high" ) defPriority = LogPriority::HighPriority;
    else
    {
        std::cerr << "Invalid priority string" << std::endl; // не смогли пропарсить
        return 1;
    }
    
    // Парсинг пути
    std::filesystem::path current_path = std::filesystem::current_path();
    std::filesystem::path logFile = current_path / argv[ 1 ];

    
    Logger logger( logFile, defPriority);
    
    std::queue< std::pair< std::string, LogPriority > > q;

    
    // установка флага ожидания логов
    run = true;
    std::thread logThread( logTask, std::ref( q ), std::ref( logger ) );
    while( true )
    {
        std::string message;
        std::cout << "Write input or write exit - ";
        std::getline( std::cin, message );
        if( message == "exit" ) break;
        
        std::pair< std::string, LogPriority > logAndPriority;
        auto pos = message.find( " " );
        if( pos != std::string::npos )
        {
            std::string str_message = message.substr( 0, pos );
            std::string str_priority = message.substr( pos + 1 );
            
            LogPriority priority;
            
            if( str_priority == "low" ) priority = LogPriority::LowPriority;
            else if( str_priority == "medium" ) priority = LogPriority::MediumPriority;
            else if( str_priority == "high" ) priority = LogPriority::HighPriority;
            else
            {
                // не смогли пропарсить
                std::cerr << "Invalid priority string. Will be default priority" << std::endl;
                priority = defPriority;
            }
            
            logAndPriority.first = str_message;
            logAndPriority.second = priority;
            
        }
        else
        {
            logAndPriority.first = message;
            logAndPriority.second = defPriority;
        }
        
        // Передача связки лога и приоритета
        {
            std::lock_guard lock( queueMutex );
            q.push( logAndPriority );
        }
        queueCond.notify_one();
    }

    // сброс флага ожидания логов
    {
        std::lock_guard lock( queueMutex );
        run = false;
    }
    queueCond.notify_all();
    logThread.join();
}
