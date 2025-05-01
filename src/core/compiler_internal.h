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
	Statement* body;
} FunctionDeclaration;

typedef struct _Declaration
{
	DeclKind kind;
	Visibility visibility;
	SourceSpan source_span;
	void* handle;

	union {
		FunctionDeclaration function;
	};
} Declaration;

typedef enum _StatementType
{
	STATEMENT_INVALID = 0,
	STATEMENT_COMPOUND,
	STATEMENT_DECLARATION,
	STATEMENT_RETURN,
} StatementType;

typedef struct _CompountStatement
{
	Statement* first;
} CompountStatement;

typedef struct _DeclarationStatement
{
	Declaration* declaration;
} DeclarationStatement;

typedef struct _ReturnStatement
{
	Expression* expression; // can be null for return;
} ReturnStatement;

typedef struct _Statement
{
	StatementType type;
	SourceSpan source_span;
	Statement* next;

	union {
		CompountStatement compound;
		DeclarationStatement declaration;
		ReturnStatement return_;
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
	SourceSpan source_span;

	union {
		LiteralExpression literal;
		BinaryExpression binary;
		UnaryExpression unary;
	};
} Expression;

typedef struct _Context
{
	Declaration** functions_declarations;
	int32 warning_count;
	int32 error_count;
} Context;

ArenaAllocator string_allocator;
ArenaAllocator declaration_allocator;
ArenaAllocator statement_allocator;
ArenaAllocator expression_allocator;

HashMap symbol_table;
HashMap type_table;

Context global_context;

void compiler_internal_initialize(void);

void compiler_internal_shutdown(void);

void global_context_emit_functions_json(Context* context, FILE* file);
void global_context_emit_json_to_file(Context* context, FILE* file);
void global_context_emit_json(Context* context);

static inline SourceSpan extend_span_with_token(SourceSpan loc, SourceSpan after)
{
	if (loc.row != after.row)
		return loc;

	loc.length = after.column + after.length - loc.column;

	return loc;
}
