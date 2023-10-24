/*
 * Database.h
 * Purpose: Database class for initializing and managing database operations for logging purposes.
 *          Provides a multi-threaded mechanism for inserting and querying log entries in an SQLite database.
 *
 * @author Szymon Si¹ka³a
 * @version 1.1 30/08/2023
 */

#pragma once
#include <sstream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <sqlite3.h>
#include <spdlog/spdlog.h>

struct Database_Entry
{
    std::string timestamp;
    std::string log_level;
    std::string IP;
    std::string message;
};

class Database
{
private:
    // Variables used to handle database.
    sqlite3* db;
    std::string path_to_db;

    // Variables used to handle worker threads.
    std::size_t thread_count;
    std::vector <std::thread> threads;
    std::queue<Database_Entry> queue;
    std::mutex mutex;
    std::condition_variable condition;
    bool stop;

    /*
     * Creates the required table in the database if it doesn't exist.
     */
    void create_table();

    /*
     * Worker thread function to process database entries from the queue and insert them into the database.
     * This function is used by multiple worker threads to process entries concurrently.
     */
    void work();

    /*
     * Obtains the current timestamp in the "YYYY-MM-DD HH:MM:SS" format.
     *
     * @return The current timestamp as a formatted string.
     * @throws std::runtime_error if unable to get local time.
     */
    std::string get_timestamp();

    /*
     * Returns the log level string corresponding to the given log level enum value.
     *
     * @param[in] log_level: The log level enum value.
     * @return The corresponding log level string.
     */
    std::string get_log_level(const spdlog::level::level_enum log_level);

    /*
     * Executes a query on the database and returns the result as a formatted string.
     *
     * @param[in] query: The SQL query to execute.
     * @param[in] param: Optional parameter for the query.
     * @return The formatted result of the query.
     * @throws std::runtime_error if unable to execute the query.
     */
    std::string get_data(const std::string& query, const std::string& param = "");

    /*
     * Inserts a new log entry into the database.
     *
     * @param[in] timestamp: The timestamp of the log entry.
     * @param[in] log_level: The log level of the entry.
     * @param[in] IP: The client's IP address.
     * @param[in] message: The log message to be written.
     * @throws std::runtime_error if unable to prepare a statement, bind parameters, or execute the statement.
     */
    void insert(const std::string& timestamp, const std::string& log_level, const std::string& IP, const std::string& message);

public:
    // Deleted default constructor to prevent creating an instance of Database without specifying thread count.
    Database() = delete;

    /*
     * Constructor that creates a Database instance with a specified number of worker threads.
     * Initializes the SQLite database and creates the required table.
     *
     * @param[in] thread_count: The number of worker threads to handle database entries.
     * @throws std::runtime_error if unable to open database or create the table.
     */
    explicit Database(const std::size_t thread_count);

    /*
     * Constructor that creates a Database instance with a specified number of worker threads and a custom database path.
     * Initializes the SQLite database at the specified path and creates the required table.
     *
     * @param[in] thread_count: The number of worker threads to handle database entries.
     * @param[in] path_to_db: The path to the SQLite database file.
     * @throws std::runtime_error if unable to open database or create the table.
     */
    explicit Database(const std::size_t thread_count, const std::string& path_to_db);

    // Delete copy constructor to prevent unintended copying.
    Database(const Database&) = delete;

    /*
     * Destructor. Stops worker threads and closes the database.
     */
    ~Database();

    // Delete assignment operator to prevent unintended copying.
    Database& operator = (const Database&) = delete;

    /*
     * Add a log entry to the queue with the specified log level, client IP address, and message.
     *
     * @param[in] log_level: The log level of the entry.
     * @param[in] IP: The client's IP address.
     * @param[in] message: The log message to be written.
     */
    void add_to_queue(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message);

    /*
     * Query all log entries in the database and return the results as a formatted string.
     *
     * @return A formatted string containing all log entries.
     */
    std::string query_all();

    /*
     * Query log entries in the database based on a specific date and optional time.
     *
     * @param[in] date: The date in the format "YYYY-MM-DD".
     * @param[in] time: The time in the format "HH:MM:SS" (optional).
     * @return A formatted string containing matching log entries.
     */
    std::string query_date(const std::string& date, const std::string& time = "");

    /*
     * Query log entries in the database based on a specific client IP address.
     *
     * @param[in] IP: The client's IP address.
     * @return A formatted string containing matching log entries.
     */
    std::string query_IP(const std::string& IP);

    /*
     * Query log entries in the database based on a specific log level.
     *
     * @param[in] log_level: The log level to query for.
     * @return A formatted string containing matching log entries.
     */
    std::string query_log_level(const std::string& log_level);

    /*
     * Query log entries in the database based on a specific message content.
     *
     * @param[in] message: The log message to query for.
     * @return A formatted string containing matching log entries.
     */
    std::string query_message(const std::string& message);

    /*
     * Clear the database by dropping and recreating the logs table.
     * Note: This operation will delete all log entries.
     */
    void clear_database();
};