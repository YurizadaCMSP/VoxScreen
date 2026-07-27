# Como Compilar VoxScreen Pro pelo Terminal (Windows 10)

**Elaborado por VoxBrasil - Yuri Nogueira**

---

## Metodo 1: Visual Studio Build Tools (RECOMENDADO)

Este e o metodo mais confiavel para projetos com DirectX 9. E gratuito e funciona no Windows 10 Home/Pro.

### Passo 1: Instalar o Visual Studio Build Tools

1. Baixe o instalador:
   ```
   https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
   ```

2. Execute o instalador e selecione:
   - **C++ build tools**
   - **Windows 10/11 SDK** (a versao mais recente)
   - **MSVC v143 - VS 2022 C++ x64/x86 build tools**

3. Aguarde a instalacao (~2-4 GB)

### Passo 2: Abrir o Terminal de Desenvolvedor

**Opcao A - Menu Iniciar:**
```
Iniciar -> Visual Studio 2022 -> x86 Native Tools Command Prompt
```

**Opcao B - Pelo terminal normal (cmd/PowerShell):**
```batch
"C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86
```
> Se voce instalou a versao Community/Professional em vez de BuildTools, o caminho muda para:
> `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat`

### Passo 3: Baixar as Dependencias

No terminal, navegue ate a pasta do projeto e execute:

```batch
cd C:\Users\SEU_USUARIO\Downloads\VoxScreen_Pro

:: Criar pastas
mkdir src\imgui src\minhook src\minhook\hde include

:: Baixar ImGui
curl -L -o src\imgui\imgui.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp
curl -L -o src\imgui\imgui_draw.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp
curl -L -o src\imgui\imgui_widgets.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_widgets.cpp
curl -L -o src\imgui\imgui_tables.cpp https://raw.githubusercontent.com/ocornut/imgui/master/imgui_tables.cpp
curl -L -o src\imgui\imgui.h https://raw.githubusercontent.com/ocornut/imgui/master/imgui.h
curl -L -o src\imgui\imgui_internal.h https://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h
curl -L -o src\imgui\imconfig.h https://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h
curl -L -o src\imgui\imgui_impl_win32.cpp https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_win32.cpp
curl -L -o src\imgui\imgui_impl_win32.h https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_win32.h
curl -L -o src\imgui\imgui_impl_dx9.cpp https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_dx9.cpp
curl -L -o src\imgui\imgui_impl_dx9.h https://raw.githubusercontent.com/ocornut/imgui/master/backends/imgui_impl_dx9.h

:: Baixar MinHook
curl -L -o minhook.zip https://github.com/TsudaKageyu/minhook/archive/refs/heads/master.zip
tar -xf minhook.zip
xcopy /E /I /Y minhook-master\src src\minhook\
del minhook.zip
rmdir /S /Q minhook-master

:: Baixar nlohmann/json
curl -L -o include\json.hpp https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp

:: Baixar stb_image_write
curl -L -o include\stb_image_write.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
```

### Passo 4: Compilar!

Ainda no **x86 Native Tools Command Prompt**, execute:

```batch
mkdir bin 2>nul

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
   src\minhook\MinHook.c ^
   src\minhook\hook.c ^
   src\minhook\buffer.c ^
   src\minhook\trampoline.c ^
   src\minhook\hde\hde32.c ^
   src\minhook\hde\hde64.c ^
   /link ^
   /LIBPATH:"C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86" ^
   d3d9.lib d3dx9.lib legacy_stdio_definitions.lib ^
   /DLL /OUT:bin\VoxScreen.asi
```

Se compilou com sucesso, o arquivo `bin\VoxScreen.asi` foi gerado!

---

## Metodo 2: Script Automatico (Facil)

Eu preparei o arquivo `compile_terminal.bat`. Basta:

1. Instalar o **Visual Studio Build Tools** (Passo 1 acima)
2. Abrir o **x86 Native Tools Command Prompt**
3. Navegar ate a pasta do projeto
4. Rodar:
   ```batch
   compile_terminal.bat
   ```

O script baixa TODAS as dependencias automaticamente e compila tudo sozinho.

---

## Metodo 3: Usando o Script de Download de Deps

Se voce prefere baixar as dependencias manualmente antes de compilar:

```batch
:: 1. Baixar deps
setup_deps.bat

:: 2. Compilar
compile_terminal.bat --skip-deps
```

---

## Instalacao no ModLoader

Apos compilar:

```batch
mkdir "%USERPROFILE%\Documents\GTA San Andreas\modloader\VoxScreen Pro" 2>nul
copy bin\VoxScreen.asi "%USERPROFILE%\Documents\GTA San Andreas\modloader\VoxScreen Pro\"
```

Ou simplesmente copie o `VoxScreen.asi` da pasta `bin\` para:
```
GTA San Andreas\modloader\VoxScreen Pro\
```

---

## Solucao de Problemas

### "cl nao e reconhecido"
Voce nao esta no **x86 Native Tools Command Prompt**. Feche o terminal e abra o correto.

### "DirectX SDK nao encontrado"
Baixe e instale:
```
https://www.microsoft.com/en-us/download/details.aspx?id=6812
```
Instale no caminho padrao.

### "curl nao e reconhecido"
O Windows 10 ja vem com curl. Se nao tiver, baixe as dependencias manualmente pelos links.

### "tar nao e reconhecido"
Use o PowerShell em vez do cmd:
```powershell
Expand-Archive -Path minhook.zip -DestinationPath . -Force
```

---

**Elaborado por VoxBrasil - Yuri Nogueira**
