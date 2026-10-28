#include <stdio.h>
#include <stdlib.h>
#include <qpdf/qpdf-c.h>

int main(int argc, char *argv[])
{

  const char* output_file = "documento_comprimido.pdf";
  
  //Verificamos si son dos argumentos
  if(argc != 2)
  {
    fprintf(stderr, "Uso: .leer_paginas <archivo.pdf>\n");
    return EXIT_FAILURE;
  }

  const char *archivo_pdf = argv[1];

  qpdf_data myPDF = qpdf_init();

  /*1.-*/
  if (qpdf_read(myPDF, archivo_pdf, NULL) != 0) 
  {
    fprintf(stderr, "Error de lectura!");
    qpdf_cleanup(&myPDF);
    return EXIT_FAILURE;
  }


   if (qpdf_init_write(myPDF, output_file) != 0) 
  {
    fprintf(stderr ,"Error al inicializar la escritura en: %s\n", output_file);
    qpdf_cleanup(&myPDF);
    return EXIT_FAILURE;
  }

  /*3.-Configuramos los parametros de compresión*/
  qpdf_set_compress_streams(myPDF, 1); //1 = True (Comprime los streams de datos)
  qpdf_set_object_stream_mode(myPDF, qpdf_o_generate); //Agrupa objetos pequeños en streams

  /*4.-inearizamos el PDF para mejor optimización de vista web*/
  qpdf_set_linearization(myPDF, 1);

  /*5.-Escribimos el nuevo PDF omptimizado y comprimido*/
  qpdf_write(myPDF);

  if(qpdf_has_error(myPDF))
  {
    fprintf(stderr, "Error crítico durante la escritura: %s\n", output_file);
    qpdf_cleanup(&myPDF);
    return EXIT_FAILURE;
  }

  printf("PDF comprimido con exito en: %s\n", output_file);

  qpdf_cleanup(&myPDF);

  return EXIT_SUCCESS;


}
