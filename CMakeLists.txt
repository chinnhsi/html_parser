cmake_minimum_required(VERSION 3.0.0)
project(html_parser VERSION 0.1.0 LANGUAGES C CXX)

aux_source_directory(./src DIR_SRCS)

add_executable(html_parser ${DIR_SRCS})
add_compile_options(-std=c++17)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
