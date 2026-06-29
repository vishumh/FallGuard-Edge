#pragma once

#include <iosfwd>

namespace fallguard {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
};

class Logger {
public:
    explicit Logger(std::ostream& output);

    void log(LogLevel level, const char* message) const;

private:
    std::ostream* output_{nullptr};
};

const char* to_string(LogLevel level);

} // namespace fallguard
