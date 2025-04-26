/**
 * @file compiler.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/compiler_internal.h"

typedef enum _DeliverableType
{
	DELIVERABLE_TYPE_EXECUTABLE,
	DELIVERABLE_TYPE_STATIC_LIB,
	DELIVERABLE_TYPE_DYNAMIC_LIB
} DeliverableType;

typedef struct _CompilerBuildOptions
{
	const char* project_name;
	DeliverableType deliverable_type;
	File* file_sources;
	bool lexer_debug;
	bool parsed_debug;
} CompilerBuildOptions;

typedef struct _Compiler
{
	CompilerBuildOptions build_options;
} Compiler;

void compiler_create(Compiler* compiler);

void compiler_lex(Compiler* compiler);

void compiler_parse(Compiler* compiler);

void compiler_compile(Compiler* compiler);
