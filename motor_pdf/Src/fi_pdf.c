#include "fi_pdf.h"

bool check_file(const char *path)
{
  /*1.-Verificamos si el archivo 'path' NO EXISTE entonces*/
  if (access(path, F_OK) != 0) 
  {
    fprintf(stderr, "Error: El archivo %s no fue encontrado.\n", path);
    return false;
  }

  /*2.-Verificamos si el archivo 'path' NO ES .pdf entonces*/
  const char *ext = strrchr(path, '.');
  if (ext == NULL || strcmp(ext, ".pdf") != 0) 
  {
    fprintf(stderr, "Error: El archivo %s no es un PDF valido.\n", path);
    return false;
  }

  /*3.- Verificamos si el archivo 'path' NO TIENE permisos de lectura entonces*/
  if(access(path, R_OK) != 0)
  {
    fprintf(stderr, "Error: El archivo %s no tiene permisos de lectura.\n", path);
    return false;
  }

  /*4.- Verificamos si el archivo 'path' NO TIENE permiso de escritura entonces*/
  if (access(path, W_OK) != 0) 
  {
    fprintf(stderr, "Error: El archivo %s no tiene permisos de escritura.\n", path);
    return false;
  }

  /*5.-Verificamos si el archivo 'path' ESTA CORRUPTO entonces*/
  FILE *file = fopen(path, "rb");
  if (file == NULL) 
  {
    fprintf(stderr, "Error: No se pudo abrir el archivo %s para lectura.\n", path);
    return false;
  }

  char buffer[6] = {0};
  size_t bytes_read = fread(buffer, sizeof(char), 5, file);
  fclose(file);

  if(bytes_read != 5 || strncmp(buffer, "%PDF-", 5) != 0)
  {
    fprintf(stderr, "Error: El archivo %s esta CORRUPTO.\n");
    return false;
  }

  return true;
}
