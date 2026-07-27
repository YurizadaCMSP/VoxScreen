// ============================================================================
//  VOXSCREEN PRO v2.0
//  Sistema Avancado de Captura de Tela para GTA:SA / SAMP
//  Elaborado por VoxBrasil - Yuri Nogueira
//  Tecnologia: DirectX 9 Hook + ImGui + stb_image_write
// ============================================================================

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>
#include <MinHook.h>
#include <fstream>
#include <string>
#include <vector>
#include <shlobj.h>
#include <commdlg.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <direct.h>
#include <chrono>
#include <shellapi.h>
#include "json.hpp"
#include "stb_image_write.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using json = nlohmann::json;

// ============================================================================
//  CONFIGURACOES GLOBAIS
// ============================================================================
struct VoxConfig {
    std::string savePath = "C:\\VoxScreen_Captures";
    std::string fileName = "VoxCapture";
    std::string fileFormat = "png";     // png, jpg, webp, bmp, tga
    int quality = 3;                      // 0=Baixa, 1=Media, 2=Alta, 3=Ultra
    int screenshotKey = VK_F12;           // Tecla padrao
    bool showNotifications = true;
    bool autoTimestamp = true;
    int jpegQuality = 95;
    bool minimizeOnCapture = false;
    bool soundEffect = true;
} g_Config;

// ============================================================================
//  ESTADOS DO SISTEMA
// ============================================================================
bool g_MenuOpen = false;
bool g_Initialized = false;
bool g_ShowNotification = false;
float g_NotificationTimer = 0.0f;
std::string g_LastCapture = "";
std::string g_StatusMessage = "Pronto para capturar";
float g_StatusTimer = 0.0f;

