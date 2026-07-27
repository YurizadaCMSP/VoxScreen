# VoxScreen Pro v2.0
## Sistema Avancado de Captura de Tela para GTA:SA / SAMP

**Elaborado por VoxBrasil - Yuri Nogueira**

---

### O que e o VoxScreen Pro?

O **VoxScreen Pro** e o sistema de captura de tela mais avancado e tecnologico para GTA San Andreas e SAMP. Desenvolvido com **DirectX 9 Hook**, **ImGui** e tecnologias modernas de processamento de imagem, ele oferece controle total sobre suas screenshots sem prejudicar sua experiencia de jogo.

---

### Recursos Inovadores

| Recurso | Descricao |
|---------|-----------|
| **Interface ImGui** | Menu moderno, responsivo e intuitivo com design cyber/tecnologico |
| **Multi-Formatos** | Suporte a PNG, JPEG, BMP, TGA e WEBP |
| **Qualidade Adaptativa** | 4 niveis de qualidade (Baixa/Media/Alta/Ultra) que se ajustam ao seu hardware |
| **Tecla Personalizavel** | Escolha qualquer tecla de funcao para captura rapida |
| **Notificacoes** | Alertas flutuantes elegantes sem interromper o gameplay |
| **Timestamp Automatico** | Nomeacao inteligente com data e hora |
| **Efeito Sonoro** | Feedback auditivo ao capturar |
| **Zero Lag** | Otimizado para nao impactar o FPS |

---

### Instalacao (ModLoader)

1. **Baixe e instale o ModLoader** (se ainda nao tiver):
   - [ModLoader para GTA:SA](https://gtaforums.com/topic/669520-mod-loader/)

2. **Copie o arquivo `VoxScreen.asi`** para a pasta do ModLoader:
   ```
   GTA San Andreas/modloader/VoxScreen Pro/VoxScreen.asi
   ```

3. **Inicie o jogo** e pressione **CTRL + 0** para abrir o menu.

---

### Controles

| Acao | Tecla |
|------|-------|
| Abrir/Fechar Menu | **CTRL + 0** |
| Captura Rapida | **F12** (padrao, configuravel) |
| Navegar no Menu | Mouse |

---

### Compilacao (Para Desenvolvedores)

#### Requisitos
- Visual Studio 2019 ou superior
- DirectX SDK (Junho 2010)
- CMake 3.16+

#### Dependencias
- [Dear ImGui](https://github.com/ocornut/imgui)
- [MinHook](https://github.com/TsudaKageyu/minhook)
- [nlohmann/json](https://github.com/nlohmann/json)
- [stb_image_write](https://github.com/nothings/stb)

#### Passos
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

---

### Estrutura de Arquivos

```
GTA San Andreas/
├── modloader/
│   └── VoxScreen Pro/
│       ├── VoxScreen.asi          <- Plugin principal
│       └── VoxScreen_Config.json  <- Configuracoes salvas
└── VoxScreen_Captures/            <- Pasta padrao de screenshots
```

---

### Niveis de Qualidade

| Nivel | Resolucao | Impacto FPS | Uso Recomendado |
|-------|-----------|-------------|-----------------|
| **Baixa** | 25% da tela | Minimo | PCs modestos, notebooks |
| **Media** | 50% da tela | Leve | Balanceado |
| **Alta** | 100% nativa | Moderado | PCs intermediarios |
| **Ultra** | 100% + processamento | Variavel | PCs potentes, conteudo |

---

### Solucao de Problemas

**O menu nao abre?**
- Certifique-se de que o ModLoader esta instalado corretamente
- Verifique se o arquivo `.asi` esta na pasta do modloader
- Tente executar como administrador

**As capturas nao aparecem?**
- Verifique a pasta de destino nas configuracoes
- Certifique-se de ter permissao de escrita na pasta

**Queda de FPS?**
- Reduza a qualidade para "Baixa" ou "Media"
- Desative o efeito sonoro

---

### Creditos

**Elaborado por VoxBrasil - Yuri Nogueira**

Tecnologias utilizadas:
- Dear ImGui by Omar Cornut
- MinHook by Tsuda Kageyu
- stb libraries by Sean Barrett
- nlohmann/json by Niels Lohmann

---

### Licenca

Este projeto e gratuito para uso pessoal. A redistribuicao comercial e proibida sem autorizacao.

**VoxBrasil - Inovando no universo GTA desde 2024**
