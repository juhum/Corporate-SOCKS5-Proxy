#include <iostream>

#include "./Logger.h"

void generate(Logger& logger, int n)
{
    std::string IP = "XXX.XXX.XXX.XXX";
    std::string comment;
    for (int i = 0; i < 100; i++)
    {
        if (i == 99)
        {
            comment = "ABC" + std::to_string(n) + " " + std::to_string(i) + " --- END ---";
            logger.add_to_queue(spdlog::level::err, IP, comment);
        }
        else
        {
            comment = "ABC" + std::to_string(n) + " " + std::to_string(i);
            logger.add_to_queue(IP, comment);
        }

    }
}

int main()
{
    try
    {
        std::size_t logger_thread_count = 2;
        Logger logger(logger_thread_count);
        Logger logger2("C:\\Logs\\log_name.txt", logger_thread_count);
        Logger logger3("C:\\Logs\\log_name", logger_thread_count);

        std::size_t generator_thread_count = 4;
        std::vector <std::thread> threads;

        for (std::size_t i = 0; i < generator_thread_count; i++)
        {
            threads.emplace_back(generate, std::ref(logger), i);
        }

        for (std::size_t i = 0; i < generator_thread_count; i++)
        {
            threads[i].join();
        }
    }
    catch (std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
    }

    return 0;
}