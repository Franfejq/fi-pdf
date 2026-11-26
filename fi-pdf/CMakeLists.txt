cmake_minimum_required(VERSION 3.20)
project(fi-pdf VERSION 1.0.0 LANGUAGES C)

#Especificamos el estandar C (17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#Banderas de Compilacion
add_compile_options(-Wall -Wextra -pedantic)

add_subdirectory(motor_pdf)

set(SOURCES
  main.c)
# Cambiar de OFF a ON activa -std=gnu17, revelando PATH_MAX nativamente.
set(CMAKE_C_EXTENSIONS ON)


#Se crea el binario
add_executable(fi-pdf ${SOURCES})

target_link_libraries(fi-pdf motor_pdf gs)

# Instalación del ejecutable (por defecto en /usr/local/bin)
include(GNUInstallDirs)
install(TARGETS fi-pdf
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Empaquetado distribuible con CPack (.tar.gz y, si hay herramientas, .deb / .rpm)
set(CPACK_PACKAGE_NAME "fi-pdf")
set(CPACK_PACKAGE_VENDOR "Facultad de Ingenieria, UNAM")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Utilidad CLI ligera en C17 para compresion y fusion de PDFs")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "Franfejq")
set(CPACK_STRIP_FILES ON)
set(CPACK_GENERATOR "TGZ")

find_program(DPKG_DEB_EXECUTABLE dpkg-deb)
if(DPKG_DEB_EXECUTABLE)
  list(APPEND CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
endif()

find_program(RPMBUILD_EXECUTABLE rpmbuild)
if(RPMBUILD_EXECUTABLE)
  list(APPEND CPACK_GENERATOR "RPM")
  set(CPACK_RPM_PACKAGE_REQUIRES "ghostscript, qpdf-libs")
  set(CPACK_RPM_PACKAGE_GROUP "Applications/Publishing")
endif()

include(CPack)

