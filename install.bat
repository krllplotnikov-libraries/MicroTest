@echo off
rem Wrapper for double-click / cmd.exe: passes all arguments to install.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0install.ps1" %*
