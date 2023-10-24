/*
 * main.cpp
 * Purpose: Entry point for the Corporate SOCKS5 Proxy service.
 *          Implements service initialization, control handling, and the main service loop.
 *
 * 
 * Commands used to start/stop the windows service, in CMD (run as administrator) input:
 * sc create Corporate_SOCKS5_Proxy binPath= "path_to.exe" start=auto
 * sc start Corporate_SOCKS5_Proxy
 * sc query Corporate_SOCKS5_Proxy
 * sc stop Corporate_SOCKS5_Proxy
 * sc delete Corporate_SOCKS5_Proxy
 * 
 * Disclaimer: it will run on Windows start due to start=auto.
 * 
 * 
 * Install.bat content:
 * @echo off
 * set ServiceName=Corporate_SOCKS5_Proxy
 * set BinaryPath="%~dp0Server application.exe"
 *
 * echo Creating the Windows service...
 * sc create %ServiceName% binPath= %BinaryPath% start= auto
 * echo Service created successfully.
 *
 * echo Starting the Windows service...
 * sc start %ServiceName%
 * echo Service started successfully.
 *
 * pause
 *
 * 
 * Uninstall.bat content:
 * @echo off
 * set ServiceName=Corporate_SOCKS5_Proxy
 *
 * echo Stopping the Windows service...
 * sc stop %ServiceName%
 * echo Service stopped successfully.
 *
 * echo Deleting the Windows service...
 * sc delete %ServiceName%
 * echo Service deleted successfully.
 *
 * pause
 * 
 * 
 * Additionally FoxyProxy plugin for Firefox has to be used to connect to the proxy server.
 * https://addons.mozilla.org/pl/firefox/addon/foxyproxy-standard/
 * 
 * 
 * Database file can be viewed using Online SQLite Viewer:
 * https://inloop.github.io/sqlite-viewer/
 * 
 * 
 * Authors: Adam Kuszczyński, Volodymyr Parakhonych, Szymon Siąkała
 * Version: 1.0
 * Date: 01.09.2023
 */

#include <iostream>
#include <fstream>

#include "Libraries/ProxyServer.h"

// Service status variables
SERVICE_STATUS service_status = { 0 };
SERVICE_STATUS_HANDLE handle_status = NULL;

// Forward declarations of service functions
void WINAPI service_main(DWORD argc, LPWSTR* argv);
void WINAPI service_ctrl_handler(DWORD);
void report_service_status(DWORD, DWORD, DWORD);

// Shared pointer to the ProxyServer instance
std::shared_ptr<ProxyServer> server;

// Log file for service messages
std::ofstream log_file("C:\\Proxy_server\\service_log.txt", std::ios::out);

int main()
{
  // Service entry point
    SERVICE_TABLE_ENTRY service_table[] =
    {
          { const_cast<LPWSTR>(static_cast<LPCWSTR>(L"")), (LPSERVICE_MAIN_FUNCTION)service_main },
          { NULL, NULL }
    };

    // Start the service control dispatcher
    StartServiceCtrlDispatcher(service_table);
    return 0;
}

// Service main function
void service_main(DWORD argc, LPWSTR* argv)
{
    // Check if the log file is open
    if (!log_file.is_open())
    {
        report_service_status(SERVICE_STOPPED, ERROR, 0);
        return;
    }

    // Register the service control handler
    handle_status = RegisterServiceCtrlHandler(L"Corporate_SOCKS5_Proxy", service_ctrl_handler);
    if (handle_status == NULL)
    {
        report_service_status(SERVICE_STOPPED, ERROR, 0);
        log_file << "Unable to RegisterServiceCtrlHandler." << GetLastError() << std::endl;
        return;
    }

    // Initialize service status
    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState = SERVICE_RUNNING;
    service_status.dwWin32ExitCode = 0;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwWaitHint = 0;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    // Report service status
    report_service_status(SERVICE_RUNNING, NO_ERROR, 0);
    log_file << "Service started." << std::endl;

    try
    {   
        // Initialize Boost.Asio io_context and proxy configuration
        boost::asio::io_context io_context;
        ProxyConfiguration proxyConfig;
        proxyConfig.loadConfigFromIni("C:\\Proxy_server\\config.ini");

        // Initialize logger and database
        std::shared_ptr<Logger> logger = std::make_shared<Logger>(2, proxyConfig.getLogFilesDir());
        std::shared_ptr<Database> database = std::make_shared<Database>(2, proxyConfig.getDbFilesDir());

        // Create and start the ProxyServer instance
        server = std::make_shared<ProxyServer>(io_context, proxyConfig.getProxyServerIp(), proxyConfig.getProxyServerPort(), proxyConfig, proxyConfig.getLoggingMethod(), logger, database);
        std::cout << "Proxy server started. Listening on " << proxyConfig.getProxyServerIp() << ":" << proxyConfig.getProxyServerPort() << std::endl;

        io_context.run();
    }
    catch (std::exception& e)
    {
        // Stop the server and log any exceptions
        server->stop();
        log_file << "Exception: " << e.what() << std::endl;
    }

    // Report service stopped and close the log file
    report_service_status(SERVICE_STOPPED, NO_ERROR, 0);
    log_file << "Service stoped." << std::endl;

    if (log_file.is_open())
    {
        log_file.close();
    }
}

// Service control handler
void WINAPI service_ctrl_handler(DWORD ctrl)
{
    switch (ctrl)
    {
    case SERVICE_CONTROL_STOP:
        // Report service stop pending, stop the server, and log the service stop
        report_service_status(SERVICE_STOP_PENDING, NO_ERROR, 3000);
        server->stop();
        log_file << "Service stoped." << std::endl;

        if (log_file.is_open())
        {
            log_file.close();
        }
        report_service_status(SERVICE_STOPPED, NO_ERROR, 0);
        break;

    default:
        break;
    }
}

// Function to report service status
void report_service_status(DWORD currentState, DWORD win32ExitCode, DWORD waitHint)
{
    service_status.dwCurrentState = currentState;
    service_status.dwWin32ExitCode = win32ExitCode;
    service_status.dwWaitHint = waitHint;

    SetServiceStatus(handle_status, &service_status);
}