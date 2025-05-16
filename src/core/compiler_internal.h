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

#define MAX_FN_PARAMETERS 16

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

typedef enum _AttributeKind
{
	ATTRIBUTE_INVALID = 0,
	ATTRIBUTE_FOREIGN,
} AttributeKind;

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

typedef enum _StatementKind
{
	STATEMENT_INVALID = 0,
	STATEMENT_COMPOUND,
	STATEMENT_DECLARATION,
	STATEMENT_EXPRESSION,
	STATEMENT_RETURN,
	STATEMENT_IF,
	STATEMENT_WHILE,
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
	EXPRESSION_CALL,
} ExpressionKind;

typedef enum _ConstantType
{
	CONSTANT_TYPE_INVALID = 0,
	CONSTANT_TYPE_BOOL,
	CONSTANT_TYPE_CHAR,
	CONSTANT_TYPE_INT,
	CONSTANT_TYPE_FLOAT,
	CONSTANT_TYPE_DOUBLE,
	CONSTANT_TYPE_STRING,
} ConstantType;

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

typedef enum _CastKind
{
	CAST_INVALID = 0,

	// bool
	CAST_BOOL_TO_CHAR,
	CAST_BOOL_TO_UCHAR,
	CAST_BOOL_TO_SHORT,
	CAST_BOOL_TO_USHORT,
	CAST_BOOL_TO_INT,
	CAST_BOOL_TO_UINT,
	CAST_BOOL_TO_LONG,
	CAST_BOOL_TO_ULONG,
	CAST_BOOL_TO_FLOAT,
	CAST_BOOL_TO_DOUBLE,

	// char
	CAST_CHAR_TO_BOOL,
	CAST_CHAR_TO_UCHAR,
	CAST_CHAR_TO_SHORT,
	CAST_CHAR_TO_USHORT,
	CAST_CHAR_TO_INT,
	CAST_CHAR_TO_UINT,
	CAST_CHAR_TO_LONG,
	CAST_CHAR_TO_ULONG,
	CAST_CHAR_TO_FLOAT,
	CAST_CHAR_TO_DOUBLE,

	// uchar
	CAST_UCHAR_TO_BOOL,
	CAST_UCHAR_TO_CHAR,
	CAST_UCHAR_TO_SHORT,
	CAST_UCHAR_TO_USHORT,
	CAST_UCHAR_TO_INT,
	CAST_UCHAR_TO_UINT,
	CAST_UCHAR_TO_LONG,
	CAST_UCHAR_TO_ULONG,
	CAST_UCHAR_TO_FLOAT,
	CAST_UCHAR_TO_DOUBLE,

	// short
	CAST_SHORT_TO_BOOL,
	CAST_SHORT_TO_CHAR,
	CAST_SHORT_TO_UCHAR,
	CAST_SHORT_TO_USHORT,
	CAST_SHORT_TO_INT,
	CAST_SHORT_TO_UINT,
	CAST_SHORT_TO_LONG,
	CAST_SHORT_TO_ULONG,
	CAST_SHORT_TO_FLOAT,
	CAST_SHORT_TO_DOUBLE,

	// ushort
	CAST_USHORT_TO_BOOL,
	CAST_USHORT_TO_CHAR,
	CAST_USHORT_TO_UCHAR,
	CAST_USHORT_TO_SHORT,
	CAST_USHORT_TO_INT,
	CAST_USHORT_TO_UINT,
	CAST_USHORT_TO_LONG,
	CAST_USHORT_TO_ULONG,
	CAST_USHORT_TO_FLOAT,
	CAST_USHORT_TO_DOUBLE,

	// int
	CAST_INT_TO_BOOL,
	CAST_INT_TO_CHAR,
	CAST_INT_TO_UCHAR,
	CAST_INT_TO_SHORT,
	CAST_INT_TO_USHORT,
	CAST_INT_TO_UINT,
	CAST_INT_TO_LONG,
	CAST_INT_TO_ULONG,
	CAST_INT_TO_FLOAT,
	CAST_INT_TO_DOUBLE,

	// uint
	CAST_UINT_TO_BOOL,
	CAST_UINT_TO_CHAR,
	CAST_UINT_TO_UCHAR,
	CAST_UINT_TO_SHORT,
	CAST_UINT_TO_USHORT,
	CAST_UINT_TO_INT,
	CAST_UINT_TO_LONG,
	CAST_UINT_TO_ULONG,
	CAST_UINT_TO_FLOAT,
	CAST_UINT_TO_DOUBLE,

	// long
	CAST_LONG_TO_BOOL,
	CAST_LONG_TO_CHAR,
	CAST_LONG_TO_UCHAR,
	CAST_LONG_TO_SHORT,
	CAST_LONG_TO_USHORT,
	CAST_LONG_TO_INT,
	CAST_LONG_TO_UINT,
	CAST_LONG_TO_ULONG,
	CAST_LONG_TO_FLOAT,
	CAST_LONG_TO_DOUBLE,

	// ulong
	CAST_ULONG_TO_BOOL,
	CAST_ULONG_TO_CHAR,
	CAST_ULONG_TO_UCHAR,
	CAST_ULONG_TO_SHORT,
	CAST_ULONG_TO_USHORT,
	CAST_ULONG_TO_INT,
	CAST_ULONG_TO_UINT,
	CAST_ULONG_TO_LONG,
	CAST_ULONG_TO_FLOAT,
	CAST_ULONG_TO_DOUBLE,

	// float
	CAST_FLOAT_TO_BOOL,
	CAST_FLOAT_TO_CHAR,
	CAST_FLOAT_TO_UCHAR,
	CAST_FLOAT_TO_SHORT,
	CAST_FLOAT_TO_USHORT,
	CAST_FLOAT_TO_INT,
	CAST_FLOAT_TO_UINT,
	CAST_FLOAT_TO_LONG,
	CAST_FLOAT_TO_ULONG,
	CAST_FLOAT_TO_DOUBLE,

	// double
	CAST_DOUBLE_TO_BOOL,
	CAST_DOUBLE_TO_CHAR,
	CAST_DOUBLE_TO_UCHAR,
	CAST_DOUBLE_TO_SHORT,
	CAST_DOUBLE_TO_USHORT,
	CAST_DOUBLE_TO_INT,
	CAST_DOUBLE_TO_UINT,
	CAST_DOUBLE_TO_LONG,
	CAST_DOUBLE_TO_ULONG,
	CAST_DOUBLE_TO_FLOAT,
} CastKind;

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
