cmake_minimum_required(VERSION 3.20)
project(PruebaGSNativo LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")

add_executable(gs_test_nativo gs_test.c)

# Enlazamos directamente la librería compartida de Ghostscript (libgs.so)
target_link_libraries(gs_test_nativo PRIVATE gs)
