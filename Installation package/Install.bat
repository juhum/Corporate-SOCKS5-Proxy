@echo off
set ServiceName=Corporate_SOCKS5_Proxy
set BinaryPath="%~dp0Server application.exe"

echo Creating the Windows service...
sc create %ServiceName% binPath= %BinaryPath% start= auto
echo Service created successfully.

echo Starting the Windows service...
sc start %ServiceName%
echo Service started successfully.

pause