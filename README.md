# fi-pdf

Utilidad de línea de comandos (CLI) ligera y de alto rendimiento escrita en **C17** para la compresión, validación y manipulación estructural de documentos PDF en sistemas GNU/Linux.

Desarrollado como proyecto de ingeniería bajo una filosofía de bajo nivel: **cero *wrappers* de terminal (`system()`)**, gestión determinista de memoria en el *Stack* y enlace directo con APIs nativas en C/C++.

---

## Arquitectura y Decisiones de Diseño

A diferencia de herramientas que invocan subprocesos externos en *bash*, `fi-pdf` controla directamente el ciclo de vida del motor de renderizado en memoria:

* **Motor Desacoplado (`libmotor_pdf.so`):** La lógica de procesamiento de archivos y manipulación de PDF está encapsulada en una biblioteca compartida independiente de la interfaz CLI (`main.c`).
* **Integración Nativa con Ghostscript (`libgs`):** Utiliza `<ghostscript/iapi.h>` para instanciar la máquina virtual de PostScript/PDF directamente en el espacio de memoria del proceso, ejecutando *downsampling* matricial real sobre imágenes incrustadas.
* **Seguridad de Memoria Determinista:** Se evita la asignación dinámica (`malloc`/`free`) para el manejo de rutas y nombres de archivos. Toda la manipulación de cadenas opera mediante *buffers* estáticos acotados por `PATH_MAX` (4096 bytes) pasados por referencia.
* **Estándar Estricto:** Compilado bajo `-std=c17` con extensiones desactivadas (`CMAKE_C_EXTENSIONS OFF`) y advertencias estrictas (`-Wall -Wextra -pedantic`).

---

## Estructura del Repositorio

```text
fi-pdf/
├── CMakeLists.txt          # Configuración principal del ejecutable (fi-pdf)
├── main.c                  # Punto de entrada, parseo de argumentos y flujo CLI
└── motor_pdf/              # Núcleo lógico compilado como biblioteca compartida
    ├── CMakeLists.txt      # Configuración de enlazado con libgs
    ├── Inc/
    │   └── fi_pdf.h        # Cabeceras públicas, macros POSIX y prototipos
    └── Src/
        └── fi_pdf.c        # Validación, generación de nombres e interfaz libgs


```
## Requisitos y Dependencias 
Para compilar y probar el proeyecto necesitas:
* **Compilador de C:** GCC O Clang con soporte para **C17**.
* **Sistema de construcción:** CMake **3.20** o superior.
* **Biblioteca Ghostscript:** Esto nos descargara la biblioteca compartida (`libgs.so`) y los headers (`iapi.h` , `ierrors.h`).

### Instalación de dependencias.
#### Arch / Arch - Based:

```bash
sudo pacman -S base-devel cmake ghostscript
```

#### Debian / Debian - Based:

```bash
sudo apt update && sudo apt install build-essential cmake libgs-dev
```

#### Fedora / RHEL:

```bash
sudo dnf install gcc cmake ghostscript-devel
```

## Compilación(Build)
Tienes que clonar el repositorio y construye el binario fuera de arbol de fuentes(`out-of-source build`):

```bash
git clone https://github.com/Franfejq/fi-pdf.git
cd fi-pdf
mkdir Build && cd Build
cmake ..
cmake --build .
```
Esto generara el ejecutable `fi-pdf` dentro de `Build/` y la biblioteca `Build/motor_pdf/libmotor_pdf.so`

## Uso y Pruebas
#### Sintaxis General

```bash
./fi-pdf <comando> <nivel> <ruta/al/archivo.pdf>
```
## Comandos disponibles
1. Compresión **Media** (-m): Reduce la resolución de imágenes a **150 DPI** (-dPDFSETTINGS=/ebook), 
este es el mas equilibrado entre calidad y peso.

```bash
./fi-pdf compress -m archivo.pdf
```

2. Compresión **Alta/Extrema** (-h): Reduce la resolución de imagen a **72 DPI** (-dPDFSETTINGS=/screen),
para máxima reduccion de tamaño.
peso.

```bash
./fi-pdf compress -h archivo.pdf
```

El archivo resultante se guardará automáticamente en el mismo directorio que el original con el sufijo _compress.pdf (ej. archivo_compress.pdf).
