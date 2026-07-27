@echo off
chcp 65001 >nul
title VoxScreen Pro - Compilador
color 0B

echo ==========================================
echo   VOXSCREEN PRO v2.0 - COMPILADOR
echo   Elaborado por VoxBrasil - Yuri Nogueira
echo ==========================================
echo.

REM Verificar se o Visual Studio esta instalado
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERRO] Visual Studio nao encontrado!
    echo Instale o Visual Studio com C++ workload.
    pause
    exit /b 1
)

REM Criar diretorios
if not exist "build" mkdir build
if not exist "bin" mkdir bin

REM Compilar
echo [INFO] Compilando VoxScreen Pro...
echo.

cl /O2 /MT /EHsc /W3 /nologo ^
   /I"include" ^
   /I"src\imgui" ^
   /I"src\minhook" ^
   /I"src\minhook\hde" ^
   /I"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" ^
   /D "_CRT_SECURE_NO_WARNINGS" ^
   /D "IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS" ^
   src\main.cpp ^
   src\imgui\imgui.cpp ^
   src\imgui\imgui_draw.cpp ^
   src\imgui\imgui_widgets.cpp ^
   src\imgui\imgui_tables.cpp ^
   src\imgui\imgui_impl_win32.cpp ^
   src\imgui\imgui_impl_dx9.cpp ^
   /link ^
   /LIBPATH:"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86" ^
   d3d9.lib d3dx9.lib legacy_stdio_definitions.lib ^
   /DLL /OUT:bin\VoxScreen.asi

if %errorlevel% equ 0 (
    echo.
    echo [SUCESSO] Compilacao concluida!
    echo Arquivo gerado: bin\VoxScreen.asi
    echo.
    echo Para instalar, copie VoxScreen.asi para:
    echo GTA San Andreas\modloader\VoxScreen Pro\
) else (
    echo.
    echo [ERRO] Falha na compilacao!
)

pause
