#ifndef FI_GS_H
#define FI_GS_H

#include <stdbool.h>

/**
 * Comprime un archivo PDF utilizando el motor nativo correspondiente al nivel:
 * - 'l': optimización estructural y compresión de streams sin pérdida vía libqpdf.
 * - 'm': rasterización a 150 DPI (/ebook) vía Ghostscript (libgs).
 * - 'h': rasterización a 72 DPI (/screen) vía Ghostscript (libgs).
 *
 * @param archivo_in  Ruta del archivo PDF original ya validado.
 * @param archivo_out Ruta del archivo PDF comprimido de salida.
 * @param nivel       Carácter de perfil: 'l' (bajo), 'm' (medio) o 'h' (alto).
 * @return            true si la compresión finalizó correctamente, false en error.
 */
bool comprimir_pdf_nativo(const char *archivo_in, const char *archivo_out, char nivel);

#endif // FI_GS_H
