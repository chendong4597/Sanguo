@echo off

::策划提交的多语言目录
set LOCALIZATION_DIR=%cd%
@echo LOCALIZATION_DIR=%LOCALIZATION_DIR%

set TOOL_DIR=%LOCALIZATION_DIR%\bin
@echo TOOL_DIR=%TOOL_DIR%

@echo importing from directory [extra]...
::%TOOL_DIR%\I18NConv.exe --merge -i extra -e xml -o i18n.xml
%TOOL_DIR%\I18NConv.exe --merge -i extra -e xml -p CR_PREFER_NEW -o i18n.xml

copy extra\*.xml backup\
del extra\*.xml

pause
