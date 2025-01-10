cmake_minimum_required(VERSION 3.16)

project(nmr_program VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_compile_definitions(kiss_fft_scalar=double)

#add_compile_definitions(DEBUG__)
#

add_compile_options(
  "-Wall" "-Wpedantic" "-Wextra"
)

add_subdirectory(src/app)
