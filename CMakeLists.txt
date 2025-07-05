cmake_minimum_required(VERSION 3.10)
project(Chess_in_C)

# Gather all source files
file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.c")

# Compile the source files
add_executable(Chess_in_C ${SRC_FILES})

set_property(TARGET Chess_in_C PROPERTY C_STANDARD 11)
set_property(TARGET Chess_in_C PROPERTY C_STANDARD_REQUIRED ON)

# Add the directory with header files
target_include_directories(Chess_in_C PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Add the directory with the raylib library
target_link_directories(Chess_in_C PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(Chess_in_C PRIVATE raylib opengl32 gdi32 winmm)

# Place the .exe file in the root directory
set_target_properties(Chess_in_C PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)