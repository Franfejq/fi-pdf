# fi-pdf

Utilidad de línea de comandos (CLI) ligera y de alto rendimiento escrita en **C17** para la compresión, validación y manipulación estructural de documentos PDF en sistemas GNU/Linux.

Desarrollado como proyecto de ingeniería bajo una filosofía de bajo nivel: **cero *wrappers* de terminal (`system()`)**, gestión determinista de memoria en el *Stack* y enlace directo con APIs nativas en C/C++.

---

## Arquitectura y Decisiones de Diseño

A diferencia de herramientas que invocan subprocesos externos en *bash*, `fi-pdf` controla directamente el ciclo de vida del motor de renderizado en memoria:

* **Motor Desacoplado (`libmotor_pdf.so`):** La lógica de procesamiento de archivos y manipulación de PDF está encapsulada en una biblioteca compartida independiente de la interfaz CLI (`main.c`).
* **Integración Nativa con QPDF (`libqpdf`) y Ghostscript (`libgs`):**
  * **Compresión (`fi_gs.h` / `fi_gs.c`):** Despacha según el nivel solicitado:
    * **`libqpdf` (`<qpdf/qpdf-c.h>`, nivel `-l`):** Optimización estructural sin pérdida (*lossless*) mediante compresión de *streams*, agrupación en *object streams* (`qpdf_o_generate`) y linearización para vista web.
    * **`libgs` (`<ghostscript/iapi.h>`, niveles `-m` y `-h`):** Instancia la máquina virtual de PostScript/PDF directamente en el espacio de memoria del proceso, ejecutando *downsampling* matricial real sobre imágenes incrustadas.
  * **Fusión secuencial (`merge.h` / `merge.c`):** Implementada directamente en **C17** sobre la API C de `libqpdf` (`<qpdf/qpdf-c.h>`), creando un documento destino vacío (`qpdf_empty_pdf`), resolviendo atributos heredados (`qpdf_push_inherited_attributes_to_page`), copiando páginas (`qpdf_add_page`) y liberando cada documento origen de inmediato (`qpdf_cleanup`) antes de escribir el resultado con compresión de *streams*.
* **Seguridad de Memoria Determinista y Límites POSIX:** Se evita la asignación dinámica (`malloc`/`free`) para el manejo de rutas y nombres de archivos. Toda la manipulación de cadenas opera mediante *buffers* estáticos acotados por `PATH_MAX` (4096 bytes; `PATH_MAX + 32` en `comprimir_pdf_ghostscript` para el argumento `-sOutputFile=`) pasados por referencia, validando retornos de `snprintf`, `access`, `gsapi_*` y el ciclo completo de `libqpdf` (`qpdf_init`/`qpdf_cleanup`, `QPDF_ERRORS` y `qpdf_has_error`).
* **Estándar Estricto:** El motor `motor_pdf` se compila bajo `-std=c17` con extensiones desactivadas (`CMAKE_C_EXTENSIONS OFF`), advertencias estrictas (`-Wall -Wextra -pedantic`) y `_POSIX_C_SOURCE 200809L` en `fi_pdf.c` y `fi_gs.c` para habilitar de forma portátil `PATH_MAX`, `access` y `F_OK` (con *include guards* `FI_PDF_H`, `FI_GS_H` y `PROYECTO_INTEGRADOR_MERGE_H`).

---

## Estructura del Repositorio

```text
fi-pdf/
├── CMakeLists.txt          # Configuración principal del ejecutable (fi-pdf)
├── main.c                  # Punto de entrada, parseo de argumentos y flujo CLI
└── motor_pdf/              # Núcleo lógico compilado como biblioteca compartida
    ├── CMakeLists.txt      # Configuración de enlazado con libgs y libqpdf (pkg-config)
    ├── Inc/
    │   ├── fi_gs.h         # Interfaz pública de compresión (QPDF / Ghostscript) y verificación QPDF
    │   ├── fi_pdf.h        # Cabeceras públicas, includes POSIX y prototipos base
    │   └── merge.h         # Interfaz pública del módulo de fusión de documentos (QPDF)
    └── Src/
        ├── fi_gs.c         # Implementación nativa con libqpdf (-l) y libgs (-m / -h)
        ├── fi_pdf.c        # Validación de archivos, nombres únicos y ayuda CLI
        └── merge.c         # Fusión secuencial de múltiples PDFs mediante libqpdf
```

