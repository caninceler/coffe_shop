@echo off
chcp 65001 >nul
cd /d "%~dp0"
title Git Tamir (Drive cakisma temizleyici)

echo ============================================
echo   GIT TAMIR
echo   (Google Drive cakisma kopyalarini temizler)
echo ============================================
echo.

echo [1/4] Kilit (.lock) dosyalari temizleniyor...
del /q ".git\*.lock" 2>nul
del /q ".git\refs\heads\*.lock" 2>nul
del /q ".git\refs\remotes\origin\*.lock" 2>nul
echo     Tamam.
echo.

echo [2/4] Drive cakisma kopyalari (HEAD (1), master (1) vb.) siliniyor...
for /r ".git" %%F in ("* (1)" "* (2)" "* (3)" "*conflict*") do (
    echo     Siliniyor: %%F
    del /q "%%F" 2>nul
)
echo     Tamam.
echo.

echo [3/4] Bozuk referanslar onariliyor...
git remote set-head origin -a >nul 2>&1
echo     Tamam.
echo.

echo [4/4] Saglik kontrolu...
git fsck --connectivity-only 2>&1 | findstr /i "error badRef" >nul
if errorlevel 1 (
    echo     >>> Referans hatasi YOK. Temiz. <<<
) else (
    echo     !!! Hala hata var. Claude'a danis.
)
echo.

echo --- Durum ---
git status -sb 2>&1
echo.

echo ============================================
echo   Tamir bitti. Artik guncelle.bat / gonder.bat
echo   calistirabilirsin.
echo ============================================
echo.
pause
