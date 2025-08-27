@echo off
for /r ..\client_config\ %%a in (*.xml) do (
	copy  /y "%%a" "..\..\ThirdConfig\localization\config_file"
	copy  /y "%%a" "..\..\..\Server\Config\GameConfig"
    move /y "%%a" "..\..\ThirdReich\Content\src_config"
)