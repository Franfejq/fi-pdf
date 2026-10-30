#include <stdio.h>
#include <stdlib.h>
#include "fi_pdf.h"

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
