start cmd /c StartConvert.bat
:wait
IF NOT EXIST proc1done GOTO wait
del proc1done
start cmd /c MoveXml.bat