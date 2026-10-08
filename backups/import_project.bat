@echo off
chcp 65001 >nul
echo Импорт проекта...

:: Распаковка архива
powershell Expand-Archive -Path "SensorMonitoring.zip" -DestinationPath "C:\HTMLprojects\"

:: Восстановление базы данных
echo Поиск PostgreSQL...

:: Поиск папки PostgreSQL
set PG_PATH=
for /d %%i in ("C:\Program Files\PostgreSQL\*") do (
    if exist "%%i\bin\pg_dump.exe" (
        set PG_PATH=%%i\bin
        echo Найден PostgreSQL: %%i
    )
)

if "%PG_PATH%"=="" (
    echo PostgreSQL не найден!
    echo Пожалуйста, укажите путь вручную.
    set /p PG_PATH="Введите путь к папке bin PostgreSQL: "
)

:: Запрос пароля PostgreSQL
echo.
echo Введите пароль пользователя postgres (по умолчанию: 12345)
set /p PGPASSWORD="Пароль: "

:: Проверка существования базы данных
echo Проверка наличия базы данных sensor_db...
"%PG_PATH%\psql" -U postgres -h localhost -c "SELECT 1 FROM pg_database WHERE datname='sensor_db'" -t | find "1" > nul
if %errorlevel% equ 0 (
    echo База данных sensor_db уже существует.
    set /p DROP_DB="Удалить существующую базу? (y/n): "
    if /i "%DROP_DB%"=="y" (
        echo Удаление существующей базы...
        "%PG_PATH%\psql" -U postgres -h localhost -c "DROP DATABASE IF EXISTS sensor_db;"
        echo Создание новой базы...
        "%PG_PATH%\psql" -U postgres -h localhost -c "CREATE DATABASE sensor_db;"
    )
) else (
    echo Создание базы данных sensor_db...
    "%PG_PATH%\psql" -U postgres -h localhost -c "CREATE DATABASE sensor_db;"
)

:: Восстановление из бэкапа
echo Восстановление данных...
"%PG_PATH%\psql" -U postgres -h localhost -d sensor_db -f sensor_db_backup.sql

if %errorlevel% equ 0 (
    echo База данных успешно восстановлена!
) else (
    echo [ОШИБКА] Не удалось восстановить базу данных!
    echo Проверьте пароль и имя файла бэкапа.
    pause
    exit /b 1
)
echo.

:: Установка зависимостей
cd C:\HTMLprojects\SensorMonitoring\AppServer
call npm install

cd ..\AdminServer
call npm install

echo Готово! Запустите проекты.
echo.
echo Файлы для запуска:
echo   - start_all.bat         - запуск всех серверов
echo   - SensorMonitoring.sln  - открыть в Visual Studio
echo.
echo Адреса после запуска:
echo   - Фронтенд:     http://localhost:5095
echo   - Админ-панель: http://localhost:3001/admin
echo.
echo Пароль администратора: admin / admin123
echo.
echo Нажмите любую клавишу для выхода...
pause