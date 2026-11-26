#ifndef FI_PDF_H
#define FI_PDF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> 
#include <errno.h>
#include <limits.h>

bool check_file(const char *path);
bool dynamic_name(const char *origen, char *destino);

/**
 * Genera una ruta de salida única con sufijo "_merge.pdf" o "_merge(n).pdf"
 * a partir del primer documento de entrada, evitando sobrescribir archivos en disco.
 *
 * @param origen  Ruta del primer archivo PDF de entrada.
 * @param destino Buffer de salida con capacidad mínima de PATH_MAX bytes.
 * @return        true si se generó una ruta válida libre, false en error.
 */
bool dynamic_merge_name(const char *origen, char *destino);
int size_str(const char *str);
void print_help(void);


#endif // FI_PDF_H


