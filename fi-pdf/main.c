#include <stdio.h>
#include <stdlib.h>
#include "fi_pdf.h"
#include "fi_gs.h"
#include "merge.h"

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
  else if (strcmp(command, "merge") == 0)
  {
    if (argc < 4)
    {
      fprintf(stderr, "Error: Se requieren al menos 2 archivos PDF para unir.\n");
      print_help();
      return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++)
    {
      if (!check_file(argv[i]))
      {
        return EXIT_FAILURE;
      }
    }

    char name_out[PATH_MAX];
    if (!dynamic_merge_name(argv[2], name_out))
    {
      return EXIT_FAILURE;
    }

    printf("[INFO] Uniendo %d archivos PDF secuencialmente...\n", argc - 2);
    printf("[INFO] Archivo de salida: %s\n", name_out);

    if (fusionar_pdfs_nativo((const char *const *)&argv[2], (size_t)(argc - 2), name_out))
    {
      printf("[ÉXITO] Archivos fusionados correctamente.\n");
    }
    else
    {
      fprintf(stderr, "[ERROR] El motor de fusión interno falló.\n");
      return EXIT_FAILURE;
    }
  }
  else if (strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0)
  {
    print_help();
    return EXIT_SUCCESS;
  }
  else
  {
    fprintf(stderr, "Error: Comando no reconocido '%s'.\n", command);
    print_help();
    return EXIT_FAILURE;
  }

  return 0;
}
