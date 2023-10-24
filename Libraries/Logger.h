/*
 * Logger.h
 * Purpose: Logger class for initializing and managing logging to a daily file.
 *          Provides a multi-threaded logging mechanism with queue-based message handling,
 *          enabling efficient and organized logging of messages from various sources.
 *
 * @author Szymon Si¹ka³a
 * @version 1.2 30/08/2023
 */

#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

struct Log_Entry
{
    spdlog::level::level_enum log_level;
    std::string IP;
    std::string message;
};

class Logger
{
private:
    // Variables used to handle logs.
    std::shared_ptr<spdlog::logger> logger;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;
    std::string path_to_file;

    // Variables used to handle worker threads.
    std::size_t thread_count;
    std::vector <std::thread> threads;
    std::queue<Log_Entry> queue;
    std::mutex mutex;
    std::condition_variable condition;
    bool stop;

    /*
     * Log a message with the specified log level, client IP address, and message content.
     *
     * @param[in] log_level: The log level for the message.
     * @param[in] IP: The client's IP address.
     * @param[in] message: The log message to be written.
     * @throws std::runtime_error if the logger is not initialized.
     */
    void write(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message);

    /*
     * Worker thread function to process log messages from the queue and write them to the log file.
     * This function is used by multiple worker threads to process messages concurrently.
     */
    void work();

public:
    // Deleted default constructor to prevent creating an instance of Logger without specifying thread count.
    Logger() = delete;

    /*
     * Constructor that creates a Logger instance with a specified number of worker threads.
     * Log messages are written to a default file named "log.txt" in the "C:\Logs" directory.
     *
     * @param[in] thread_count: The number of worker threads to handle log messages.
     * @throws std::runtime_error if unable to create file sink or logger.
     */
    explicit Logger(const std::size_t thread_count);

    /*
     * Constructor that creates a Logger instance with a specified number of worker threads.
     * Log messages are written to a file with custom name in the "Logs" directory.
     *
     * @param[in] path_to_file: Path to the log file.
     * @param[in] thread_count: The number of worker threads to handle log messages.
     * @throws std::runtime_error if unable to create file sink or logger.
     */
    explicit Logger(const std::size_t thread_count, const std::string& path_to_file);

    // Delete copy constructor to prevent unintended copying
    Logger(const Logger&) = delete;

    /*
     * Destructor. Stops worker threads and releases resources.
     */
    ~Logger();

    // Delete assignment operator to prevent unintended copying
    Logger& operator = (const Logger&) = delete;

    /*
     * Add a log message to the queue with the default 'info' log level.
     *
     * @param[in] IP: The client's IP address.
     * @param[in] message: The log message to be written.
     */
    void add_to_queue(const std::string& IP, const std::string& message);

    /*
     * Add a log message to the queue with a specified log level.
     *
     * @param[in] log_level: The log level of the message.
     * @param[in] IP: The client's IP address.
     * @param[in] message: The log message to be written.
     */
    void add_to_queue(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message);
};