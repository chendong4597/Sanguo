@echo off

::�߻���������Ŀ¼
set EXDATACONVERTER_DIR=%cd%
@echo EXDATACONVERTER_DIR=%EXDATACONVERTER_DIR%

set ROOT_DIR=%EXDATACONVERTER_DIR%\..\..\
@echo "ROOT_DIR=%ROOT_DIR%"

::�߻��ύ�Ķ�����Ŀ¼
set LOCALIZATION_DIR=%EXDATACONVERTER_DIR%\..\localization\
@echo LOCALIZATION_DIR=%LOCALIZATION_DIR%

::�߻�����Clinet ����Ŀ¼
set SERVER_CONFIG_DIR=%EXDATACONVERTER_DIR%\..\server_config\
@echo SERVER_CONFIG_DIR=%SERVER_CONFIG_DIR%

set TOOL_DIR=%LOCALIZATION_DIR%\bin
@echo TOOL_DIR=%TOOL_DIR%


::��ʱ��ԴĿ¼
set TEMP_DIR=%LOCALIZATION_DIR%\temp
@echo TEMP_DIR=%TEMP_DIR%

cd %SERVER_CONFIG_DIR%
@echo.
@echo Start Convert ...
for %%s in (*.xml) do ( 
%TOOL_DIR%\enca.exe -L zh_CN %%~nxs | findstr "GB2312" > NUL && echo Convert %%s To UTF8 ... &&  %TOOL_DIR%\iconv.exe -f GBK -t UTF-8 %%~nxs > %TEMP_DIR%\%%~nxs && move %TEMP_DIR%\%%~nxs %%~nxs > NUL
) 



@echo.
@echo End Convert
PAUSE


