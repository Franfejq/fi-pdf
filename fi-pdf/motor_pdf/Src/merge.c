#include "merge.h"
#include "fi_gs.h"

#include <stdio.h>

bool fusionar_pdfs_nativo(const char *const *archivos_in, size_t num_archivos, const char *archivo_out)
{
    if (archivos_in == NULL || num_archivos < 2 || archivo_out == NULL)
    {
        fprintf(stderr, "[ERROR] Parámetros inválidos para la fusión de PDFs.\n");
        return false;
    }

    bool exito = false;
    qpdf_data dst = qpdf_init();
    qpdf_data src = NULL;

    if (dst == NULL)
    {
        fprintf(stderr, "[ERROR] Memoria insuficiente para inicializar libqpdf (destino).\n");
        return false;
    }

    QPDF_ERROR_CODE st = qpdf_empty_pdf(dst);
    if (!verificar_estado_qpdf(dst, st, "qpdf_empty_pdf"))
    {
        goto limpiar;
    }

    for (size_t i = 0; i < num_archivos; i++)
    {
        if (archivos_in[i] == NULL)
        {
            fprintf(stderr, "[ERROR] Ruta nula en el archivo de entrada #%zu.\n", i + 1);
            goto limpiar;
        }

        src = qpdf_init();
        if (src == NULL)
        {
            fprintf(stderr, "[ERROR] Memoria insuficiente para inicializar libqpdf (origen %s).\n",
                    archivos_in[i]);
            goto limpiar;
        }

        st = qpdf_read(src, archivos_in[i], NULL);
        if (!verificar_estado_qpdf(src, st, "qpdf_read"))
        {
            goto limpiar;
        }

        /* Resolvemos atributos heredados antes de copiar páginas entre documentos */
        st = qpdf_push_inherited_attributes_to_page(src);
        if (!verificar_estado_qpdf(src, st, "qpdf_push_inherited_attributes_to_page"))
        {
            goto limpiar;
        }

        int n_pages = qpdf_get_num_pages(src);
        if (n_pages < 0)
        {
            verificar_estado_qpdf(src, QPDF_ERRORS, "qpdf_get_num_pages");
            goto limpiar;
        }

        for (int p = 0; p < n_pages; p++)
        {
            qpdf_oh page = qpdf_get_page_n(src, (size_t)p);
            st = qpdf_add_page(dst, src, page, QPDF_FALSE);
            if (!verificar_estado_qpdf(dst, st, "qpdf_add_page"))
            {
                goto limpiar;
            }
        }

        /* Liberamos el documento fuente inmediatamente para acotar el uso de memoria */
        qpdf_cleanup(&src);
    }

    st = qpdf_init_write(dst, archivo_out);
    if (!verificar_estado_qpdf(dst, st, "qpdf_init_write"))
    {
        goto limpiar;
    }

    qpdf_set_compress_streams(dst, QPDF_TRUE);

    st = qpdf_write(dst);
    if (!verificar_estado_qpdf(dst, st, "qpdf_write"))
    {
        goto limpiar;
    }

    exito = true;

limpiar:
    if (src != NULL)
    {
        qpdf_cleanup(&src);
    }
    if (dst != NULL)
    {
        qpdf_cleanup(&dst);
    }
    return exito;
}
