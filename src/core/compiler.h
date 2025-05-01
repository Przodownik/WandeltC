/**
 * @file compiler.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/compiler_internal.h"

typedef struct _Compiler
{
	CompilerBuildOptions build_options;
} Compiler;

void compiler_create(Compiler* compiler);

void compiler_lex(Compiler* compiler);

void compiler_parse(Compiler* compiler);

void compiler_compile(Compiler* compiler);
