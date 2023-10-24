#include "ProxyServer.h"

ProxyServer::ProxyServer(boost::asio::io_context& io_context, const std::string& ip_address, unsigned short port, const ProxyConfiguration& config, const int logging_method, const std::shared_ptr<Logger> logger, const std::shared_ptr<Database> database)
    : acceptor_(io_context),
    socket_(std::make_shared<boost::asio::ip::tcp::socket>(acceptor_.get_executor())),
    proxyConfig_(config),
    logging_method_(logging_method),
    logger_(logger),
    database_(database) {

    boost::asio::ip::address_v4 custom_ip_address = boost::asio::ip::make_address_v4(ip_address);
    boost::asio::ip::tcp::endpoint endpoint(custom_ip_address, port);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept(socket_);
}

void ProxyServer::stop()
{
    if (acceptor_.is_open())
    {
        acceptor_.close();
	}

    if (socket_ && socket_->is_open())
    {
        socket_->close();
    }

    for (auto& session : active_sessions_)
    {
        session->close();
    }
}

ProxyServer::ProxySession::ProxySession(boost::asio::ip::tcp::socket socket, const ProxyConfiguration& config, const int logging_method, const std::shared_ptr<Logger> logger, const std::shared_ptr<Database> database)
    : client_socket_(std::move(socket)),
    server_socket_(client_socket_.get_executor()),
    proxyConfig_(config),
    logging_method_(logging_method),
    logger_(logger),
    database_(database) {
    memset(client_data_, 0, BUFFER_SIZE);
    memset(server_data_, 0, BUFFER_SIZE);
}


void ProxyServer::ProxySession::start() {
    memset(client_data_, 0, BUFFER_SIZE);
    memset(server_data_, 0, BUFFER_SIZE);
    read_socks_request();
}

void ProxyServer::ProxySession::read_socks_request() {
    std::cout << "Reading SOCKS request from client..." << std::endl;
    boost::system::error_code error;
    std::size_t bytes_transferred = boost::asio::read(client_socket_, boost::asio::buffer(client_data_, 1), error);

    if (!error) {
        int version = static_cast<unsigned char>(client_data_[0]); // The socks version

        if (version != 5) {
            send_socks_reply(5);
            return;
        }

        Handle_Authentication handle(proxyConfig_, std::move(client_socket_), BUFFER_SIZE);
        Authentication_Result result = handle.handle_authentication();
        const bool authenticated = result.authenticated;
        client_socket_ = std::move(result.socket);
        const int authentication_method = result.authentication_method;
        const std::string error = result.error;

        if (!error.empty())
        {
            std::cerr << "Error while authenticating: " << error << std::endl;
            log_to_file(spdlog::level::err, client_socket_.remote_endpoint().address().to_string(), "Error while authenticating: " + error);
            return;
        }

        if (authenticated) {
            std::cout << "Authenticated successfuly with method: " << authentication_method << std::endl;
            log_to_file(spdlog::level::info, client_socket_.remote_endpoint().address().to_string(), "Authenticated successfuly with method: " + std::to_string(authentication_method));
            memset(client_data_, 0, BUFFER_SIZE);
            memset(server_data_, 0, BUFFER_SIZE);

            // Asynchronously read the rest of the SOCKS5 request
            client_socket_.async_read_some(
                boost::asio::buffer(client_data_, BUFFER_SIZE),
                [self = shared_from_this()](boost::system::error_code async_error, std::size_t async_bytes_transferred) {
                    self->handle_socks_request(async_error, async_bytes_transferred);
                });
        }
        else {
            std::cerr << "Authentication failed." << std::endl;
            log_to_file(spdlog::level::err, client_socket_.remote_endpoint().address().to_string(), "Authentication failed.");
            return;
        }
    }
    else {
        std::cerr << "Error reading initial SOCKS request: " << error.message() << std::endl;
        log_to_file(spdlog::level::err, client_socket_.remote_endpoint().address().to_string(), "Error while reading initial SOCKS request: " + error.message());
    }
}

