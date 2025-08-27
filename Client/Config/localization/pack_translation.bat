@echo off

if "%1"=="" goto restart
set LANGUAGE=%1
goto check

:restart
@echo Choose the Language to pack, the choice can be: "trunk"
set /p LANGUAGE=Please choose the Language id:

:check
if %LANGUAGE% NEQ trunk if %LANGUAGE% NEQ en_us if %LANGUAGE% NEQ zh_tw if %LANGUAGE% NEQ fp_tw if %LANGUAGE% NEQ de if %LANGUAGE% NEQ fr if %LANGUAGE% NEQ th if %LANGUAGE% NEQ kr if %LANGUAGE% NEQ jp if %LANGUAGE% NEQ ru if %LANGUAGE% NEQ it if %LANGUAGE% NEQ ba if %LANGUAGE% NEQ es if %LANGUAGE% NEQ arb if %LANGUAGE% NEQ pt if %LANGUAGE% NEQ tr if %LANGUAGE% NEQ vn if %LANGUAGE% NEQ id (
@echo Error, unsupported Language id: %LANGUAGE%
goto restart
)

@echo LANGUAGE=%LANGUAGE%

::策划提交的多语言目录
cd ..\..\
set ROOT_DIR=%cd%
@echo "ROOT_DIR=%ROOT_DIR%"

::工具目录
set TOOL_DIR=%ROOT_DIR%\ThirdConfig\localization\bin
@echo TOOL_DIR=%TOOL_DIR%

::临时资源目录
set TEMP_DIR=%ROOT_DIR%\ThirdConfig\localization\temp
@echo TEMP_DIR=%TEMP_DIR%

::i18n目录
set I18N_DIR=%ROOT_DIR%\ThirdConfig\localization\i18n
@echo I18N_DIR=%I18N_DIR%

::mo目录
set MO_DIR=%ROOT_DIR%\ThirdConfig\localization\mo
@echo MO_DIR=%MO_DIR%

::多语言配置跟目录
set LOCALIZATION_ROOT_DIR=%ROOT_DIR%\ThirdConfig\localization
@echo LOCALIZATION_ROOT_DIR=%LOCALIZATION_ROOT_DIR%

:: 项目美术目录
set ART_DIR=%ROOT_DIR%\arts
@echo ART_DIR=%ART_DIR%

:: 项目资源目录
set RESOURCE_DIR=%ROOT_DIR%\bin
@echo RESOURCE_DIR=%RESOURCE_DIR%

::配置目录
set CONFIG_FILE_DIR=%ROOT_DIR%\ThirdConfig\localization\config_file
@echo CONFIG_FILE_DIR=%CONFIG_FILE_DIR%

:: 工程配置目录
set PROJECT_DIR=%ROOT_DIR%\ThirdReich\Content
@echo PROJECT_DIR=%PROJECT_DIR%


cd %LOCALIZATION_ROOT_DIR%

if "%2"=="no_export_trans" goto language_switch

call export_translation.bat
if %errorlevel% NEQ 0 goto error_exit


:: zh_tw, fp_tw, kr 语言需要特殊处理
:language_switch
if %LANGUAGE%==zh_tw goto special
if %LANGUAGE%==kr goto special
if %LANGUAGE%==jp goto special
if %LANGUAGE%==fp_tw goto special
if %LANGUAGE%==trunk goto special
if %LANGUAGE%==en_us goto normal
if %LANGUAGE%==de goto special
if %LANGUAGE%==ru goto special
if %LANGUAGE%==it goto special
if %LANGUAGE%==ba goto special
if %LANGUAGE%==fr goto special
if %LANGUAGE%==th goto special
if %LANGUAGE%==es goto special
if %LANGUAGE%==arb goto special
if %LANGUAGE%==pt goto special
if %LANGUAGE%==tr goto special
if %LANGUAGE%==vn goto special
if %LANGUAGE%==id goto special
goto final


:special

@echo dumping charset...
%TOOL_DIR%\I18NConv.exe --charset -i %LOCALIZATION_ROOT_DIR%\i18n.xml -i %LOCALIZATION_ROOT_DIR%\addition_%LANGUAGE%.xml -l %LANGUAGE% -o %TEMP_DIR%\default_unicode.txt
if %errorlevel% NEQ 0 goto error_exit
goto final


:normal 
@echo creating default bitmap font...
%TOOL_DIR%\BMFont\bmfont.exe -c %TOOL_DIR%\BMFont\%LANGUAGE%\default.bmfc -o %RESOURCE_DIR%\fonts\%LANGUAGE%\default.fnt -t %CONFIG_FILE_DIR%\common.template
if %errorlevel% NEQ 0 goto error_exit
goto final


:final
@echo making distance field...
for /r %RESOURCE_DIR%\fonts\%LANGUAGE% %%a in (*.png) do %TOOL_DIR%\DistanceFieldGen.exe %%a %%a

@echo copying png files...
xcopy %RESOURCE_DIR%\fonts\%LANGUAGE%\* %ART_DIR%\fonts\%LANGUAGE%\* /e /r /h /s /d /y

@echo importing from %LOCALIZATION_ROOT_DIR%\extra\empty_%LANGUAGE%.xml ...
::%TOOL_DIR%\I18NConv.exe --merge -i extra -e xml -o i18n.xml
%TOOL_DIR%\I18NConv.exe --merge -i %LOCALIZATION_ROOT_DIR%\extra\empty_%LANGUAGE%.xml -i %TEMP_DIR%\..\customize_key.xml -p CR_PREFER_NEW -o %I18N_DIR%\i18n.xml

copy extra\*.xml backup\
del extra\*.xml

@echo converting all_key.xml to %LANGUAGE%.mo
%TOOL_DIR%\I18NConv.exe --tomo -i %I18N_DIR%\i18n.xml -k %TEMP_DIR%\all_key.xml -l %LANGUAGE% -o %MO_DIR%\%LANGUAGE%.mo > %TEMP_DIR%\pack_translation_warning.xml
if %errorlevel% NEQ 0 goto error_exit

@echo extra empty keys to %TEMP_DIR%\empty_keys_%LANGUAGE%.xml
%TOOL_DIR%\I18NConv.exe --extract -i %I18N_DIR%\i18n.xml -k %TEMP_DIR%\all_key.xml -l %LANGUAGE% -p EXTRA_EMPTY -o %TEMP_DIR%\empty_keys_%LANGUAGE%.xml

@echo copying project files...
copy %MO_DIR%\%LANGUAGE%.mo %PROJECT_DIR%\config
goto end

if %errorlevel% NEQ 0 goto error_exit

::@echo copying lua Key_Export files...
::copy %RESOURCE_DIR%\script\Key_Export.lua %ROOT_DIR%\config\localization\lua_export\

::@echo copying art files...
::copy %RESOURCE_DIR%\config\%LANGUAGE%.mo %ART_DIR%\config\
::goto end


:error_exit
@echo Error occur while running the script, check the step above for more information
pause
exit 1

:end
pause
