#pragma once

#include <memory>
#include <string>
#include <format>

namespace Cosmic {
    enum class LogLevel {
        Message = -1,
        Trace = 0,
        Info,
        Warn,
        Error,
        Fatal
    };

    class Log {
    public:
        static void init();
        static std::shared_ptr<Log> &getCoreLogger();

        void setLogLevel(LogLevel level);
        void log(LogLevel level, const std::string &message);

        template<typename... Args>
        void log(const LogLevel level, const std::format_string<Args...> &formatString, Args &&... args) {
            log(level, std::format(formatString, std::forward<Args>(args)...));
        }
    private:
        static std::string getCurrentTime();
        static constexpr const char* getLogLevelString(LogLevel level) noexcept; // unchanged
        static void applyColor(LogLevel level);
        static void resetColor();

        static std::shared_ptr<Log> coreLogger;
        LogLevel logLevel = LogLevel::Trace;
    };
}

#define CS_FATAL(...) { Cosmic::Log::getCoreLogger()->log(LogLevel::Fatal, __VA_ARGS__); }
#define CS_ERROR(...) { Cosmic::Log::getCoreLogger()->log(LogLevel::Error, __VA_ARGS__); }
#define CS_WARN(...)  { Cosmic::Log::getCoreLogger()->log(LogLevel::Warn, __VA_ARGS__); }
#define CS_INFO(...)  { Cosmic::Log::getCoreLogger()->log(LogLevel::Info, __VA_ARGS__); }
#define CS_TRACE(...) { Cosmic::Log::getCoreLogger()->log(LogLevel::Trace, __VA_ARGS__); }
#define CS_MESSAGE(...) { Cosmic::Log::getCoreLogger()->log(LogLevel::Message, __VA_ARGS__); }
