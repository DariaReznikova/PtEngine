#pragma once
#include "Common.hpp"
#include "fmt/format.h"
#include <string>
#include <mutex>
#include <memory>
#include <cstdio>

#define PT_ASSERT(expr) assert(#expr && pt::Logger::get().checkAssert(expr, #expr, __FILE__, __FUNCSIG__, __LINE__))
#define PT_LOG_FATAL(expr)  do { assert(#expr && pt::Logger::get().checkFatalError(expr, #expr, __FILE__, __FUNCSIG__, __LINE__)) exit(); } while(false);
#define PT_LOG_ERROR(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Error, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_WARNING(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Warning, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_INFO(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Info, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_TRACE(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Trace, fmt::format(fmtmsg, __VA_ARGS__))

namespace pt {

enum class LogLevel {
    Error,
    Warning,
    Info,
    Trace,
};

class Logger {
    PT_SINGLETON(Logger)
public:
    Logger();

    bool initialize();
    void write(LogLevel level, std::string_view message);
    bool checkAssert(bool expr, std::string_view message, const char *file, const char *funcName, int line);

private:
    float mGetTime() const;

    std::unique_ptr<FILE, decltype(&fclose)> mLogFile;
    std::mutex mWriteMutex;
    float mStartTime;
};

}