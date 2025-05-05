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
	DECLARATION_VARIABLE
} DeclKind;

typedef struct _FunctionSignature
{
	Type* return_type;
	const char* name;
	Declaration** parameters;
} FunctionSignature;

typedef struct _FunctionDeclaration
{
	FunctionSignature signature;
	Statement* body;
} FunctionDeclaration;

typedef struct _VariableDeclaration
{
	Type* type;
	const char* name;
	Expression* initializer;
} VariableDeclaration;

typedef struct _Declaration
{
	DeclKind kind;
	Visibility visibility;
	SourceSpan source_span;
	void* handle;

	union {
		FunctionDeclaration function;
		VariableDeclaration variable;
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
	Statement* next; // next statement in the compound or null if it's the last one

	union {
		CompountStatement compound;
		DeclarationStatement declaration;
		ReturnStatement return_;
	};
} Statement;

typedef enum _BinaryOperator
{
	BINARY_OPERATOR_INVALID  = 0,
	BINARY_OPERATOR_ADD      = TOKEN_PLUS,
	BINARY_OPERATOR_SUBTRACT = TOKEN_MINUS,
	BINARY_OPERATOR_MULTIPLY = TOKEN_STAR,
	BINARY_OPERATOR_DIVIDE   = TOKEN_SLASH
} BinaryOperator;

const char* binary_operator_to_string(BinaryOperator op);

typedef enum _UnaryOperator
{
	UNARY_OPERATOR_INVALID = 0,
	UNARY_OPERATOR_NEGATE,
} UnaryOperator;

typedef enum _ExpressionKind
{
	EXPRESSION_INVALID = 0,
	EXPRESSION_LITERAL,
	EXPRESSION_BINARY,
	EXPRESSION_UNARY,
	EXPRESSION_GROUP,
	EXPRESSION_IDENTIFIER,
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
	BinaryOperator operator;
	Expression* left;
	Expression* right;
} BinaryExpression;

typedef struct _UnaryExpression
{
	UnaryOperator operator;
	Expression* operand;
} UnaryExpression;

typedef struct _GroupedExpression
{
	Expression* expression;
} GroupedExpression;

typedef struct _IdentifierExpression
{
	const char* name;
	Declaration* refered; // declaration the identifier refers to
} IdentifierExpression;

typedef struct _Expression
{
	ExpressionKind kind;
	SourceSpan source_span;

	union {
		LiteralExpression literal;
		BinaryExpression binary;
		UnaryExpression unary;
		GroupedExpression group;
		IdentifierExpression identifier;
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

void global_context_emit_functions_json(Context* context, void* element);
void global_context_emit_json_to_file(Context* context, FILE* file);
void global_context_emit_json(Context* context);

// Function to get the index of a character in the file content based on row and column
uint32 get_index_from_position(const File* file, uint32 row, uint32 column);

// Function to get the row and column from an index in the file content
void get_position_from_index(const File* file, uint32 index, uint32* row, uint32* column);

// Function to extend a SourceSpan with another SourceSpan, till its end
SourceSpan extend_span_with_token(SourceSpan loc, SourceSpan after);
