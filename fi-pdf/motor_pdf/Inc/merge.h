#ifndef PROYECTO_INTEGRADOR_MERGE_H
#define PROYECTO_INTEGRADOR_MERGE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Fusiona secuencialmente múltiples archivos PDF en un único documento de salida
 * utilizando la API nativa en C de libqpdf.
 *
 * @param archivos_in  Arreglo de rutas de los archivos PDF de entrada ya validados.
 * @param num_archivos Cantidad de archivos de entrada (mínimo 2).
 * @param archivo_out  Ruta del archivo PDF resultante.
 * @return             true si la fusión finalizó correctamente, false en error.
 */
bool fusionar_pdfs_nativo(const char *const *archivos_in, size_t num_archivos, const char *archivo_out);

#endif // PROYECTO_INTEGRADOR_MERGE_H
