#include "compiler_internal_enums.h"

#include "token.h"

const char* deliverable_type_to_string(DeliverableType type)
{
	static_assert(DELIVERABLE_TYPE_COUNT == 3,
	              "const char* deliverable_type_to_string(DeliverableType type) must be updated to match "
	              "the number of deliverable types in the enum.");

	switch (type)
	{
	case DELIVERABLE_TYPE_EXECUTABLE:
		return "executable";
	case DELIVERABLE_TYPE_STATIC_LIB:
		return "static library";
	case DELIVERABLE_TYPE_DYNAMIC_LIB:
		return "dynamic library";

	default:
		break;
	}

	UNREACHABLE;
}

const char* visibility_to_string(Visibility visibility)
{
	static_assert(VISIBILITY_COUNT == 2,
	              "const char* visibility_to_string(Visibility visibility) must be updated to match "
	              "the number of visibility types in the enum.");

	switch (visibility)
	{
	case VISIBILITY_PUBLIC:
		return "public";
	case VISIBILITY_PRIVATE:
		return "private";

	default:
		break;
	}

	UNREACHABLE;
}

const char* resolve_status_to_string(ResolveStatus status)
{
	static_assert(RESOLVE_STATUS_COUNT == 3,
	              "const char* resolve_status_to_string(ResolveStatus status) must be updated to match "
	              "the number of resolve statuses in the enum.");

	switch (status)
	{
	case RESOLVE_STATUS_UNRESOLVED:
		return "unresolved";
	case RESOLVE_STATUS_RESOLVING:
		return "resolving";
	case RESOLVE_STATUS_RESOLVED:
		return "resolved";

	default:
		break;
	}

	UNREACHABLE;
}

const char* type_kind_to_string(TypeKind kind)
{
	static_assert(TYPE_KIND_COUNT == 13, "const char* type_kind_to_string(TypeKind kind) must be updated to match "
	                                     "the number of type kinds in the enum.");

	switch (kind)
	{
	case TYPE_KIND_FUNCTION:
		return "function";

	case TYPE_KIND_VOID:
		return "void";
	case TYPE_KIND_BOOL:
		return "bool";
	case TYPE_KIND_CHAR:
		return "char";
	case TYPE_KIND_UCHAR:
		return "uchar";
	case TYPE_KIND_SHORT:
		return "short";
	case TYPE_KIND_USHORT:
		return "ushort";
	case TYPE_KIND_INT:
		return "int";
	case TYPE_KIND_UINT:
		return "uint";
	case TYPE_KIND_LONG:
		return "long";
	case TYPE_KIND_ULONG:
		return "ulong";
	case TYPE_KIND_FLOAT:
		return "float";
	case TYPE_KIND_DOUBLE:
		return "double";

	default:
		break;
	}

	UNREACHABLE;
}

const char* decl_kind_to_string(DeclKind kind)
{
	static_assert(DECLARATION_COUNT == 3, "const char* decl_kind_to_string(DeclKind kind) must be updated to match "
	                                      "the number of declaration kinds in the enum.");

	switch (kind)
	{
	case DECLARATION_INVALID:
		return "invalid";
	case DECLARATION_FUNCTION:
		return "function";
	case DECLARATION_VARIABLE:
		return "variable";

	default:
		break;
	}

	UNREACHABLE;
}

const char* attribute_kind_to_string(AttributeKind kind)
{
	static_assert(ATTRIBUTE_COUNT == 2,
	              "const char* attribute_kind_to_string(AttributeKind kind) must be updated to match "
	              "the number of attribute kinds in the enum.");

	switch (kind)
	{
	case ATTRIBUTE_INVALID:
		return "invalid";
	case ATTRIBUTE_FOREIGN:
		return "foreign";

	default:
		break;
	}

	UNREACHABLE;
}

const char* statement_kind_to_string(StatementKind kind)
{
	static_assert(STATEMENT_COUNT == 7,
	              "const char* statement_kind_to_string(StatementKind kind) must be updated to match "
	              "the number of statement kinds in the enum.");

	switch (kind)
	{
	case STATEMENT_INVALID:
		return "invalid";
	case STATEMENT_COMPOUND:
		return "compound";
	case STATEMENT_DECLARATION:
		return "declaration";
	case STATEMENT_EXPRESSION:
		return "expression";
	case STATEMENT_RETURN:
		return "return";
	case STATEMENT_IF:
		return "if";
	case STATEMENT_WHILE:
		return "while";

	default:
		break;
	}

	UNREACHABLE;
}

