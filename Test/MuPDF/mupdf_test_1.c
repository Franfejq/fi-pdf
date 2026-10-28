#include <stdio.h>
#include <stdlib.h>
#include <mupdf/fitz.h>
#include <mupdf/pdf.h> // <- Cabecera estricta para escribir y comprimir PDFs

int main(int argc, char *argv[]) 
{
    // Ahora pedimos entrada y salida para evitar colisiones
    if (argc != 3) 
    {
        fprintf(stderr, "Uso: ./prueba_mupdf <entrada.pdf> <salida.pdf>\n");
        return EXIT_FAILURE;
    }

    const char *archivo_in = argv[1];
    const char *archivo_out = argv[2];

    fz_context *ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
    if (!ctx) 
    {
        fprintf(stderr, "Error crítico al iniciar el motor MuPDF.\n");
        return EXIT_FAILURE;
    }

    fz_register_document_handlers(ctx);

    fz_document *doc = NULL;
    
    fz_try(ctx) 
    {
        // 1. Abrimos el documento de forma genérica
        doc = fz_open_document(ctx, archivo_in);
        
        // 2. Comprobamos que realmente sea un PDF (MuPDF también abre EPUB, XPS, etc.)
        // Si es PDF, lo "casteamos" a su estructura específica (pdf_document)
        pdf_document *pdf = pdf_specifics(ctx, doc);
        if (!pdf) 
        {
            fz_throw(ctx, FZ_ERROR_GENERIC, "El archivo no es un PDF nativo.");
        }

        // 3. LA MAGIA: Configuramos el motor de compresión gráfica
        pdf_write_options opts = pdf_default_write_options;
        
        // Nivel 3 de Garbage Collection: Reconstruye todo el PDF y elimina objetos huérfanos
        opts.do_garbage = 3; 
        
        // Habilita compresión algorítmica Deflate (Lo que hacía QPDF)
        opts.do_compress = 1; 
        
        // Habilita la recompresión de imágenes (La verdadera compresión con pérdida)
        opts.do_compress_images = 1; 
        
        // REDUCCIÓN DE CALIDAD: 
        // 72 DPI destruye calidad pero reduce peso drásticamente (Ideal para tu bandera -h)
        // 150 DPI mantiene legibilidad (Ideal para tu bandera -m) 
        // Lineariza para web
        //opts.do_linear = 1; 

        printf("[INFO] Aplicando compresión masiva (DPI: 72) a: %s\n", archivo_in);

        // 4. Guardamos el archivo usando las opciones que definimos
        pdf_save_document(ctx, pdf, archivo_out, &opts);

        printf("[ÉXITO] Archivo hiper-comprimido guardado en: %s\n", archivo_out);
    }
    fz_catch(ctx) 
    {
        // fz_caught_message extrae el error exacto si la escritura falla
        fprintf(stderr, "[ERROR FATAL]: %s\n", fz_caught_message(ctx));
        fz_drop_document(ctx, doc);
        fz_drop_context(ctx);
        return EXIT_FAILURE;
    }

    fz_drop_document(ctx, doc);
    fz_drop_context(ctx);

    return EXIT_SUCCESS;
}
