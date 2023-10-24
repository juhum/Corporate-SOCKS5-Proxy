/*
 * Username_Password.cpp
 * Purpose: Class representing a no authentication strategy.
 *          This class allows a user to be authenticated by username and password.
 *
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#include "Username_Password.h"

Username_Password::Username_Password(const std::string username, const std::string password) : username(username), password(password) {}

Authentication_Result Username_Password::authenticate(boost::asio::ip::tcp::socket socket)
{
    const std::array<unsigned char, 2> response = { static_cast<unsigned char>(5), static_cast<unsigned char>(0x02) };
    boost::asio::write(socket, boost::asio::buffer(response));

    std::array<unsigned char, 2> header;
    boost::asio::read(socket, boost::asio::buffer(header));
    if (header[0] != 0x01)
    {
        Authentication_Result result = { false, std::move(socket), 2, "Invalid authentication request header." };
        return result;
    }

    const unsigned char username_length = header[1];
    std::vector<unsigned char> username_read(username_length);
    boost::asio::read(socket, boost::asio::buffer(username_read));

    unsigned char password_length;
    boost::asio::read(socket, boost::asio::buffer(&password_length, 1));

    std::vector<unsigned char> password_read(password_length);
    boost::asio::read(socket, boost::asio::buffer(password_read));

    const std::string username_str(username_read.begin(), username_read.end());
    const std::string password_str(password_read.begin(), password_read.end());

    if ((username_str == username) && (password_str == password))
    {
        const std::array<unsigned char, 2> successful = { static_cast<unsigned char>(0x01), static_cast<unsigned char>(0x00) };
        boost::asio::write(socket, boost::asio::buffer(successful));

        Authentication_Result result = { true, std::move(socket), 2, "" };
        return result;
    }

    const std::array<unsigned char, 2> successful = { static_cast<unsigned char>(0x01), static_cast<unsigned char>(0x01) };
    boost::asio::write(socket, boost::asio::buffer(successful));

    Authentication_Result result = { false, std::move(socket), 2, "" };
    return result;
}