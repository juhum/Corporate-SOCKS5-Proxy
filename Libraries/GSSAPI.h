/*
 * GSSAPI.h
 * Purpose: Class representing a GSSAPI authentication strategy.
 *          This class allows a user to be authenticated using the GSSAPI protocol.
 * 
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#pragma once
#include "Authentication_Method.h"

class GSSAPI : public Authentication_Method
{
public:
    GSSAPI();

    /*
     * Authenticate method for the GSSAPI strategy.
     *
     * @param[in] socket: The socket to authenticate.
     * @return `Authentication_Result` struct indicating whether authentication is successful and associated details.
     */
    Authentication_Result authenticate(boost::asio::ip::tcp::socket socket);
};