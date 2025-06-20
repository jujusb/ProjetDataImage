cmake_minimum_required(VERSION 3.14)
project(Projet_OpenCV_CMake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(OpenCV_DIR D:/opencv2/Install/opencv/x64/vc16)
set(OpenCV_LIB_DIR %OPENCV_DIR%/lib)
set(OpenCV_INCLUDE_DIR D:/opencv2/Install/opencv/include)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(Projet_OpenCV_CMake
        include/histogram.hpp
        src/main.cpp
        include/shape.h src/shape.cpp
        include/imageProcessor.h src/imageProcessor.cpp
        include/explorer.h src/explorer.cpp)



target_link_libraries(Projet_OpenCV_CMake ${OpenCV_LIBS})