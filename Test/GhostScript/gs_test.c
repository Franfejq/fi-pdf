#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Uso: ./gs_test <entrada.pdf> <salida.pdf>\n");
        return EXIT_FAILURE;
    }

    const char *archivo_in = argv[1];
    const char *archivo_out = argv[2];

    // Asignamos un buffer suficientemente grande para el comando completo
    char comando[1024];

    /* 
     * LA MAGIA DE GHOSTSCRIPT
     * -sDEVICE=pdfwrite        : Le dice a GS que queremos exportar un PDF.
     * -dCompatibilityLevel=1.4 : Asegura compatibilidad máxima.
     * -dPDFSETTINGS=/screen    : ¡El santo grial! Fuerza el downsampling a 72 DPI (Nivel Alto).
     *                            (Para tu bandera -m, usaríamos /ebook que es 150 DPI).
     * -dNOPAUSE -dQUIET -dBATCH: Hace que GS corra en silencio sin pedir interacción.
     */
    
    // Armamos el comando de terminal inyectando los nombres de los archivos.
    // OJO: Encerramos %s entre comillas simples '%s' para evitar que espacios 
    // en los nombres de los archivos rompan el comando en la terminal.
    int escritos = snprintf(comando, sizeof(comando),
             "gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dPDFSETTINGS=/screen "
             "-dNOPAUSE -dQUIET -dBATCH -sOutputFile='%s' '%s'", 
             archivo_out, archivo_in);

    // Validamos que el comando no haya excedido el tamaño del buffer
    if (escritos < 0 || (size_t)escritos >= sizeof(comando)) 
    {
        fprintf(stderr, "[ERROR] El comando superó el límite de memoria.\n");
        return EXIT_FAILURE;
    }

    printf("[INFO] Ejecutando compresión masiva (GS /screen) en: %s\n", archivo_in);

    // system() pausa nuestro programa en C, abre una terminal invisible, 
    // ejecuta el string que le pasamos, y nos devuelve el control cuando termina.
    int resultado = system(comando);

    if (resultado == 0) 
    {
        printf("[ÉXITO] Archivo hiper-comprimido guardado en: %s\n", archivo_out);
    } 
    else 
    {
        fprintf(stderr, "[ERROR FATAL] Ghostscript falló o no está instalado (Código: %d)\n", resultado);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
