#ifndef MUDUO_LOGGER_H
#define MUDUO_LOGGER_H

#include <string>

#include "muduo_noncopyable.h"

#define LOG_INFO(LogmsgFormat, ...)\
    do \
    {\
        Logger &logger = Logger::getInstance();\
        logger.setLogLevel(INFO);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__);\
        logger.write(buf);\
    }while (0)
    
#define LOG_ERROR(LogmsgFormat, ...)\
    do \
    {\
        Logger &logger = Logger::getInstance();\
        logger.setLogLevel(ERROR);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__);\
        logger.write(buf);\
    }while (0)

#define LOG_FATAL(LogmsgFormat, ...)\
    do \
    {\
        Logger &logger = Logger::getInstance();\
        logger.setLogLevel(FATAL);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__);\
        logger.write(buf);\
        exit(-1);\
    }while (0)

#define LOG_DEBUG(LogmsgFormat, ...)\
    do \
    {\
        Logger &logger = Logger::getInstance();\
        logger.setLogLevel(DEBUG);\
        char buf[1024] = {0};\
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__);\
        logger.write(buf);\
    }while (0)

enum LogLevel
{
    INFO = 0,
    ERROR,
    FATAL,
    DEBUG,
};

class Logger : noncopyable
{
public:
    //单例模式获取唯一的实例
    static Logger& getInstance();
    //设置日志级别
    void setLogLevel(LogLevel level);
    //写日志
    void write(std::string msg);

private:
    LogLevel logLevel;
    Logger(){}
};

#endif