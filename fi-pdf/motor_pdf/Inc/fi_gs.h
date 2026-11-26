#ifndef FI_GS_H
#define FI_GS_H

#include <stdbool.h>

/**
 * Comprime un archivo PDF utilizando la API nativa de Ghostscript (libgs).
 *
 * @param archivo_in  Ruta del archivo PDF original ya validado.
 * @param archivo_out Ruta del archivo PDF comprimido de salida.
 * @param nivel       Carácter de perfil: 'm' (150 DPI, /ebook) o 'h' (72 DPI, /screen).
 * @return            true si la compresión finalizó correctamente, false en error.
 */
bool comprimir_pdf_nativo(const char *archivo_in, const char *archivo_out, char nivel);

#endif // FI_GS_H
