#include <stdio.h>
#include <stdlib.h>
#include "fi_pdf.h"

void print_help(void);

int main(int argc, char *argv[])
{

  if(argc < 2)
  {
    fprintf(stderr, "Error: Faltan Argumentos.\n");
    print_help();
    return EXIT_FAILURE;
  }

  const char* command = argv[1];

  if(strcmp(command, "compress") == 0)
  {
    if(argc < 4)
    {
      fprintf(stderr, "Error: Necesitas 4 o mas arumentos en compresión.\n");
      return EXIT_FAILURE;
    }
    else 
    {
      if(!(check_file(argv[3])))
      {
        return  EXIT_FAILURE;
      }

      
      printf("[INFO]: El archivo %s es valido y esta listo para comprimir.\n", argv[3]);
    }
  }

  return 0;
}

void print_help(void)
{
  printf("fi-pdf - Utilidad CLI para manipulación de PDFs\n");
  printf("Uso: fi-pdf <comando> [opciones] <archivo(s)>\n\n");
  printf("Opciones:\n");
  printf("  compress    Comprime un archivo PDF. Requiere un nivel de compresión.\n");
  printf("              Niveles: -l (baja), -m (media), -h (alta)\n");
  printf("              Ejemplo: fi-pdf compress -h documento.pdf\n\n");
  printf("  merge       Une múltiples archivos PDF secuencialmente.\n");
  printf("              Ejemplo: fi-pdf merge doc1.pdf doc2.pdf\n\n");
}
