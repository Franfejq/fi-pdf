cmake_minimum_required(VERSION 3.20)
project(fi-pdf VERSION 1.0.0 LANGUAGES C)

#Especificamos el estandar C (17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(motor_pdf)
#Banderas de Compilacion
add_compile_options(-Wall -Wextra -pedantic)

set(SOURCES
  main.c)
# Cambiar de OFF a ON activa -std=gnu17, revelando PATH_MAX nativamente.
set(CMAKE_C_EXTENSIONS ON)


#Se crea el binario
add_executable(fi-pdf ${SOURCES})

target_link_libraries(fi-pdf motor_pdf gs)

