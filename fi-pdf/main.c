#include <stdio.h>
#include <stdlib.h>
#include "fi_pdf.h"
#include "fi_gs.h"

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

      char bandera = argv[2][1];
      const char *file_name = argv[3];
      char name_out[PATH_MAX];

      /* Generamos una ruta de salida libre y abortamos si ocurre algún error */
      if (!dynamic_name(file_name, name_out))
      {
        return EXIT_FAILURE;
      }

      printf("[INFO] Archivo de salida: %s\n", name_out);
      printf("[INFO] Procesando compresión nivel '%c'...\n", bandera);

      if (comprimir_pdf_nativo(file_name, name_out, bandera))
      {
        printf("[ÉXITO] Archivo comprimido correctamente sin depender de la terminal.\n");
      }
      else
      {
        fprintf(stderr, "[ERROR] El motor de compresión interno falló.\n");
        return EXIT_FAILURE;
      }
    }
  }

  return 0;
}
