#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <string>

/// @brief Enumeration representing different types of log levels
enum LogType
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

/// @brief Structure representing a single log entry
struct LogEntry
{
    LogType type;
    std::string message;
};

/// @brief Logger class for managing and storing log messages
/// Provides static methods for logging different types of messages
class Logger
{
public:
    /// @brief Static vector to store all log entries
    /// Allows access to the complete log history
    static std::vector<LogEntry> messages;

    /// @brief Log an informational message
    /// @param message Reference to the message string to be logged
    static void Log(const std::string &message);

    /// @brief Log an error message
    /// @param message Reference to the error message string to be logged
    static void Err(const std::string &message);
};

#endif /* LOGGER_H */