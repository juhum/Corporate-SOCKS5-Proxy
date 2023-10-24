/*
 * Handle_Authentication.h
 * Purpose: Class to handle authentication for a given socket.
 *          This class reads the authentication method and delegates the authentication process
 *          to the `Authenticator` class.
 *
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#pragma once
#include "Authenticator.h"
#include "ProxyConfiguration.h"

class Handle_Authentication
{
private:
    ProxyConfiguration proxy_config;
    boost::asio::ip::tcp::socket socket;
    int buffer_size;
    char* data;

public:
    /*
     * Constructor to initialize the Handle_Authentication instance.
     *
     * @param[in] config: The proxy configuration.
     * @param[in] socket: The socket to authenticate.
     * @param[in] buffer_size: The size of the buffer to be used for reading data from the socket.
     */
    Handle_Authentication(const ProxyConfiguration& config, boost::asio::ip::tcp::socket socket, const int buffer_size);

    /*
     * Destructor to free the memory allocated for the buffer.
     */
    ~Handle_Authentication();

    /*
     * Handle authentication for incoming connections.
     *
     * @return `Authentication_Result` struct indicating whether authentication is successful and associated details.
     */
    Authentication_Result handle_authentication();
};