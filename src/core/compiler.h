/**
 * @file compiler.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/compiler_internal.h"

typedef struct _CompilerBuildOptions
{
	char** file_sources;
} CompilerBuildOptions;

typedef struct _Compiler
{
	CompilerBuildOptions build_options;
} Compiler;

void compiler_create(Compiler* compiler);

void compiler_lex(void);
