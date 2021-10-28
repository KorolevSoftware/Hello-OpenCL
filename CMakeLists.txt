cmake_minimum_required(VERSION 3.17)

project(MatrixMultiplyCMake VERSION 0.9 LANGUAGES C)
find_package(OpenCL REQUIRED)

include_directories(
	${OpenCL_INCLUDE_DIR}
)

configure_file(kernal.cl ${CMAKE_CURRENT_BINARY_DIR}/kernal.cl COPYONLY)
add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARY})