/*
 * No_Authentication.h
 * Purpose: Class representing a no authentication strategy.
 *          This class allows any user to be authenticated without any checks.
 *
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#pragma once
#include "Authentication_Method.h"

class No_Authentication : public Authentication_Method
{
public:
    No_Authentication();

    /*
     * Abstract method for authentication.
     *
     * @param[in] socket: The socket to authenticate.
     * @return `Authentication_Result` struct indicating whether authentication is successful and associated details.
     */
    Authentication_Result authenticate(boost::asio::ip::tcp::socket socket) override;
};