cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(WandeltCore VERSION 0.0.1 LANGUAGES C)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.c")

set(WDT_DYNAMIC_RUNTIME ON)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

if(MSVC)
    if(WDT_DYNAMIC_RUNTIME)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${PROJECT_NAME} PUBLIC /MDd)
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
        else()
            target_compile_options(${PROJECT_NAME} PUBLIC /MD)
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
        endif()
    else()
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${PROJECT_NAME} PUBLIC /MTd)
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
        else()
            target_compile_options(${PROJECT_NAME} PUBLIC /MT)
            set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
        endif()
    endif()

    message(STATUS "MSVC version ${MSVC_VERSION}")

    if(WDT_DYNAMIC_RUNTIME)
        message("Loading Windows LLVM dynamic debug libraries, this may take a while...")
        FetchContent_Declare(
            LLVM_Windows_debug
            URL https://github.com/c3lang/win-llvm/releases/download/llvm_19_1_5/llvm-19.1.5-windows-amd64-msvc17-msvcrt-dbg.7z
        )
    else()
        message("Loading Windows LLVM static debug libraries, this may take a while...")
        FetchContent_Declare(
            LLVM_Windows_debug
            URL https://github.com/c3lang/win-llvm/releases/download/llvm_19_1_5/llvm-19.1.5-windows-amd64-msvc17-libcmt-dbg.7z
        )
    endif()

    FetchContent_MakeAvailable(LLVM_Windows_debug)

    set(llvm_dir ${llvm_windows_debug_SOURCE_DIR})
    set(CMAKE_SYSTEM_PREFIX_PATH ${llvm_dir} ${CMAKE_SYSTEM_PREFIX_PATH})

    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    message(STATUS "Libraries located in: ${LLVM_LIBRARY_DIRS}")

    string(REPLACE "." ";" VERSION_LIST ${LLVM_PACKAGE_VERSION})
    list(GET VERSION_LIST 0 LLVM_MAJOR_VERSION)

    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    link_directories(${LLVM_LIBRARY_DIRS})
    add_definitions(${LLVM_DEFINITIONS})

    if(NOT WDT_DYNAMIC_RUNTIME)
        set(LLVM_LINK_COMPONENTS
            AllTargetsAsmParsers
            AllTargetsCodeGens
            AllTargetsDescs
            AllTargetsDisassemblers
            AllTargetsInfos
            Analysis
            AsmPrinter
            BitReader
            Core
            DebugInfoPDB
            InstCombine
            IrReader
            LibDriver
            Linker
            LTO
            MC
            MCDisassembler
            native
            nativecodegen
            Object
            Option
            ScalarOpts
            Support
            Target
            TransformUtils
            WindowsManifest
            WindowsDriver
        )

        llvm_map_components_to_libnames(llvm_libs ${LLVM_LINK_COMPONENTS})
    else()
        find_library(LLVM_C_LIB NAMES LLVM-C PATHS ${LLVM_LIBRARY_DIRS} NO_DEFAULT_PATH)

        if(NOT LLVM_C_LIB)
            message(FATAL_ERROR "LLVM-C.lib not found in ${LLVM_LIBRARY_DIRS}")
        endif()

        set(llvm_libs ${LLVM_C_LIB})
    endif()

    message(STATUS "Linking to LLVM libs: ${llvm_libs}")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${llvm_libs} ${lld_libs})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}")
target_precompile_headers(${PROJECT_NAME} PUBLIC "pch.h")