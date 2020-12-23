set(EXECUTABLE_OUTPUT_PATH ./)


add_executable(libmpo_example example.c 3dsraw.c 3dsraw_right.c)
target_link_libraries(libmpo_example mpo_static jpeg)
add_executable(libmpo_decompress decompress.c)
target_link_libraries(libmpo_decompress mpo_static jpeg)

file(GLOB MPOFILES
  "*.mpo"
)
file(COPY ${MPOFILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})