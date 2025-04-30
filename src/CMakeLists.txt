cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(WandeltCore VERSION 0.0.1 LANGUAGES C)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.c")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${PROJECT_NAME} PUBLIC /MTd)
    else()
        target_compile_options(${PROJECT_NAME} PUBLIC /MT)
    endif()

    message(STATUS "MSVC version ${MSVC_VERSION}")

    FetchContent_Declare(
        LLVM_Windows_debug
        URL https://github.com/c3lang/win-llvm/releases/download/llvm_19_1_5/llvm-19.1.5-windows-amd64-msvc17-libcmt-dbg.7z
    )

    message("Loading Windows LLVM debug libraries, this may take a while...")

    FetchContent_MakeAvailable(LLVM_Windows_debug)

    set(llvm_dir ${llvm_windows_debug_SOURCE_DIR})

    set(CMAKE_SYSTEM_PREFIX_PATH ${llvm_dir} ${CMAKE_SYSTEM_PREFIX_PATH})

    find_package(LLVM REQUIRED CONFIG)
    find_package(LLD REQUIRED CONFIG)

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    message(STATUS "Libraries located in: ${LLVM_LIBRARY_DIRS}")

    string(REPLACE "." ";" VERSION_LIST ${LLVM_PACKAGE_VERSION})
    list(GET VERSION_LIST 0 LLVM_MAJOR_VERSION)

    target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    link_directories(${LLVM_LIBRARY_DIRS})
    add_definitions(${LLVM_DEFINITIONS})

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

    find_library(LLD_LOONG NAMES libLLVMLoongArchCodeGen.lib libLLVMLoongArchAsmParser.lib libLLVMLoongArchCodeGen.a libLLVMLoongArchAsmParser.a PATHS ${LLVM_LIBRARY_DIRS} NO_DEFAULT_PATH)
    set(lld_libs
        ${LLD_COFF}
        ${LLD_WASM}
        ${LLD_MINGW}
        ${LLD_ELF}
        ${LLD_MACHO}
        ${LLD_COMMON}
    )

    message(STATUS "linking to llvm libs ${lld_libs}")
    message(STATUS "Found lld libs ${lld_libs}")
endif()

target_link_libraries(${PROJECT_NAME} ${llvm_libs} ${lld_libs})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}")
target_precompile_headers(${PROJECT_NAME} PUBLIC "pch.h")
