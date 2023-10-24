/*
 * Database.h
 * Purpose: Database class for initializing and managing database operations for logging purposes.
 *          Provides a multi-threaded mechanism for inserting and querying log entries in an SQLite database.
 *
 * @author Szymon Si¹ka³a
 * @version 1.1 30/08/2023
 */

#include "Database.h"

void Database::create_table()
{
    const std::string query = "CREATE TABLE IF NOT EXISTS logs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "timestamp TEXT,"
        "log_level TEXT,"
        "IP TEXT,"
        "message TEXT)";

    const int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK)
    {
        throw std::runtime_error("Unable to create a table.");
    }
}

void Database::work()
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

        const Database_Entry& entry = queue.front();
        insert(entry.timestamp, entry.log_level, entry.IP, entry.message);
        queue.pop();
        lock.unlock();
    }
}

std::string Database::get_timestamp()
{
    time_t now = time(nullptr);
    struct tm time_info = {};

    if (localtime_s(&time_info, &now) != 0)
    {
        throw std::runtime_error("Unable to get local time.");
    }

    char timestamp_buffer[20] = {};
    strftime(timestamp_buffer, sizeof(timestamp_buffer), "%Y-%m-%d %H:%M:%S", &time_info);
    return timestamp_buffer;
}

std::string Database::get_log_level(const spdlog::level::level_enum log_level)
{
    switch (log_level)
    {
    case spdlog::level::trace: return "trace";
    case spdlog::level::debug: return "debug";
    case spdlog::level::info: return "info";
    case spdlog::level::warn: return "warn";
    case spdlog::level::err: return "err";
    case spdlog::level::critical: return "critical";
    case spdlog::level::off: return "off";
    default: return "unknown";
    }
}


std::string Database::get_data(const std::string& query, const std::string& param)
{
    std::stringstream result("");
    sqlite3_stmt* stmt;
    const int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK)
    {
        if (!param.empty())
        {
            sqlite3_bind_text(stmt, 1, param.c_str(), -1, SQLITE_STATIC);
        }
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const int id = sqlite3_column_int(stmt, 0);
            const unsigned char* timestamp = sqlite3_column_text(stmt, 1);
            const unsigned char* logLevelStr = sqlite3_column_text(stmt, 2);
            const unsigned char* IP = sqlite3_column_text(stmt, 3);
            const unsigned char* msg = sqlite3_column_text(stmt, 4);

            result << "ID: " << id << "\n"
                << "Timestamp: " << timestamp << "\n"
                << "Log Level: " << logLevelStr << "\n"
                << "IP: " << IP << "\n"
                << "Message: " << msg << "\n";
        }
        sqlite3_finalize(stmt);
    }
    else
    {
        throw std::runtime_error("Unable to execute query.");
    }

    return result.str();
}

void Database::insert(const std::string& timestamp, const std::string& log_level, const std::string& IP, const std::string& message)
{

    const std::string query = "INSERT INTO logs (timestamp, log_level, IP, message) VALUES (?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        throw std::runtime_error("Unable to prepare a statement.");
    }
    sqlite3_bind_text(stmt, 1, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, log_level.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, IP.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, message.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Unable to bind parameters.");
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Unable to execute statement.");
    }

    sqlite3_finalize(stmt);
}

Database::Database(const std::size_t thread_count) : path_to_db("C:\\Proxy_server\\database.db"), thread_count(thread_count), stop(false)
{
    const int rc = sqlite3_open_v2(path_to_db.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (rc)
    {
        throw std::runtime_error("Unable to open database.");
    }

    create_table();

    for (std::size_t i = 0; i < thread_count; ++i)
    {
        threads.emplace_back([this] { work(); });
    }
}

Database::Database(const std::size_t thread_count, const std::string& path_to_db) : path_to_db(path_to_db), thread_count(thread_count), stop(false)
{
    const int rc = sqlite3_open_v2(path_to_db.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (rc)
    {
        throw std::runtime_error("Unable to open database.");
    }

    create_table();

    for (std::size_t i = 0; i < thread_count; ++i)
    {
        threads.emplace_back([this] { work(); });
    }
}

Database::~Database()
{
    std::unique_lock<std::mutex> lock(mutex);
    stop = true;
    condition.notify_all();
    lock.unlock();

    for (std::thread& thread : threads)
    {
        thread.join();
    }

    sqlite3_close(db);
}

void Database::add_to_queue(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message)
{
    const std::string timestamp = get_timestamp();
    const std::string log_level_string = get_log_level(log_level);

    std::unique_lock<std::mutex> lock(mutex);
    queue.push({ timestamp, log_level_string, IP, message });
    condition.notify_one();
}

std::string Database::query_all()
{
    const std::string query = "SELECT * FROM logs;";

    return get_data(query);
}

std::string Database::query_date(const std::string& date, const std::string& time)
{
    std::string query = "SELECT * FROM logs WHERE timestamp >= ?";
    if (!time.empty())
    {
        query += " AND timestamp <= ?";
    }

    return get_data(query, time.empty() ? date : date + " " + time);
}

std::string Database::query_IP(const std::string& IP)
{
    const std::string query = "SELECT * FROM logs WHERE IP = ?";

    return get_data(query, IP);
}

std::string Database::query_log_level(const std::string& log_level)
{
    const std::string query = "SELECT * FROM logs WHERE log_level = ?";

    return get_data(query, log_level);
}

std::string Database::query_message(const std::string& message)
{
    const std::string query = "SELECT * FROM logs WHERE message = ?";

    return get_data(query, message);
}

void Database::clear_database()
{
    const std::string query = "DROP TABLE IF EXISTS logs";
    const int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK)
    {
        throw std::runtime_error("Unable to drop table.");
    }
    create_table();
}