// DirectX
IDirect3DDevice9* g_pDevice = nullptr;
HWND g_hWnd = nullptr;
WNDPROC g_OrigWndProc = nullptr;
typedef HRESULT(WINAPI* Present_t)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(WINAPI* Reset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
Present_t g_OrigPresent = nullptr;
Reset_t g_OrigReset = nullptr;

// ============================================================================
//  UTILITARIOS
// ============================================================================
std::string GetTimestamp() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

std::string GetDisplayTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

void EnsureDirectory(const std::string& path) {
    _mkdir(path.c_str());
}

void PlayCaptureSound() {
    if (!g_Config.soundEffect) return;
    Beep(1200, 80);
    Beep(1800, 120);
}

void ShowStatus(const std::string& msg) {
    g_StatusMessage = msg;
    g_StatusTimer = 3.0f;
}

void TextCentered(const char* text) {
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text("%s", text);
}

// ============================================================================
//  SISTEMA DE CONFIGURACAO (JSON)
// ============================================================================
std::string GetConfigPath() {
    char path[MAX_PATH];
    GetModuleFileNameA(GetModuleHandle(NULL), path, MAX_PATH);
    std::string str(path);
    size_t pos = str.find_last_of("\\/");
    return str.substr(0, pos) + "\\VoxScreen_Config.json";
}

void LoadConfig() {
    std::ifstream f(GetConfigPath());
    if (!f.is_open()) {
        EnsureDirectory(g_Config.savePath);
        return;
    }
    try {
        json j;
        f >> j;
        g_Config.savePath = j.value("savePath", g_Config.savePath);
        g_Config.fileName = j.value("fileName", g_Config.fileName);
        g_Config.fileFormat = j.value("fileFormat", g_Config.fileFormat);
        g_Config.quality = j.value("quality", g_Config.quality);
        g_Config.screenshotKey = j.value("screenshotKey", g_Config.screenshotKey);
        g_Config.showNotifications = j.value("showNotifications", true);
        g_Config.autoTimestamp = j.value("autoTimestamp", true);
        g_Config.jpegQuality = j.value("jpegQuality", 95);
        g_Config.minimizeOnCapture = j.value("minimizeOnCapture", false);
        g_Config.soundEffect = j.value("soundEffect", true);
    } catch (...) {}
    EnsureDirectory(g_Config.savePath);
}

void SaveConfig() {
    json j;
    j["savePath"] = g_Config.savePath;
    j["fileName"] = g_Config.fileName;
    j["fileFormat"] = g_Config.fileFormat;
    j["quality"] = g_Config.quality;
    j["screenshotKey"] = g_Config.screenshotKey;
    j["showNotifications"] = g_Config.showNotifications;
    j["autoTimestamp"] = g_Config.autoTimestamp;
    j["jpegQuality"] = g_Config.jpegQuality;
    j["minimizeOnCapture"] = g_Config.minimizeOnCapture;
    j["soundEffect"] = g_Config.soundEffect;

    std::ofstream f(GetConfigPath());
    f << j.dump(4);
}

// ============================================================================
//  DIALOGO DE PASTA (Windows API)
// ============================================================================
std::string BrowseFolder() {
    BROWSEINFOA bi;
    char path[MAX_PATH];
    ZeroMemory(&bi, sizeof(bi));
    bi.lpszTitle = "Selecione a pasta para salvar as capturas VoxScreen";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = nullptr;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (pidl != nullptr) {
        SHGetPathFromIDListA(pidl, path);
        CoTaskMemFree(pidl);
        return std::string(path);
    }
    return "";
}

// ============================================================================
//  SISTEMA DE CAPTURA DE TELA
// ============================================================================
bool CaptureScreen() {
    if (!g_pDevice) return false;

    IDirect3DSurface9* pBackBuffer = nullptr;
    IDirect3DSurface9* pDestSurface = nullptr;
    D3DSURFACE_DESC desc;

    HRESULT hr = g_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    if (FAILED(hr) || !pBackBuffer) return false;

    pBackBuffer->GetDesc(&desc);

    int div = 1;
    switch (g_Config.quality) {
        case 0: div = 4; break;
        case 1: div = 2; break;
        case 2: div = 1; break;
        case 3: div = 1; break;
    }

    UINT w = desc.Width / div;
    UINT h = desc.Height / div;

    hr = g_pDevice->CreateOffscreenPlainSurface(
        desc.Width, desc.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pDestSurface, nullptr
    );
    if (FAILED(hr)) { pBackBuffer->Release(); return false; }

    hr = g_pDevice->GetRenderTargetData(pBackBuffer, pDestSurface);
    pBackBuffer->Release();
    if (FAILED(hr)) { pDestSurface->Release(); return false; }

    D3DLOCKED_RECT lockedRect;
    hr = pDestSurface->LockRect(&lockedRect, nullptr, D3DLOCK_READONLY);
    if (FAILED(hr)) { pDestSurface->Release(); return false; }

    std::vector<uint8_t> pixels(w * h * 4);
    uint8_t* src = (uint8_t*)lockedRect.pBits;

    if (div > 1) {
        for (UINT y = 0; y < h; y++) {
            for (UINT x = 0; x < w; x++) {
                UINT srcX = x * div;
                UINT srcY = y * div;
                uint8_t* p = src + srcY * lockedRect.Pitch + srcX * 4;
                uint8_t* d = &pixels[(y * w + x) * 4];
                d[0] = p[2];
                d[1] = p[1];
                d[2] = p[0];
                d[3] = 255;
            }
        }
    } else {
        for (UINT y = 0; y < h; y++) {
            for (UINT x = 0; x < w; x++) {
                uint8_t* p = src + y * lockedRect.Pitch + x * 4;
                uint8_t* d = &pixels[(y * w + x) * 4];
                d[0] = p[2];
                d[1] = p[1];
                d[2] = p[0];
                d[3] = 255;
            }
        }
    }

    pDestSurface->UnlockRect();
    pDestSurface->Release();

    std::string filename = g_Config.fileName;
    if (g_Config.autoTimestamp) {
        filename += "_" + GetTimestamp();
    }
    filename += "." + g_Config.fileFormat;
    std::string fullPath = g_Config.savePath + "\\" + filename;

    bool success = false;
    EnsureDirectory(g_Config.savePath);

    if (g_Config.fileFormat == "png") {
        success = stbi_write_png(fullPath.c_str(), w, h, 4, pixels.data(), w * 4) != 0;
    }
    else if (g_Config.fileFormat == "jpg" || g_Config.fileFormat == "jpeg") {
        success = stbi_write_jpg(fullPath.c_str(), w, h, 4, pixels.data(), w * 4 * g_Config.jpegQuality / 100) != 0;
    }
    else if (g_Config.fileFormat == "bmp") {
        success = stbi_write_bmp(fullPath.c_str(), w, h, 4, pixels.data()) != 0;
    }
    else if (g_Config.fileFormat == "tga") {
        success = stbi_write_tga(fullPath.c_str(), w, h, 4, pixels.data()) != 0;
    }
    else {
        success = stbi_write_png(fullPath.c_str(), w, h, 4, pixels.data(), w * 4) != 0;
    }

    if (success) {
        g_LastCapture = filename;
        g_ShowNotification = true;
        g_NotificationTimer = 2.5f;
        ShowStatus("Captura salva: " + filename);
        PlayCaptureSound();
        if (g_Config.minimizeOnCapture) g_MenuOpen = false;
    } else {
        ShowStatus("Erro ao salvar captura!");
    }

    return success;
}

// ============================================================================
//  INTERFACE IMGUI - MENU VOXSCREEN
// ============================================================================
void DrawVoxMenu() {
    if (!g_MenuOpen) return;

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.96f);
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.75f, 1.00f, 0.60f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.40f, 0.60f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.00f, 0.50f, 0.75f, 0.80f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.65f, 0.95f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.80f, 1.00f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.15f, 0.80f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.22f, 0.90f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.45f, 0.70f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.55f, 0.85f, 0.90f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.85f, 1.00f, 1.00f);

    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowBorderSize = 1.5f;

    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(520, 580), ImGuiCond_FirstUseEver);

    ImGui::Begin("VoxScreen Pro v2.0", &g_MenuOpen, 
        ImGuiWindowFlags_NoCollapse);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.8f, 1.0f, 1.0f));
    TextCentered("CAPTURA DE TELA AVANCADA");
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("TIRAR CAPTURA AGORA", ImVec2(-1, 40))) {
        CaptureScreen();
    }
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("PASTA DE DESTINO");
    ImGui::PopStyleColor();
    ImGui::Separator();

    static char pathBuf[512];
    strncpy_s(pathBuf, g_Config.savePath.c_str(), sizeof(pathBuf));
    ImGui::InputText("##path", pathBuf, sizeof(pathBuf), ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    if (ImGui::Button("Procurar", ImVec2(80, 0))) {
        std::string newPath = BrowseFolder();
        if (!newPath.empty()) {
            g_Config.savePath = newPath;
            EnsureDirectory(g_Config.savePath);
        }
    }

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("NOME DO ARQUIVO");
    ImGui::PopStyleColor();
    ImGui::Separator();

    static char nameBuf[128];
    strncpy_s(nameBuf, g_Config.fileName.c_str(), sizeof(nameBuf));
    if (ImGui::InputText("Prefixo", nameBuf, sizeof(nameBuf))) {
        g_Config.fileName = nameBuf;
    }
    ImGui::Checkbox("Adicionar data/hora automatica", &g_Config.autoTimestamp);

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("FORMATO DO ARQUIVO");
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* formats[] = { "PNG (Sem perdas)", "JPEG (Compactado)", "BMP (Bruto)", "TGA (Alta qualidade)", "WEBP (Moderno)" };
    const char* formatVals[] = { "png", "jpg", "bmp", "tga", "webp" };
    int fmtIdx = 0;
    for (int i = 0; i < 5; i++) if (g_Config.fileFormat == formatVals[i]) fmtIdx = i;

    if (ImGui::Combo("Formato", &fmtIdx, formats, IM_ARRAYSIZE(formats))) {
        g_Config.fileFormat = formatVals[fmtIdx];
    }

    if (g_Config.fileFormat == "jpg") {
        ImGui::SliderInt("Qualidade JPEG", &g_Config.jpegQuality, 50, 100, "%d%%");
    }

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("QUALIDADE DA CAPTURA");
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* qualities[] = { "Baixa (Performance+)", "Media (Balanceado)", "Alta (Qualidade)", "Ultra (Maxima Fidelidade)" };
    ImGui::Combo("Nivel", &g_Config.quality, qualities, IM_ARRAYSIZE(qualities));

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
    switch (g_Config.quality) {
        case 0: ImGui::TextWrapped("Resolucao reduzida a 25%%. Ideal para PCs modestos. Minimo impacto no FPS."); break;
        case 1: ImGui::TextWrapped("Resolucao reduzida a 50%%. Bom equilibrio entre qualidade e performance."); break;
        case 2: ImGui::TextWrapped("Resolucao nativa. Captura exatamente o que voce ve na tela."); break;
        case 3: ImGui::TextWrapped("Resolucao nativa com processamento avancado. Usa recursos maximos do sistema."); break;
    }
    ImGui::PopStyleColor();

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("TECLA DE CAPTURA");
    ImGui::PopStyleColor();
    ImGui::Separator();

    const char* keys[] = { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", 
                           "Print Screen", "Insert", "Home", "End", "Page Up", "Page Down" };
    int keyVals[] = { VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12,
                      VK_SNAPSHOT, VK_INSERT, VK_HOME, VK_END, VK_PRIOR, VK_NEXT };
    int keyIdx = 11;
    for (int i = 0; i < 18; i++) if (g_Config.screenshotKey == keyVals[i]) keyIdx = i;

    if (ImGui::Combo("Tecla Rapida", &keyIdx, keys, IM_ARRAYSIZE(keys))) {
        g_Config.screenshotKey = keyVals[keyIdx];
    }
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Pressione CTRL + 0 para abrir este menu");

    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.7f, 1.0f, 1.0f));
    ImGui::Text("OPCOES ADICIONAIS");
    ImGui::PopStyleColor();
    ImGui::Separator();

    ImGui::Checkbox("Mostrar notificacoes", &g_Config.showNotifications);
    ImGui::Checkbox("Efeito sonoro ao capturar", &g_Config.soundEffect);
    ImGui::Checkbox("Minimizar menu ao capturar", &g_Config.minimizeOnCapture);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Salvar Configuracoes", ImVec2(160, 30))) {
        SaveConfig();
        ShowStatus("Configuracoes salvas com sucesso!");
    }
    ImGui::SameLine();
    if (ImGui::Button("Abrir Pasta", ImVec2(100, 30))) {
        ShellExecuteA(nullptr, "open", g_Config.savePath.c_str(), nullptr, nullptr, SW_SHOW);
    }
    ImGui::SameLine();
    if (ImGui::Button("Fechar", ImVec2(80, 30))) {
        g_MenuOpen = false;
    }

    if (g_StatusTimer > 0.0f) {
        ImGui::Spacing();
        ImVec4 statusColor = g_StatusMessage.find("Erro") != std::string::npos ? 
            ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(0.0f, 0.9f, 0.4f, 1.0f);
        ImGui::TextColored(statusColor, "Status: %s", g_StatusMessage.c_str());
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.5f, 0.7f, 0.8f));
    TextCentered("Elaborado por VoxBrasil - Yuri Nogueira");
    TextCentered("Tecnologia VoxScreen Pro v2.0");
    ImGui::PopStyleColor();

    ImGui::End();
}

