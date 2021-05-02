#pragma once
#include "Common.hpp"
#include "fmt/format.h"
#include <string>
#include <mutex>
#include <memory>
#include <cstdio>

//#define PT_ASSERT(expr) assert(#expr && pt::Logger::get().checkAssert(expr, #expr, __FILE__, __FUNCSIG__, __LINE__))
#define PT_ASSERT(expr) { assert(expr && #expr); pt::Logger::get().write(pt::LogLevel::AssertionFailed, #expr, __FILE__, __FUNCSIG__, __LINE__); }   
#define PT_LOG_FATAL(fmtmsg, __FILE__, __FUNCSIG__, __LINE__) do { pt::Logger::get().write(pt::LogLevel::FatalError, fmtmsg, __FILE__, __FUNCSIG__, __LINE__); \
exit(-1); } while(false);
#define PT_LOG_ERROR(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Error, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_WARNING(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Warning, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_INFO(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Info, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_TRACE(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Trace, fmt::format(fmtmsg, __VA_ARGS__))

namespace pt {

enum class LogLevel {
    FatalError,
    Error,
    Warning,
    Info,
    Trace,
    AssertionFailed
};

class Logger {
    PT_SINGLETON(Logger)
public:
    Logger();

    bool initialize();
    void write(LogLevel level, std::string_view message);
    void write(LogLevel level, std::string_view message, const char *file, const char *funcName, int line);
   // bool checkAssert(bool expr, std::string_view message, const char *file, const char *funcName, int line);

private:
    float mGetTime() const;

    std::unique_ptr<FILE, decltype(&fclose)> mLogFile;
    std::mutex mWriteMutex;
    float mStartTime;
};

}