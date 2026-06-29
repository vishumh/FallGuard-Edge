#include "fallguard/logger.hpp"

#include <ostream>

namespace fallguard {

Logger::Logger(std::ostream& output)
    : output_(&output)
{
}

void Logger::log(LogLevel level, const char* message) const
{
    if (output_ == nullptr) {
        return;
    }

    *output_ << '[' << to_string(level) << "] " << message << '\n';
}

const char* to_string(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:
        return "debug";
    case LogLevel::Info:
        return "info";
    case LogLevel::Warning:
        return "warning";
    case LogLevel::Error:
        return "error";
    }

    return "unknown";
}

} // namespace fallguard
