@echo off

::策划导出工具目录
set EXDATACONVERTER_DIR=%cd%
@echo EXDATACONVERTER_DIR=%EXDATACONVERTER_DIR%

set ROOT_DIR=%EXDATACONVERTER_DIR%\..\..\
@echo "ROOT_DIR=%ROOT_DIR%"

::策划提交的多语言目录
set LOCALIZATION_DIR=%EXDATACONVERTER_DIR%\..\localization\
@echo LOCALIZATION_DIR=%LOCALIZATION_DIR%

::策划导出Clinet 配置目录
set CLIENT_CONFIG_DIR=%EXDATACONVERTER_DIR%\..\client_config\
@echo CLIENT_CONFIG_DIR=%CLIENT_CONFIG_DIR%

set TOOL_DIR=%LOCALIZATION_DIR%\bin
@echo TOOL_DIR=%TOOL_DIR%

::资源配置目录
set RESOURCE_CONFIG_DIR=%ROOT_DIR%\client\bin\config
@echo RESOURCE_CONFIG_DIR=%RESOURCE_CONFIG_DIR%

::临时资源目录
set TEMP_DIR=%LOCALIZATION_DIR%\temp
@echo TEMP_DIR=%TEMP_DIR%

cd %CLIENT_CONFIG_DIR%
@echo.
@echo Start Convert client_config ...
for /R %%s in (*.xml) do ( 
%TOOL_DIR%\enca.exe -L zh_CN %%~nxs | findstr "GB2312" > NUL && echo Convert %%s To UTF8 ... &&  %TOOL_DIR%\iconv.exe -f GBK -t UTF-8 %%~nxs > %TEMP_DIR%\%%~nxs && move %TEMP_DIR%\%%~nxs %%~nxs > NUL
) 


cd %RESOURCE_CONFIG_DIR%
@echo.
@echo Start Convert bin\config ...
for /R %%s in (*.xml) do ( 
%TOOL_DIR%\enca.exe -L zh_CN %%~nxs | findstr "GB2312" > NUL && echo Convert %%s To UTF8 ... &&  %TOOL_DIR%\iconv.exe -f GBK -t UTF-8 %%~nxs > %TEMP_DIR%\%%~nxs && move %TEMP_DIR%\%%~nxs %%~nxs > NUL
) 

@echo.
@echo End Convert
PAUSE


