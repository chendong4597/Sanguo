@ECHO OFF

REM 复制一份
echo 复制一份 i18n_unuse.xml
copy i18n.xml i18n_unuse.xml

echo 从 all_key.xml 中过滤重复KEY
bin\I18NConv.exe --filter -i i18n_unuse.xml -e xml -f temp\all_key.xml
bin\I18NConv.exe --filter -i i18n.xml -e xml -f i18n_unuse.xml

echo 清除 i18n_unuse.xml
del i18n_unuse.xml

PAUSE