:: Build Configuration Variables Script

@echo off

set Libraries=setargv.obj

:: Select which architecture you want by uncommenting.
:: set Architecture=x86
   set Architecture=amd64

:: Select which build mode you want by uncommenting.
   set BuildMode=Debug
:: set BuildMode=Release

set IncludeDirs=-I ..\source\
set LibraryDirs=

set Defines=

set CompilerFlags=-Zc:__cplusplus -std:c++17 -nologo -Oi -MT -Gm- -GR -EHsc -Z7 -W4
set LinkerFlags=-opt:ref -incremental:no

set CompilerWarnings=-wd4100 -wd4505 -wd4189
set LinkerWarnings=-ignore:4099

set ResourceFile=..\resource\resources.rc
set ResourcePath=..\resource\

set InputResource=..\resource\resources.res
set InputSource=..\source\main.cpp

set OutputExecutable=GPAKUnpacker-%Architecture%

if %BuildMode%==Release (
    set CompilerFlags=%CompilerFlags% -WX
    set LinkerFlags=%LinkerFlags% -subsystem:windows
)
if %BuildMode%==Debug (
    set OutputExecutable=%OutputExecutable%-Debug
    set Defines=%Defines% -DBUILD_DEBUG
    set InputResource=
    set LinkerFlags=%LinkerFlags% -subsystem:console
)

if %Architecture%==x86 (
    set CompilerFlags=%CompilerFlags% -arch:IA32
    set LinkerFlags=%LinkerFlags%,5.1
)
