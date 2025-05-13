/**
 * @file compiler_internal.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

#include "core/token.h"

// Yellow highlight red text
#define YHRT(text) ANSI_COLOR_YELLOW text ANSI_COLOR_RED

// Yellow highlight orange text
#define YHOT(text) ANSI_COLOR_YELLOW text ANSI_COLOR_ORANGE

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

typedef enum _ResolveStatus
{
	RESOLVE_STATUS_UNRESOLVED = 0,
	RESOLVE_STATUS_RESOLVING,
	RESOLVE_STATUS_RESOLVED,
} ResolveStatus;

#define TOKEN_TYPE_KINDS       \
	case TOKEN_BOOL_KEYWORD:   \
	case TOKEN_CHAR_KEYWORD:   \
	case TOKEN_UCHAR_KEYWORD:  \
	case TOKEN_SHORT_KEYWORD:  \
	case TOKEN_USHORT_KEYWORD: \
	case TOKEN_INT_KEYWORD:    \
	case TOKEN_UINT_KEYWORD:   \
	case TOKEN_LONG_KEYWORD:   \
	case TOKEN_ULONG_KEYWORD:  \
	case TOKEN_FLOAT_KEYWORD:  \
	case TOKEN_DOUBLE_KEYWORD

typedef enum _TypeKind
{
	TYPE_KIND_FUNCTION = 0,

	TYPE_KIND_VOID,
	TYPE_KIND_BOOL,
	TYPE_KIND_CHAR,
	TYPE_KIND_UCHAR,
	TYPE_KIND_SHORT,
	TYPE_KIND_USHORT,
	TYPE_KIND_INT,
	TYPE_KIND_UINT,
	TYPE_KIND_LONG,
	TYPE_KIND_ULONG,
	TYPE_KIND_FLOAT,
	TYPE_KIND_DOUBLE,

	TYPE_KIND_COUNT,
} TypeKind;

typedef struct _Type
{
	TypeKind kind;
} Type;

typedef struct _TypeInfo
{
	ResolveStatus resolve_status;
	Type* type;
} TypeInfo;

const char* type_kind_to_string(TypeKind kind);

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

typedef enum _StatementKind
{
	STATEMENT_INVALID = 0,
	STATEMENT_COMPOUND,
	STATEMENT_DECLARATION,
	STATEMENT_EXPRESSION,
	STATEMENT_RETURN,
} StatementKind;

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
	};
} Statement;

#define LOGICAL_OPERATORS                  \
	case BINARY_OPERATOR_EQUAL:            \
	case BINARY_OPERATOR_NOT_EQUAL:        \
	case BINARY_OPERATOR_GREATER:          \
	case BINARY_OPERATOR_LESS:             \
	case BINARY_OPERATOR_GREATER_OR_EQUAL: \
	case BINARY_OPERATOR_LESS_OR_EQUAL

typedef enum _BinaryOperator
{
	BINARY_OPERATOR_INVALID          = 0,
	BINARY_OPERATOR_ADD              = TOKEN_PLUS,
	BINARY_OPERATOR_SUBTRACT         = TOKEN_MINUS,
	BINARY_OPERATOR_MULTIPLY         = TOKEN_STAR,
	BINARY_OPERATOR_DIVIDE           = TOKEN_SLASH,
	BINARY_OPERATOR_EQUAL            = TOKEN_EQUAL_EQUAL,
	BINARY_OPERATOR_NOT_EQUAL        = TOKEN_NOT_EQUAL,
	BINARY_OPERATOR_GREATER          = TOKEN_GREATER,
	BINARY_OPERATOR_LESS             = TOKEN_LESS,
	BINARY_OPERATOR_GREATER_OR_EQUAL = TOKEN_GREATER_OR_EQUAL,
	BINARY_OPERATOR_LESS_OR_EQUAL    = TOKEN_LESS_OR_EQUAL,
	BINARY_OPERATOR_ASSIGN           = TOKEN_EQUAL,
} BinaryOperator;

const char* binary_operator_to_string(BinaryOperator op);

typedef enum _UnaryOperator
{
	UNARY_OPERATOR_INVALID = 0,
	UNARY_OPERATOR_NEGATE,
} UnaryOperator;

const char* unary_operator_to_string(UnaryOperator op);

typedef enum _AssignOperator
{
	ASSIGN_OPERATOR_INVALID = 0,
	ASSIGN_OPERATOR_ASSIGN,
} AssignOperator;

const char* assign_operator_to_string(AssignOperator op);

typedef enum _ExpressionKind
{
	EXPRESSION_INVALID = 0,
	EXPRESSION_CONSTANT,
	EXPRESSION_BINARY,
	EXPRESSION_UNARY,
	EXPRESSION_GROUP,
	EXPRESSION_IDENTIFIER,
	EXPRESSION_CAST,
} ExpressionKind;

typedef enum _ConstantType
{
	CONSTANT_TYPE_INVALID = 0,
	CONSTANT_TYPE_INT,
	CONSTANT_TYPE_BOOL,
} ConstantType;

typedef struct _ConstantExpression
{
	ConstantType type;

	union {
		bool bool_value;
		int64 int_value;
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

typedef enum _CastKind
{
	CAST_INVALID = 0,
	CAST_BOOL_TO_INT32,
	CAST_INT32_TO_BOOL,
} CastKind;

typedef struct _CastExpression
{
	Expression* expression;
	Type* cast_to;
	CastKind cast_kind;
} CastExpression;

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
