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
