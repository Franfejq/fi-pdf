#ifndef FI_PDF_HPDF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> 
#include <errno.h>
#include <limits.h>

bool check_file(const char *path);
void dynamic_name(const char* origen, char *destino);
int size_str(const char *str);
void print_help(void);


#endif // FI_PDF_H


