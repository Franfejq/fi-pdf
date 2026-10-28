add_library(motor_pdf SHARED
  Inc/fi_pdf.h
  Src/fi_pdf.c)

target_include_directories(motor_pdf PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Inc)
