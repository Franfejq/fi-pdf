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
  int base_len = n - 4;
  
  printf("Numero de letras: %i\n", n);

  char n_name[base_len + 1];

  for (int i = 0; i < base_len; i++) 
  {
    n_name[i] = file_name[i];
  }
  n_name[base_len] = '\0';

  printf("Nombre recortado: %s\n", n_name);

  int new_base = size_str(n_name) + size_str(sufix);

  char new_file[new_base + 1];

  for(int i = 0; i < base_len; i++)
  {
    new_file[i] = n_name[i];
  }
  for(int i = 0; i < size_str(sufix); i++)
  {
    new_file[i + base_len] = sufix[i];
  }
  new_file[new_base] = '\0';

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