// ============================================================================
//  NOTIFICACAO FLUTUANTE
// ============================================================================
void DrawNotification(float deltaTime) {
    if (!g_ShowNotification || !g_Config.showNotifications) return;

    g_NotificationTimer -= deltaTime;
    if (g_NotificationTimer <= 0.0f) {
        g_ShowNotification = false;
        return;
    }

    float alpha = g_NotificationTimer / 2.5f;
    if (alpha > 1.0f) alpha = 1.0f;

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 notifSize(320, 70);
    ImVec2 pos(screenSize.x - notifSize.x - 20, screenSize.y - notifSize.y - 20);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(notifSize, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(alpha * 0.9f);

    ImGui::Begin("##notif", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.85f, 1.0f, alpha));
    ImGui::Text("CAPTURA REALIZADA!");
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, alpha * 0.9f));
    ImGui::Text("Arquivo: %s", g_LastCapture.c_str());
    ImGui::Text("Hora: %s", GetDisplayTime().c_str());
    ImGui::PopStyleColor();

    ImGui::End();
}

// ============================================================================
//  HOOKS DIRECTX
// ============================================================================
HRESULT WINAPI HookedPresent(IDirect3DDevice9* pDevice, const RECT* pSourceRect, 
    const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {

    if (!g_Initialized) {
        g_pDevice = pDevice;
        D3DDEVICE_CREATION_PARAMETERS cp;
        pDevice->GetCreationParameters(&cp);
        g_hWnd = cp.hFocusWindow;

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplWin32_Init(g_hWnd);
        ImGui_ImplDX9_Init(pDevice);

        LoadConfig();
        g_Initialized = true;
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawVoxMenu();

    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    if (g_StatusTimer > 0.0f) g_StatusTimer -= deltaTime;
    DrawNotification(deltaTime);

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    return g_OrigPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT WINAPI HookedReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_OrigReset(pDevice, pPresentationParameters);
    ImGui_ImplDX9_CreateDeviceObjects();
    return hr;
}

// ============================================================================
//  HOOK WNDPROC (INPUT)
// ============================================================================
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_Initialized && g_MenuOpen) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;
    }

    if (msg == WM_KEYDOWN) {
        if (wParam == '0' && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
            g_MenuOpen = !g_MenuOpen;
            return 0;
        }
        if (wParam == g_Config.screenshotKey && !g_MenuOpen) {
            CaptureScreen();
            return 0;
        }
    }

    return CallWindowProc(g_OrigWndProc, hWnd, msg, wParam, lParam);
}