> **Nota:** La documentación del proyecto se centraliza en este `README.md` raíz (se eliminó el antiguo `fi-pdf/README.md` vacío).

---

## Requisitos y Dependencias

Para compilar y probar el proyecto necesitas:
* **Compilador de C:** GCC o Clang con soporte para **C17**.
* **Sistema de construcción:** CMake **3.20** o superior y **`pkg-config`**.
* **Biblioteca QPDF:** Proporciona la biblioteca compartida (`libqpdf`) y la cabecera C (`qpdf/qpdf-c.h`).
* **Biblioteca Ghostscript:** Proporciona la biblioteca compartida (`libgs.so`) y las cabeceras (`iapi.h`, `ierrors.h`).

### Instalación de dependencias

#### Arch / Arch-based:
```bash
sudo pacman -S base-devel cmake pkgconf ghostscript qpdf
```

#### Debian / Debian-based:
```bash
sudo apt update && sudo apt install build-essential cmake pkg-config libgs-dev libqpdf-dev
```

#### Fedora / RHEL:
```bash
sudo dnf install gcc cmake pkgconf-pkg-config ghostscript-devel qpdf-devel
```

---

## Compilación (Build)

Clona el repositorio y construye el binario fuera del árbol de fuentes (*out-of-source build*):

```bash
git clone https://github.com/Franfejq/fi-pdf.git
cd fi-pdf/fi-pdf
mkdir Build && cd Build
cmake ..
cmake --build .
```

Esto generará el ejecutable `fi-pdf` dentro de `Build/` y la biblioteca `Build/motor_pdf/libmotor_pdf.so`.

---

## Uso y Pruebas

### Sintaxis General

```bash
./fi-pdf compress <-l|-m|-h> <ruta/al/archivo.pdf>
./fi-pdf merge <doc1.pdf> <doc2.pdf> [doc3.pdf ...]
```

### Comandos disponibles

1. **Compresión (`compress`):**
   - **Baja / Estructural (`-l`):** Optimiza el PDF mediante `libqpdf` (compresión de *streams*, agrupación en *object streams* y linearización web), sin recomprimir ni reducir la resolución de imágenes.
     ```bash
     ./fi-pdf compress -l archivo.pdf
     ```
   - **Media (`-m`):** Reduce la resolución de imágenes a **150 DPI** (`-dPDFSETTINGS=/ebook`) vía `libgs`, logrando un equilibrio entre calidad y peso.
     ```bash
     ./fi-pdf compress -m archivo.pdf
     ```
   - **Alta/Extrema (`-h`):** Reduce la resolución de imágenes a **72 DPI** (`-dPDFSETTINGS=/screen`) vía `libgs`, para máxima reducción de tamaño.
     ```bash
     ./fi-pdf compress -h archivo.pdf
     ```

2. **Fusión de documentos (`merge`):** Une secuencialmente dos o más archivos PDF validados en un único documento utilizando `libqpdf` con compresión de *streams* activada.
   ```bash
   ./fi-pdf merge doc1.pdf doc2.pdf
   ./fi-pdf merge portada.pdf capitulo1.pdf capitulo2.pdf
   ```

### Generación Segura de Nombres de Salida

El archivo resultante se guarda automáticamente en el mismo directorio que el archivo base (o que el primer documento en `merge`) con el sufijo correspondiente (`_compress.pdf` o `_merge.pdf`), mostrando la ruta resuelta en consola (`[INFO] Archivo de salida: ...`):

* **Protección contra sobrescritura:** `dynamic_name()` (para compresión) y `dynamic_merge_name()` (para fusión) comparten el generador interno `generar_nombre_unico()`, que verifica la disponibilidad en disco con `access(..., F_OK)` (`errno == ENOENT`). Si el archivo destino ya existe, incrementa automáticamente a `archivo_compress(1).pdf` / `archivo_merge(1).pdf`, etc., validando límites de `PATH_MAX` y desbordamiento de contador. Si falla la validación o excede `PATH_MAX`, la CLI aborta de forma segura (`EXIT_FAILURE`).
* **Normalización de sufijos previos:** `normalizar_base_sufijo()` detecta y remueve sufijos previos (`_compress` / `_compress(n)` o `_merge` / `_merge(n)`) antes de `.pdf`, evitando encadenamientos como `archivo_compress_compress.pdf` o `doc1_merge_merge.pdf` al reprocesar un documento.

---

## Guía para Testers (Búsqueda de Issues)

