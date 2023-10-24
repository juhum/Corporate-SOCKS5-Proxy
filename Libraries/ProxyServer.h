/*
 * ProxyServer.h
 * Purpose: Implements a proxy server with SOCKS5 support for handling client connections,
 *          forwarding data to target servers, and managing logging and configuration.
 *
 * Author: Adam Kuszczyñski
 * Edited by: Szymon Si¹ka³a
 * Version: 1.1
 * Date: 01.09.2023
 */

#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "Logger.h"
#include "Handle_Authentication.h"
#include "Database.h"
#include "ProxyConfiguration.h"

const int BUFFER_SIZE = 4096;
const int SOCKS_VERSION = 5;

class ProxyServer {
public:

    /*
     * Constructor for the ProxyServer class.
     *
     * @param[in] io_context: The Boost.Asio io_context for managing asynchronous operations.
     * @param[in] ip_address: The IP address to bind the proxy server to.
     * @param[in] port: The port to listen on for incoming connections.
     * @param[in] config: The ProxyConfiguration instance with proxy server configuration.
     * @param[in] logging_method: The method used for logging (1 for database, 2 for both, and default for file).
     * @param[in] logger: A shared_ptr to a Logger instance for logging.
     * @param[in] database: A shared_ptr to a Database instance for database logging.
     */
    ProxyServer(boost::asio::io_context& io_context, const std::string& ip_address, unsigned short port, const ProxyConfiguration& config, const int logging_method, const std::shared_ptr<Logger> logger, const std::shared_ptr<Database> database);

    /*
     * Stops the proxy server by closing the acceptor and active sessions.
     */
    void stop();

private:
    class ProxySession : public std::enable_shared_from_this<ProxySession> {
    public:
        /*
         * Constructor for the ProxySession class.
         *
         * @param[in] socket: The client socket connected to the proxy.
         * @param[in] config: The ProxyConfiguration instance with proxy server configuration.
         * @param[in] logging_method: The method used for logging (1 for database, 2 for both, and default for file).
         * @param[in] logger: A shared_ptr to a Logger instance for logging.
         * @param[in] database: A shared_ptr to a Database instance for database logging.
         */
        ProxySession(boost::asio::ip::tcp::socket socket, const ProxyConfiguration& config, const int logging_method, const std::shared_ptr<Logger> logger, const std::shared_ptr<Database> database);

        /*
         * Starts the proxy session by reading the SOCKS request.
         */
        void start();

        /*
         * Closes the proxy session by closing both client and server sockets.
         */
        void close();

    private:
        /*
         * Reads the initial SOCKS request from the client.
         */
        void read_socks_request();

        /*
         * Handles the SOCKS request received from the client.
         *
         * @param[in] error: The error code indicating the success or failure of the operation.
         * @param[in] bytes_transferred: The number of bytes transferred in the request.
         */
        void handle_socks_request(const boost::system::error_code& error, std::size_t bytes_transferred);

        /*
         * Handles the connection to the target server.
         *
         * @param[in] error: The error code indicating the success or failure of the connection.
         */
        void handle_connect(const boost::system::error_code& error);

        /*
         * Sends a SOCKS reply with the specified status code to the client.
         *
         * @param[in] status: The SOCKS reply status code.
         */
        void send_socks_reply(int status);

        /*
         * Handles the completion of write operations to the client socket.
         *
         * @param[in] error: The error code indicating the success or failure of the write operation.
         */
        void handle_write(const boost::system::error_code& error);

        /*
         * Forwards data between the client and server.
         */
        void forward_data();

        /*
         * Handles read operations from the client socket.
         *
         * @param[in] error: The error code indicating the success or failure of the read operation.
         * @param[in] bytes_transferred: The number of bytes transferred in the read operation.
         */
        void handle_client_read(const boost::system::error_code& error, std::size_t bytes_transferred);

        /*
         * Handles read operations from the server socket.
         *
         * @param[in] error: The error code indicating the success or failure of the read operation.
         * @param[in] bytes_transferred: The number of bytes transferred in the read operation.
         */
        void handle_server_read(const boost::system::error_code& error, std::size_t bytes_transferred);

        /*
         * Handles write operations to the server socket.
         *
         * @param[in] error: The error code indicating the success or failure of the write operation.
         */
        void handle_server_write(const boost::system::error_code& error);

        /*
         * Handles write operations to the client socket.
         *
         * @param[in] error: The error code indicating the success or failure of the write operation.
         */
        void handle_client_write(const boost::system::error_code& error);

        /*
         * Logs a message to the appropriate log destination based on the logging method.
         *
         * @param[in] log_level: The log level of the message.
         * @param[in] IP: The IP address associated with the log message.
         * @param[in] message: The log message to be written.
         */
        void log_to_file(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message);

        boost::asio::ip::tcp::socket client_socket_;
        boost::asio::ip::tcp::socket server_socket_;
        char client_data_[BUFFER_SIZE];
        char server_data_[BUFFER_SIZE];
        ProxyConfiguration proxyConfig_;
        int logging_method_;
        std::shared_ptr<Logger> logger_;
        std::shared_ptr<Database> database_;
        std::shared_ptr<boost::asio::ip::tcp::socket> client_socket_ptr_;
    };

    /*
     * Starts accepting incoming client connections asynchronously.
     *
     * @param[in] socket: A shared_ptr to the server socket.
     */
    void start_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    boost::asio::ip::tcp::acceptor acceptor_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    ProxyConfiguration proxyConfig_;
    int logging_method_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Database> database_;
    std::vector<std::shared_ptr<ProxySession>> active_sessions_;
};