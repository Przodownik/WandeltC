/**
 * @file compiler_internal_enums.h
 * @author TF
 * @copyright Copyright (c) 2025 TF
 */
#pragma once

// -------------------------------------------------

typedef enum _DeliverableType
{
	DELIVERABLE_TYPE_EXECUTABLE,
	DELIVERABLE_TYPE_STATIC_LIB,
	DELIVERABLE_TYPE_DYNAMIC_LIB,

	DELIVERABLE_TYPE_COUNT,
} DeliverableType;

const char* deliverable_type_to_string(DeliverableType type);

// -------------------------------------------------

typedef enum _Visibility
{
	VISIBILITY_PUBLIC = 0,
	VISIBILITY_PRIVATE,

	VISIBILITY_COUNT,
} Visibility;

const char* visibility_to_string(Visibility visibility);

// -------------------------------------------------

typedef enum _ResolveStatus
{
	RESOLVE_STATUS_UNRESOLVED = 0,
	RESOLVE_STATUS_RESOLVING,
	RESOLVE_STATUS_RESOLVED,

	RESOLVE_STATUS_COUNT,
} ResolveStatus;

const char* resolve_status_to_string(ResolveStatus status);

// -------------------------------------------------

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

const char* type_kind_to_string(TypeKind kind);

// -------------------------------------------------

typedef enum _DeclKind
{
	DECLARATION_INVALID = 0,
	DECLARATION_FUNCTION,
	DECLARATION_VARIABLE,

	DECLARATION_COUNT,
} DeclKind;

const char* decl_kind_to_string(DeclKind kind);

// -------------------------------------------------

typedef enum _AttributeKind
{
	ATTRIBUTE_INVALID = 0,
	ATTRIBUTE_FOREIGN,

	ATTRIBUTE_COUNT,
} AttributeKind;

const char* attribute_kind_to_string(AttributeKind kind);

// -------------------------------------------------

typedef enum _StatementKind
{
	STATEMENT_INVALID = 0,
	STATEMENT_COMPOUND,
	STATEMENT_DECLARATION,
	STATEMENT_EXPRESSION,
	STATEMENT_RETURN,
	STATEMENT_IF,
	STATEMENT_WHILE,

	STATEMENT_COUNT,
} StatementKind;

const char* statement_kind_to_string(StatementKind kind);

// -------------------------------------------------

typedef enum _BinaryOperator
{
	BINARY_OPERATOR_INVALID = 0,
	BINARY_OPERATOR_ADD,
	BINARY_OPERATOR_SUBTRACT,
	BINARY_OPERATOR_MULTIPLY,
	BINARY_OPERATOR_DIVIDE,
	BINARY_OPERATOR_EQUAL,
	BINARY_OPERATOR_NOT_EQUAL,
	BINARY_OPERATOR_GREATER,
	BINARY_OPERATOR_LESS,
	BINARY_OPERATOR_GREATER_OR_EQUAL,
	BINARY_OPERATOR_LESS_OR_EQUAL,
	BINARY_OPERATOR_ASSIGN,
	BINARY_OPERATOR_MODULO,

	BINARY_OPERATOR_COUNT,
} BinaryOperator;

#define LOGICAL_OPERATORS                  \
	case BINARY_OPERATOR_EQUAL:            \
	case BINARY_OPERATOR_NOT_EQUAL:        \
	case BINARY_OPERATOR_GREATER:          \
	case BINARY_OPERATOR_LESS:             \
	case BINARY_OPERATOR_GREATER_OR_EQUAL: \
	case BINARY_OPERATOR_LESS_OR_EQUAL

const char* binary_operator_to_string(BinaryOperator op);

BinaryOperator token_type_to_binary_operator(int type);

// -------------------------------------------------

typedef enum _UnaryOperator
{
	UNARY_OPERATOR_INVALID = 0,
	UNARY_OPERATOR_NEGATE,

	UNARY_OPERATOR_COUNT,
} UnaryOperator;

const char* unary_operator_to_string(UnaryOperator op);

// -------------------------------------------------

