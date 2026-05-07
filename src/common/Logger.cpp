#include "common/Logger.h"
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace NanoDB {

Logger* Logger::instance_ = nullptr;

Logger::Logger() : logFile_(nullptr), initialized_(false) {
#ifdef _WIN32
    mutex_ = new CRITICAL_SECTION;
    InitializeCriticalSection((CRITICAL_SECTION*)mutex_);
#else
    pthread_mutex_t* mtx = new pthread_mutex_t;
    pthread_mutex_init(mtx, nullptr);
    mutex_[0] = (unsigned int)mtx;
#endif
}

Logger::~Logger() {
    shutdown();
#ifdef _WIN32
    DeleteCriticalSection((CRITICAL_SECTION*)mutex_);
    delete (CRITICAL_SECTION*)mutex_;
#else
    pthread_mutex_t* mtx = (pthread_mutex_t*)mutex_[0];
    pthread_mutex_destroy(mtx);
    delete mtx;
#endif
}

Logger* Logger::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new Logger();
    }
    return instance_;
}

void Logger::destroy() {
    if (instance_ != nullptr) {
        delete instance_;
        instance_ = nullptr;
    }
}

void Logger::initialize() {
    lock();
    
    if (!initialized_) {
        logFile_ = fopen("logs/nanodb_execution.log", "w");
        if (logFile_ != nullptr) {
            fclose(logFile_);
        }
        logFile_ = fopen("logs/nanodb_execution.log", "a");
        initialized_ = true;
        
        const char* startupMsg = "[INFO] NanoDB Logger initialized\n";
        writeLog(startupMsg);
    }
    
    unlock();
}

void Logger::shutdown() {
    lock();
    
    if (logFile_ != nullptr) {
        fclose(logFile_);
        logFile_ = nullptr;
    }
    initialized_ = false;
    
    unlock();
}

void Logger::log(LogLevel level, const char* message) {
    if (message == nullptr) {
        return;
    }
    
    lock();
    
    char formattedMessage[512];
    const char* levelStr = levelToString(level);
    
    int pos = 0;
    formattedMessage[pos++] = '[';
    
    int len = 0;
    while (levelStr[len] != '\0') {
        formattedMessage[pos++] = levelStr[len++];
    }
    
    formattedMessage[pos++] = ']';
    formattedMessage[pos++] = ' ';
    
    len = 0;
    while (message[len] != '\0' && pos < 510) {
        formattedMessage[pos++] = message[len++];
    }
    
    formattedMessage[pos++] = '\n';
    formattedMessage[pos] = '\0';
    
    writeLog(formattedMessage);
    
    unlock();
}

void Logger::logInfo(const char* message) {
    log(LogLevel::INFO, message);
}

void Logger::logLog(const char* message) {
    log(LogLevel::LOG, message);
}

void Logger::logWarn(const char* message) {
    log(LogLevel::WARN, message);
}

void Logger::logError(const char* message) {
    log(LogLevel::ERROR, message);
}

void Logger::clearLogFile() {
    lock();
    
    if (logFile_ != nullptr) {
        fclose(logFile_);
    }
    
    logFile_ = fopen("logs/nanodb_execution.log", "w");
    if (logFile_ != nullptr) {
        fclose(logFile_);
    }
    
    logFile_ = fopen("logs/nanodb_execution.log", "a");
    
    unlock();
}

void Logger::writeLog(const char* formattedMessage) {
    printf("%s", formattedMessage);
    fflush(stdout);
    
    if (logFile_ != nullptr) {
        fprintf(logFile_, "%s", formattedMessage);
        fflush(logFile_);
    }
}

const char* Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::LOG:
            return "LOG";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

void Logger::lock() {
#ifdef _WIN32
    EnterCriticalSection((CRITICAL_SECTION*)mutex_);
#else
    pthread_mutex_lock((pthread_mutex_t*)mutex_[0]);
#endif
}

void Logger::unlock() {
#ifdef _WIN32
    LeaveCriticalSection((CRITICAL_SECTION*)mutex_);
#else
    pthread_mutex_unlock((pthread_mutex_t*)mutex_[0]);
#endif
}

} // namespace NanoDB
