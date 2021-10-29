cmake_minimum_required(VERSION 3.17)

project(MatrixMultiplyCMake VERSION 0.9 LANGUAGES C)

find_package(OpenCL REQUIRED)
find_package(Python3 COMPONENTS Interpreter)

set(EMBED_FILE "kernal.cl")
set(EMBED_DIR_OUT ${CMAKE_CURRENT_BINARY_DIR}/generate/)
set(EMBEDING_PROG ${CMAKE_CURRENT_SOURCE_DIR}/embede_resource.py)

file(MAKE_DIRECTORY ${EMBED_DIR_OUT})

add_custom_command(
	OUTPUT ${EMBED_DIR_OUT}/sourse.h
	WORKING_DIRECTORY ${EMBED_DIR_OUT}
	COMMAND ${Python3_EXECUTABLE} ${EMBEDING_PROG}
	ARGS --f ${CMAKE_CURRENT_SOURCE_DIR}/${EMBED_FILE}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/sourse.h
    COMMENT echo "Embeding ${EMBED_FILE}"
    VERBATIM
)

message(STATUS "Python: version=${Python3_VERSION} interpreter=${Python3_EXECUTABLE}")

include_directories(
	${OpenCL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${EMBED_DIR_OUT}
)


set_property(SOURCE main.c APPEND PROPERTY OBJECT_DEPENDS ${EMBED_DIR_OUT}/sourse.h)

add_executable(${PROJECT_NAME} main.c )
target_link_libraries(${PROJECT_NAME} ${OpenCL_LIBRARY})