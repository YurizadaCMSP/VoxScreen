@echo off
chcp 65001 >nul
title VoxScreen Pro - Download de Dependencias
color 0E
cls

echo.
echo  ============================================
echo   VOXSCREEN PRO - DOWNLOAD DE DEPENDENCIAS
echo  ============================================
echo.

where curl >nul 2>nul
if %errorlevel% neq 0 (
    echo  [ERRO] curl nao encontrado no sistema!
    echo  O Windows 10 ja deve ter curl. Verifique seu PATH.
    pause
    exit /b 1
)

:: Criar pastas
mkdir src\imgui 2>nul
mkdir src\minhook\hde 2>nul
mkdir include 2>nul

echo  [1/4] Baixando Dear ImGui...
curl -# -L -o src\imgui\imgui.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp
curl -# -L -o src\imgui\imgui_draw.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp
curl -# -L -o src\imgui\imgui_widgets.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_widgets.cpp
curl -# -L -o src\imgui\imgui_tables.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_tables.cpp
curl -# -L -o src\imgui\imgui.h https://raw.githubusercontent.com/ocornut/imgui/master/imgui.h
curl -# -L -o src\imgui\imgui_internal.h https://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h
curl -# -L -o src\imgui\imconfig.h https://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h
curl -# -L -o src\imgui\imgui_impl_win32.cpp https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_win32.cpp
curl -# -L -o src\imgui\imgui_impl_win32.h https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_win32.h
curl -# -L -o src\imgui\imgui_impl_dx9.cpp https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_dx9.cpp
curl -# -L -o src\imgui\imgui_impl_dx9.h https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_dx9.h

echo  [2/4] Baixando MinHook...
curl -# -L -o minhook.zip https://github.com/TsudaKageyu/minhook/archive/refs/heads/master.zip

where tar >nul 2>nul
if %errorlevel% equ 0 (
    tar -xf minhook.zip 2>nul
) else (
    powershell -Command "Expand-Archive -Path 'minhook.zip' -DestinationPath '.' -Force" 2>nul
)

if exist "minhook-master" (
    xcopy /E /I /Y "minhook-master\src\*" "src\minhook\" >nul
    rmdir /S /Q "minhook-master" 2>nul
    del minhook.zip 2>nul
)

echo  [3/4] Baixando nlohmann/json...
curl -# -L -o include\json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp

echo  [4/4] Baixando stb_image_write...
curl -# -L -o include\stb_image_write.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

echo.
echo  [OK] Todas as dependencias foram baixadas com sucesso!
echo.
echo  Agora compile com:
echo    compile_terminal.bat --skip-deps
echo.
pause
