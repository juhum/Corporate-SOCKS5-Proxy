/*
 * No_Authentication.cpp
 * Purpose: Class representing a no authentication strategy.
 *          This class allows any user to be authenticated without any checks.
 *
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#include "No_Authentication.h"

No_Authentication::No_Authentication() {}

Authentication_Result No_Authentication::authenticate(boost::asio::ip::tcp::socket socket)
{
    const std::array<unsigned char, 2> response = { static_cast<unsigned char>(5), static_cast<unsigned char>(0x00) };
    boost::asio::write(socket, boost::asio::buffer(response));

    Authentication_Result result = { true, std::move(socket), 0,  "" };
    return result;
}