#ifndef NANODB_LOGGER_H
#define NANODB_LOGGER_H

namespace NanoDB {

enum class LogLevel {
    INFO,
    LOG,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger* getInstance();
    static void destroy();
    
    void initialize();
    void shutdown();
    
    void log(LogLevel level, const char* message);
    void logInfo(const char* message);
    void logLog(const char* message);
    void logWarn(const char* message);
    void logError(const char* message);
    
    void clearLogFile();
    
private:
    Logger();
    ~Logger();
    
    Logger(const Logger&);
    Logger& operator=(const Logger&);
    
    void writeLog(const char* formattedMessage);
    const char* levelToString(LogLevel level);
    
    static Logger* instance_;
    
    FILE* logFile_;
    bool initialized_;
    
#ifdef _WIN32
    void* mutex_;
#else
    unsigned int mutex_[5];
#endif
    
    void lock();
    void unlock();
};

} // namespace NanoDB

#define NANOLOG(level, message) NanoDB::Logger::getInstance()->log(NanoDB::LogLevel::level, message)

#endif // NANODB_LOGGER_H
