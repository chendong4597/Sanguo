@echo off

::�߻��ύ�Ķ�����Ŀ¼
cd ..\..\
set ROOT_DIR=%cd%
@echo "ROOT_DIR=%ROOT_DIR%"

::����Ŀ¼
set TOOL_DIR=%ROOT_DIR%\config\localization\bin
@echo TOOL_DIR=%TOOL_DIR%

::��ʱ��ԴĿ¼
set TEMP_DIR=%ROOT_DIR%\config\localization\temp
@echo TEMP_DIR=%TEMP_DIR%

::���������ø�Ŀ¼
set LOCALIZATION_ROOT_DIR=%ROOT_DIR%\config\localization
@echo LOCALIZATION_ROOT_DIR=%LOCALIZATION_ROOT_DIR%

:: ��Ŀ����Ŀ¼
set ART_DIR=%ROOT_DIR%\arts
@echo ART_DIR=%ART_DIR%

:: ��Ŀ��ԴĿ¼
set RESOURCE_DIR=%ROOT_DIR%\bin
@echo RESOURCE_DIR=%RESOURCE_DIR%

::����Ŀ¼
set CONFIG_FILE_DIR=%ROOT_DIR%\config\localization\config_file
@echo CONFIG_FILE_DIR=%CONFIG_FILE_DIR%

%TOOL_DIR%\I18NConv.exe --extract -i %LOCALIZATION_ROOT_DIR%\i18n.xml -l en_us -l trunk -p EXTRA_FULL -o %TEMP_DIR%\output.xml
