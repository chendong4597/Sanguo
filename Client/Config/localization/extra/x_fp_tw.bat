@echo off

set LANGUAGE=fp_tw

::策划提交的多语言目录
cd ..\
set ROOT_DIR=%cd%
@echo ROOT_DIR=%ROOT_DIR%

set TOOL_DIR=%ROOT_DIR%\bin
@echo TOOL_DIR=%TOOL_DIR%

@echo extraing empty_%LANGUAGE%.xml...
%TOOL_DIR%\I18NConv.exe --extract -i %ROOT_DIR%\i18n.xml -l %LANGUAGE% -p EXTRA_EMPTY -o %ROOT_DIR%\extra\empty_%LANGUAGE%.xml

pause
