@echo off
:: check if the script is running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting administrator privileges...
	powershell -Command "Start-Process cmd -ArgumentList '/c cd /d %~dp0 && %~fnx0' -Verb runAs"
	exit /b
)

echo Running with administrator privileges...

cd ..

:: check if the directory exists
if exist "build" (
    echo Directory exists. Removing...
    rmdir /s /q "build" 
    echo Directory removed.
) else (
    echo Directory does not exist.
)

mkdir "build"
cd "build"
cmake ../repo

pause