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

/**
 * Comprime un archivo PDF usando la API nativa de Ghostscript.
 * @param archivo_in  Ruta del archivo original ya validado.
 * @param archivo_out Ruta de destino generada previamente.
 * @param nivel       Carácter 'm' (150 DPI) o 'h' (72 DPI).
 * @return            true si tuvo éxito, false si hubo error.
 */
bool comprimir_pdf_nativo(const char *archivo_in, const char *archivo_out, char nivel) 
{
    char arg_salida[1024];
    
    // 1. Armar el string del archivo de salida
    if (snprintf(arg_salida, sizeof(arg_salida), "-sOutputFile=%s", archivo_out) >= sizeof(arg_salida)) 
    {
        fprintf(stderr, "[ERROR] Ruta demasiado larga.\n");
        return false;
    }

    // 2. Definir el nivel de rasterización basado en la bandera de tu CLI
    // /ebook  = 150 DPI (Equilibrio)
    // /screen = 72 DPI  (Destrucción gráfica máxima)
    const char *ajuste_calidad = (nivel == 'm') ? "-dPDFSETTINGS=/ebook" : "-dPDFSETTINGS=/screen";

    // 3. Matriz de configuración del intérprete
    const char *gs_argv[] = {
        "fi-pdf-engine",
        "-sDEVICE=pdfwrite",
        "-dCompatibilityLevel=1.4",
        ajuste_calidad,         // Inyectamos la calidad dinámica
        "-dNOPAUSE",
        "-dQUIET",
        "-dBATCH",
        arg_salida,             // Inyectamos el archivo de destino
        archivo_in              // Archivo original
    };
    
    int gs_argc = sizeof(gs_argv) / sizeof(gs_argv[0]);

    // 4. Ciclo de vida de la memoria nativa
    void *minst = NULL;
    if (gsapi_new_instance(&minst, NULL) < 0) 
    {
        fprintf(stderr, "[ERROR] Memoria insuficiente para inicializar libgs.\n");
        return false;
    }

    // Ejecución bloqueante del motor
    int code = gsapi_init_with_args(minst, gs_argc, (char **)gs_argv);

    // gs_error_Quit es el estado natural de salida exitosa cuando se usa -dBATCH
    bool exito = (code == 0 || code == gs_error_Quit);

    // Limpieza estricta de memoria (Heap)
    gsapi_exit(minst);
    gsapi_delete_instance(minst);

    return exito;
}


