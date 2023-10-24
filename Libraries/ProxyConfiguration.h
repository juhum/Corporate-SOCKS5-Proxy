/*
 * ProxyConfiguration.h
 * Purpose: Definition of the ProxyConfiguration class for managing allowed and blocked IP addresses and ports,
 *          as well as the proxy server's IP address and port.
 *
 * Author: Volodymyr Parakhonych
 * Date of the last change: 01/09/2023
 */

#ifndef PROXY_CONFIGURATION_H
#define PROXY_CONFIGURATION_H

#include <string>
#include <vector>

class ProxyConfiguration {
private:
    std::vector<std::string> allowedIPs; // List of allowed IP addresses.
    std::vector<std::string> blockedIPs; // List of blocked IP addresses.
    std::vector<int> allowedPorts; // List of allowed ports.
    std::vector<int> blockedPorts; // List of blocked ports.
    std::string proxyIP; // IP address of the proxy server.
    int proxyPort; // Port number of the proxy server.
    std::string logFilesDir; // Directory for log files.
    std::string authFilesDir; // Directory for authentication files.
    std::string username; // Username for authentication.
    std::string password; // Password for authentication.
    std::string dbFilesDir; // Directory for database files.
    int numActiveThreads; // Number of active threads for logger.
    int loggingMethod;    // Logging method with int input/output.
    int authenticationMethod; // Authentication method.

public:
    /*
     * Add an IP address to the list of allowed IP addresses.
     *
     * @param[in] ip: The IP address to be added to the allowed list.
     */
    void addAllowedIP(const std::string& ip);

    /*
     * Remove an IP address from the list of allowed IP addresses.
     *
     * @param[in] ip: The IP address to be removed from the allowed list.
     */
    void removeAllowedIP(const std::string& ip);

    /*
     * Add an IP address to the list of blocked IP addresses.
     *
     * @param[in] ip: The IP address to be added to the blocked list.
     */
    void addBlockedIP(const std::string& ip);

    /*
     * Remove an IP address from the list of blocked IP addresses.
     *
     * @param[in] ip: The IP address to be removed from the blocked list.
     */
    void removeBlockedIP(const std::string& ip);

    /*
     * Add a port number to the list of allowed ports.
     *
     * @param[in] port: The port number to be added to the allowed list.
     */
    void addAllowedPort(int port);

    /*
     * Remove a port number from the list of allowed ports.
     *
     * @param[in] port: The port number to be removed from the allowed list.
     */
    void removeAllowedPort(int port);

    /*
     * Add a port number to the list of blocked ports.
     *
     * @param[in] port: The port number to be added to the blocked list.
     */
    void addBlockedPort(int port);

    /*
     * Remove a port number from the list of blocked ports.
     *
     * @param[in] port: The port number to be removed from the blocked list.
     */
    void removeBlockedPort(int port);

    /*
     * Set the IP address of the proxy server.
     *
     * @param[in] ip: The IP address to be set for the proxy server.
     */
    void setProxyServerIp(const std::string& ip);

    /*
     * Get the IP address of the proxy server.
     *
     * @return The IP address of the proxy server.
     */
    std::string getProxyServerIp() const;

    /*
     * Set the port number of the proxy server.
     *
     * @param[in] port: The port number to be set for the proxy server.
     */
    void setProxyServerPort(int port);

    /*
     * Get the port number of the proxy server.
     *
     * @return The port number of the proxy server.
     */
    int getProxyServerPort() const;

    /*
     * Get a reference to the list of allowed IP addresses.
     *
     * @return A reference to the list of allowed IP addresses.
     */
    const std::vector<std::string>& getAllowedIPs() const;

    /*
     * Get a reference to the list of blocked IP addresses.
     *
     * @return A reference to the list of blocked IP addresses.
     */
    const std::vector<std::string>& getBlockedIPs() const;

    /*
     * Get a reference to the list of allowed ports.
     *
     * @return A reference to the list of allowed ports.
     */
    const std::vector<int>& getAllowedPorts() const;

    /*
     * Get a reference to the list of blocked ports.
     *
     * @return A reference to the list of blocked ports.
     */
    const std::vector<int>& getBlockedPorts() const;

    /*
     * Set the directory for log files.
     *
     * @param[in] dir: The directory path for log files.
     */
    void setLogFilesDir(const std::string& dir);

    /*
     * Get the directory for log files.
     *
     * @return The directory path for log files.
     */
    std::string getLogFilesDir() const;

    /*
     * Set the directory for authentication files.
     *
     * @param[in] dir: The directory path for authentication files.
     */
    void setAuthFilesDir(const std::string& dir);

    /*
     * Get the directory for authentication files.
     *
     * @return The directory path for authentication files.
     */
    std::string getAuthFilesDir() const;

    /*
     * Set the username for authentication.
     *
     * @param[in] user: The username for authentication.
     */
    void setUsername(const std::string& user);

    /*
     * Get the username for authentication.
     *
     * @return The username for authentication.
     */
    std::string getUsername() const;

    /*
     * Set the password for authentication.
     *
     * @param[in] pass: The password for authentication.
     */
    void setPassword(const std::string& pass);

    /*
     * Get the password for authentication.
     *
     * @return The password for authentication.
     */
    std::string getPassword() const;

    /*
    * Set the directory for database files.
    *
    * @param[in] dir: The directory path for database files.
    */
    void setDbFilesDir(const std::string& dir);

    /*
     * Get the directory for database files.
     *
     * @return The directory path for database files.
     */
    std::string getDbFilesDir() const;

    /**
     * Set the number of active threads.
     *
     * @param[in] num: The number of active threads.
     */
    void setNumActiveThreads(int num);

    /**
     * Get the number of active threads.
     *
     * @return The number of active threads.
     */
    int getNumActiveThreads() const;

    /**
     * Set the logging method
     *
     * @param[in] method: The logging method.
     */
    void setLoggingMethod(int method);

    /**
     * Get the logging method
     *
     * @return The logging method.
     */
    int getLoggingMethod() const;

    /**
     * Set the authentication method.
     *
     * @param[in] method: The authentication method.
     */
    void setAuthenticationMethod(int method);

    /**
     * Get the authentication method.
     *
     * @return The authentication method.
     */
    int getAuthenticationMethod() const;

    /*
     * Save the current configuration to an INI file.
     *
     * @param[in] filename: The name of the INI file to save the configuration to.
     * @throws std::exception: If there is an error during the saving process.
     */
    void saveConfigToIni(const std::string& filename);

    /*
     * Load configuration from an INI file and update the current configuration.
     *
     * @param[in] filename: The name of the INI file to load the configuration from.
     * @throws std::exception: If there is an error during the loading process.
     */
    void loadConfigFromIni(const std::string& filename);
};

#endif // PROXY_CONFIGURATION_H