typedef enum _AssignOperator
{
	ASSIGN_OPERATOR_INVALID = 0,
	ASSIGN_OPERATOR_ASSIGN,

	ASSIGN_OPERATOR_COUNT,
} AssignOperator;

const char* assign_operator_to_string(AssignOperator op);

// -------------------------------------------------

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

	EXPRESSION_COUNT,
} ExpressionKind;

const char* expression_kind_to_string(ExpressionKind kind);

// -------------------------------------------------

typedef enum _ConstantType
{
	CONSTANT_TYPE_INVALID = 0,
	CONSTANT_TYPE_BOOL,
	CONSTANT_TYPE_CHAR,
	CONSTANT_TYPE_INT,
	CONSTANT_TYPE_FLOAT,
	CONSTANT_TYPE_DOUBLE,
	CONSTANT_TYPE_STRING,

	CONSTANT_TYPE_COUNT,
} ConstantType;

const char* constant_type_to_string(ConstantType type);

// -------------------------------------------------

typedef enum _CastKind
{
	CAST_INVALID = 0,
	CAST_SAME_TYPE,

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

	CAST_COUNT,
} CastKind;

static_assert(CAST_COUNT == 112, "compiler_internal_cast_table must be updated to match "
                                 "the number of cast kinds in the enum.");

