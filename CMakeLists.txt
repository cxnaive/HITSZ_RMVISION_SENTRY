﻿CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(HITSZ-RM-CV)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_compile_options(-O3)
set(use_u2_camera false)


# cuda
find_package(CUDA REQUIRED)
include_directories(/usr/local/cuda/include)
link_directories(/usr/local/cuda/lib64)

#Include
include_directories(${PROJECT_SOURCE_DIR}/armor/include)
include_directories(${PROJECT_SOURCE_DIR}/net/include)
include_directories(${PROJECT_SOURCE_DIR}/others/include)
include_directories(${PROJECT_SOURCE_DIR}/config/include)
include_directories(${PROJECT_SOURCE_DIR}/com/include)
include_directories(../tiny-tensorrt)
link_directories(../tiny-tensorrt/build)
find_package(OpenCV 4.5 REQUIRED)

if (use_u2_camera)
    message(STATUS "Linking u2 camera lib on linux...")
    message(STATUS "u2_camera_root $ENV{GENICAM_ROOT_V2_3}")
    link_directories($ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64)
    link_directories($ENV{GENICAM_ROOT_V2_3}/bin/Linux64_x64/GenApi/Generic)
    link_libraries(-ldhgentl -lpthread)
    link_libraries(-ldx_image_proc -lGCBase_gcc40_v2_3 -lGenApi_gcc40_v2_3 -llog4cpp_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3)
endif ()

# Resources

file(GLOB_RECURSE resources "${PROJECT_SOURCE_DIR}/resources/*")
message(STATUS "Copying resources file(s): ${resources}")
file(COPY ${resources} DESTINATION ${CMAKE_BINARY_DIR}/)

# find_package(glog REQUIRED)
#Src
file(GLOB_RECURSE sourcefiles 
"others/**/*.h" "others/**/*.cpp"
"net/**/*.cpp" "net/**/*.h" 
"config/**/*.cpp" "config/**/*.h" 
"com/**/*.h"  "com/**/*.cpp" "com/**/*.cc" "com/**/*.c"
"armor/**/*.h" "armor/**/*.cpp"
)

file(GLOB_RECURSE camfiles "others/**/*.h" "others/**/*.cpp" "config/src/rmconfig.cpp" "config/include/rmconfig.h")

add_executable(rmcv main.cpp ${sourcefiles})

link_directories(${OpenCV_LIBRARY_DIRS})

target_link_libraries(rmcv ${OpenCV_LIBS} -lglog -lgxiapi libtinytrt.so -lpthread nvinfer nvinfer_plugin nvparsers nvonnxparser)