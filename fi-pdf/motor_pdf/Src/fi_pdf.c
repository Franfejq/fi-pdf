/* Habilita definiciones POSIX (PATH_MAX, access, F_OK) bajo el estándar estricto C17 */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "fi_pdf.h"

int size_str(const char *str);
static size_t normalizar_base_compresion(const char *origen, size_t base_len);

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
    fprintf(stderr, "Error: El archivo %s esta CORRUPTO.\n", path);
    return false;
  }

  return true;
}

/**
 * Normaliza la longitud base antes de ".pdf" removiendo un sufijo previo
 * "_compress" o "_compress(<dígitos>)", siempre que permanezca al menos
 * un carácter del nombre de archivo antes de dicho sufijo.
 */
static size_t normalizar_base_compresion(const char *origen, size_t base_len)
{
  const char *prefijo = "_compress";
  const size_t prefijo_len = 9;
  size_t corte = base_len;

  /* Si la base termina en ')', retrocedemos sobre los dígitos para detectar "(n)" */
  if (corte >= 3 && origen[corte - 1] == ')')
  {
    size_t pos = corte - 1;
    while (pos > 0 && origen[pos - 1] >= '0' && origen[pos - 1] <= '9')
    {
      pos--;
    }
    /* Solo ajustamos el corte si hubo al menos un dígito precedido por '(' */
    if (pos < corte - 1 && pos > 0 && origen[pos - 1] == '(')
    {
      corte = pos - 1;
    }
  }

  /* Recortamos "_compress" solo si queda al menos 1 carácter útil del nombre (y no es '/') */
  if (corte > prefijo_len &&
      origen[corte - prefijo_len - 1] != '/' &&
      strncmp(origen + (corte - prefijo_len), prefijo, prefijo_len) == 0)
  {
    return corte - prefijo_len;
  }

  /* Si no coincide con el patrón de compresión, se conserva la base original */
  return base_len;
}

/**
 * Genera una ruta de salida única con sufijo "_compress.pdf" o
 * "_compress(n).pdf" evitando sobrescribir archivos existentes en disco.
 * @param origen  Ruta original del archivo PDF.
 * @param destino Buffer de salida con capacidad mínima de PATH_MAX bytes.
 * @return        true si se generó un nombre libre dentro de PATH_MAX, false en error.
 */
bool dynamic_name(const char *origen, char *destino)
{
  /* Validamos que ambos punteros recibidos sean válidos */
  if (origen == NULL || destino == NULL)
  {
    fprintf(stderr, "Error: Rutas inválidas al generar el nombre de salida.\n");
    return false;
  }

  /* Comprobamos longitud mínima y que la ruta termine exactamente en ".pdf" */
  size_t origen_len = (size_t)size_str(origen);
  if (origen_len <= 4 || strcmp(origen + (origen_len - 4), ".pdf") != 0)
  {
    fprintf(stderr, "Error: El archivo %s no tiene extensión .pdf válida.\n", origen);
    return false;
  }

  /* Obtenemos la raíz lógica del nombre removiendo ".pdf" y cualquier sufijo "_compress..." previo */
  size_t base_len = normalizar_base_compresion(origen, origen_len - 4);
  const char sufix[] = "_compress.pdf";
  size_t sufix_len = (size_t)size_str(sufix);

  /* Verificamos que la ruta candidata quepa en PATH_MAX incluyendo el terminador '\0' */
  if (base_len + sufix_len >= PATH_MAX)
  {
    fprintf(stderr, "Error: La ruta de salida excede el límite PATH_MAX (%d).\n", PATH_MAX);
    return false;
  }

  /* 1.- Copiamos la base del nombre original (normalizada y sin .pdf) */
  memcpy(destino, origen, base_len);

  /* 2.- Pegamos el sufijo inicial "_compress.pdf" donde termina la base */
  memcpy(destino + base_len, sufix, sufix_len);

  /* 3.- Agregamos el carácter nulo al final */
  destino[base_len + sufix_len] = '\0';

  /* 4.- Verificamos colisión en disco con access(..., F_OK) e incrementamos (n) si ya existe */
  int n = 1;
  while (1)
  {
    errno = 0;
    if (access(destino, F_OK) != 0)
    {
      /* ENOENT confirma que la ruta está libre y puede usarse sin sobrescribir */
      if (errno == ENOENT)
      {
        return true;
      }
      /* Cualquier otro errno indica un problema real de acceso/permisos/ruta */
      fprintf(stderr, "Error: No se pudo verificar la ruta %s (%s).\n",
              destino, strerror(errno));
      return false;
    }

    /* Evitamos desbordamiento del contador entero n */
    if (n == INT_MAX)
    {
      fprintf(stderr, "Error: Límite de intentos de nombres alcanzado para %s.\n", origen);
      return false;
    }

    /* Reconstruimos el sufijo con "_compress(n).pdf" respetando el espacio restante en PATH_MAX */
    int escritos = snprintf(destino + base_len, PATH_MAX - base_len,
                            "_compress(%d).pdf", n);
    if (escritos < 0 || (size_t)escritos >= PATH_MAX - base_len)
    {
      fprintf(stderr, "Error: La ruta de salida excede el límite PATH_MAX (%d).\n", PATH_MAX);
      return false;
    }
    n++;
  }
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
    char arg_salida[PATH_MAX + 32];
    
    // 1. Armar el string del archivo de salida
    int escritos = snprintf(arg_salida, sizeof(arg_salida), "-sOutputFile=%s", archivo_out);
    if (escritos < 0 || (size_t)escritos >= sizeof(arg_salida)) 
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


