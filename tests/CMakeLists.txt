cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(WandeltTests VERSION 0.0.1 LANGUAGES C)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.c")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(WandeltTests PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${PROJECT_NAME} PRIVATE WandeltCore)

target_include_directories(WandeltTests PRIVATE "${CMAKE_SOURCE_DIR}/tests")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_SECURE_NO_DEPRECATE       
    _CRT_NONSTDC_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
    _SCL_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_DEPRECATE
    UNICODE
    _UNICODE
    __STDC_CONSTANT_MACROS
    __STDC_FORMAT_MACROS
    __STDC_LIMIT_MACROS
    _GLIBCXX_ASSERTIONS             
)
