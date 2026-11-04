#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

// Cabeceras nativas de Ghostscript
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>

/**
 * Comprime un archivo PDF usando la API nativa de Ghostscript.
 * @param archivo_in  Ruta del archivo original ya validado.
 * @param archivo_out Ruta de destino generada previamente.
 * @param nivel       Carácter 'm' (150 DPI) o 'h' (72 DPI).
 * @return            true si tuvo éxito, false si hubo error.
 */
bool comprimir_pdf_nativo(const char *archivo_in, const char *archivo_out, char nivel) 
{
    char arg_salida[1024];
    
    // 1. Armar el string del archivo de salida
    if (snprintf(arg_salida, sizeof(arg_salida), "-sOutputFile=%s", archivo_out) >= sizeof(arg_salida)) 
    {
        fprintf(stderr, "[ERROR] Ruta demasiado larga.\n");
        return false;
    }

    // 2. Definir el nivel de rasterización basado en la bandera de tu CLI
    // /ebook  = 150 DPI (Equilibrio)
    // /screen = 72 DPI  (Destrucción gráfica máxima)
    const char *ajuste_calidad = (nivel == 'm') ? "-dPDFSETTINGS=/ebook" : "-dPDFSETTINGS=/screen";

    // 3. Matriz de configuración del intérprete
    const char *gs_argv[] = {
        "fi-pdf-engine",
        "-sDEVICE=pdfwrite",
        "-dCompatibilityLevel=1.4",
        ajuste_calidad,         // Inyectamos la calidad dinámica
        "-dNOPAUSE",
        "-dQUIET",
        "-dBATCH",
        arg_salida,             // Inyectamos el archivo de destino
        archivo_in              // Archivo original
    };
    
    int gs_argc = sizeof(gs_argv) / sizeof(gs_argv[0]);

    // 4. Ciclo de vida de la memoria nativa
    void *minst = NULL;
    if (gsapi_new_instance(&minst, NULL) < 0) 
    {
        fprintf(stderr, "[ERROR] Memoria insuficiente para inicializar libgs.\n");
        return false;
    }

    // Ejecución bloqueante del motor
    int code = gsapi_init_with_args(minst, gs_argc, (char **)gs_argv);

    // gs_error_Quit es el estado natural de salida exitosa cuando se usa -dBATCH
    bool exito = (code == 0 || code == gs_error_Quit);

    // Limpieza estricta de memoria (Heap)
    gsapi_exit(minst);
    gsapi_delete_instance(minst);

    return exito;
}

// Ejemplo de cómo lo llamará tu CLI principal
int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "Uso: ./fi-pdf -m <entrada.pdf> <salida.pdf>\n");
        return EXIT_FAILURE;
    }

    // Simulando que parseaste la bandera desde argv[1]
    char bandera = argv[1][1]; // Toma la 'm' o la 'h' de "-m" / "-h"
    const char *archivo_entrada = argv[2];
    const char *archivo_salida = argv[3];

    printf("[INFO] Procesando compresión nivel '%c'...\n", bandera);

    if (comprimir_pdf_nativo(archivo_entrada, archivo_salida, bandera)) 
    {
        printf("[ÉXITO] Archivo comprimido correctamente sin depender de la terminal.\n");
    } 
    else 
    {
        fprintf(stderr, "[ERROR] El motor de compresión interno falló.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
