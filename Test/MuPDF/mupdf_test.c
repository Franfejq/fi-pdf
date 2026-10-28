#include <stdio.h>
#include <stdlib.h>

#include <mupdf/fitz.h>

int main(int argc, char *argv[])
{

  if(argc != 2)
  {
    fprintf(stderr, "Uso: .num_pages <archivo.pdf>\n");
    return EXIT_FAILURE;
  }

  const char *archivo_pdf = argv[1];

  /*1.-Inicializamos el motor de pdf's (Contexto de memoria)*/
  //FZ_STORE_UNLIMITED le permite a MuPDF usar la RAM necesaria para procesar
  fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);

  if(!ctx)
  {
    fprintf(stderr, "Erro critico: no se pudo inicializar el motor de MuPDF\n");
    return EXIT_FAILURE;
  }

  /*2.- Registar los manejadores o sea enseñarle al motor a reconocer el formato PDF*/
  fz_register_document_handlers(ctx);

  printf("[INFO] Intentado leer '%s' con MuPDF...\n", archivo_pdf);

  fz_document *doc = NULL;

  /*3.- Manejo de excepciones de MuPDF*/
  fz_try(ctx)
  {
    //Abrimos el documento cargandolo al Contexto
    doc = fz_open_document(ctx, archivo_pdf);

    int paginas = fz_count_pages(ctx, doc);

    printf("[EXITO] Archivo cargado correctamente en memoria.\n");
    printf("[INFO] El documento tiene %i página(s).\n", paginas);
  }
  fz_catch(ctx)
  {
     // Si el archivo no existe o está corrupto, MuPDF salta directo aquí
    fprintf(stderr, "[ERROR] Fallo al intentar leer la estructura del PDF.\n");
    fz_drop_context(ctx);
    return EXIT_FAILURE;

  }

  /*4.-Liberamos estrictamente la memoria de adentro hacia afuera*/
  fz_drop_document(ctx, doc);
  fz_drop_context(ctx);

  return EXIT_SUCCESS;
}