// ============================================================================
//  INSTALACAO DO HOOK (MinHook)
// ============================================================================
void InstallD3D9Hook() {
    HMODULE hD3D9 = GetModuleHandleA("d3d9.dll");
    if (!hD3D9) return;

    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D) return;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = GetForegroundWindow();

    IDirect3DDevice9* pDevice = nullptr;
    pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);

    if (pDevice) {
        void** vTable = *(void***)pDevice;

        MH_Initialize();
        MH_CreateHook(vTable[16], HookedReset, (void**)&g_OrigReset);
        MH_CreateHook(vTable[17], HookedPresent, (void**)&g_OrigPresent);
        MH_EnableHook(MH_ALL_HOOKS);

        pDevice->Release();
    }
    pD3D->Release();

    g_hWnd = FindWindowA("Grand theft auto San Andreas", nullptr);
    if (!g_hWnd) g_hWnd = FindWindowA(nullptr, "GTA:SA:MP");
    if (g_hWnd) {
        g_OrigWndProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
    }
}

// ============================================================================
//  THREAD INICIAL
// ============================================================================
DWORD WINAPI InitThread(LPVOID lpParam) {
    Sleep(3000);
    InstallD3D9Hook();
    return 0;
}

// ============================================================================
//  DLL MAIN
// ============================================================================
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        if (g_Initialized) {
            ImGui_ImplDX9_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
        }
        MH_Uninitialize();
    }
    return TRUE;
}
