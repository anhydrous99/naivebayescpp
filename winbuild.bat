@echo off
setlocal enabledelayedexpansion
REM BUILD ZLIB
pushd extern\zlib
nmake -f win32/Makefile.msc

REM CREATE ZLIB INCLUDE
mkdir include
xcopy *.h include
mkdir lib
xcopy *.lib lib

REM BUILD CURL
cd ..\curl
CALL buildconf.bat
cd winbuild
nmake -f Makefile.vc mode=static WITH_ZLIB=static ZLIB_PATH=..\..\zlib

cd ..\..\..\
mkdir build
cd build
mkdir objs

SET SOURCE_DIR=..\src
SET INCLUDE_DIR=/I..\src
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\eigen-git-mirror
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\json\include
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\cxxopts\include
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\curl\include
SET INCLUDE_DIR=%INCLUDE_DIR% /I..\extern\zlib\include

SET LIBPATHS=/LIBPATH:"..\extern\zlib\lib"
SET LIBPATHS=%LIBPATHS% /LIBPATH:"..\extern\curl\builds\libcurl-vc-x64-release-static-zlib-static-ipv6-sspi-winssl\lib"

SET LINK_OBJECTS=zlib.lib libcurl_a.lib ws2_32.lib wldap32.lib MSVCRT.lib
SET LINK_OBJECTS=%LINK_OBJECTS% advapi32.lib crypt32.lib Normaliz.lib

for %%I in (%SOURCE_DIR%\*.cpp) do (
	cl %INCLUDE_DIR% /O2 /DNDEBUG /MD /std:c++14 /W4 /wd4267 /nologo /EHsc /DWIN32 /FD /c /Foobjs\%%~nI.obj %%I
)

for %%I in (objs\*.obj) do (
	SET LINK_OBJECTS=!LINK_OBJECTS! %%I
)
echo %LIBPATHS%
echo %LINK_OBJECTS%
link /incremental:no /out:naivebayescpp.exe /subsystem:console /nologo /machine:x64 %LIBPATHS% %LINK_OBJECTS%

popd