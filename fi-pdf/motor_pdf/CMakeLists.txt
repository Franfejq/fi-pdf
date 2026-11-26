add_library(motor_pdf SHARED
  Inc/fi_pdf.h
  Inc/fi_gs.h
  Src/fi_pdf.c
  Src/fi_gs.c)

target_include_directories(motor_pdf PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Inc)
target_link_libraries(motor_pdf PUBLIC gs)
