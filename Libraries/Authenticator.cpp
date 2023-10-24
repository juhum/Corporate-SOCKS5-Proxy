/*
 * Authenticator.cpp
 * Purpose: Class to perform authentication using a specific authentication method.
 *          This class delegates the authentication process to the provided method.
 *
 * @author Szymon Si¹ka³a
 * @version 1.3 03/09/2023
 */

#include "Authenticator.h"

Authenticator::Authenticator(const std::shared_ptr<Authentication_Method> method) : method(method) {}

Authentication_Result Authenticator::authenticate(boost::asio::ip::tcp::socket socket)
{
    return method->authenticate(std::move(socket));
}
