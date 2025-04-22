cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(Wandelt VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 23)

file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.c")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -WX /EHsc)
target_precompile_headers(${PROJECT_NAME} PRIVATE "src/pch.h")

if(MSVC)
    # /Od: Disables optimizations for easier debugging.
    # /O2: Enables optimizations for maximum speed.
    # /Zi: Generates debugging information.
    # /EHa: Enables asynchronous structured exception handling.
    # /EHsc: Enables standard C++ exception handling.
    # /utf-8: Sets the source and execution character set to UTF-8.
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od /Zi /utf-8")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /utf-8")
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WDT_PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WDT_PLATFORM_LINUX)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WDT_PLATFORM_MACOS)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)

include(InstallRequiredSystemLibraries) # Include runtime libraries (e.g., MSVC CRT)

set(CPACK_PACKAGE_NAME "Wandelt Compiler")
set(CPACK_PACKAGE_VENDOR "TF")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Wandelt Compiler")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Wandelt Compiler")

set(CPACK_STRIP_FILES TRUE)
set(CPACK_SOURCE_STRIP_FILES TRUE)
set(CPACK_PACKAGE_DEFAULT_COMPRESSION_LEVEL 9) # [0 - 9]
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(WIN32)
    set(CPACK_GENERATOR "NSIS")

    set(CPACK_NSIS_DISPLAY_NAME "Wandelt Compiler")
    set(CPACK_NSIS_PACKAGE_NAME "Wandelt Compiler")
endif()

include(CPack)
