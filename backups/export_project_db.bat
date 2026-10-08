@echo off
chcp 65001 >nul
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

:: Экспорт базы данных
echo Экспорт базы данных...
"%PG_PATH%\pg_dump" -U postgres -d sensor_db > sensor_db_backup.sql

if %errorlevel% equ 0 (
    echo База данных экспортирована: sensor_db_backup.sql
) else (
    echo Ошибка экспорта! Проверьте пароль и имя базы.
    pause
    exit /b 1
)


echo Готово!
echo Файл: sensor_db_backup.sql
pause