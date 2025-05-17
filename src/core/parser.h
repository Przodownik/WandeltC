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

bool parser_expect(Parser* parser, TokenType expected_type);

bool parser_advance_and_expect(Parser* parser, TokenType expected_type);

bool try_advance(Parser* parser, TokenType type);

void recover_from_error(Parser* parser);

void parser_report_error(SourceSpan* location, const char* message, ...);

bool parse_identifier(Parser* parser, const char** identifier);

bool parse_type(Parser* parser, Type** type);

bool parse_parameters(Parser* parser, Declaration*** parameters);

bool parse_function_signature(Parser* parser, FunctionSignature* signature);

Expression* parse_expression(Parser* parser);

Statement* parse_return_statement(Parser* parser);

Statement* parse_statement(Parser* parser);

Statement* parse_compound_statement(Parser* parser);

Declaration* parse_top_level_statement(Parser* parser);

void parser_parse(Parser* parser);
