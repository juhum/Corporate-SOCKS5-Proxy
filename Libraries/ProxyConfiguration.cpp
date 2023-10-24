/*
 * ProxyConfiguration.cpp
 * Purpose: Implementation of the ProxyConfiguration class functions for managing allowed and blocked IP addresses and ports,
 *          as well as the proxy server's IP address and port.
 *
 * Author: Volodymyr Parakhonych
 * Date of the last change: 1/09/2023
 */

#include "ProxyConfiguration.h"
#include <algorithm> 
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>

namespace pt = boost::property_tree;

void ProxyConfiguration::addAllowedIP(const std::string& ip) {
    allowedIPs.push_back(ip); // Add the provided IP address to the list of allowed IP addresses.
}

void ProxyConfiguration::removeAllowedIP(const std::string& ip) {
    std::erase_if(allowedIPs, [&ip](const std::string& address) {
        return address == ip; // Remove IP addresses matching the provided IP from the list of allowed IP addresses.
        });
}

void ProxyConfiguration::addBlockedIP(const std::string& ip) {
    blockedIPs.push_back(ip); // Add the provided IP address to the list of blocked IP addresses.
}

void ProxyConfiguration::removeBlockedIP(const std::string& ip) {
    std::erase_if(blockedIPs, [&ip](const std::string& address) {
        return address == ip; // Remove IP addresses matching the provided IP from the list of blocked IP addresses.
        });
}

void ProxyConfiguration::addAllowedPort(int port) {
    allowedPorts.push_back(port); // Add the provided port number to the list of allowed ports.
}

void ProxyConfiguration::removeAllowedPort(int port) {
    std::erase_if(allowedPorts, [port](int p) {
        return p == port; // Remove port numbers matching the provided port from the list of allowed ports.
        });
}

void ProxyConfiguration::addBlockedPort(int port) {
    blockedPorts.push_back(port); // Add the provided port number to the list of blocked ports.
}

void ProxyConfiguration::removeBlockedPort(int port) {
    std::erase_if(blockedPorts, [port](int p) {
        return p == port; // Remove port numbers matching the provided port from the list of blocked ports.
        });
}

void ProxyConfiguration::setProxyServerIp(const std::string& ip) {
    proxyIP = ip;
}

std::string ProxyConfiguration::getProxyServerIp() const {
    return proxyIP; 
}

void ProxyConfiguration::setProxyServerPort(int port) {
    proxyPort = port; 
}

int ProxyConfiguration::getProxyServerPort() const {
    return proxyPort; 
}

const std::vector<std::string>& ProxyConfiguration::getAllowedIPs() const {
    return allowedIPs; 
}

const std::vector<std::string>& ProxyConfiguration::getBlockedIPs() const {
    return blockedIPs; 
}

const std::vector<int>& ProxyConfiguration::getAllowedPorts() const {
    return allowedPorts;
}

const std::vector<int>& ProxyConfiguration::getBlockedPorts() const {
    return blockedPorts; 
}

void ProxyConfiguration::setLogFilesDir(const std::string& dir) {
    logFilesDir = dir; 
}

void ProxyConfiguration::setAuthFilesDir(const std::string& dir) {
    authFilesDir = dir; 
}

void ProxyConfiguration::setUsername(const std::string& user) {
    username = user; 
}

void ProxyConfiguration::setPassword(const std::string& pass) {
    password = pass;
}

void ProxyConfiguration::setDbFilesDir(const std::string& dir) {
    dbFilesDir = dir; 
}

std::string ProxyConfiguration::getLogFilesDir() const {
    return logFilesDir; 
}

std::string ProxyConfiguration::getAuthFilesDir() const {
    return authFilesDir; 
}

std::string ProxyConfiguration::getUsername() const {
    return username; 
}

std::string ProxyConfiguration::getPassword() const {
    return password; 
}

std::string ProxyConfiguration::getDbFilesDir() const {
    return dbFilesDir; 
}

void ProxyConfiguration::setNumActiveThreads(int num) {
    numActiveThreads = num; 
}

int ProxyConfiguration::getNumActiveThreads() const {
    return numActiveThreads;
}

void ProxyConfiguration::setLoggingMethod(int method) {
    loggingMethod = method; 
}

int ProxyConfiguration::getLoggingMethod() const {
    return loggingMethod; 
}

void ProxyConfiguration::setAuthenticationMethod(int method) {
    authenticationMethod = method;
}

int ProxyConfiguration::getAuthenticationMethod() const {
    return authenticationMethod;
}


