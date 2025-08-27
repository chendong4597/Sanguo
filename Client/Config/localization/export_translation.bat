@echo off

::策划提交的多语言目录
cd ..\..\
set ROOT_DIR=%cd%\ThirdConfig
@echo ROOT_DIR=%ROOT_DIR%

::工具目录
set TOOL_DIR=%ROOT_DIR%\localization\bin
@echo TOOL_DIR=%TOOL_DIR%

::临时资源目录
set TEMP_DIR=%ROOT_DIR%\localization\temp
@echo TEMP_DIR=%TEMP_DIR%

::i18n目录
set I18N_DIR=%ROOT_DIR%\localization\i18n
@echo I18N_DIR=%I18N_DIR%

::配置目录
set CONFIG_FILE_DIR=%ROOT_DIR%\localization\config_file
@echo CONFIG_FILE_DIR=%CONFIG_FILE_DIR%

::资源目录
set RESOURCE_DIR=%ROOT_DIR%\localization\bin
@echo RESOURCE_DIR=%RESOURCE_DIR%

:: C++源代码目录
set CLASS_DIR=%ROOT_DIR%\..\ThirdReich\Source\ThirdReich
@echo CLASS_DIR=%CLASS_DIR%

:: C++源代码目录
set CLASS_DIR_LOC=%ROOT_DIR%\..\ThirdReich\static\static_logic
@echo CLASS_DIR_LOC=%CLASS_DIR_LOC%

:: C++源代码目录
set CLASS_DIR_DAT=%ROOT_DIR%\..\ThirdReich\static\static_data
@echo CLASS_DIR_DAT=%CLASS_DIR_DAT%

:: IOS源代码目录
set CLASS_IOS_DIR=%ROOT_DIR%\client\bzbee\libbaseutil\source\platform\ios
@echo CLASS_IOS_DIR=%CLASS_IOS_DIR%

del /Q %TEMP_DIR%\*.*
if NOT EXIST %TEMP_DIR% mkdir %TEMP_DIR%

@echo exporting config...
cd %ROOT_DIR%\config\localization

:: 开始处理 XML ============================================================
:: 导出配置字段
:: SET FIELD_NAME=s_Name
:: @echo exporting %FIELD_NAME%
:: for %%i in (AchieveDetail.xml) do (
:: 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
:: 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
:: 	if %errorlevel% NEQ 0 @echo exporting file error = %CONFIG_FILE_DIR%\%%i
:: )

:: 导出配置字段
:: SET FIELD_NAME=s_attack_type_key
:: @echo exporting %FIELD_NAME%
:: for %%i in (PetTypeCfg.xml) do (
:: 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
:: 	%TOOL_DIR%\I18NConv.exe --export_attr -i %RESOURCE_DIR%\config\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
:: 	if %errorlevel% NEQ 0 PAUSE
:: )
:: 
:: 导出配置字段
SET FIELD_NAME=s_name
for %%i in (MapSetting.xml,PlantCfg.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 @echo exporting file error = %CONFIG_FILE_DIR%\%%i
)
:: 
:: 导出配置字段
SET FIELD_NAME=s_name
for %%i in (Item.xml,Material.xml,PlayerAttrName.xml,GenelTemplate.xml,Mob.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 @echo exporting file error = %CONFIG_FILE_DIR%\%%i
)
:: 
:: 导出配置字段
SET FIELD_NAME=s_type_name
for %%i in (Material.xml,NpcType.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 @echo exporting file error = %CONFIG_FILE_DIR%\%%i
)
:: 
:: 导出配置字段
SET FIELD_NAME=s_Name
for %%i in (FuncNpcType.xml,FuncDetailType.xml,NpcType.xml,HouseTpyCfg.xml,UITrans.xml,AchieveDetail.xml,DonateCfg.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 @echo exporting file error = %CONFIG_FILE_DIR%\%%i
)
:: 
:: 导出配置字段
SET FIELD_NAME=s_say
for %%i in (FuncNpcType.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 @echo exporting file error = %CONFIG_FILE_DIR%\%%i
)
::
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_title
for %%i in (HelpCfg.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
::
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_desc
for %%i in (SkillType.xml,NpcType.xml,HelpCfg.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
::
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_Desc
for %%i in (AchieveDetail.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_say1
for %%i in (NpcType.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_say2
for %%i in (NpcType.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_say3
for %%i in (NpcType.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_say4
for %%i in (NpcType.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_desc_1
for %%i in (Item.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
:: 
:: 
:: :: 导出配置字段
SET FIELD_NAME=s_NeedTips
for %%i in (AchieveDetail.xml) do (
 	@echo exporting Field [%FIELD_NAME%] ======== %RESOURCE_DIR%\config\%%i
 	%TOOL_DIR%\I18NConv.exe --export_attr -i %CONFIG_FILE_DIR%\%%i -a %FIELD_NAME% -o %TEMP_DIR%\temp.xml
 	if %errorlevel% NEQ 0 PAUSE
)
::



:: 开始处理Client 代码 _LC ============================================================   -i %CLASS_IOS_DIR% -i %RESOURCE_DIR%\script
@echo listing code files...
%TOOL_DIR%\I18NConv.exe --list_file -i %CLASS_DIR% -i %CLASS_DIR_LOC% -i %CLASS_DIR_DAT% -e mm -e m -e lua -e h -e cpp -e inl -e cc -e c -o %TEMP_DIR%\list.txt
if %errorlevel% NEQ 0 goto error_exit


:: 开始生成 ============================================================
@echo generating po files...
%TOOL_DIR%\xgettext.exe --files-from=%TEMP_DIR%\list.txt --output=%TEMP_DIR%\code.po --c++ --from-code=utf-8 --keyword=_LC --omit-header
if %errorlevel% NEQ 0 goto error_exit

@echo converting po file to xml files...
%TOOL_DIR%\I18NConv.exe --toxml -i %TEMP_DIR%\code.po -o %TEMP_DIR%\code.xml
if %errorlevel% NEQ 0 goto error_exit

@echo merging xml files to i18n.xml...
cd %ROOT_DIR%\config\localization
%TOOL_DIR%\I18NConv.exe --merge -i %TEMP_DIR%\temp.xml -i %TEMP_DIR%\code.xml -p CR_PREFER_NEW -l trunk -o %I18N_DIR%\i18n.xml
if %errorlevel% NEQ 0 goto error_exit

@echo filtering i18n.xml...
%TOOL_DIR%\I18NConv.exe --filter -i %I18N_DIR%\i18n.xml -l trunk -f useless\translate_useless.xml
if %errorlevel% NEQ 0 goto error_exit

@echo formating i18n.xml...
%TOOL_DIR%\I18NConv.exe --format -i %I18N_DIR%\i18n.xml -l trunk -o %I18N_DIR%\i18n.xml
if %errorlevel% NEQ 0 goto error_exit

@echo merging all key files...
%TOOL_DIR%\I18NConv.exe --merge -i %TEMP_DIR%\code.xml -i %TEMP_DIR%\temp.xml -i %TEMP_DIR%\..\customize_key.xml -o %TEMP_DIR%\all_key.xml
if %errorlevel% NEQ 0 goto error_exit

@echo filter all_key.xml
%TOOL_DIR%\I18NConv.exe --filter -i %TEMP_DIR%\all_key.xml -f useless\translate_useless.xml
if %errorlevel% NEQ 0 goto error_exit

goto end

:error_exit
@echo Error occur while running the script, check the step above for more information
pause
exit 1

:end
pause

