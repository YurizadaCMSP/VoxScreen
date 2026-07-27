cmake_minimum_required(VERSION 3.16)
project(VoxScreenPro VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuracao do ASI Plugin
set(TARGET_NAME VoxScreen)

# Arquivos fonte
set(SOURCES
    src/main.cpp
    src/imgui/imgui.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_impl_win32.cpp
    src/imgui/imgui_impl_dx9.cpp
    src/minhook/MinHook.c
    src/minhook/hook.c
    src/minhook/buffer.c
    src/minhook/trampoline.c
    src/minhook/hde/hde32.c
    src/minhook/hde/hde64.c
)

# Diretorios de include
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/minhook
    ${CMAKE_CURRENT_SOURCE_DIR}/src/minhook/hde
    "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"
)

# Link directories
link_directories(
    "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x86"
)

# Criar DLL ASI
add_library(${TARGET_NAME} SHARED ${SOURCES})

# Definicoes
 target_compile_definitions(${TARGET_NAME} PRIVATE
    IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
    _CRT_SECURE_NO_WARNINGS
)

# Linkar bibliotecas
target_link_libraries(${TARGET_NAME} PRIVATE
    d3d9
    d3dx9
    legacy_stdio_definitions
)

# Configuracoes de compilacao
set_target_properties(${TARGET_NAME} PROPERTIES
    SUFFIX ".asi"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# Otimizacoes para Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${TARGET_NAME} PRIVATE /O2 /MT)
endif()
