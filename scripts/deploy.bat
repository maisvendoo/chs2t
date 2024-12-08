set ADDON_NAME=chs2t
set VERSION=v5.0.2
set DIR_NAME=%ADDON_NAME%-%VERSION%

set PKG_DIR=..\..\package

rem Создаем каталог для пакета
mkdir %PKG_DIR%
mkdir %PKG_DIR%\%DIR_NAME%

rem Создаем структуру каталогов
mkdir %PKG_DIR%\%DIR_NAME%\cfg\trains
mkdir %PKG_DIR%\%DIR_NAME%\cfg\vehicles\%ADDON_NAME%
mkdir %PKG_DIR%\%DIR_NAME%\docs
mkdir %PKG_DIR%\%DIR_NAME%\data\animations\%ADDON_NAME%
mkdir %PKG_DIR%\%DIR_NAME%\data\models\%ADDON_NAME%
mkdir %PKG_DIR%\%DIR_NAME%\data\sounds\%ADDON_NAME%
mkdir %PKG_DIR%\%DIR_NAME%\modules\%ADDON_NAME%

rem Копируем модули
xcopy /E /Y ..\..\modules\%ADDON_NAME%\*.dll %PKG_DIR%\%DIR_NAME%\modules\%ADDON_NAME%

rem Копируем конфиги
xcopy /E /Y ..\cfg\trains\*.* %PKG_DIR%\%DIR_NAME%\cfg\trains\
del %PKG_DIR%\%DIR_NAME%\cfg\trains\tep70bs*.xml
xcopy /E /Y ..\cfg\vehicles\%ADDON_NAME%\*.* %PKG_DIR%\%DIR_NAME%\cfg\vehicles\%ADDON_NAME%\

rem Копируем ресурсы
xcopy /E /Y ..\data\animations\%ADDON_NAME%\*.* %PKG_DIR%\%DIR_NAME%\data\animations\%ADDON_NAME%
xcopy /E /Y ..\data\models\%ADDON_NAME%\*.* %PKG_DIR%\%DIR_NAME%\data\models\%ADDON_NAME%
xcopy /E /Y ..\data\sounds\%ADDON_NAME%\*.* %PKG_DIR%\%DIR_NAME%\data\sounds\%ADDON_NAME%

rem Копируем документацию
xcopy /E /Y ..\docs\*.pdf %PKG_DIR%\%DIR_NAME%\docs\

rem Архифируем в 7zip
cd %PKG_DIR%\%DIR_NAME%
7z a -r ..\%DIR_NAME%.7z 
