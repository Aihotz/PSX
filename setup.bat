@echo off

set repo_folder=repo

:: check if the "repo" folder exists
if not exist "%repo_folder%" (
    echo Creating "%repo_folder%" folder...
    mkdir %repo_folder%
)

:: move all files and folders (excluding "repo") into the "repo" folder
for /d %%D in (*) do (
    if /i not "%%D"=="%repo_folder%" (
        move "%%D" "%repo_folder%"
    )
)

for %%F in (*) do (
	:: avoid moving self
    if /i not "%%F"=="%~nx0" (
        move "%%F" "%repo_folder%"
    )
)


attrib -h .git
move .git "%repo_folder%\.git"
attrib +h "%repo_folder%\.git"

echo Done. Files have been moved into "%repo_folder%".

cd %repo_folder%

:: Create install.bat
echo @echo off >> install.bat
echo echo Installing...  >> install.bat
echo move "..\setup.bat" "setup.bat"  >> install.bat
echo python fetch_sources.py  >> install.bat
echo call build.bat  >> install.bat
echo del "%%~f0" >> install.bat

call install.bat