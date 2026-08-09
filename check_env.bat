@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -no_logo
echo VsDevCmd returned: %errorlevel%
where cl.exe 2>&1
echo INCLUDE=%INCLUDE%
echo LIB=%LIB%