// Lookup table for cast kinds
static const CastKind compiler_internal_cast_table[TYPE_KIND_DOUBLE + 1][TYPE_KIND_DOUBLE + 1] =
    {
        [TYPE_KIND_VOID] =
            {
                [TYPE_KIND_VOID]   = CAST_INVALID,
                [TYPE_KIND_BOOL]   = CAST_INVALID,
                [TYPE_KIND_CHAR]   = CAST_INVALID,
                [TYPE_KIND_UCHAR]  = CAST_INVALID,
                [TYPE_KIND_SHORT]  = CAST_INVALID,
                [TYPE_KIND_USHORT] = CAST_INVALID,
                [TYPE_KIND_INT]    = CAST_INVALID,
                [TYPE_KIND_UINT]   = CAST_INVALID,
                [TYPE_KIND_LONG]   = CAST_INVALID,
                [TYPE_KIND_ULONG]  = CAST_INVALID,
                [TYPE_KIND_FLOAT]  = CAST_INVALID,
                [TYPE_KIND_DOUBLE] = CAST_INVALID,
            },
        [TYPE_KIND_BOOL] =
            {
                [TYPE_KIND_BOOL]   = CAST_SAME_TYPE,
                [TYPE_KIND_CHAR]   = CAST_BOOL_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_BOOL_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_BOOL_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_BOOL_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_BOOL_TO_INT,
                [TYPE_KIND_UINT]   = CAST_BOOL_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_BOOL_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_BOOL_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_BOOL_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_BOOL_TO_DOUBLE,
            },
        [TYPE_KIND_CHAR] =
            {
                [TYPE_KIND_BOOL]   = CAST_CHAR_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_SAME_TYPE,
                [TYPE_KIND_UCHAR]  = CAST_CHAR_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_CHAR_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_CHAR_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_CHAR_TO_INT,
                [TYPE_KIND_UINT]   = CAST_CHAR_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_CHAR_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_CHAR_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_CHAR_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_CHAR_TO_DOUBLE,
            },
        [TYPE_KIND_UCHAR] =
            {
                [TYPE_KIND_BOOL]   = CAST_UCHAR_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_UCHAR_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_SAME_TYPE,
                [TYPE_KIND_SHORT]  = CAST_UCHAR_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_UCHAR_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_UCHAR_TO_INT,
                [TYPE_KIND_UINT]   = CAST_UCHAR_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_UCHAR_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_UCHAR_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_UCHAR_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_UCHAR_TO_DOUBLE,
            },
        [TYPE_KIND_SHORT] =
            {
                [TYPE_KIND_BOOL]   = CAST_SHORT_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_SHORT_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_SHORT_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_SAME_TYPE,
                [TYPE_KIND_USHORT] = CAST_SHORT_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_SHORT_TO_INT,
                [TYPE_KIND_UINT]   = CAST_SHORT_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_SHORT_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_SHORT_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_SHORT_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_SHORT_TO_DOUBLE,
            },
        [TYPE_KIND_USHORT] =
            {
                [TYPE_KIND_BOOL]   = CAST_USHORT_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_USHORT_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_USHORT_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_USHORT_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_SAME_TYPE,
                [TYPE_KIND_INT]    = CAST_USHORT_TO_INT,
                [TYPE_KIND_UINT]   = CAST_USHORT_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_USHORT_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_USHORT_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_USHORT_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_USHORT_TO_DOUBLE,
            },
        [TYPE_KIND_INT] =
            {
                [TYPE_KIND_BOOL]   = CAST_INT_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_INT_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_INT_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_INT_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_INT_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_SAME_TYPE,
                [TYPE_KIND_UINT]   = CAST_INT_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_INT_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_INT_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_INT_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_INT_TO_DOUBLE,
            },
        [TYPE_KIND_UINT] =
            {
                [TYPE_KIND_BOOL]   = CAST_UINT_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_UINT_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_UINT_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_UINT_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_UINT_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_UINT_TO_INT,
                [TYPE_KIND_UINT]   = CAST_SAME_TYPE,
                [TYPE_KIND_LONG]   = CAST_UINT_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_UINT_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_UINT_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_UINT_TO_DOUBLE,
            },
        [TYPE_KIND_LONG] =
            {
                [TYPE_KIND_BOOL]   = CAST_LONG_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_LONG_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_LONG_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_LONG_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_LONG_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_LONG_TO_INT,
                [TYPE_KIND_UINT]   = CAST_LONG_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_SAME_TYPE,
                [TYPE_KIND_ULONG]  = CAST_LONG_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_LONG_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_LONG_TO_DOUBLE,
            },
        [TYPE_KIND_ULONG] =
            {
                [TYPE_KIND_BOOL]   = CAST_ULONG_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_ULONG_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_ULONG_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_ULONG_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_ULONG_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_ULONG_TO_INT,
                [TYPE_KIND_UINT]   = CAST_ULONG_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_ULONG_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_SAME_TYPE,
                [TYPE_KIND_FLOAT]  = CAST_ULONG_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_ULONG_TO_DOUBLE,
            },
        [TYPE_KIND_FLOAT] =
            {
                [TYPE_KIND_BOOL]   = CAST_FLOAT_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_FLOAT_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_FLOAT_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_FLOAT_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_FLOAT_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_FLOAT_TO_INT,
                [TYPE_KIND_UINT]   = CAST_FLOAT_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_FLOAT_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_FLOAT_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_SAME_TYPE,
                [TYPE_KIND_DOUBLE] = CAST_FLOAT_TO_DOUBLE,
            },
        [TYPE_KIND_DOUBLE] =
            {
                [TYPE_KIND_BOOL]   = CAST_DOUBLE_TO_BOOL,
                [TYPE_KIND_CHAR]   = CAST_DOUBLE_TO_CHAR,
                [TYPE_KIND_UCHAR]  = CAST_DOUBLE_TO_UCHAR,
                [TYPE_KIND_SHORT]  = CAST_DOUBLE_TO_SHORT,
                [TYPE_KIND_USHORT] = CAST_DOUBLE_TO_USHORT,
                [TYPE_KIND_INT]    = CAST_DOUBLE_TO_INT,
                [TYPE_KIND_UINT]   = CAST_DOUBLE_TO_UINT,
                [TYPE_KIND_LONG]   = CAST_DOUBLE_TO_LONG,
                [TYPE_KIND_ULONG]  = CAST_DOUBLE_TO_ULONG,
                [TYPE_KIND_FLOAT]  = CAST_DOUBLE_TO_FLOAT,
                [TYPE_KIND_DOUBLE] = CAST_SAME_TYPE,
            },
};

const char* cast_kind_to_string(CastKind kind);

// -------------------------------------------------
