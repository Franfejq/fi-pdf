add_library(motor_pdf STATIC
  Inc/fi_pdf.h
  Inc/fi_gs.h
  Inc/merge.h
  Src/fi_pdf.c
  Src/fi_gs.c
  Src/merge.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(QPDF REQUIRED libqpdf)

target_include_directories(motor_pdf
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Inc
  PUBLIC ${QPDF_INCLUDE_DIRS})

if(QPDF_LIBRARY_DIRS)
  target_link_directories(motor_pdf PUBLIC ${QPDF_LIBRARY_DIRS})
endif()

target_link_libraries(motor_pdf PUBLIC gs ${QPDF_LIBRARIES})
