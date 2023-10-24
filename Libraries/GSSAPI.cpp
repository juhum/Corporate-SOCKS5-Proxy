/*
 * GSSAPI.cpp
 * Purpose: Class representing a GSSAPI authentication strategy.
 *          This class allows a user to be authenticated using the GSSAPI protocol.
 *
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#include "GSSAPI.h"

GSSAPI::GSSAPI() {}

Authentication_Result GSSAPI::authenticate(boost::asio::ip::tcp::socket socket)
{
    const std::array<unsigned char, 2> response = { static_cast<unsigned char>(5), static_cast<unsigned char>(0x01) };
    boost::asio::write(socket, boost::asio::buffer(response));

    Authentication_Result result = { false, std::move(socket), 1, "" };
    return result;
}