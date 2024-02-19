cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
project(sound-processor)

# Debug
set(CMAKE_BUILD_TYPE Debug)

# Warnings
add_compile_options(-Wall -Wextra -pedantic -Werror)

# Add converters
add_library(converter SHARED IMPORTED)
set_property(TARGET converter PROPERTY IMPORTED_LOCATION ../libconverter.so)

# Add master library
add_library(corelib OBJECT rw.cpp wav.cpp wav_commander.cpp wav_rw.cpp)

# Link
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} converter)
target_link_libraries(${PROJECT_NAME} corelib)
