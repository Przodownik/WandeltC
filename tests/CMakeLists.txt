cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(WandeltTests VERSION 0.0.1 LANGUAGES C)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.c")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(WandeltTests PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${PROJECT_NAME} PRIVATE WandeltCore)

target_include_directories(WandeltTests PRIVATE "${CMAKE_SOURCE_DIR}/tests")