Si clonaste el repositorio para ayudar a probar y depurar el motor, intenta romper el programa probando los siguientes escenarios y reporta cualquier comportamiento inesperado:

* **Rutas con espacios o caracteres especiales:**
  ```bash
  ./fi-pdf compress -h "/ruta/con espacios/Práctica 8 Modularidad.pdf"
  ```

* **Validación de integridad (`check_file`):**
  * Pasar un archivo inexistente.
  * Pasar un archivo con extensión distinta a **.pdf** o un binario renombrado (`falso.pdf` sin cabecera `%PDF-`).
  * Pasar un PDF sin permisos de lectura o escritura (`chmod 000 prueba.pdf` / solo lectura).

* **Colisiones y recomposición de nombres (`dynamic_name`):**
  * Comprimir el mismo archivo varias veces consecutivas y verificar la secuencia `archivo_compress.pdf`, `archivo_compress(1).pdf`, `archivo_compress(2).pdf` sin sobrescribir archivos existentes.
  * Volver a comprimir `archivo_compress.pdf` o `archivo_compress(1).pdf` y confirmar que el nombre resultante normaliza la base en lugar de apilar `_compress_compress.pdf`.

* **Límites de longitud de ruta:**
  * Probar rutas anidadas cercanas al límite de **PATH_MAX** (4096 bytes) o nombres de archivo de 255 bytes (`NAME_MAX`) para comprobar el rechazo controlado sin truncamientos.

* **Fugas de memoria (Memory Leaks):**
  Ejecutar el binario bajo **valgrind** para verificar que `qpdf_cleanup()` (nivel `-l` y `merge`) y `gsapi_delete_instance()` (niveles `-m`/`-h`) liberan todo el Heap asignado:
  ```bash
  valgrind --leak-check=full ./fi-pdf compress -l prueba.pdf
  valgrind --leak-check=full ./fi-pdf compress -m prueba.pdf
  valgrind --leak-check=full ./fi-pdf merge doc1.pdf doc2.pdf
  ```

* **Comandos y banderas no soportadas:**
  Probar qué ocurre al pasar argumentos incompletos (por ejemplo, menos de 2 archivos en `merge`), comandos desconocidos o banderas distintas a **`-l` / `-m` / `-h`** (el motor debe rechazarlos explícitamente retornando `EXIT_FAILURE`).

---

## ¿Cómo reportar un issue?

Al abrir un issue en GitHub, por favor incluye:
1. Distribución de Linux y versión del compilador (`gcc --version`).
2. Comando exacto ejecutado.
3. Salida completa de la terminal (mensajes `[INFO]` / `[ERROR]`).
4. Características del PDF de prueba (peso original, si contiene imágenes escaneadas, solo texto, o si está protegido con contraseña).

---

## Estado del proyecto (Roadmap)

- [x] Módulo de validación de existencia, extensión, permisos (`R_OK`/`W_OK`) y cabecera `%PDF-` (`check_file`).
- [x] Generador seguro de nombres de salida sin asignación dinámica (`dynamic_name` y `dynamic_merge_name` sobre `generar_nombre_unico`), con normalización de sufijos previos (`normalizar_base_sufijo`) y validación estricta de `PATH_MAX`.
- [x] Protección contra sobrescritura accidental mediante validación `access(..., F_OK)` e incremento automático `_compress(n).pdf` / `_merge(n).pdf`.
- [x] Compresión gráfica nativa mediante `libgs` (niveles `-m` y `-h`), con manejo seguro de rutas de hasta `PATH_MAX` en `-sOutputFile=`.
- [x] Compresión estructural sin pérdida (`-l`) mediante `libqpdf` (compresión de *streams*, *object streams* y linearización web).
- [x] Módulo de fusión de documentos (`merge`) implementado en C17 puro mediante la API nativa en C de `libqpdf` (`qpdf/qpdf-c.h`).
- [ ] Empaquetado e instalación global en `/usr/local/bin` vía `cmake --install`.

---

## Autores

Proyecto desarrollado en la **Facultad de Ingeniería, UNAM**:
- Francisco Emiliano Juárez Quintero — Arquitectura de software, desarrollo en C17 e integración nativa de motores.
- Isaac Josué Hernández Reyes — Diseño algorítmico y estructura lógica.
- Mario Alberto González Navarro — Análisis matemático y modelado de compresión.
- Ángel Antonio Tinoco Puebla — Aseguramiento de calidad (QA) y pruebas de escritorio.
