#include <stdio.h>
#include <stdlib.h>
#include <qpdf/qpdf-c.h>

int main(int argc, char *argv[])
{
  /*1.-Verificamos que si se hayan pasado dos argmentos incluyendo el nombre del programa*/
  if (argc != 2) 
  {
    fprintf(stderr, "Uso: ./leer_paginas <archivo.pdf>\n");
    return EXIT_FAILURE;
  }

  const char* archivo_pdf = argv[1];

  /*2.- Inicializamos un objeto de datos de QPDF*/
  qpdf_data q = qpdf_init();

  /*3.- Usando el validor y lector de QPDF*/
  printf("Intento de lectura: %s ...\n", archivo_pdf);
  if (qpdf_read(q, archivo_pdf, NULL) != 0) 
  {
    //Si hay error, QPDF no dice exactamente que pasado
    fprintf(stderr, "Error de lectura!");
    qpdf_cleanup(&q);
    return EXIT_FAILURE;
  }

  /*4.- Checamos la versión del PDF ya que QPDF tiene muchas herramientas para buscar objetos o metadatos*/
  const char *version = qpdf_get_pdf_version(q);
  printf("El archivo es valido y fue hecho bajo la PDF version: %s\n", version);

  /*5.-Limpiamos la memoria*/
  qpdf_cleanup(&q);

  return EXIT_SUCCESS;
}
