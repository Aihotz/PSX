@echo off
setlocal enabledelayedexpansion

:: check if the minimum required parameters are passed
if "%~1"=="" (
    echo Usage: %~nx0 name extension1 [optional: extension2 ... extensionN]
    exit /b 1
)

:: extract the base name and extensions
set "name=%~1"
shift

:: if no extensions are specified, cpp and hpp are used by default
set "args="
set "hppPresent="
if "%~1"=="" (
    set "args=cpp hpp"
	set "hppPresent=Y"
) else (
	for %%A in (%*) do (
		if "!args!"=="" (
			set "args= "
		) else (
			set "args=!args! %%A"
		)
		
		if /i "%%~A"=="HPP" (
			set "hppPresent=Y"
		)
	)
)

:: path to CMakeLists.txt
set "cmakelists=CMakeLists.txt"

:: verify CMakeLists.txt exists
if not exist "%cmakelists%" (
    echo Error: CMakeLists.txt not found in the current directory.
    exit /b 1
)

:: create files and update CMakeLists.txt
set "new_files="
for %%E in (!args!) do (
    set "file=%name%.%%E"
	
	if exist !file! (
		echo File !file! already exists
	) else (
	    echo Creating file: !file!
		copy nul "!file!" >nul

		if /i "%%E"=="hpp" (
			echo Adding include guard to: !file!
			
			:: Convert %name% to uppercase
			set "uppername=%name%"
			for %%b in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
				set "uppername=!uppername:%%b=%%b!"
			)
			set "uppername=!uppername: =_!" 

			set "guard=!uppername!_HPP"
			
			(
				<nul set /p=#ifndef !guard!
				echo.
				<nul set /p=#define !guard!
				echo.
				echo.
				echo.
				echo.
				<nul set /p=#endif
			) > "!file!"
		) else if /i "%%E"=="cpp" if "!hppPresent!"=="Y" (
			echo Adding include directive for header in !file!
			
			(
				<nul set /p=#include "%name%.hpp"
				echo.
				echo.
			) > "!file!"
		)

		set "new_files=!new_files!    ^"%name%.%%E^""$LF$
	)
)

if "!new_files!"=="" (
	echo Nothing to update!
	exit /b 1
)

echo Updating CMakeLists.txt

:: update the SOURCE_FILES in CMakeLists.txt
set "printed="
(for /f "delims=" %%L in ('findstr /N "^" "%cmakelists%"') do (
	set "line=%%L"
	set "line=!line:*:=!"
	
		
	if "!line!"=="" (
		echo.
	) else (
	
		if "!printed!"=="" (
			set "printed=Y"
		) else (
			echo.
		)
	
		echo !line! | findstr /c:"set(SOURCE_FILES" >nul && (
			<nul set /p="!line!"
			echo.
			echo !new_files!
			set "printed="
		) || (
			<nul set /p="!line!"
		)
	)
)) > "%cmakelists%.temp"

move /y "%cmakelists%.temp" "%cmakelists%" >nul
echo Updated %cmakelists% with new source files.

echo Script completed successfully!