void ProxyConfiguration::saveConfigToIni(const std::string& filename) {
    try {
        pt::ptree tree;

        // Save allowed IPs
        for (size_t i = 0; i < allowedIPs.size(); ++i) {
            tree.add("allowedIPs.IP" + std::to_string(i), allowedIPs[i]);
        }

        // Save blocked IPs
        for (size_t i = 0; i < blockedIPs.size(); ++i) {
            tree.add("blockedIPs.IP" + std::to_string(i), blockedIPs[i]);
        }

        // Save allowed ports
        for (size_t i = 0; i < allowedPorts.size(); ++i) {
            tree.add("allowedPorts.Port" + std::to_string(i), allowedPorts[i]);
        }

        // Save blocked ports
        for (size_t i = 0; i < blockedPorts.size(); ++i) {
            tree.add("blockedPorts.Port" + std::to_string(i), blockedPorts[i]);
        }

        // Save proxy server IP and port
        tree.put("proxyIP", proxyIP);
        tree.put("proxyPort", proxyPort);

        // Save other configuration settings
        tree.put("logFilesDir", logFilesDir);
        tree.put("authFilesDir", authFilesDir);
        tree.put("username", username);
        tree.put("password", password);
        tree.put("dbFilesDir", dbFilesDir);
        tree.put("numActiveThreads", numActiveThreads);
        tree.put("loggingMethod", loggingMethod);
        tree.put("authenticationMethod", authenticationMethod);

        // Write to INI file
        pt::write_ini(filename, tree);
    }
    catch (const std::exception& ex) {
        std::cerr << "Error during INI saving: " << ex.what() << std::endl;
    }
}

void ProxyConfiguration::loadConfigFromIni(const std::string& filename) {
    try {
        pt::ptree tree;

        // Try to read INI file
        pt::read_ini(filename, tree);

        // Load allowed IPs
        if (tree.get_child_optional("allowedIPs")) {
            for (const auto& entry : tree.get_child("allowedIPs")) {
                std::string ip = entry.second.get_value<std::string>();
                allowedIPs.push_back(ip);
            }
        }

        // Load blocked IPs
        if (tree.get_child_optional("blockedIPs")) {
            for (const auto& entry : tree.get_child("blockedIPs")) {
                std::string ip = entry.second.get_value<std::string>();
                blockedIPs.push_back(ip);
            }
        }

        // Load allowed ports
        if (tree.get_child_optional("allowedPorts")) {
            for (const auto& entry : tree.get_child("allowedPorts")) {
                int port = entry.second.get_value<int>();
                allowedPorts.push_back(port);
            }
        }

        // Load blocked ports
        if (tree.get_child_optional("blockedPorts")) {
            for (const auto& entry : tree.get_child("blockedPorts")) {
                int port = entry.second.get_value<int>();
                blockedPorts.push_back(port);
            }
        }

        // Load proxy server IP and port
        if (tree.get_optional<std::string>("proxyIP")) {
            proxyIP = tree.get<std::string>("proxyIP");
        }
        if (tree.get_optional<int>("proxyPort")) {
            proxyPort = tree.get<int>("proxyPort");
        }

        // Load other configuration settings
        if (tree.get_optional<std::string>("logFilesDir")) {
            logFilesDir = tree.get<std::string>("logFilesDir");
        }
        if (tree.get_optional<std::string>("authFilesDir")) {
            authFilesDir = tree.get<std::string>("authFilesDir");
        }
        if (tree.get_optional<std::string>("username")) {
            username = tree.get<std::string>("username");
        }
        if (tree.get_optional<std::string>("password")) {
            password = tree.get<std::string>("password");
        }
        if (tree.get_optional<std::string>("dbFilesDir")) {
            dbFilesDir = tree.get<std::string>("dbFilesDir");
        }
        if (tree.get_optional<int>("numActiveThreads")) {
            numActiveThreads = tree.get<int>("numActiveThreads");
        }
        if (tree.get_optional<int>("loggingMethod")) {
            loggingMethod = tree.get<int>("loggingMethod");
        }
        if (tree.get_optional<int>("authenticationMethod")) {
            authenticationMethod = tree.get<int>("authenticationMethod");
        }
    }
    catch (const boost::wrapexcept<pt::ini_parser::ini_parser_error>& ex) {
        throw std::runtime_error("INI Parsing Error: " + std::string(ex.what()));
    }
    catch (const std::exception& ex) {
        throw std::runtime_error("Error: " + std::string(ex.what()));
    }
}