cmake_minimum_required(VERSION 3.23)
project(matplotlibcpp)
find_package(Python3 3.10 COMPONENTS Interpreter Development NumPy REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${Python3_NumPy_INCLUDE_DIRS})
include_directories(${LIBXL_INCLUDE_DIR})
include_directories(include)

add_executable(output src/main.cpp)

# Add these lines
find_library(LIBXL_LIBRARY
             NAMES xl
             HINTS ${LIBXL_LIBRARY_DIR})
target_link_libraries(output ${Python3_LIBRARIES} ${LIBXL_LIBRARY})

# Explicitly set the libxl include directory and library directory
target_include_directories(output PRIVATE ${LIBXL_INCLUDE_DIR})
target_link_directories(output PRIVATE ${LIBXL_LIBRARY_DIR})
