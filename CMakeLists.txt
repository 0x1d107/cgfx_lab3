# CMakeList.txt : CMake project for cgfx_lab2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.11)


project ("cgfx_lab3")



# Add source to this project's executable.
add_executable (cgfx_lab3 "cgfx_lab3.cpp" )

target_link_libraries(cgfx_lab3 sfml-graphics sfml-window sfml-system sfml-audio)


# TODO: Add tests and install targets if needed.
