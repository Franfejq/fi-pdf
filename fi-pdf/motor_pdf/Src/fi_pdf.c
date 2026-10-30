#include "fi_pdf.h"

int size_str(const char *str);

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

void dynamic_name(const char* origen, char *destino)
{

  const char sufix[] = "_compress.pdf";
  int base_len = size_str(origen) - 4;
  
  /*1.-Copiamos la base del  nombre original(sin el .pdf)*/
  for(int i = 0; i < base_len; i++)
  {
    destino[i] = origen[i];
  }

  /*2.-Pegamos el sufijo donde termina la base*/
  int sufix_len = size_str(sufix);
  for(int i = 0; i < sufix_len; i++)
  {
    destino[base_len + i] = sufix[i];
  }
  /*3.-Agregamos al el caracter nulo al final*/
  destino[sufix_len + base_len] = '\0';
}
int size_str(const char *str)
{
  int m_size = 0;
  while(str[m_size]!= '\0')
  {
    m_size++;  
  }
  return m_size;
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
