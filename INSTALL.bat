@echo off
chcp 65001 >nul
title VoxScreen Pro - Instalador
color 0A

echo ==========================================
echo   VOXSCREEN PRO v2.0 - INSTALADOR
echo   Elaborado por VoxBrasil - Yuri Nogueira
echo ==========================================
echo.

set "GTASA_PATH="

REM Tentar encontrar GTA:SA automaticamente
if exist "C:\Program Files (x86)\Steam\steamapps\common\GTA San Andreas\gta_sa.exe" (
    set "GTASA_PATH=C:\Program Files (x86)\Steam\steamapps\common\GTA San Andreas"
) else if exist "C:\Program Files\Rockstar Games\GTA San Andreas\gta_sa.exe" (
    set "GTASA_PATH=C:\Program Files\Rockstar Games\GTA San Andreas"
)

if not defined GTASA_PATH (
    echo [INFO] Nao foi possivel encontrar o GTA:SA automaticamente.
    set /p GTASA_PATH="Digite o caminho completo da pasta do GTA San Andreas: "
)

if not exist "%GTASA_PATH%\gta_sa.exe" (
    echo [ERRO] gta_sa.exe nao encontrado no caminho especificado!
    pause
    exit /b 1
)

REM Verificar ModLoader
if not exist "%GTASA_PATH%\modloader" (
    echo [AVISO] ModLoader nao detectado!
    echo O ModLoader e necessario para funcionar.
    echo Baixe em: https://gtaforums.com/topic/669520-mod-loader/
    echo.
    pause
    exit /b 1
)

REM Criar pasta do mod
set "MOD_PATH=%GTASA_PATH%\modloader\VoxScreen Pro"
if not exist "%MOD_PATH%" mkdir "%MOD_PATH%"

REM Copiar arquivos
echo [INFO] Instalando VoxScreen Pro...
copy /Y "bin\VoxScreen.asi" "%MOD_PATH%\" >nul 2>&1
if exist "VoxScreen_Config.json" (
    copy /Y "VoxScreen_Config.json" "%GTASA_PATH%\" >nul 2>&1
)

REM Criar pasta de capturas
set "CAPTURES_PATH=C:\VoxScreen_Captures"
if not exist "%CAPTURES_PATH%" mkdir "%CAPTURES_PATH%"

echo.
echo [SUCESSO] VoxScreen Pro instalado!
echo.
echo Local: %MOD_PATH%
echo Capturas: %CAPTURES_PATH%
echo.
echo Controles:
echo   CTRL + 0  = Abrir menu
echo   F12       = Captura rapida (padrao)
echo.
pause
