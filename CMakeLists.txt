# #########################################################################
# Project settings
# #########################################################################
cmake_minimum_required(VERSION 3.1.0)
project(wavefront_renderer VERSION 0.1)
set(PROJECT_NAME wavefront_renderer)

set(CMAKE_CXX_STANDARD 17)
set(FREEGLUT_PATH "C:/Program Files (x86)/freeglut")

# #########################################################################
# Build project
# #########################################################################
add_compile_options(
  -Wall
  -Os
  -D __RENDERER_VERSION__="${PROJECT_VERSION}"
)

add_executable(wavefront_renderer "${PROJECT_SOURCE_DIR}/src/main.cpp")

target_link_libraries(wavefront_renderer "${FREEGLUT_PATH}/lib/freeglutd.lib")
target_include_directories(wavefront_renderer PRIVATE
  "${FREEGLUT_PATH}/include"
  "${PROJECT_SOURCE_DIR}/src"
)
add_custom_command(
        TARGET wavefront_renderer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${FREEGLUT_PATH}/bin/freeglutd.dll
                ${CMAKE_CURRENT_BINARY_DIR}/freeglutd.dll)
