@echo off
setlocal
cd /D "%~dp0"

:: --- Unpack argument
for %%a in (%*) do set "%%a=1"
if not "%clang%"=="1" 	if not "%msvc%"=="1" set clang=1
if not "%release%"=="1" set debug=1
if "%debug%"=="1" 			set release=0 && echo [debug mode]
if "%release%"=="1" 		set debug=0 	&& echo [release mode]
if "%clang%"=="1" 			set msvc=0    && echo [compiling with clang]
if "%msvc%"=="1" 			  set clang=0		&& echo [compiling with msvc]

:: --- Unpack command line build argument
set auto_compile_flags=
if "%asan%"=="1" 				set auto_compile_flags=%auto_compile_flags% -fsanitize=address && echo [asan enabled]

set profiler_flags=
if "%profile%"=="1" (
	if "%sp%"=="1" 				set profiler_flags=%profiler_flags% -DBUILD_PROFILE=1 -DPROFILER_SUPERLUMINAL=1 && echo [profiler enabled, using SuperluminalPerfomance profiler]
	if "%tracy%"=="1" 		set profiler_flags=%profiler_flags% -DBUILD_PROFILE=1 -DPROFILER_TRACY=1 				&& echo [profiler enabled, using Tracy profiler]
)

:: clang writes the result of getting only the preprocessed code to whatever is pointed to by '-o'.
set preprocessor_flags=
if "%preprocess%"=="1" (
	if "%clang%"=="1" set preprocessor_flags=%preprocessor_flags% -E
	if "%msvc%"=="1"  set preprocessor_flags=%preprocessor_flags% /P
	echo [preprocessor output only]
)

:: --- Clang 
set clang_common=   -I..\src\ -Wall -std=c++11 -ferror-limit=200 -gcodeview -fdiagnostics-absolute-paths -fno-exceptions -Wno-initializer-overrides -Wno-unused-function -Wno-missing-braces -Wno-unused-variable -Wno-writable-strings -Wno-address-of-temporary -Wno-switch -Wno-return-type -Wno-unused-command-line-argument -Wno-unused-but-set-variable
set clang_debug=    call clang -g -O0 -DBUILD_DEBUG=1 %clang_common% %auto_compile_flags% %preprocessor_flags% %profiler_flags%
set clang_release=  call clang -g -O2 -DBUILD_DEBUG=0 -DBUILD_RELEASE=1 %clang_common% %auto_compile_flags%
set clang_link=     -fuse-ld=lld -Xlinker /MANIFEST:EMBED -Xlinker /pdbaltpath:%%%%_PDB%%%% -Wl,/ignore:4099
set clang_out=      -o

:: --- MSVC
set cl_common=     /I..\src\ /nologo /FC /Z7 /EHsc /W1
set cl_debug=      call cl /Od /DBUILD_DEBUG=1 %cl_common% %auto_compile_flags% %preprocessor_flags% %profiler_flags%
set cl_release=    call cl /O2 /DBUILD_DEBUG=0 -DBUILD_RELEASE=1 %cl_common% %auto_compile_flags%
set cl_link=       /link /MANIFEST:EMBED /DEBUG:FULL /PDBALTPATH:%%%%_PDB%%%% /ignore:4099
set cl_out=        /out:

:: --- Build setting
set link_resource=resource.res
if "%msvc%"=="1" set only_compile=/c
if "%msvc%"=="1" set EHsc=/EHsc
if "%msvc%"=="1" set no_aslr=/DYNAMICBASE:NO
if "%msvc%"=="1" set rc=call rc

if "%clang%"=="1" set only_compile=-c
if "%clang%"=="1" set EHsc=
if "%clang%"=="1" set no_aslr=-Wl,/DYNAMICBASE:NO
if "%clang%"=="1" set rc=call llvm-rc

:: --- Compile/Link
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

:: --- Prep directories
if not exist run_tree (
 mkdir run_tree
 echo "[WARNING] creating run_tree, data files do not exist."
)

:: Process rc files
pushd run_tree
	%rc% /nologo /fo resource.res .\data\resource.rc || exit /b 1
popd

:: Get subversion revision history
for /f "tokens=2" %%i in ('call svn info ^| findstr "Revision"') do set compile=%compile% -DBUILD_SVN_REVISION=%%i

:: --- Build Things 
pushd run_tree
	REM if "%manic%"=="1"		set didbuild=1 && %compile% ..\src\manic\main.cc %compile_link% %link_resource% %out%manic.exe || exit /b 1
	%compile% ..\src\manic\main.cc %compile_link% %link_resource% %out%manic.exe || exit /b 1

popd

:: --- Warn On No Builds 
REM if "%didbuild%"=="" (
REM   echo [WARNING] no valid build target specified; must use build target names as arguments to this script, like `build manic` or `build tests`.
REM   exit /b 1
REM )

endlocal