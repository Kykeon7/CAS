# CAS — Cellular Automaton Simulator

Simulador de autómata celular desarrollado en C++23 utilizando SDL3 e ImGui.  
El proyecto utiliza CMake y vcpkg para garantizar portabilidad y reproducibilidad en Windows y Linux.

---

## ✨ Características

- Autómata celular hexagonal
- Renderizado con SDL3
- Interfaz gráfica con ImGui
- Arquitectura modular (core / automaton / event)
- Sistema de build multiplataforma con CMake
- Gestión de dependencias con vcpkg

---

## 📦 Requisitos

- Git
- CMake 3.21+
- Compilador compatible con C++23
  - MSVC (Windows)
  - GCC 12+ (Linux)
  - Clang 15+
- Ninja (recomendado)
- vcpkg

---

# 🪟 Windows (VSCode o Terminal)

## 1️⃣ Clonar el repositorio

```bash
git clone https://github.com/TU_USUARIO/CAS.git
cd CAS
git checkout cmake-migration
```

---

## 2️⃣ Instalar vcpkg (si no está instalado)

```powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Opcional: configurar variable de entorno

```powershell
setx VCPKG_ROOT "C:\ruta\a\vcpkg"
```

---

## 3️⃣ Configurar el proyecto

```bash
cmake -B build -S . ^
  -DCMAKE_TOOLCHAIN_FILE=C:/ruta/a/vcpkg/scripts/buildsystems/vcpkg.cmake ^
  -G Ninja
```

---

## 4️⃣ Compilar

```bash
cmake --build build
```

---

## 5️⃣ Ejecutar

```bash
./build/CAS.exe
```

---

# 🐧 Linux (Ubuntu ejemplo)

## 1️⃣ Instalar dependencias base

```bash
sudo apt install build-essential ninja-build git curl zip unzip tar
```

---

## 2️⃣ Instalar vcpkg

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
export VCPKG_ROOT=$HOME/vcpkg
```

---

## 3️⃣ Clonar el proyecto

```bash
git clone https://github.com/TU_USUARIO/CAS.git
cd CAS
git checkout cmake-migration
```

---

## 4️⃣ Configurar

```bash
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
  -G Ninja
```

---

## 5️⃣ Compilar

```bash
cmake --build build
```

---

## 6️⃣ Ejecutar

```bash
./build/CAS
```

---

# 🏗 Estructura del proyecto

```
CAS/
├── CMakeLists.txt
├── vcpkg.json
├── include/
│   ├── automaton/
│   ├── core/
│   └── event/
└── src/
```

---

# 🔀 Ramas

- `main` → Versión original basada en Visual Studio (.sln)
- `cmake-migration` → Versión multiplataforma basada en CMake

---

# 🛠 Desarrollo

El proyecto utiliza:

- SDL3
- ImGui
- CMake (modo moderno basado en targets)
- vcpkg en modo manifest

El build es completamente reproducible en cualquier sistema compatible.

---

# 📜 Licencia

Proyecto académico desarrollado como trabajo de titulación.
