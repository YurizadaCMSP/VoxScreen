# Dependencias do VoxScreen Pro

Para compilar o VoxScreen Pro, voce precisa das seguintes bibliotecas:

## 1. Dear ImGui
- **URL:** https://github.com/ocornut/imgui
- **Versao:** v1.89+ (branch docking recomendada)
- **Arquivos necessarios:**
  - `imgui.cpp`
  - `imgui_draw.cpp`
  - `imgui_widgets.cpp`
  - `imgui_tables.cpp`
  - `imgui.h`
  - `imgui_internal.h`
  - `imconfig.h`
  - `imgui_impl_win32.cpp/.h`
  - `imgui_impl_dx9.cpp/.h`

**Instalacao:**
```bash
cd src
git clone https://github.com/ocornut/imgui.git
```

## 2. MinHook
- **URL:** https://github.com/TsudaKageyu/minhook
- **Versao:** v1.3.3+
- **Arquivos necessarios:** Todos os arquivos fonte da pasta `src/`

**Instalacao:**
```bash
cd src
git clone https://github.com/TsudaKageyu/minhook.git
```

## 3. nlohmann/json (header-only)
- **URL:** https://github.com/nlohmann/json
- **Arquivo:** `json.hpp` (single include)

**Instalacao:**
```bash
cd include
curl -L -o json.hpp https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
```

## 4. stb_image_write (header-only)
- **URL:** https://github.com/nothings/stb
- **Arquivo:** `stb_image_write.h`

**Instalacao:**
```bash
cd include
curl -L -o stb_image_write.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
```

## 5. DirectX SDK
- **Download:** https://www.microsoft.com/en-us/download/details.aspx?id=6812
- **Versao:** June 2010
- **Instalacao padrao:** `C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)`

---

## Estrutura Final Esperada

```
VoxScreen_Pro/
├── src/
│   ├── main.cpp
│   ├── imgui/           <- Dear ImGui
│   └── minhook/         <- MinHook
├── include/
│   ├── json.hpp         <- nlohmann/json
│   └── stb_image_write.h
├── build.bat
├── CMakeLists.txt
└── README.md
```
