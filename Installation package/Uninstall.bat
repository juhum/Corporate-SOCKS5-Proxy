@echo off
set ServiceName=Corporate_SOCKS5_Proxy

echo Stopping the Windows service...
sc stop %ServiceName%
echo Service stopped successfully.

echo Deleting the Windows service...
sc delete %ServiceName%
echo Service deleted successfully.

pause