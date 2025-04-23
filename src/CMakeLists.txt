cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(WandeltCore VERSION 0.0.1 LANGUAGES C)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.c")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}")
target_precompile_headers(${PROJECT_NAME} PUBLIC "pch.h")
