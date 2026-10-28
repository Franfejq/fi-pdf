cmake_minimum_required(VERSION 3.20)
project(PruebaQPDF LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(QPDF REQUIRED libqpdf)

add_executable(leer_paginas Test2.c)

target_include_directories(leer_paginas PRIVATE ${QPDF_INCLUDE_DIRS})

target_link_libraries(leer_paginas PRIVATE ${QPDF_LIBRARIES})
