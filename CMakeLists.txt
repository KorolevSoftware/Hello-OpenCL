cmake_minimum_required(VERSION 3.17)

project(MatrixMultiplyCMake VERSION 0.9 LANGUAGES C)

find_package(OpenCL REQUIRED)
include(cmake/FileEmbed.cmake)



FileEmbedSetup()
FileEmbedAdd(${CMAKE_SOURCE_DIR}/kernal.cl)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARY} file_embed)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIR})