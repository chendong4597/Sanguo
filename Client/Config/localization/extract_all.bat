@echo off

::策划提交的多语言目录
cd ..\..\
set ROOT_DIR=%cd%
@echo "ROOT_DIR=%ROOT_DIR%"

::工具目录
set TOOL_DIR=%ROOT_DIR%\config\localization\bin
@echo TOOL_DIR=%TOOL_DIR%

::临时资源目录
set TEMP_DIR=%ROOT_DIR%\config\localization\temp
@echo TEMP_DIR=%TEMP_DIR%

::多语言配置跟目录
set LOCALIZATION_ROOT_DIR=%ROOT_DIR%\config\localization
@echo LOCALIZATION_ROOT_DIR=%LOCALIZATION_ROOT_DIR%

:: 项目美术目录
set ART_DIR=%ROOT_DIR%\arts
@echo ART_DIR=%ART_DIR%

:: 项目资源目录
set RESOURCE_DIR=%ROOT_DIR%\bin
@echo RESOURCE_DIR=%RESOURCE_DIR%

::配置目录
set CONFIG_FILE_DIR=%ROOT_DIR%\config\localization\config_file
@echo CONFIG_FILE_DIR=%CONFIG_FILE_DIR%

%TOOL_DIR%\I18NConv.exe --extract -i %LOCALIZATION_ROOT_DIR%\i18n.xml -l en_us -l trunk -p EXTRA_FULL -o %TEMP_DIR%\output.xml
