@echo off
:: uninstall_driver.bat - must be run as Administrator

echo Removing PCIe Fix driver
echo =========================

:: Stop and delete service
sc stop PCIeFix
sc delete PCIeFix

:: Remove driver file
del /F C:\Windows\System32\drivers\pcie_fix.sys 2>nul

:: Clean up registry
reg delete "HKLM\SYSTEM\CurrentControlSet\Services\PCIeFix" /f 2>nul

echo Driver removed. Reboot your computer.
pause