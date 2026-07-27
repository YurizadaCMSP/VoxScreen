@echo off
chcp 65001 >nul
title VoxScreen Pro - Compilador Terminal
color 0B
cls

echo.
echo    ╔══════════════════════════════════════════════════════════════╗
echo    ║                                                              ║
echo    ║           VOXSCREEN PRO v2.0 - COMPILADOR TERMINAL           ║
echo    ║              Elaborado por VoxBrasil - Yuri Nogueira          ║
echo    ║                                                              ║
echo    ╚══════════════════════════════════════════════════════════════╝
echo.

:: ========================================================================
::  VERIFICAR SE ESTA NO TERMINAL DE DESENVOLVEDOR MSVC
:: ========================================================================
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo  [ERRO] O compilador C++ (cl.exe) nao foi encontrado!
    echo.
    echo  Voce precisa executar este script dentro do:
    echo    -^> "x86 Native Tools Command Prompt for VS 2022"
    echo.
    echo  Como abrir:
    echo    1. Pressione WIN + R
    echo    2. Digite: cmd
    echo    3. Execute este comando:
    echo.
    echo    "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
    echo.
    echo    Ou procure no Menu Iniciar:
    echo    "x86 Native Tools Command Prompt"
    echo.
    pause
    exit /b 1
)

echo  [OK] Compilador MSVC detectado: & cl 2>&1 | findstr "Version"
echo.

:: ========================================================================
::  VERIFICAR ARGUMENTOS
:: ========================================================================
set "SKIP_DEPS=false"
if "%1"=="--skip-deps" set "SKIP_DEPS=true"

:: ========================================================================
::  CRIAR ESTRUTURA DE PASTAS
:: ========================================================================
echo  [INFO] Preparando estrutura de pastas...
if not exist "bin" mkdir bin
if not exist "include" mkdir include
if not exist "src\imgui" mkdir src\imgui
if not exist "src\minhook\hde" mkdir src\minhook\hde

:: ========================================================================
::  BAIXAR DEPENDENCIAS
:: ========================================================================
if "%SKIP_DEPS%"=="false" (
    echo.
    echo  [INFO] Baixando dependencias... Isso pode levar alguns minutos.
    echo.

    :: Verificar curl
    where curl >nul 2>nul
    if %errorlevel% neq 0 (
        echo  [ERRO] curl nao encontrado! Baixe as dependencias manualmente.
        pause
        exit /b 1
    )

    :: ImGui
    echo  [DOWNLOAD] Dear ImGui...
    curl -sL -o src\imgui\imgui.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp
    curl -sL -o src\imgui\imgui_draw.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp
    curl -sL -o src\imgui\imgui_widgets.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_widgets.cpp
    curl -sL -o src\imgui\imgui_tables.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_tables.cpp
    curl -sL -o src\imgui\imgui.h https://raw.githubusercontent.com/ocornut/imgui/master/imgui.h
    curl -sL -o src\imgui\imgui_internal.h https://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h
    curl -sL -o src\imgui\imconfig.h https://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h
    curl -sL -o src\imgui\imgui_impl_win32.cpp https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_win32.cpp
    curl -sL -o src\imgui\imgui_impl_win32.h https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_win32.h
    curl -sL -o src\imgui\imgui_impl_dx9.cpp https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_dx9.cpp
    curl -sL -o src\imgui\imgui_impl_dx9.h https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_dx9.h

    :: MinHook
    echo  [DOWNLOAD] MinHook...
    curl -sL -o minhook.zip https://github.com/TsudaKageyu/minhook/archive/refs/heads/master.zip

    where tar >nul 2>nul
    if %errorlevel% equ 0 (
        tar -xf minhook.zip 2>nul
    ) else (
        powershell -Command "Expand-Archive -Path 'minhook.zip' -DestinationPath '.' -Force" 2>nul
    )

    if exist "minhook-master" (
        xcopy /E /I /Y "minhook-master\src\*" "src\minhook\" >nul 2>&1
        rmdir /S /Q "minhook-master" 2>nul
        del minhook.zip 2>nul
    )

    :: nlohmann/json
    echo  [DOWNLOAD] nlohmann/json...
    curl -sL -o include\json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp

    :: stb_image_write
    echo  [DOWNLOAD] stb_image_write...
    curl -sL -o include\stb_image_write.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

    echo.
    echo  [OK] Todas as dependencias foram baixadas!
    echo.
) else (
    echo  [INFO] Pulando download de dependencias (--skip-deps)
    echo.
)

:: ========================================================================
::  VERIFICAR ARQUIVOS
:: ========================================================================
echo  [INFO] Verificando arquivos necessarios...
set "MISSING=0"

