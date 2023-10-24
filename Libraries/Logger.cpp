/*
 * Logger.cpp
 * Purpose: Logger class for initializing and managing logging to a daily file.
 *          Provides a multi-threaded logging mechanism with queue-based message handling,
 *          enabling efficient and organized logging of messages from various sources.
 *
 * @author Szymon Si¹ka³a
 * @version 1.2 30/08/2023
 */

#include "Logger.h"

void Logger::write(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message)
{
    if (!logger)
    {
        throw std::runtime_error("Logger is not initialized.");
    }

    logger->log(log_level, "Client IP: {}, {}", IP, message);
}

void Logger::work()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [this] { return stop || !queue.empty(); });

        if (stop && queue.empty())
        {
            lock.unlock();
            return;
        }

        const Log_Entry& entry = queue.front();
        write(entry.log_level, entry.IP, entry.message);
        queue.pop();
        lock.unlock();
    }
}

Logger::Logger(const std::size_t thread_count) : path_to_file("C:\\Logs\\log.txt"), thread_count(thread_count), stop(false)
{
    file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(path_to_file, 0, 0);
    if (!file_sink)
    {
        throw std::runtime_error("Unable to create file sink for logger.");
    }

    logger = std::make_shared<spdlog::logger>("logger", file_sink);
    if (!logger)
    {
        throw std::runtime_error("Unable to create logger.");
    }

    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");

    for (std::size_t i = 0; i < thread_count; ++i)
    {
        threads.emplace_back([this] { work(); });
    }
}

Logger::Logger(const std::size_t thread_count, const std::string& path_to_file) : path_to_file(path_to_file), thread_count(thread_count), stop(false)
{
    file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(path_to_file, 0, 0);
    if (!file_sink)
    {
        throw std::runtime_error("Unable to create file sink for logger.");
    }

    logger = std::make_shared<spdlog::logger>("logger", file_sink);
    if (!logger)
    {
        throw std::runtime_error("Unable to create logger.");
    }

    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");

    for (std::size_t i = 0; i < thread_count; ++i)
    {
        threads.emplace_back([this] { work(); });
    }
}

Logger::~Logger()
{
    std::unique_lock<std::mutex> lock(mutex);
    stop = true;
    condition.notify_all();
    lock.unlock();

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    if (logger)
    {
        logger->flush();
        spdlog::drop_all();
    }
}

void Logger::add_to_queue(const std::string& IP, const std::string& message)
{
    add_to_queue(spdlog::level::info, IP, message);
}

void Logger::add_to_queue(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message)
{
    std::unique_lock<std::mutex> lock(mutex);
    queue.push({ log_level, IP, message });
    condition.notify_one();
}