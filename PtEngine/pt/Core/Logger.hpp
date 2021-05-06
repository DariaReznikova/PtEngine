#pragma once
#include "Common.hpp"
#include "fmt/format.h"
#include <string>
#include <mutex>
#include <memory>
#include <cstdio>

#define PT_ASSERT(expr) do { if (!(expr)) { pt::Logger::get().write(pt::LogLevel::AssertionFailed, __FILE__, __FUNCSIG__, __LINE__,  #expr); assert(expr);} }  while(false);
#define PT_LOG_FATAL(fmtmsg, ...) do { pt::Logger::get().write(pt::LogLevel::FatalError, __FILE__, __FUNCSIG__, __LINE__, fmt::format(fmtmsg, __VA_ARGS__)); exit(-1); } while(false);
#define PT_LOG_ERROR(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Error, __FILE__, __FUNCSIG__, __LINE__, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_WARNING(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Warning, __FILE__, __FUNCSIG__, __LINE__, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_INFO(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Info, __FILE__, __FUNCSIG__, __LINE__, fmt::format(fmtmsg, __VA_ARGS__))
#define PT_LOG_TRACE(fmtmsg, ...) pt::Logger::get().write(pt::LogLevel::Trace, __FILE__, __FUNCSIG__, __LINE__, fmt::format(fmtmsg, __VA_ARGS__))

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
        void write(LogLevel level, const char* file, const char* funcName, int line, std::string_view message);

    private:
        float mGetTime() const;

        std::unique_ptr<FILE, decltype(&fclose)> mLogFile;
        std::mutex mWriteMutex;
        float mStartTime;
    };

}