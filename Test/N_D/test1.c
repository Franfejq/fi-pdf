#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int size_str(const char *str);
void dynamic_name(const char* origen, char *destino);


int main(int argc, char *argv[])
{
  if(argc != 2)
  {
    fprintf(stderr, "Tienes que poner al menos dos argumentos!!\n");
    return EXIT_FAILURE;
  }
  const char *file_name = argv[1];
  char name_out[PATH_MAX]; 
  dynamic_name(file_name, name_out);
  printf("Nombre final: %s\n", name_out);
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


