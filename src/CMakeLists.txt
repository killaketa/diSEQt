# CMakeList.txt : CMake project for diSEQt, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (diSEQt "main.c" "brseq.c" "brseq_commands.c" "rseq_parse.c" "midi.c" "common.c")
target_link_libraries(diSEQt PRIVATE build_settings)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET diSEQt PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
