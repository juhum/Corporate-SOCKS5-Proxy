# Corporate SOCKS5 Proxy
Corporate SOCKS5 Proxy is a powerful proxy server application designed for internal corporate use. It enables secure and efficient data exchange between the corporate network and the external internet through a SOCKS5 proxy server. This README provides an overview of the project's structure and usage.

## Table of Contents
1. [Project Structure](#project-structure)
2. [Authentication Method Class](#authentication-method-class)
3. [Database Class](#database-class)
4. [Logger Class](#logger-class)
5. [Proxy Configuration Class](#proxyconfiguration-class)
6. [Proxy Server](#proxyserver-class)
7. [Technologies and Dependencies](#technologies-and-dependencies)
8. [Installation Guide](#installation-guide)
9. [Usage Examples](#usage-examples)
10. [Authors](#authors)

## Project Structure

The project directory structure is as follows:
- `Installation package/`: Contains files necessary to install application.
  - `FoxyProxy configuration` : Contains files with FoxyProxy add-on configuration.
    - `Proxy_no_authentication.png` : File with configuration for no authentication option.
    - `Proxy_username_password.png` : File with configuration for username and password authentication option.
  - `config.ini` : exemplary configuration file.
  - `Install.bat` : Installer.
  - `Server application.exe` : Application executable.
  - `sqlite3.dll` : SQLite3 dll file necessary for proper operation of the application.
  - `Uninstall.bat` : Uninstaller.

- `Libraries/`: Contains the project's custom libraries and modules.
  - `Authentication_Method.h`: Abstract base class for defining authentication methods.
  - `Authenticator.cpp`: Implementation of a class that delegates the authentication process to the provided method.
  - `Authenticator.h`: Header file for a class that delegates the authentication process to the provided method.
  - `Authenticator_example.cpp`: Example code demonstrating how to use the Authenticator module.
  - `Database.cpp`: Implementation of the database module.
  - `Database.h`: Header file for the database module.
  - `Database_example.cpp`: Example code demonstrating how to use the Database module.
  - `GSSAPI.cpp`: Implementation of a class that allows a user to be authenticated using the GSSAPI protocol.
  - `GSSAPI.h`: Header file for a class that allows a user to be authenticated using the GSSAPI protocol.
  - `Handle_Authentication.cpp`: Implementation of a class that handles authentication for a given socket.
  - `Handle_Authentication.h`: Header file for a class that handles authentication for a given socket.
  - `Logger.cpp`: Implementation of the logging module.
  - `Logger.h`: Header file for the logging module.
  - `Logger_example.cpp`: Example code demonstrating how to use the Logger module.
  - `No_Authentication.cpp`: Implementation of a class that allows any user to be authenticated without any checks.
  - `No_Authentication.h`: Header file for a class that allows any user to be authenticated without any checks.
  - `ProxyConfiguration.cpp`: Implementation of the proxy configuration module.
  - `ProxyConfiguration.h`: Header file for the proxy configuration module.
  - `ProxyServer.cpp`: Implementation of the proxy server.
  - `ProxyServer.h`: Header file for the proxy server.
  - `Username_Password.cpp`: Implementation of a class that allows a user to be authenticated by username and password.
  - `Username_Password.h`: Header file for a class that allows a user to be authenticated by username and password.

- `.gitignore`: Specifies files and directories to be ignored by Git.

- `README.md`: This file, providing project information, instructions, and structure.

- `config.ini`: Configuration file for the Corporate SOCKS5 Proxy.

- `main.cpp`: The main entry point for the Corporate SOCKS5 Proxy application.

## Authentication Method Classes
The Authentication Method Classes offer a range of strategies to facilitate user authentication in application.

### Features
- Abstract base class (`Authentication_Method`) for defining authentication methods.
- Concrete implementations (`No_Authentication`, `Username_Password`, `GSSAPI`) for different authentication strategies.
- `Authenticator` class to facilitate authentication using the chosen method.
- `Handle_Authentication` class that handles authentication for a given socket.

### Installation
1. Include the necessary header files in your code:
   ```cpp
   #include "Handle_Authentication.h"
   ```
2. Compile your project with the required dependencies.

## Database Class

The `Database` class is designed to facilitate database operations for logging purposes. It provides a multi-threaded mechanism for inserting and querying log entries in an SQLite database.

### Features

- Initialization of an SQLite database for logging.
- Multi-threaded support for inserting log entries efficiently.
- Querying log entries based on various criteria.
- Clearing the database by dropping and recreating the logs table.

### Installation
1. Make sure you have SQLite3 and spdlog installed.
2. Include the necessary header files in your code:
   ```cpp
   #include "Database.h"
   ```
3. Compile your project with the required dependencies.


## Logger Class
The Logger class is a multi-threaded logging utility designed to efficiently manage and log messages from various sources in a concurrent environment. This README provides a brief overview of the Logger class. For detailed API documentation and customization options, refer to the comments within the Logger.h header file.

### Features
- Multi-threaded logging for efficient message handling.
- Queue-based mechanism to organize and process log messages.
- Support for custom log levels and message formatting.

### Installation
1. Include the necessary header files in your code:
   ```cpp
   #include "Logger.h"
   ```
2. Compile your project with the required dependencies.

## ProxyConfiguration Class

The `ProxyConfiguration` class manages the configuration of a proxy server, allowing you to define lists of allowed and blocked IP addresses and ports, as well as various server parameters.

### Features

- Add and remove IP addresses from the allowed and blocked lists.
- Add and remove ports from the allowed and blocked lists.
- Set the IP address and port of the proxy server.
- Specify directories for log files, authentication files, and other file-related settings, such as database files.
- Define the number of active threads for logging.
- Choose the logging and authentication methods.

### Installation

To use the `ProxyConfiguration` class, follow these steps:

1. Make sure you have Boost installed.
2. Include the necessary header files in your code:
   ```cpp
   #include "ProxyConfiguration.h"
   ```
3. Create an instance of the `ProxyConfiguration` class.
4. Use the class methods to configure and manage your proxy server settings.

For detailed information on how to use the `ProxyConfiguration` class and its methods, refer to the code comments within the class.

# ProxyServer Class

The `ProxyServer` class is responsible for managing a proxy server that handles incoming client connections and forwards traffic according to the provided configuration.

## Features

- Accepts incoming client connections and establishes proxy sessions.
- Supports SOCKS5 protocol for handling client requests.
- Filters client requests based on allowed and blocked IP addresses and ports.
- Provides logging capabilities with different methods, including file-based and database-based logging.
- Integrates with external components such as a `Authenticator`, a `Database`, a `Logger`, a `ProxyConfiguration`. 
- Efficiently handles data forwarding between clients and target servers.

## Usage

To use the `ProxyServer` class in your project, follow these steps:

1. Include the required libraries such as `<iostream>`, `<vector>`, `<algorithm>`, and `<boost/asio.hpp>`.
2. Include the header files for other classes used by `ProxyServer`, such as `Logger`, `Authenticator`, `Database`, and `ProxyConfiguration`.
3. Create an instance of the `ProxyServer` class by providing the necessary parameters:
   - `io_context`: Boost ASIO's I/O context for managing asynchronous operations.
   - `ip_address`: The IP address on which the proxy server should listen.
   - `port`: The port on which the proxy server should listen.
   - `config`: An instance of the `ProxyConfiguration` class for configuring proxy settings.
   - `logging_method`: An integer specifying the logging method (e.g., 1 for file-based logging).
   - `logger`: An instance of the `Logger` class for handling log messages.
   - `database`: An instance of the `Database` class for database-based logging.

4. Start the proxy server by calling the appropriate methods, such as `start_accept`.

Please note that these are general usage instructions, and specific steps may vary depending on your project and development environment.


## Example

Here's an example of how to create and start a `ProxyServer` instance:

```cpp
#include <iostream>

#include "Libraries/ProxyServer.h"

int main() {
    try {
        boost::asio::io_context io_context;

        ProxyConfiguration proxyConfig;

        proxyConfig.loadConfigFromIni("C:\\Proxy_server\\config.ini");

        std::shared_ptr<Logger> logger = std::make_shared<Logger>(2, proxyConfig.getLogFilesDir());
        std::shared_ptr<Database> database = std::make_shared<Database>(2, proxyConfig.getDbFilesDir());

        ProxyServer server(io_context, proxyConfig.getProxyServerIp(), proxyConfig.getProxyServerPort(), proxyConfig, proxyConfig.getLoggingMethod(), logger, database);
        std::cout << "Proxy server started. Listening on " << proxyConfig.getProxyServerIp() << ":" << proxyConfig.getProxyServerPort() << std::endl;

        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
```

## Technologies and Dependencies

Before getting started with the Corporate SOCKS5 Proxy, make sure you have the following technologies and dependencies installed:

- **C++ 20**: Ensure you have a C++20 compliant compiler installed.
- **Boost.Asio**: You can download and install Boost from [the official Boost website](https://www.boost.org/users/download/).
- **Boost Algorithm**: Boost libraries are often available as a single package, but you can also specify the specific Boost libraries you need when installing Boost.
- **SQLite3**: You can download SQLite from [the official SQLite website](https://www.sqlite.org/download.html).
- **SPDLog**: You can include SPDLog in your project by following the instructions in the [SPDLog repository](https://github.com/gabime/spdlog).
- **Boost Property Tree**: Boost Property Tree is typically included as part of the Boost libraries, so you should have it available if you've installed Boost as mentioned above.
- **Boost Bind**: Boost Bind is also part of the Boost library collection and should be available if you've installed Boost.

Please make sure to install these dependencies before proceeding with the project setup.


## Installation Guide

To install and run the Corporate SOCKS5 Proxy on your system, follow these steps:

1. Clone the repository (or use "Installation package" and skip to step 4):

   ```bash
   git clone https://github.com/apriorit/SOCKS5_Proxy_Summer_2023.git
   cd Corporate-SOCKS5-Proxy
   ```
   
2. Install Dependencies:
   
   Before building and running the project, make sure you have the required dependencies installed. Refer to the [Technologies and Dependencies](#technologies-and-dependencies) section for details.

3. Build the Project:

   ```bash
   g++ -std=c++20 -o myApp main.cpp -lboost_system -lboost_filesystem -lsqlite3 -lspdlog
   
   ./myApp
   ```

4. Run as administrator CMD and input (or run `install.bat`):
   ```bash
   sc create Corporate_SOCKS5_Proxy binPath= %BinaryPath% start= auto
   sc start Corporate_SOCKS5_Proxy
   ```

5. Configure Firefox FoxyProxy add-on and input necessary informations in the `config.ini` file. Example:
   ```
   proxyIP=127.0.67.2                                    - proxy IP
   proxyPort=1080                                        - proxy port
   logFilesDir=C:\Proxy_server\Logs\log.txt              - log files directory
   authFilesDir=C:\Proxy_server\authentication_file.txt
   username=my_username                                  - username used to log to the proxy server
   password=my_password                                  - password used to log to the proxy server
   dbFilesDir=C:\Proxy_server\database.db                - database file directory
   numActiveThreads=2                                    - number of threads used to run logger/database
   loggingMethod=2                                       - method of logging data (0 - logs, 1 - database, 2 - both)
   authenticationMethod=-1                               - method of authentication (-1 - any available, 0 - no authentication, 1 - GSSAPI, 2 - username and password)
   [allowedIPs]                                          - list of allowed IPs (if IP0=all then all IPs are allowed)
   IP0=all
   IP1=www.youtube.com
   IP2=www.googlevideo.com
   IP3=www.google.com
   [blockedIPs]                                          - list of blocked IPs
   IP0=bing.com
   [allowedPorts]                                        - list of allowed ports (if IP0=all then all ports are allowed)
   Port0=-1
   Port1=80
   Port2=443
   [blockedPorts]                                        - list of blocked ports
   Port0=123
   ```
   Disclaimer: The configuration file should be in the folder `C:\Proxy_server`.

## Usage Examples
- Automatic Startup : The service is configured to start automatically after a server reboot. You don't need to manually start it every time the server restarts.
- Proxy Usage : Once the service is running, it will automatically intercept outgoing client traffic and accept incoming responses. The SOCKS5 proxy server will use its own IP address for communication, ensuring your privacy and bypassing any blocking.
- Concurrent Connections and Statistics : The Corporate SOCKS5 Proxy is designed to support up to 10,000 concurrent connections. It also records records data about connections in a log file or database.
- Configuration : The server is configurable via a file.

## Authors
- Adam Kuszczynski
- Volodymyr Parakhonych
- Szymon Siakala
