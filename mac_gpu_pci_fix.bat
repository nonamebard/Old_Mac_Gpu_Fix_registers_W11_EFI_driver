@echo off
:: install_driver.bat - must be run as Administrator

echo Installing PCIe Fix driver for MacBook 9400M
echo ==============================================

:: Copy driver to system32
copy /Y pcie_fix.sys C:\Windows\System32\drivers\

:: Create service
sc create PCIeFix type= kernel start= boot binPath= C:\Windows\System32\drivers\pcie_fix.sys

:: Verify service creation
sc query PCIeFix

:: Add to registry for early boot loading
reg add "HKLM\SYSTEM\CurrentControlSet\Services\PCIeFix" /v Group /t REG_SZ /d "Extended Base" /f

echo Driver installed. Reboot your computer.
pause