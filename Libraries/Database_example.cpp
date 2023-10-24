#include <iostream>

#include "Database.h"

void generate(Database& database, int n)
{
    std::string IP = "XXX.XXX.XXX.XXX";
    std::string comment;
    for (int i = 0; i < 100; i++)
    {
        if (i == 99)
        {
            comment = "ABC" + std::to_string(n) + " " + std::to_string(i) + " --- END ---";
            database.add_to_queue(spdlog::level::err, IP, comment);
        }
        else
        {
            comment = "ABC" + std::to_string(n) + " " + std::to_string(i);
            database.add_to_queue(spdlog::level::info, IP, comment);
        }

    }
}

int main()
{
    try
    {
        std::size_t thread_count = 2;
        Database database(thread_count);

        std::size_t generator_thread_count = 4;
        std::vector <std::thread> threads;

        for (std::size_t i = 0; i < generator_thread_count; i++)
        {
            threads.emplace_back(generate, std::ref(database), i);
        }

        for (std::size_t i = 0; i < generator_thread_count; i++)
        {
            threads[i].join();
        }

        std::cout << "Database entries:" << std::endl;
        std::cout << database.query_all() << std::endl;

        std::cout << "Database entries with date: 2023-08-27" << std::endl;
        std::cout << database.query_date("2023-08-27") << std::endl;

        std::cout << "Database entries with IP: 192.168.1.1" << std::endl;
        std::cout << database.query_IP("192.168.1.1") << std::endl;

        std::cout << "Database entries with log level: err" << std::endl;
        std::cout << database.query_log_level("err") << std::endl;

        std::cout << "Database entries with message: ABC2 95" << std::endl;
        std::cout << database.query_message("ABC2 95") << std::endl;

        std::cout << "Database entries after drop:" << std::endl;
        database.clear_database();
        std::cout << database.query_all() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}