void ProxyServer::ProxySession::handle_socks_request(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        std::cout << "Handling SOCKS5 request..." << std::endl;
        int version = static_cast<unsigned char>(client_data_[0]); // The socks version
        int command = static_cast<unsigned char>(client_data_[1]); // The socks command
        int reserved = static_cast<unsigned char>(client_data_[2]); // The reserved field
        int address_type = static_cast<unsigned char>(client_data_[3]); // The address type
        std::cout << "bytes transferred: " << bytes_transferred << std::endl;
        std::cout << "Received SOCKS version: " << version << std::endl;
        std::cout << "Received SOCKS command: " << command << std::endl;
        std::cout << "Received SOCKS reserved: " << reserved << std::endl;
        std::cout << "Received SOCKS address type: " << address_type << std::endl;

        std::string address_type_string = "";

        boost::asio::ip::tcp::endpoint targetEndpoint;
        // Check if the socks version is valid
        if (version == SOCKS_VERSION) {
            // Check if the socks command is connect
            if (command == 1) {

                std::string address;
                unsigned short port;

                switch (address_type) {
                case 1: // IPv4 address
                    std::cout << "address type is IPv4" << std::endl;
                    address_type_string = "IPv4";
                    if (bytes_transferred >= 10) {
                        address = boost::asio::ip::address_v4(
                            boost::asio::detail::socket_ops::network_to_host_long(
                                *reinterpret_cast<unsigned long*>(client_data_ + 4)))
                            .to_string();
                        port = boost::asio::detail::socket_ops::network_to_host_short(
                            *reinterpret_cast<unsigned short*>(client_data_ + 8));
                    }
                    else {
                        // Send a socks reply with address type not supported
                        boost::asio::ip::tcp::endpoint clientEndpoint = client_socket_.remote_endpoint();
                        std::cout << "Client connected from: " << clientEndpoint.address() << ":" << clientEndpoint.port() << std::endl;
                        send_socks_reply(8);
                        return;
                    }
                    break;
                case 3: // Domain name
                    std::cout << "address type is domain name" << std::endl;
                    address_type_string = "domain name";
                    if (bytes_transferred >= 5) { // Ensure enough data for domain name length
                        int length = client_data_[4];
                        if (bytes_transferred >= 5 + length + 2) { // Ensure enough data for domain name and port
                            address = std::string(client_data_ + 5, length);
                            port = boost::asio::detail::socket_ops::network_to_host_short(
                                *reinterpret_cast<unsigned short*>(client_data_ + 5 + length));
                        }
                        else {
                            // Send a socks reply with address type not supported
                            send_socks_reply(8);
                            return;
                        }
                    }
                    else {
                        // Send a socks reply with address type not supported
                        send_socks_reply(8);
                        return;
                    }
                    break;
                case 4: // IPv6 address
                    std::cout << "address type is IPv6" << std::endl;
                    address_type_string = "IPv6";
                    if (bytes_transferred >= 22) {
                        boost::asio::ip::address_v6::bytes_type ipv6_bytes;
                        for (int i = 0; i < 16; ++i) {
                            ipv6_bytes[i] = static_cast<unsigned char>(client_data_[i + 4]);
                        }
                        boost::asio::ip::address_v6 ipv6_address(ipv6_bytes);

                        port = boost::asio::detail::socket_ops::network_to_host_short(
                            *reinterpret_cast<unsigned short*>(client_data_ + 20));

                        address = ipv6_address.to_string();
                    }
                    else {
                        // Send a socks reply with address type not supported
                        send_socks_reply(8);
                        return;
                    }
                    break;
                default: // Invalid address type
                    boost::asio::ip::tcp::endpoint clientEndpoint = client_socket_.remote_endpoint();
                    std::cout << "Client connected from: " << clientEndpoint.address() << ":" << clientEndpoint.port() << std::endl;
                    address_type_string = clientEndpoint.address().to_string() + ":" + std::to_string(clientEndpoint.port());
                    send_socks_reply(8);
                    return;
                }

                const std::string message = "Handling SOCKS5 request (bytes transferred: " + std::to_string(bytes_transferred) +
                    ", version: " + std::to_string(version) +
                    ", command: " + std::to_string(command) +
                    ", reserved: " + std::to_string(reserved) +
                    ", address type: " + address_type_string + ").";
                log_to_file(spdlog::level::info, client_socket_.remote_endpoint().address().to_string(), message);

                std::vector<std::string> blocked_sites = proxyConfig_.getBlockedIPs();
                std::vector<int> blocked_ports = proxyConfig_.getBlockedPorts();
                std::vector<std::string> allowed_sites = proxyConfig_.getAllowedIPs();
                std::vector<int> allowed_ports = proxyConfig_.getAllowedPorts();
                
                bool isSiteBlocked = false;
                bool isPortBlocked = false;
                bool isSiteAllowed = false;
                bool isPortAllowed = false;

                if (allowed_sites[0] == "all")
                {
                    isSiteAllowed = true;
                }
                else
                {
                    isSiteBlocked = std::find(blocked_sites.begin(), blocked_sites.end(), address) != blocked_sites.end();
                    isSiteAllowed = std::find(allowed_sites.begin(), allowed_sites.end(), address) != allowed_sites.end();
                }

                if (allowed_ports[0] == -1)
                {
                    isPortAllowed = true;
                }
                else
                {
                    isPortBlocked = std::find(blocked_ports.begin(), blocked_ports.end(), port) != blocked_ports.end();
                    isPortAllowed = std::find(allowed_ports.begin(), allowed_ports.end(), port) != allowed_ports.end();
                }

                std::cout << "Resolved " << address << ":" << port << std::endl;
                log_to_file(spdlog::level::info, client_socket_.remote_endpoint().address().to_string(), "Resolved: " + address + ":" + std::to_string(port) + ".");

                if (isSiteBlocked || isPortBlocked) {
                    // Send a socks reply indicating forbidden access
                    send_socks_reply(7);
                    return;
                }
                else if (isSiteAllowed && isPortAllowed) {
                    try {
                        boost::asio::ip::tcp::resolver resolver(server_socket_.get_executor());
                        boost::asio::ip::tcp::resolver::query target_query(address, std::to_string(port));
                        boost::asio::async_connect(server_socket_, resolver.resolve(target_query),
                            [self = shared_from_this()](const boost::system::error_code& connect_error,
                                const boost::asio::ip::tcp::endpoint& endpoint) {
                                    self->handle_connect(connect_error);
                            });
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Exception: " << e.what() << std::endl;

                        log_to_file(spdlog::level::err, client_socket_.remote_endpoint().address().to_string(), "Exception: " + std::string(e.what()));
                    }
                }
                else {
                    // neither blocked or allowed
                    send_socks_reply(5);
                    return;
                }
            }
        }
        else {
            send_socks_reply(1);
        }
    }
    else {
        close();
    }

}

