add_executable (dynamic "")
target_link_libraries (dynamic PUBLIC sat)

target_sources (dynamic
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/server.c
)

target_include_directories (dynamic
  PUBLIC
  ${CMAKE_SOURCE_DIR}/route_base
)
