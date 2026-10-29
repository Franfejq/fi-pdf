#include <stdio.h>
#include <stdlib.h>

int size_str(const char *str);

int main(int argc, char* argv[])
{

  if(argc != 2)
  {
    fprintf(stderr, "Tienes que poner al menos dos argumentos!!\n");
    return EXIT_FAILURE;
  }

  const char *file_name = argv[1];
  const char sufix[] = "_compress.pdf";
  //Contamos el numero de letras primero
  int n = size_str(file_name);
  //Recortamos los 4 caracteres al tamaño total del nombre del archivo
  int base_len = n - 4;
  printf("Numero de letras: %i\n", n);

  //Creamos el nuevo array con la longitud definida en base_len mas uno ya que necesita un espacio más para el caracter nulo '\0'
  char n_name[base_len + 1];

  //Copiamos uno a uno los caracteres del filename original al nuevo array sin la extension .pdf
  for (int i = 0; i < base_len; i++) 
  {
    n_name[i] = file_name[i];
  }
  n_name[base_len] = '\0'; //En el ultimo caracter insertamos el caracter nulo para poder imprimirlo

  printf("Nombre recortado: %s\n", n_name);

  int new_base = size_str(n_name) + size_str(sufix); //Creamos una nueva variable que define el tamaño de el array final
  //tamaño del nombre sin extension + el tamaño del sufijo(_compress.pdf)
  char new_file[new_base + 1]; //Añadimos un espacio mas para el caracter nulo '\0'

  //Copiamos uno a uno los caracteres de n_name al array final que es new_file
  for(int i = 0; i < base_len; i++)
  {
    new_file[i] = n_name[i];
  }
  //Copiamos ahora los caracteres del sufijo al final del arra final
  for(int i = 0; i < size_str(sufix); i++) 
  {
    new_file[i + base_len] = sufix[i];
  }
  new_file[new_base] = '\0'; //Añadimos e caracter nulo al caracter final

  printf("Nombre final: %s\n", new_file);

  

  return EXIT_SUCCESS;
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
