/**
 * @file parser.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/compiler_internal.h"
#include "core/lexer.h"

typedef struct _Parser
{
	Context* context;
	Lexer* lexer;
} Parser;

Parser parser_create(Context* context, Lexer* lexer);

void parser_advance(Parser* parser);

void recover_from_error(Parser* parser);

void parser_report_error(SourceSpan* location, const char* message, ...);

Declaration* parse_top_level_statement(Parser* parser);

void parser_parse(Parser* parser);