const char* binary_operator_to_string(BinaryOperator op)
{
	static_assert(BINARY_OPERATOR_COUNT == 13,
	              "const char* binary_operator_to_string(BinaryOperator op) must be updated to match "
	              "the number of binary operators in the enum.");

	switch (op)
	{
	case BINARY_OPERATOR_INVALID:
		return "invalid";
	case BINARY_OPERATOR_ADD:
		return "+";
	case BINARY_OPERATOR_SUBTRACT:
		return "-";
	case BINARY_OPERATOR_MULTIPLY:
		return "*";
	case BINARY_OPERATOR_DIVIDE:
		return "/";
	case BINARY_OPERATOR_EQUAL:
		return "==";
	case BINARY_OPERATOR_NOT_EQUAL:
		return "!=";
	case BINARY_OPERATOR_GREATER:
		return ">";
	case BINARY_OPERATOR_LESS:
		return "<";
	case BINARY_OPERATOR_GREATER_OR_EQUAL:
		return ">=";
	case BINARY_OPERATOR_LESS_OR_EQUAL:
		return "<=";
	case BINARY_OPERATOR_ASSIGN:
		return "=";
	case BINARY_OPERATOR_MODULO:
		return "%";

	default:
		break;
	}

	UNREACHABLE;
}

BinaryOperator token_type_to_binary_operator(int type)
{
	static_assert(TOKEN_TYPE_COUNT == 65,
	              "BinaryOperator token_type_to_binary_operator(int type) must be updated to match "
	              "the number of tokens in the enum.");

	switch (type)
	{
	case TOKEN_PLUS:
		return BINARY_OPERATOR_ADD;
	case TOKEN_MINUS:
		return BINARY_OPERATOR_SUBTRACT;
	case TOKEN_STAR:
		return BINARY_OPERATOR_MULTIPLY;
	case TOKEN_SLASH:
		return BINARY_OPERATOR_DIVIDE;
	case TOKEN_EQUAL_EQUAL:
		return BINARY_OPERATOR_EQUAL;
	case TOKEN_NOT_EQUAL:
		return BINARY_OPERATOR_NOT_EQUAL;
	case TOKEN_LESS:
		return BINARY_OPERATOR_LESS;
	case TOKEN_LESS_OR_EQUAL:
		return BINARY_OPERATOR_LESS_OR_EQUAL;
	case TOKEN_GREATER:
		return BINARY_OPERATOR_GREATER;
	case TOKEN_GREATER_OR_EQUAL:
		return BINARY_OPERATOR_GREATER_OR_EQUAL;
	case TOKEN_EQUAL:
		return BINARY_OPERATOR_ASSIGN;
	case TOKEN_PERCENT:
		return BINARY_OPERATOR_MODULO;
	default:
		return BINARY_OPERATOR_INVALID;
	}
}

const char* unary_operator_to_string(UnaryOperator op)
{
	static_assert(UNARY_OPERATOR_COUNT == 2,
	              "const char* unary_operator_to_string(UnaryOperator op) must be updated to match "
	              "the number of unary operators in the enum.");

	switch (op)
	{
	case UNARY_OPERATOR_INVALID:
		return "invalid";
	case UNARY_OPERATOR_NEGATE:
		return "-";

	default:
		break;
	}

	UNREACHABLE;
}

const char* assign_operator_to_string(AssignOperator op)
{
	static_assert(ASSIGN_OPERATOR_COUNT == 2,
	              "const char* assign_operator_to_string(AssignOperator op) must be updated to match "
	              "the number of assign operators in the enum.");

	switch (op)
	{
	case ASSIGN_OPERATOR_INVALID:
		return "invalid";
	case ASSIGN_OPERATOR_ASSIGN:
		return "=";

	default:
		break;
	}

	UNREACHABLE;
}

const char* expression_kind_to_string(ExpressionKind kind)
{
	static_assert(EXPRESSION_COUNT == 8,
	              "const char* expression_kind_to_string(ExpressionKind kind) must be updated to match "
	              "the number of expression kinds in the enum.");

	switch (kind)
	{
	case EXPRESSION_INVALID:
		return "invalid";
	case EXPRESSION_BINARY:
		return "binary";
	case EXPRESSION_UNARY:
		return "unary";
	case EXPRESSION_GROUP:
		return "group";
	case EXPRESSION_IDENTIFIER:
		return "identifier";
	case EXPRESSION_CONSTANT:
		return "constant";
	case EXPRESSION_CAST:
		return "cast";

	default:
		break;
	}

	UNREACHABLE;
}

