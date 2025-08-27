@echo off

set BASE_LANGUAGE=en_us
set LANGUAGE=ba
set APPEND_LANGUAGE=-l fp_tw

::策划提交的多语言目录
cd ..\
set ROOT_DIR=%cd%
@echo ROOT_DIR=%ROOT_DIR%

set TOOL_DIR=%ROOT_DIR%\bin
@echo TOOL_DIR=%TOOL_DIR%

@echo extraing empty_%LANGUAGE%.xml...
%TOOL_DIR%\I18NConv.exe --extract -i %ROOT_DIR%\i18n.xml -l %BASE_LANGUAGE% -l %LANGUAGE% %APPEND_LANGUAGE% -p EXTRA_FULL -o %ROOT_DIR%\extra\empty_%LANGUAGE%.xml

@echo extraing temp_empty.xml...
%TOOL_DIR%\I18NConv.exe --extract -i %ROOT_DIR%\i18n.xml -l %BASE_LANGUAGE% -p EXTRA_EMPTY -o %ROOT_DIR%\extra\temp_empty.xml

@echo extraing temp_non_empty.xml...
%TOOL_DIR%\I18NConv.exe --extract -i %ROOT_DIR%\i18n.xml -l %LANGUAGE% -p EXTRA_NON_EMPTY -o %ROOT_DIR%\extra\temp_non_empty.xml

@echo filtering empty_%LANGUAGE%.xml...
%TOOL_DIR%\I18NConv.exe --filter -i %ROOT_DIR%\extra\empty_%LANGUAGE%.xml -l %BASE_LANGUAGE% -l %LANGUAGE% %APPEND_LANGUAGE% -f %ROOT_DIR%\extra\temp_empty.xml -f %ROOT_DIR%\extra\temp_non_empty.xml

del %ROOT_DIR%\extra\temp_empty.xml
del %ROOT_DIR%\extra\temp_non_empty.xml

pause
