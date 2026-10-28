cmake_minimum_required(VERSION 3.20)
project(PruebaMuPDF LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MUPDF REQUIRED mupdf)

add_executable(num_page mupdf_test_1.c)

target_include_directories(num_page PRIVATE ${MUPDF_INCLUDE_DIRS})

target_link_libraries(num_page PRIVATE ${MUPDF_LIBRARIES} m)
