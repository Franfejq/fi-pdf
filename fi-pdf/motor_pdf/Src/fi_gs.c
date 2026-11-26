/* Habilita definiciones POSIX (PATH_MAX) bajo el estándar estricto C17 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "fi_gs.h"

#include <limits.h>
#include <stdio.h>
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>
#include <qpdf/qpdf-c.h>

/**
 * Verifica el código de retorno y el estado interno de error de qpdf.
 * Reporta el mensaje descriptivo en stderr si ocurrió un error.
 */
bool verificar_estado_qpdf(qpdf_data pdf, QPDF_ERROR_CODE estado, const char *etapa)
{
    if (pdf == NULL)
    {
        fprintf(stderr, "[ERROR] Contexto libqpdf nulo en (%s).\n", etapa);
        return false;
    }

    if ((estado & QPDF_ERRORS) == 0 && !qpdf_has_error(pdf))
    {
        return true;
    }

    if (qpdf_has_error(pdf))
    {
        qpdf_error err = qpdf_get_error(pdf);
        fprintf(stderr, "[ERROR] Fallo en libqpdf (%s): %s\n",
                etapa, qpdf_get_error_full_text(pdf, err));
    }
    else
    {
        fprintf(stderr, "[ERROR] Fallo en libqpdf (%s) con código %d.\n", etapa, estado);
    }

    return false;
}

/**
 * Optimiza la estructura del PDF, comprime sus streams y lo lineariza usando libqpdf.
 */
static bool comprimir_pdf_qpdf(const char *archivo_in, const char *archivo_out)
{
    qpdf_data pdf = qpdf_init();
    if (pdf == NULL)
    {
        fprintf(stderr, "[ERROR] Memoria insuficiente para inicializar libqpdf.\n");
        return false;
    }

    bool exito = false;

    if (!verificar_estado_qpdf(pdf, qpdf_read(pdf, archivo_in, NULL), "lectura"))
    {
        goto limpiar;
    }

    if (!verificar_estado_qpdf(pdf, qpdf_init_write(pdf, archivo_out), "inicialización de escritura"))
    {
        goto limpiar;
    }

    /* Configurar compresión de streams, agrupación en object streams y linearización web */
    qpdf_set_compress_streams(pdf, QPDF_TRUE);
    qpdf_set_object_stream_mode(pdf, qpdf_o_generate);
    qpdf_set_linearization(pdf, QPDF_TRUE);

    if (!verificar_estado_qpdf(pdf, qpdf_write(pdf), "escritura"))
    {
        goto limpiar;
    }

    exito = true;

limpiar:
    qpdf_cleanup(&pdf);
    return exito;
}

/**
 * Comprime el PDF mediante rasterización y downsampling con Ghostscript (libgs).
 */
static bool comprimir_pdf_ghostscript(const char *archivo_in, const char *archivo_out, char nivel)
{
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

bool comprimir_pdf_nativo(const char *archivo_in, const char *archivo_out, char nivel)
{
    if (archivo_in == NULL || archivo_out == NULL)
    {
        fprintf(stderr, "[ERROR] Rutas inválidas para el motor de compresión.\n");
        return false;
    }

    if (nivel == 'l')
    {
        return comprimir_pdf_qpdf(archivo_in, archivo_out);
    }

    if (nivel == 'm' || nivel == 'h')
    {
        return comprimir_pdf_ghostscript(archivo_in, archivo_out, nivel);
    }

    fprintf(stderr, "[ERROR] Nivel de compresión no reconocido: '%c'. Use -l, -m o -h.\n", nivel);
    return false;
}