if not exist "src\main.cpp" (
    echo  [FALTA] src\main.cpp
    set "MISSING=1"
)
if not exist "src\imgui\imgui.cpp" (
    echo  [FALTA] src\imgui\imgui.cpp - Execute sem --skip-deps
    set "MISSING=1"
)
if not exist "src\minhook\MinHook.c" (
    echo  [FALTA] src\minhook\MinHook.c - Execute sem --skip-deps
    set "MISSING=1"
)
if not exist "include\json.hpp" (
    echo  [FALTA] include\json.hpp - Execute sem --skip-deps
    set "MISSING=1"
)
if not exist "include\stb_image_write.h" (
    echo  [FALTA] include\stb_image_write.h - Execute sem --skip-deps
    set "MISSING=1"
)

if "%MISSING%"=="1" (
    echo.
    echo  [ERRO] Arquivos faltando! Execute: compile_terminal.bat
    pause
    exit /b 1
)

echo  [OK] Todos os arquivos estao presentes.
echo.

:: ========================================================================
::  VERIFICAR DIRECTX SDK
:: ========================================================================
set "DXSDK=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)"
if not exist "%DXSDK%\Include\d3d9.h" (
    echo  [AVISO] DirectX SDK (June 2010) nao encontrado!
    echo  Baixe em: https://www.microsoft.com/en-us/download/details.aspx?id=6812
    echo.
    echo  Tentando usar headers do Windows SDK...
    set "DXSDK="
)

:: ========================================================================
::  COMPILAR
:: ========================================================================
echo  [COMPILACAO] Iniciando build... Aguarde.
echo.

set "DX_INC="
set "DX_LIB="
if exist "%DXSDK%\Include\d3d9.h" (
    set "DX_INC=/I"%DXSDK%\Include""
    set "DX_LIB=/LIBPATH:"%DXSDK%\Lib\x86""
)

cl /O2 /MT /EHsc /W3 /nologo ^
   /I"include" ^
   /I"src\imgui" ^
   /I"src\minhook" ^
   /I"src\minhook\hde" ^
   %DX_INC% ^
   /D "_CRT_SECURE_NO_WARNINGS" ^
   /D "IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS" ^
   /D "NOMINMAX" ^
   src\main.cpp ^
   src\imgui\imgui.cpp ^
   src\imgui\imgui_draw.cpp ^
   src\imgui\imgui_widgets.cpp ^
   src\imgui\imgui_tables.cpp ^
   src\imgui\imgui_impl_win32.cpp ^
   src\imgui\imgui_impl_dx9.cpp ^
   src\minhook\MinHook.c ^
   src\minhook\hook.c ^
   src\minhook\buffer.c ^
   src\minhook\trampoline.c ^
   src\minhook\hde\hde32.c ^
   src\minhook\hde\hde64.c ^
   /link ^
   %DX_LIB% ^
   d3d9.lib d3dx9.lib legacy_stdio_definitions.lib ^
   /DLL /OUT:bin\VoxScreen.asi

if %errorlevel% equ 0 (
    echo.
    echo    ╔══════════════════════════════════════════════════════════════╗
    echo    ║                    COMPILACAO CONCLUIDA!                     ║
    echo    ╠══════════════════════════════════════════════════════════════╣
    echo    ║  Arquivo gerado: bin\VoxScreen.asi                           ║
    echo    ║                                                              ║
    echo    ║  Para instalar, copie para:                                  ║
    echo    ║  GTA San Andreas\modloader\VoxScreen Pro\                     ║
    echo    ╚══════════════════════════════════════════════════════════════╝
    echo.

    :: Tentar instalar automaticamente
    set "GTASA="
    if exist "C:\Program Files (x86)\Steam\steamapps\common\GTA San Andreas\gta_sa.exe" (
        set "GTASA=C:\Program Files (x86)\Steam\steamapps\common\GTA San Andreas"
    )
    if exist "C:\Program Files\Rockstar Games\GTA San Andreas\gta_sa.exe" (
        set "GTASA=C:\Program Files\Rockstar Games\GTA San Andreas"
    )

    if defined GTASA (
        if exist "%GTASA%\modloader" (
            mkdir "%GTASA%\modloader\VoxScreen Pro" 2>nul
            copy /Y "bin\VoxScreen.asi" "%GTASA%\modloader\VoxScreen Pro\" >nul
            echo  [INSTALACAO] VoxScreen.asi instalado automaticamente!
            echo  Local: %GTASA%\modloader\VoxScreen Pro\
        )
    )

    echo.
    echo  Controles no jogo:
    echo    CTRL + 0  = Abrir menu
    echo    F12       = Captura rapida
    echo.
) else (
    echo.
    echo  [ERRO] A compilacao falhou! Verifique as mensagens acima.
    echo.
)

pause
