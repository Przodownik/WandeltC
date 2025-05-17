/**
 * @file compiler_internal.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/compiler_internal_enums.h"
#include "core/token.h"

#define MAX_FN_PARAMETERS  16
#define MAX_CALL_ARGUMENTS MAX_FN_PARAMETERS

typedef struct _Declaration Declaration;
typedef struct _Statement Statement;
typedef struct _Type Type;
typedef struct _Expression Expression;

typedef struct _CompilerBuildOptions
{
	const char* project_name;
	DeliverableType deliverable_type;
	File* file_sources;
	bool lexer_debug;
	bool parsed_debug;
} CompilerBuildOptions;

typedef struct _Type
{
	TypeKind kind;
} Type;

typedef struct _TypeInfo
{
	ResolveStatus resolve_status;
	Type* type;
} TypeInfo;

typedef struct _ForeignAttribute
{
	const char* foreign_name;
} ForeignAttribute;

typedef struct _Attribute
{
	AttributeKind kind;

	union {
		ForeignAttribute foreign;
	};
} Attribute;

bool has_attribute(Attribute* attributes, AttributeKind kind);

typedef struct _FunctionSignature
{
	Type* return_type;
	const char* name;
	Declaration** parameters; // nullable
	Attribute* attributes;    // nullable
	SourceSpan source_span;
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
	ResolveStatus resolve_status;

	void* handle;

	union {
		FunctionDeclaration function;
		VariableDeclaration variable;
	};
} Declaration;

typedef struct _CompountStatement
{
	Statement* first;
} CompountStatement;

typedef struct _DeclarationStatement
{
	Declaration* declaration;
} DeclarationStatement;

typedef struct _ExpressionStatement
{
	Expression* expression;
} ExpressionStatement;

typedef struct _ReturnStatement
{
	Expression* expression; // can be null for return;
} ReturnStatement;

typedef struct _IfStatement
{
	Expression* condition;
	Statement* then_branch;
	Statement* else_branch; // can be null
} IfStatement;

typedef struct _WhileStatement
{
	Expression* condition;
	Statement* body;
} WhileStatement;

typedef struct _Statement
{
	StatementKind kind;
	SourceSpan source_span;
	Statement* next; // next statement in the compound or null if it's the last one

	union {
		CompountStatement compound;
		DeclarationStatement declaration;
		ExpressionStatement expression;
		ReturnStatement return_;
		IfStatement if_;
		WhileStatement while_;
	};
} Statement;

typedef struct _ConstantExpression
{
	ConstantType type;

	// characters are represented by the integer value of the character (int8)
	union {
		bool bool_value;
		int64 int_value;
		float float_value;
		double double_value;
		struct
		{
			char* string_value;
			uint32 length;
		} string;
	};
} ConstantExpression;

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
	Declaration* refered; // declaration the identifier refers to (resolved during semantic analysis!)
} IdentifierExpression;

typedef struct _CastExpression
{
	Expression* expression;
	Type* cast_to;
	CastKind cast_kind;
} CastExpression;

typedef struct _CallExpression
{
	Expression* callee;
	Expression** arguments; // nullable
} CallExpression;

typedef struct _Expression
{
	ExpressionKind kind;
	SourceSpan source_span;
	ResolveStatus resolve_status;
	Type* type; // resultant type of the expression

	union {
		ConstantExpression constant;
		BinaryExpression binary;
		UnaryExpression unary;
		GroupedExpression group;
		IdentifierExpression identifier;
		CastExpression cast;
		CallExpression call;
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

#define TAB_SIZE 4

// Function to get the index of a character in the file content based on row and column
uint32 get_index_from_position(const File* file, uint32 row, uint32 column);

// Function to get the row and column from an index in the file content
void get_position_from_index(const File* file, uint32 index, uint32* row, uint32* column);

// get column with '\t' support
uint32 get_display_column(const char* line_start, uint32 char_column);

// Function to extend a SourceSpan with another SourceSpan, till its end
SourceSpan extend_span_with_token(SourceSpan loc, SourceSpan after);
