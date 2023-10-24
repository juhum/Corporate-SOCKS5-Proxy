#include <iostream>

#include "./Authenticator.h"

int main()
{
    const std::string path_to_file = "C:\\authentication_file.txt";
    const std::string username = "my_username";
    const std::string password = "my_password";

    const std::shared_ptr<Authentication_Method> no_authentication = std::make_shared<No_Authentication>();
    const std::shared_ptr<Authentication_Method> username_password = std::make_shared<Username_Password>(path_to_file, username, password);
    const std::shared_ptr<Authentication_Method> gssapi = std::make_shared<GSSAPI>();

    Authenticator auth_1(username_password);

    if (auth_1.authenticate())
    {
        std::cout << "Client authenticated successfully." << std::endl;
    }
    else
    {
        std::cout << "Client authentication failed." << std::endl;
    }

    Authenticator auth_2(username_password);

    if (auth_2.authenticate())
    {
        std::cout << "Client authenticated successfully." << std::endl;
    }
    else
    {
        std::cout << "Client authentication failed." << std::endl;
    }

    Authenticator auth_3(gssapi);

    if (auth_3.authenticate())
    {
        std::cout << "Client authenticated successfully." << std::endl;
    }
    else
    {
        std::cout << "Client authentication failed." << std::endl;
    }

    return 0;
}