@echo off
chcp 65001 >nul
cd /d "%~dp0"
title Degisiklikleri Gonder (push)

echo ============================================
echo   DEGISIKLIKLERI GONDER
echo ============================================
echo.

echo [1/4] Once karsi taraftan guncel cekiliyor (pull)...
git pull
if errorlevel 1 (
    echo.
    echo !!! PULL SIRASINDA SORUN OLDU.
    echo Muhtemelen bir cakisma var. Yardim icin Claude'a sor.
    echo.
    pause
    exit /b 1
)
echo.

echo [2/4] Degisiklikler hazirlaniyor...
git add -A
echo.

echo [3/4] Kayit (commit)...
set /p mesaj="Ne yaptin? (kisa aciklama yaz, Enter'a bas): "
if "%mesaj%"=="" set mesaj=guncelleme
git commit -m "%mesaj%"
if errorlevel 1 (
    echo.
    echo Gonderilecek yeni degisiklik yok gibi gorunuyor.
    echo.
    pause
    exit /b 0
)
echo.

echo [4/4] GitHub'a gonderiliyor (push)...
git push
if errorlevel 1 (
    echo.
    echo !!! PUSH BASARISIZ. Internet veya giris sorunu olabilir.
    echo.
    pause
    exit /b 1
)

echo.
echo ============================================
echo   TAMAM! Degisiklikler GitHub'a gonderildi.
echo   Arkadasin "guncelle" calistirinca gorecek.
echo ============================================
echo.
pause
