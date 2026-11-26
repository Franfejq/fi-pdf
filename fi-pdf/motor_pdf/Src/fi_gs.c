/* Habilita definiciones POSIX (PATH_MAX) bajo el estándar estricto C17 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "fi_gs.h"

#include <limits.h>
#include <stdio.h>
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>

bool comprimir_pdf_nativo(const char *archivo_in, const char *archivo_out, char nivel)
{
    if (archivo_in == NULL || archivo_out == NULL)
    {
        fprintf(stderr, "[ERROR] Rutas inválidas para el motor de compresión.\n");
        return false;
    }

    char arg_salida[PATH_MAX + 32];

    /* 1. Armar el argumento de archivo de salida para Ghostscript */
    int escritos = snprintf(arg_salida, sizeof(arg_salida), "-sOutputFile=%s", archivo_out);
    if (escritos < 0 || (size_t)escritos >= sizeof(arg_salida))
    {
        fprintf(stderr, "[ERROR] Ruta demasiado larga.\n");
        return false;
    }

    /*
     * 2. Definir el nivel de rasterización basado en la bandera de la CLI:
     * /ebook  = 150 DPI (Equilibrio)
     * /screen = 72 DPI  (Reducción máxima)
     */
    const char *ajuste_calidad = (nivel == 'm') ? "-dPDFSETTINGS=/ebook" : "-dPDFSETTINGS=/screen";

    /* 3. Matriz de configuración del intérprete */
    const char *gs_argv[] = {
        "fi-pdf-engine",
        "-sDEVICE=pdfwrite",
        "-dCompatibilityLevel=1.4",
        ajuste_calidad,
        "-dNOPAUSE",
        "-dQUIET",
        "-dBATCH",
        arg_salida,
        archivo_in
    };

    int gs_argc = (int)(sizeof(gs_argv) / sizeof(gs_argv[0]));

    /* 4. Ciclo de vida de la instancia nativa en memoria */
    void *minst = NULL;
    if (gsapi_new_instance(&minst, NULL) < 0)
    {
        fprintf(stderr, "[ERROR] Memoria insuficiente para inicializar libgs.\n");
        return false;
    }

    /* Ejecución bloqueante del motor */
    int code = gsapi_init_with_args(minst, gs_argc, (char **)gs_argv);

    /* gs_error_Quit es el estado natural de salida exitosa cuando se usa -dBATCH */
    bool exito = (code == 0 || code == gs_error_Quit);

    /* Limpieza estricta de memoria (Heap) validando el cierre del intérprete */
    int exit_code = gsapi_exit(minst);
    if (exit_code < 0 && exit_code != gs_error_Quit)
    {
        fprintf(stderr, "[ERROR] Error al finalizar la instancia de Ghostscript (%d).\n", exit_code);
        exito = false;
    }
    gsapi_delete_instance(minst);

    return exito;
}
