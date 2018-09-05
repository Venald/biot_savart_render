# CMakeList.txt : CMake project for cmake_test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(Biot-Savart)
set(SFML_DIR "${PROJECT_SOURCE_DIR}/SFML/lib/CMake")
set(SFML_LIBS "${PROJECT_SOURCE_DIR}/SFML/lib")
set(libs "${PROJECT_SOURCE_DIR}/lib/")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
# Headers

# add libraries
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(OpenGL)
include_directories(${PROJECT_SOURCE_DIR}/SFML/include)
# Add source to this project's executable.
add_executable (cmaketest "src/cmake_test.cpp" "src/cmake_test.h")

target_link_libraries(cmaketest ${SFML_LIBS}/sfml-main.lib)
target_link_libraries(cmaketest ${SFML_LIBS}/sfml-system-d.lib)
target_link_libraries(cmaketest ${SFML_LIBS}/sfml-window-d.lib)
target_link_libraries(cmaketest ${SFML_LIBS}/sfml-graphics-d.lib)
target_link_libraries(cmaketest opengl32.lib)
target_link_libraries(cmaketest ${libs}/glew32.lib)
# TODO: Add tests and install targets if needed.
