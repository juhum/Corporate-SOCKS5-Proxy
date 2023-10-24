/*
 * Handle_Authentication.cpp
 * Purpose: Class to handle authentication for a given socket.
 *          This class reads the authentication method and delegates the authentication process
 *          to the `Authenticator` class.
 *
 * @author Szymon Si¹ka³a
 * @version 1.0 03/09/2023
 */

#include "Handle_Authentication.h"

Handle_Authentication::Handle_Authentication(const ProxyConfiguration& config, boost::asio::ip::tcp::socket socket, const int buffer_size) : proxy_config(config), socket(std::move(socket)), buffer_size(buffer_size)
{
    data = new char[buffer_size];
}

Handle_Authentication::~Handle_Authentication()
{
    delete[] data;
}

Authentication_Result Handle_Authentication::handle_authentication()
{
    boost::system::error_code error;
    std::size_t bytes_transferred = boost::asio::read(socket, boost::asio::buffer(data, 1), error);

    if (!error)
    {
        const int nmethods = static_cast<unsigned char>(data[0]);
        bytes_transferred = boost::asio::read(socket, boost::asio::buffer(data + 1, nmethods), error);

        if (!error)
        {
            for (int i = 0; i < nmethods; i++)
            {
                const int method = static_cast<unsigned char>(data[i + 1]);
                if ((method == 0x00 && proxy_config.getAuthenticationMethod() == 0) || (method == 0x00 && proxy_config.getAuthenticationMethod() == -1))
                {
                    const std::shared_ptr<Authentication_Method> auth_method = std::make_shared<No_Authentication>();
                    Authenticator auth(auth_method);

                    return auth.authenticate(std::move(socket));
                }
                else if ((method == 0x01 && proxy_config.getAuthenticationMethod() == 1) || (method == 0x01 && proxy_config.getAuthenticationMethod() == -1))
                {
                    const std::shared_ptr<Authentication_Method> auth_method = std::make_shared<GSSAPI>();
                    Authenticator auth(auth_method);

                    return auth.authenticate(std::move(socket));
                }
                else if ((method == 0x02 && proxy_config.getAuthenticationMethod() == 2) || (method == 0x02 && proxy_config.getAuthenticationMethod() == -1))
                {
                    const std::shared_ptr<Authentication_Method> auth_method = std::make_shared<Username_Password>(proxy_config.getUsername(), proxy_config.getPassword());
                    Authenticator auth(auth_method);

                    return auth.authenticate(std::move(socket));
                }
                else
                {
                    Authentication_Result result = { false, std::move(socket), -1, "Unsupported authentication method." };
                    return result;
                }
            }
        }

        Authentication_Result result = { false, std::move(socket), -1, "Error while reading authentication methods: " + error.message() };
        return result;
    }

    Authentication_Result result = { false, std::move(socket), -1, "Error while reading SOCKS request: " + error.message() };
    return result;
}