void ProxyServer::ProxySession::handle_connect(const boost::system::error_code& error) {
    if (!error) {
        send_socks_reply(0);
        forward_data();
    }
    else {
        // Send a socks reply with connection refused
        send_socks_reply(5);
    }
}

void ProxyServer::ProxySession::send_socks_reply(int status) {
    std::cout << "Sending SOCKS reply with status: " << status << std::endl;
    const std::string message = "Sending SOCKS reply with status: " + std::to_string(status);
    log_to_file(spdlog::level::info, client_socket_.remote_endpoint().address().to_string(), message);

    server_data_[0] = SOCKS_VERSION;
    server_data_[1] = status;
    server_data_[2] = 0;
    server_data_[3] = 1;

    boost::asio::async_write(
        client_socket_,
        boost::asio::buffer(server_data_, 10),
        boost::bind(&ProxySession::handle_write, this,
            boost::asio::placeholders::error));
}
void ProxyServer::ProxySession::handle_write(const boost::system::error_code& error) {
    if (!error) {
        // Do nothing
    }
    else {
        close();
    }
}

void ProxyServer::ProxySession::forward_data() {
    client_socket_.async_read_some(
        boost::asio::buffer(client_data_, BUFFER_SIZE),
        [self = shared_from_this()](boost::system::error_code error, std::size_t bytes_transferred) {
            self->handle_client_read(error, bytes_transferred);
        });

    server_socket_.async_read_some(
        boost::asio::buffer(server_data_, BUFFER_SIZE),
        [self = shared_from_this()](boost::system::error_code error, std::size_t bytes_transferred) {
            self->handle_server_read(error, bytes_transferred);
        });
}

void ProxyServer::ProxySession::handle_client_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        boost::asio::async_write(
            server_socket_,
            boost::asio::buffer(client_data_, bytes_transferred),
            [self = shared_from_this()](const boost::system::error_code& write_error, std::size_t) {
                if (!write_error) {
                    self->client_socket_.async_read_some(
                        boost::asio::buffer(self->client_data_, BUFFER_SIZE),
                        [self](const boost::system::error_code& read_error, std::size_t read_bytes) {
                            self->handle_client_read(read_error, read_bytes);
                        });
                }
                else {
                    self->close();
                }
            });
    }
    else {
        close();
    }
}

void ProxyServer::ProxySession::handle_server_read(const boost::system::error_code& error, std::size_t bytes_transferred) {
    if (!error) {
        boost::asio::async_write(
            client_socket_,
            boost::asio::buffer(server_data_, bytes_transferred),
            [self = shared_from_this()](const boost::system::error_code& write_error, std::size_t) {
                if (!write_error) {

                    self->server_socket_.async_read_some(
                        boost::asio::buffer(self->server_data_, BUFFER_SIZE),
                        [self](const boost::system::error_code& read_error, std::size_t read_bytes) {
                            self->handle_server_read(read_error, read_bytes);
                        });
                }
                else {
                    self->close();
                }
            });
    }
    else {
        close();
    }
}
void ProxyServer::ProxySession::handle_server_write(const boost::system::error_code& error) {
    if (!error) {
        // Do nothing
    }
    else {
        close();
    }
}
void ProxyServer::ProxySession::handle_client_write(const boost::system::error_code& error) {
    if (!error) {
        // Do nothing
    }
    else {
        close();
    }
}

void ProxyServer::ProxySession::log_to_file(const spdlog::level::level_enum log_level, const std::string& IP, const std::string& message)
{
    switch (logging_method_)
    {
    case 1:
        database_->add_to_queue(log_level, IP, message);
        break;
    case 2:
        database_->add_to_queue(log_level, IP, message);
    default:
        logger_->add_to_queue(log_level, IP, message);
        break;
    }
}

void ProxyServer::ProxySession::close() {
    client_socket_.close();
    server_socket_.close();
}

// Proxy server function definitions

void ProxyServer::start_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    acceptor_.async_accept(
        *socket,
        [this, socket](const boost::system::error_code& error) {
            if (!error) {
                auto session = std::make_shared<ProxySession>(std::move(*socket), proxyConfig_, logging_method_, logger_, database_);
                active_sessions_.emplace_back(session);
                session->start();
            }
            start_accept(socket);
        });
}