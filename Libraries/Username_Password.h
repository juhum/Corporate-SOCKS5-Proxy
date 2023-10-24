/*
 * Username_Password.h
 * Purpose: Class representing a no authentication strategy.
 *          This class allows a user to be authenticated by username and password.
 * 
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#pragma once
#include "Authentication_Method.h"

class Username_Password : public Authentication_Method
{
private:
    std::string username;
    std::string password;

public:
    Username_Password() = delete;

    /*
     * Constructor to initialize the Username_Password instance.
     *
     * @param[in] path_to_file: Path to the authentication file.
     * @param[in] username: The expected username for authentication.
     * @param[in] password: The expected password for authentication.
     */
    Username_Password(const std::string username, const std::string password);

    /*
     * Authenticate method for the username and password strategy.
     *
     * @param[in] socket: The socket to authenticate.
     * @return `Authentication_Result` struct indicating whether authentication is successful and associated details.
     */
    Authentication_Result authenticate(boost::asio::ip::tcp::socket socket) override;
};