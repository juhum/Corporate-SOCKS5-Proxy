/*
 * Authentication_Method.h
 * Purpose: Abstract base class for defining authentication methods.
 *          This class provides an interface for various authentication strategies.
 * 
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#pragma once
#include <boost/asio.hpp>

struct Authentication_Result
{
    const bool authenticated;
    boost::asio::ip::tcp::socket socket;
    const int authentication_method;
    std::string error;
};

class Authentication_Method
{
public:
    Authentication_Method() {}

    /*
     * Virtual destructor for proper cleanup of derived instances.
     */
    virtual ~Authentication_Method() {}

    /*
     * Abstract method for authentication.
     *
     * @param[in] socket: The socket to authenticate.
     * @return `Authentication_Result` struct indicating whether authentication is successful and associated details.
     */
    virtual Authentication_Result authenticate(boost::asio::ip::tcp::socket socket) = 0;
};