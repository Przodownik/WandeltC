/**
 * @file compiler_internal.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/token.h"

typedef struct _Declaration Declaration;
typedef struct _Statement Statement;
typedef struct _Type Type;
typedef struct _Expression Expression;

typedef struct _SourceSpan
{
	const File* source_file;
	uint32 row;
	uint32 column;
	uint32 length;
} SourceSpan;

typedef struct _Token
{
	TokenType type;
	const char* lexeme;
	SourceSpan source_span;
} Token;

typedef enum _Visibility
{
	VISIBILITY_PUBLIC = 0,
	VISIBILITY_PRIVATE
} Visibility;

typedef enum _TypeKind
{
	TYPE_KIND_INT_32 = 0,
	TYPE_KIND_FUNCTION,
	TYPE_KIND_COUNT,
} TypeKind;

typedef struct _Type
{
	TypeKind kind;
} Type;

typedef enum _DeclKind
{
	DECLARATION_INVALID = 0,
	DECLARATION_FUNCTION,
} DeclKind;

typedef struct _FunctionSignature
{
	Type return_type;
	const char* name;
	Declaration** parameters;
} FunctionSignature;

typedef struct _FunctionDeclaration
{
	FunctionSignature signature;
	// Ast* body;
} FunctionDeclaration;

typedef struct _Declaration
{
	DeclKind kind;
	Visibility visibility;
	union {
		FunctionDeclaration function;
	};
} Declaration;

typedef enum _StatementType
{
	STATEMENT_INVALID = 0,
	STATEMENT_COMPOUND,
} StatementType;

typedef struct _CompountAst
{
	Statement** statements;
} CompountStatement;

typedef struct _Statement
{
	StatementType type;

	union {
		CompountStatement compound;
	};
} Statement;

typedef enum _ExpressionKind
{
	EXPRESSION_INVALID = 0,
	EXPRESSION_LITERAL,
	EXPRESSION_BINARY,
	EXPRESSION_UNARY,
} ExpressionKind;

typedef struct _LiteralExpression
{
	Type* type;

	union {
		int32 int_value;
	};
} LiteralExpression;

typedef struct _BinaryExpression
{
	// BinaryOperator operator;
	Expression* left;
	Expression* right;
} BinaryExpression;

typedef struct _UnaryExpression
{
	// UnaryOperator operator;
	Expression* operand;
} UnaryExpression;

typedef struct _Expression
{
	ExpressionKind kind;

	union {
		LiteralExpression literal;
		BinaryExpression binary;
		UnaryExpression unary;
	};
} Expression;

typedef struct _Context
{
	Declaration** functions_declarations;
} Context;

ArenaAllocator string_allocator;
ArenaAllocator declaration_allocator;

HashMap symbol_table;
HashMap type_table;

Context global_context;

void compiler_internal_initialize(void);

void compiler_internal_shutdown(void);

void global_context_emit_functions_json(Context* context, FILE* file);
void global_context_emit_json_to_file(Context* context, FILE* file);
void global_context_emit_json(Context* context);
