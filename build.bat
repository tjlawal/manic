:: Copyright Frost Gorilla, Inc. All Rights Reserved.

@echo off
setlocal
cd /D "%~dp0"

:: ----------- Unpack argument ----------------------------------------
for %%a in (%*) do set "%%a=1"
if not "%msvc%"=="1" if not "%clang%"=="1" set msvc=1
if not "%release%"=="1" set debug=1
if "%debug%"=="1"       set release=0  && echo [debug mode]
if "%release%"=="1"     set debug=0    && echo [release mode]
if "%msvc%"=="1"        set clang=0    && echo [msvc compile]
if "%clang%"=="1"       set msvc=0     && echo [clang compile]

:: ------------ Unpack Command Line Build Arguments -------------------
set auto_compile_flags=
if "%asan%"=="1" 				set auto_compile_flags=%auto_compile_flags% -fsanitize=address && echo [asan enabled]

:: ------------ MSVC --------------------------------------------------
set cl_common=     /I..\src\ /nologo /FC /Z7 /W1
set cl_debug=      call cl /Od /DBUILD_DEBUG=1 %cl_common% %auto_compile_flags%
set cl_release=    call cl /O2 /DBUILD_DEBUG=0 %cl_common% %auto_compile_flags%
set cl_link=       /link /MANIFEST:EMBED /INCREMENTAL:NO /PDBALTPATH:%%%%_PDB%%%%
set cl_out=        /out:

:: ------------ Clang -------------------------------------------------
set clang_common=   -I..\src\ -Wall -gcodeview -fdiagnostics-absolute-paths -std=c99 -Wno-initializer-overrides -Wno-unused-function -Wno-missing-braces -Wno-unused-variable 
set clang_debug=    call clang -g -O0 -DBUILD_DEBUG=1 %clang_common% %auto_compile_flags%
set clang_release=  call clang -g -O2 -DBUILD_DEBUG=0 %clang_common% %auto_compile_flags%
set clang_link=     -fuse-ld=lld -Xlinker /MANIFEST:EMBED -Xlinker /pdbaltpath:%%%%_PDB%%%%
set clang_out=      -o

:: ------------ Per-Build Setting -------------------------------------
set link_resource=resource.res
if "%msvc%"=="1" set only_compile=/c
if "%msvc%"=="1" set EHsc=/EHsc
if "%msvc%"=="1" set no_aslr=/DYNAMICBASE:NO
if "%msvc%"=="1" set rc=call rc

if "%clang%"=="1" set only_compile=-c
if "%clang%"=="1" set EHsc=
if "%clang%"=="1" set no_aslr=-Wl,/DYNAMICBASE:NO
if "%clang%"=="1" set rc=call llvm-rc

:: ------------- Choose Compile/Link Lines
if "%msvc%"=="1"      set compile_debug=%cl_debug%
if "%msvc%"=="1"      set compile_release=%cl_release%
if "%msvc%"=="1"      set compile_link=%cl_link%
if "%msvc%"=="1"      set out=%cl_out%

if "%clang%"=="1"     set compile_debug=%clang_debug%
if "%clang%"=="1"     set compile_release=%clang_release%
if "%clang%"=="1"     set compile_link=%clang_link%
if "%clang%"=="1"     set out=%clang_out%

if "%debug%"=="1"     set compile=%compile_debug%
if "%release%"=="1"   set compile=%compile_release%

:: Prep Directories
if not exist run_tree (
 mkdir run_tree
 echo "[WARNING] creating run_tree, data files do not exist."
)

:: Process rc Files
pushd run_tree
%rc% /nologo /fo resource.res .\data\resource.rc || exit /b 1
popd

:: Get Subversion Revision History
for /f "tokens=2" %%i in ('call svn info ^| findstr "Revision"') do set compile=%compile% -DBUILD_SVN_REVISION=\"%%i\"

:: Build Things ---------------------------------------------------------------
pushd run_tree
if "%manic%"=="1"           		set didbuild=1 && %compile% ..\src\manic\manic_main.c %compile_link% %link_resource% %out%manic.exe || exit /b 1
if "%manic_console%"=="1"   		set didbuild=1 && %compile% ..\src\manic\manic_main.c %compile_link% %out%manic_console.exe || exit /b 1

:: Experimentals
if "%scratch_main%"=="1"				set didbuild=1 && %compile% ..\src\scratch\scratch_main.c %compile_link% %out%scratch_main.exe || exit /b 1
if "%scratch_fps%"=="1"					set didbuild=1 && %compile% ..\src\scratch\fps_counter.c %compile_link% %out%scratch_fps.exe || exit /b 1
popd

:: --- Warn On No Builds ------------------------------------------------------
if "%didbuild%"=="" (
  echo [WARNING] no valid build target specified; must use build target names as arguments to this script, like `build manic` or `build tests`.
  exit /b 1
)