const char* constant_type_to_string(ConstantType type)
{
	static_assert(CONSTANT_TYPE_COUNT == 7,
	              "const char* constant_type_to_string(ConstantType type) must be updated to match "
	              "the number of constant types in the enum.");

	switch (type)
	{
	case CONSTANT_TYPE_INVALID:
		return "invalid";
	case CONSTANT_TYPE_BOOL:
		return "bool";
	case CONSTANT_TYPE_CHAR:
		return "char";
	case CONSTANT_TYPE_INT:
		return "int";
	case CONSTANT_TYPE_FLOAT:
		return "float";
	case CONSTANT_TYPE_DOUBLE:
		return "double";
	case CONSTANT_TYPE_STRING:
		return "string";

	default:
		break;
	}

	UNREACHABLE;
}

const char* cast_kind_to_string(CastKind kind)
{
	static_assert(CAST_COUNT == 112, "const char* cast_kind_to_string(CastKind kind) must be updated to match "
	                                 "the number of cast kinds in the enum.");

	switch (kind)
	{
	case CAST_INVALID:
		return "invalid";
	case CAST_SAME_TYPE:
		return "same type";

	// bool
	case CAST_BOOL_TO_CHAR:
		return "bool to char";
	case CAST_BOOL_TO_UCHAR:
		return "bool to uchar";
	case CAST_BOOL_TO_SHORT:
		return "bool to short";
	case CAST_BOOL_TO_USHORT:
		return "bool to ushort";
	case CAST_BOOL_TO_INT:
		return "bool to int";
	case CAST_BOOL_TO_UINT:
		return "bool to uint";
	case CAST_BOOL_TO_LONG:
		return "bool to long";
	case CAST_BOOL_TO_ULONG:
		return "bool to ulong";
	case CAST_BOOL_TO_FLOAT:
		return "bool to float";
	case CAST_BOOL_TO_DOUBLE:
		return "bool to double";

	// char
	case CAST_CHAR_TO_BOOL:
		return "char to bool";
	case CAST_CHAR_TO_UCHAR:
		return "char to uchar";
	case CAST_CHAR_TO_SHORT:
		return "char to short";
	case CAST_CHAR_TO_USHORT:
		return "char to ushort";
	case CAST_CHAR_TO_INT:
		return "char to int";
	case CAST_CHAR_TO_UINT:
		return "char to uint";
	case CAST_CHAR_TO_LONG:
		return "char to long";
	case CAST_CHAR_TO_ULONG:
		return "char to ulong";
	case CAST_CHAR_TO_FLOAT:
		return "char to float";
	case CAST_CHAR_TO_DOUBLE:
		return "char to double";

	// uchar
	case CAST_UCHAR_TO_BOOL:
		return "uchar to bool";
	case CAST_UCHAR_TO_CHAR:
		return "uchar to char";
	case CAST_UCHAR_TO_SHORT:
		return "uchar to short";
	case CAST_UCHAR_TO_USHORT:
		return "uchar to ushort";
	case CAST_UCHAR_TO_INT:
		return "uchar to int";
	case CAST_UCHAR_TO_UINT:
		return "uchar to uint";
	case CAST_UCHAR_TO_LONG:
		return "uchar to long";
	case CAST_UCHAR_TO_ULONG:
		return "uchar to ulong";
	case CAST_UCHAR_TO_FLOAT:
		return "uchar to float";
	case CAST_UCHAR_TO_DOUBLE:
		return "uchar to double";

	// short
	case CAST_SHORT_TO_BOOL:
		return "short to bool";
	case CAST_SHORT_TO_CHAR:
		return "short to char";
	case CAST_SHORT_TO_UCHAR:
		return "short to uchar";
	case CAST_SHORT_TO_USHORT:
		return "short to ushort";
	case CAST_SHORT_TO_INT:
		return "short to int";
	case CAST_SHORT_TO_UINT:
		return "short to uint";
	case CAST_SHORT_TO_LONG:
		return "short to long";
	case CAST_SHORT_TO_ULONG:
		return "short to ulong";
	case CAST_SHORT_TO_FLOAT:
		return "short to float";
	case CAST_SHORT_TO_DOUBLE:
		return "short to double";

	// ushort
	case CAST_USHORT_TO_BOOL:
		return "ushort to bool";
	case CAST_USHORT_TO_CHAR:
		return "ushort to char";
	case CAST_USHORT_TO_UCHAR:
		return "ushort to uchar";
	case CAST_USHORT_TO_SHORT:
		return "ushort to short";
	case CAST_USHORT_TO_INT:
		return "ushort to int";
	case CAST_USHORT_TO_UINT:
		return "ushort to uint";
	case CAST_USHORT_TO_LONG:
		return "ushort to long";
	case CAST_USHORT_TO_ULONG:
		return "ushort to ulong";
	case CAST_USHORT_TO_FLOAT:
		return "ushort to float";
	case CAST_USHORT_TO_DOUBLE:
		return "ushort to double";

	// int
	case CAST_INT_TO_BOOL:
		return "int to bool";
	case CAST_INT_TO_CHAR:
		return "int to char";
	case CAST_INT_TO_UCHAR:
		return "int to uchar";
	case CAST_INT_TO_SHORT:
		return "int to short";
	case CAST_INT_TO_USHORT:
		return "int to ushort";
	case CAST_INT_TO_UINT:
		return "int to uint";
	case CAST_INT_TO_LONG:
		return "int to long";
	case CAST_INT_TO_ULONG:
		return "int to ulong";
	case CAST_INT_TO_FLOAT:
		return "int to float";
	case CAST_INT_TO_DOUBLE:
		return "int to double";

	// uint
	case CAST_UINT_TO_BOOL:
		return "uint to bool";
	case CAST_UINT_TO_CHAR:
		return "uint to char";
	case CAST_UINT_TO_UCHAR:
		return "uint to uchar";
	case CAST_UINT_TO_SHORT:
		return "uint to short";
	case CAST_UINT_TO_USHORT:
		return "uint to ushort";
	case CAST_UINT_TO_INT:
		return "uint to int";
	case CAST_UINT_TO_LONG:
		return "uint to long";
	case CAST_UINT_TO_ULONG:
		return "uint to ulong";
	case CAST_UINT_TO_FLOAT:
		return "uint to float";
	case CAST_UINT_TO_DOUBLE:
		return "uint to double";

	// long
	case CAST_LONG_TO_BOOL:
		return "long to bool";
	case CAST_LONG_TO_CHAR:
		return "long to char";
	case CAST_LONG_TO_UCHAR:
		return "long to uchar";
	case CAST_LONG_TO_SHORT:
		return "long to short";
	case CAST_LONG_TO_USHORT:
		return "long to ushort";
	case CAST_LONG_TO_INT:
		return "long to int";
	case CAST_LONG_TO_UINT:
		return "long to uint";
	case CAST_LONG_TO_ULONG:
		return "long to ulong";
	case CAST_LONG_TO_FLOAT:
		return "long to float";
	case CAST_LONG_TO_DOUBLE:
		return "long to double";

	// ulong
	case CAST_ULONG_TO_BOOL:
		return "ulong to bool";
	case CAST_ULONG_TO_CHAR:
		return "ulong to char";
	case CAST_ULONG_TO_UCHAR:
		return "ulong to uchar";
	case CAST_ULONG_TO_SHORT:
		return "ulong to short";
	case CAST_ULONG_TO_USHORT:
		return "ulong to ushort";
	case CAST_ULONG_TO_INT:
		return "ulong to int";
	case CAST_ULONG_TO_UINT:
		return "ulong to uint";
	case CAST_ULONG_TO_LONG:
		return "ulong to long";
	case CAST_ULONG_TO_FLOAT:
		return "ulong to float";
	case CAST_ULONG_TO_DOUBLE:
		return "ulong to double";

	// float
	case CAST_FLOAT_TO_BOOL:
		return "float to bool";
	case CAST_FLOAT_TO_CHAR:
		return "float to char";
	case CAST_FLOAT_TO_UCHAR:
		return "float to uchar";
	case CAST_FLOAT_TO_SHORT:
		return "float to short";
	case CAST_FLOAT_TO_USHORT:
		return "float to ushort";
	case CAST_FLOAT_TO_INT:
		return "float to int";
	case CAST_FLOAT_TO_UINT:
		return "float to uint";
	case CAST_FLOAT_TO_LONG:
		return "float to long";
	case CAST_FLOAT_TO_ULONG:
		return "float to ulong";
	case CAST_FLOAT_TO_DOUBLE:
		return "float to double";

	// double
	case CAST_DOUBLE_TO_BOOL:
		return "double to bool";
	case CAST_DOUBLE_TO_CHAR:
		return "double to char";
	case CAST_DOUBLE_TO_UCHAR:
		return "double to uchar";
	case CAST_DOUBLE_TO_SHORT:
		return "double to short";
	case CAST_DOUBLE_TO_USHORT:
		return "double to ushort";
	case CAST_DOUBLE_TO_INT:
		return "double to int";
	case CAST_DOUBLE_TO_UINT:
		return "double to uint";
	case CAST_DOUBLE_TO_LONG:
		return "double to long";
	case CAST_DOUBLE_TO_ULONG:
		return "double to ulong";
	case CAST_DOUBLE_TO_FLOAT:
		return "double to float";

	default:
		break;
	}

	UNREACHABLE;
}
