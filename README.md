# CAS — Cellular Automaton Simulator

Simulador de autómata celular desarrollado en C++23 utilizando SDL3, Dear ImGui e ImPlot.

El proyecto utiliza CMake y vcpkg para gestionar la compilación y las dependencias externas, buscando ofrecer portabilidad y reproducibilidad en Windows y Linux.

---

## ✨ Características

- Autómata celular hexagonal.
- Renderizado mediante SDL3 GPU.
- Interfaz gráfica con Dear ImGui.
- Visualización de datos y gráficas mediante ImPlot.
- Arquitectura modular.
- Sistema de compilación multiplataforma con CMake.
- Gestión de dependencias externas mediante vcpkg.

---

## 📦 Requisitos

- Git
- CMake 3.21+
- Compilador compatible con C++23:
  - MSVC (Windows)
  - GCC 12+ (Linux)
  - Clang 15+
- Ninja (recomendado).
- vcpkg.

### Dependencias

| Dependencia | Gestión |
|---|---|
| SDL3 | vcpkg |
| nlohmann_json | vcpkg |
| Dear ImGui | Incluida en `external/imgui` |
| ImPlot | Incluida en `external/implot` |

Dear ImGui e ImPlot se compilan directamente desde el código fuente incluido en el repositorio. No es necesario instalarlos mediante vcpkg.

---

# 🪟 Windows (VS Code o terminal)

## 1. Clonar el repositorio

```bash
git clone https://github.com/TU_USUARIO/CAS.git
cd CAS
git checkout cmake-migration
```

## 2. Instalar vcpkg

Si todavía no está instalado:

```powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Configurar la variable de entorno `VCPKG_ROOT`:

```powershell
setx VCPKG_ROOT "C:\ruta\a\vcpkg"
```

Después de configurar la variable, abrir una terminal nueva.

## 3. Configurar el proyecto

```powershell
cmake -B build -S . -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
  -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## 4. Compilar

```powershell
cmake --build build
```

## 5. Ejecutar

```powershell
.\build\CAS.exe
```

---

# 🐧 Linux (Ubuntu)

## 1. Instalar dependencias base

```bash
sudo apt update
sudo apt install build-essential ninja-build cmake git curl zip unzip tar pkg-config
```

## 2. Instalar vcpkg

```bash
git clone https://github.com/microsoft/vcpkg.git "$HOME/vcpkg"

cd "$HOME/vcpkg"

./bootstrap-vcpkg.sh
```

Configurar la variable de entorno:

```bash
export VCPKG_ROOT="$HOME/vcpkg"
```

Para conservarla entre sesiones, agregarla al archivo de configuración del shell, por ejemplo `~/.bashrc`:

```bash
echo 'export VCPKG_ROOT="$HOME/vcpkg"' >> ~/.bashrc
```

## 3. Clonar el repositorio

```bash
git clone https://github.com/TU_USUARIO/CAS.git
cd CAS
git checkout cmake-migration
```

## 4. Configurar el proyecto

```bash
cmake -B build -S . -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## 5. Compilar

```bash
cmake --build build
```

## 6. Ejecutar

```bash
./build/CAS
```

---

# 🏗 Estructura del proyecto

```text
CAS/
├── CMakeLists.txt
├── vcpkg.json
├── external/
│   ├── imgui/
│   └── implot/
├── include/
└── src/
```

---

# 🔀 Ramas

- `main` → Versión original basada en Visual Studio (.sln).
- `cmake-migration` → Versión multiplataforma basada en CMake.

---

# 🛠 Desarrollo

El proyecto utiliza:

- **C++23** como estándar del lenguaje.
- **SDL3** para la gestión de ventanas, eventos y renderizado GPU.
- **Dear ImGui** para la interfaz gráfica.
- **ImPlot** para la visualización de gráficas.
- **nlohmann_json** para el manejo de datos JSON.
- **CMake** para la configuración y generación del sistema de compilación.
- **vcpkg** para la gestión de dependencias externas.

### Configuraciones de compilación

| Configuración | Optimización | Símbolos de depuración |
|---|---|---|
| Debug | Desactivada | Sí |
| RelWithDebInfo | Activada | Sí |
| Release | Activada | No |

Se recomienda utilizar `RelWithDebInfo` durante el desarrollo para aprovechar las optimizaciones del compilador sin perder los símbolos de depuración.

---

# 📜 Licencia

Proyecto académico desarrollado como trabajo de titulación.