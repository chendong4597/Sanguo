@echo off
for /r ..\client_config\ %%a in (*.xml) do (
	copy  /y "%%a" "..\..\..\Server\Config\GameConfig"g"
)