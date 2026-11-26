#ifndef FI_PDF_H
#define FI_PDF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> 
#include <errno.h>
#include <limits.h>
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>

bool check_file(const char *path);
bool comprimir_pdf_nativo(const char *archivo_in, const char *archivo_out, char nivel); 
bool dynamic_name(const char *origen, char *destino);
int size_str(const char *str);
void print_help(void);


#endif // FI_PDF_H


