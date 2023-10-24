/*
 * Authenticator.h
 * Purpose: Class to perform authentication using a specific authentication method.
 *          This class delegates the authentication process to the provided method.
 *
 * @author Szymon Si¹ka³a
 * @version 1.3 03/09/2023
 */

#pragma once
#include "No_Authentication.h"
#include "Username_Password.h"
#include "GSSAPI.h"

class Authenticator
{
private:
    std::shared_ptr<Authentication_Method> method;

public:
    /*
     * Constructor to initialize the Authenticator instance.
     * 
     * @param[in] method: The authentication method to be used.
     */
    Authenticator(const std::shared_ptr<Authentication_Method> method);

    /*
     * Perform authentication using the provided method.
     * 
     * @param[in] socket: The socket to authenticate.
     * @return `Authentication_Result` struct indicating whether authentication is successful and associated details.
     */
    Authentication_Result authenticate(boost::asio::ip::tcp::socket socket);
};
