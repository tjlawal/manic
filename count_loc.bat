@echo off
setlocal
setlocal enabledelayedexpansion

set folder="..\playground\Aseprite-v1.3.10.1-Source"
set total_lines=0
set total_c_files=0
set total_h_files=0

for /r "%folder%" %%a in (*.c) do (
    for /f %%b in ('findstr /R /N "^" "%%a" ^| find /C ":"') do (
        set /a total_lines+=%%b
        set /a total_c_lines+=%%b
    )
)
echo Total lines of '.c' files: %total_c_lines%


for /r "%folder%" %%a in (*.h) do (
    for /f %%b in ('findstr /R /N "^" "%%a" ^| find /C ":"') do (
        set /a total_lines += %%b
        set /a total_h_lines += %%b
    )
)
echo Total lines of '.h' files: %total_h_lines%

echo Total lines of code: %total_lines%
