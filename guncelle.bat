@echo off
chcp 65001 >nul
cd /d "%~dp0"
title Guncel Cek (pull)

echo ============================================
echo   GUNCEL CEK (arkadasinin degisiklikleri)
echo ============================================
echo.

echo GitHub'dan en guncel hali cekiliyor...
echo.
git pull
if errorlevel 1 (
    echo.
    echo !!! PULL SIRASINDA SORUN OLDU.
    echo Belki kaydetmedigin degisikliklerin var ya da bir cakisma var.
    echo Yardim icin Claude'a sor.
    echo.
    pause
    exit /b 1
)

echo.
echo ============================================
echo   TAMAM! Proje guncel.
echo ============================================
echo.
pause
