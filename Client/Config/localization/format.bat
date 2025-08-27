@echo off

::策划提交的多语言目录
set LOCALIZATION_DIR=%cd%
@echo LOCALIZATION_DIR=%LOCALIZATION_DIR%

set TOOL_DIR=%LOCALIZATION_DIR%\bin
@echo TOOL_DIR=%TOOL_DIR%

@echo filtering i18n.xml...
%TOOL_DIR%\I18NConv.exe --filter -i i18n.xml -l trunk -f useless\translate_useless.xml

@echo formating i18n.xml...
%TOOL_DIR%\I18NConv.exe --format -i i18n.xml -l trunk -o i18n.xml

pause
