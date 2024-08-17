add_library (test SHARED "")
target_link_libraries (test PUBLIC sat)

target_sources (test
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/test.c
)

target_include_directories (test
  PUBLIC
  ${CMAKE_SOURCE_DIR}/route_base
)
