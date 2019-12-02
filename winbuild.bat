@echo off

IF "%~1"=="clean" (
	PUSHD
	RMDIR /S build
	CD extern\curl
	RMDIR /S builds
	CALL buildconf.bat -clean
	CD ..\zlib
	RMDIR /S include
	RMDIR /S lib
	nmake -f win32/Makefile.msc
	POPD
	EXIT /B
)
IF "%~1"=="getdeps" (
	MKDIR extern
	PUSHD extern
	ECHO GETTING CURL
	powershell -Command "Invoke-WebRequest https://github.com/curl/curl/releases/download/curl-7_67_0/curl-7.67.0.zip -OutFile curl.zip"
	ECHO GETTING CXXOPTS
	powershell -Command "Invoke-WebRequest https://github.com/jarro2783/cxxopts/archive/v2.2.0.zip -OutFile cxxopt.zip"
	ECHO GETTING EIGEN
	powershell -Command "Invoke-WebRequest https://github.com/eigenteam/eigen-git-mirror/archive/3.3.7.zip -OutFile eigen.zip"
	ECHO GETTING JSON
	powershell -Command "Invoke-WebRequest https://github.com/nlohmann/json/archive/v3.7.3.zip -OutFile json.zip"
	ECHO GETTING ZLIB
	powershell -Command "Invoke-WebRequest https://www.zlib.net/zlib1211.zip -OutFile zlib.zip"
	unzip curl.zip
	unzip cxxopt.zip
	unzip eigen.zip
	unzip json.zip
	unzip zlib.zip
	DEL curl.zip
	DEL cxxopt.zip
	DEL eigen.zip
	DEL json.zip
	DEL zlib.zip
	MOVE curl-7.67.0 curl
	MOVE cxxopts-2.2.0 cxxopts
	MOVE eigen-git-mirror-3.3.7 eigen-git-mirror
	MOVE json-3.7.3 json
	MOVE zlib-1.2.11 zlib
	POPD
	EXIT /B
)

SETLOCAL enabledelayedexpansion
REM BUILD ZLIB
PUSHD extern\zlib
nmake -f win32/Makefile.msc

REM CREATE ZLIB INCLUDE
MKDIR include
XCOPY *.h include
MKDIR lib
XCOPY *.lib lib

REM BUILD CURL
CD ..\curl
CALL buildconf.bat
CD winbuild
nmake -f Makefile.vc mode=static WITH_ZLIB=static ZLIB_PATH=..\..\zlib

CD ..\..\..\
MKDIR build
CD build
MKDIR objs

SET SOURCE_DIR=..\src
SET INCLUDE_DIR=/I..\src
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\eigen-git-mirror
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\json\include
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\cxxopts\include
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\curl\include
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\zlib\include

SET LIBPATHS=/LIBPATH:"..\extern\zlib\lib"
SET LIBPATHS=%LIBPATHS% /LIBPATH:"..\extern\curl\builds\libcurl-vc-x64-release-static-zlib-static-ipv6-sspi-winssl-obj-lib"

SET LINK_OBJECTS=ws2_32.lib wldap32.lib MSVCRT.lib advapi32.lib crypt32.lib Normaliz.lib zlib.lib libcurl_a.lib

for %%I in (%SOURCE_DIR%\*.cpp) do (
	cl %INCLUDE_DIR% /O2 /DNDEBUG /MD /DCURL_STATICLIB /std:c++14 /W4 /wd4267 /nologo /EHsc /DWIN32 /FD /c /Foobjs\%%~nI.obj %%I
)

for %%I in (objs\*.obj) do (
	SET LINK_OBJECTS=!LINK_OBJECTS! %%I
)
ECHO %LIBPATHS%
ECHO %LINK_OBJECTS%
link /out:naivebayescpp.exe /subsystem:console /nologo /machine:x64 %LIBPATHS% %LINK_OBJECTS%
COPY ..\stop_words.txt
COPY ..\webconfig.json
POPD