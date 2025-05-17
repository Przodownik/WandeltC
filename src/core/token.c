#include "token.h"

const char* token_type_to_string(TokenType type)
{
	switch (type)
	{
	case TOKEN_FUNCTION_KEYWORD:
		return "fn";
	case TOKEN_RETURN_KEYWORD:
		return "return";
	case TOKEN_TRUE_KEYWORD:
		return "true";
	case TOKEN_FALSE_KEYWORD:
		return "false";
	case TOKEN_PUBLIC_KEYWORD:
		return "public";
	case TOKEN_PRIVATE_KEYWORD:
		return "private";
	case TOKEN_IF_KEYWORD:
		return "if";
	case TOKEN_ELSE_KEYWORD:
		return "else";
	case TOKEN_WHILE_KEYWORD:
		return "while";
	case TOKEN_FOREIGN_KEYWORD:
		return "foreign";

	case TOKEN_VOID_KEYWORD:
		return "void";
	case TOKEN_BOOL_KEYWORD:
		return "bool";
	case TOKEN_CHAR_KEYWORD:
		return "char";
	case TOKEN_UCHAR_KEYWORD:
		return "uchar";
	case TOKEN_SHORT_KEYWORD:
		return "short";
	case TOKEN_USHORT_KEYWORD:
		return "ushort";
	case TOKEN_INT_KEYWORD:
		return "int";
	case TOKEN_UINT_KEYWORD:
		return "uint";
	case TOKEN_LONG_KEYWORD:
		return "long";
	case TOKEN_ULONG_KEYWORD:
		return "ulong";
	case TOKEN_FLOAT_KEYWORD:
		return "float";
	case TOKEN_DOUBLE_KEYWORD:
		return "double";

	case TOKEN_OPEN_PAREN:
		return "(";
	case TOKEN_CLOSE_PAREN:
		return ")";
	case TOKEN_OPEN_BRACE:
		return "{";
	case TOKEN_CLOSE_BRACE:
		return "}";
	case TOKEN_OPEN_BRACKET:
		return "[";
	case TOKEN_CLOSE_BRACKET:
		return "]";
	case TOKEN_PLUS:
		return "+";
	case TOKEN_MINUS:
		return "-";
	case TOKEN_STAR:
		return "*";
	case TOKEN_SLASH:
		return "/";
	case TOKEN_PERCENT:
		return "%";
	case TOKEN_EQUAL:
		return "=";
	case TOKEN_EXCLAMATION:
		return "!";
	case TOKEN_LESS:
		return "<";
	case TOKEN_GREATER:
		return ">";
	case TOKEN_COMMA:
		return ",";
	case TOKEN_DOT:
		return ".";
	case TOKEN_COLON:
		return ":";
	case TOKEN_SEMICOLON:
		return ";";
	case TOKEN_APOSTROPHE:
		return "'";
	case TOKEN_QUOTE:
		return "\"";
	case TOKEN_STAR_STAR:
		return "**";
	case TOKEN_EQUAL_EQUAL:
		return "==";
	case TOKEN_NOT_EQUAL:
		return "!=";
	case TOKEN_LESS_OR_EQUAL:
		return "<=";
	case TOKEN_GREATER_OR_EQUAL:
		return ">=";
	case TOKEN_ARROW:
		return "->";
	case TOKEN_COLON_COLON:
		return "::";
	case TOKEN_AT:
		return "@";

	case TOKEN_IDENTIFIER:
		return "identifier";
	case TOKEN_INTEGER:
		return "integer";
	case TOKEN_FLOAT:
		return "float";
	case TOKEN_DOUBLE:
		return "double";
	case TOKEN_STRING:
		return "string";
	case TOKEN_CHARACTER:
		return "character";
	case TOKEN_UNKNOWN:
		return "unknown";
	case TOKEN_EOF:
		return "EOF";
	case TOKEN_KEYWORD_COUNT:
	default:
		break;
	}

	UNREACHABLE;
}

const char* token_type_to_enum_stringified(TokenType type)
{
	switch (type)
	{
	case TOKEN_FUNCTION_KEYWORD:
		return "TOKEN_FUNCTION_KEYWORD";
	case TOKEN_RETURN_KEYWORD:
		return "TOKEN_RETURN_KEYWORD";
	case TOKEN_TRUE_KEYWORD:
		return "TOKEN_TRUE_KEYWORD";
	case TOKEN_FALSE_KEYWORD:
		return "TOKEN_FALSE_KEYWORD";
	case TOKEN_PUBLIC_KEYWORD:
		return "TOKEN_PUBLIC_KEYWORD";
	case TOKEN_PRIVATE_KEYWORD:
		return "TOKEN_PRIVATE_KEYWORD";
	case TOKEN_IF_KEYWORD:
		return "TOKEN_IF_KEYWORD";
	case TOKEN_ELSE_KEYWORD:
		return "TOKEN_ELSE_KEYWORD";
	case TOKEN_WHILE_KEYWORD:
		return "TOKEN_WHILE_KEYWORD";
	case TOKEN_FOREIGN_KEYWORD:
		return "TOKEN_FOREIGN_KEYWORD";

	case TOKEN_VOID_KEYWORD:
		return "TOKEN_VOID_KEYWORD";
	case TOKEN_BOOL_KEYWORD:
		return "TOKEN_BOOL_KEYWORD";
	case TOKEN_CHAR_KEYWORD:
		return "TOKEN_CHAR_KEYWORD";
	case TOKEN_UCHAR_KEYWORD:
		return "TOKEN_UCHAR_KEYWORD";
	case TOKEN_SHORT_KEYWORD:
		return "TOKEN_SHORT_KEYWORD";
	case TOKEN_USHORT_KEYWORD:
		return "TOKEN_USHORT_KEYWORD";
	case TOKEN_INT_KEYWORD:
		return "TOKEN_INT_KEYWORD";
	case TOKEN_UINT_KEYWORD:
		return "TOKEN_UINT_KEYWORD";
	case TOKEN_LONG_KEYWORD:
		return "TOKEN_LONG_KEYWORD";
	case TOKEN_ULONG_KEYWORD:
		return "TOKEN_ULONG_KEYWORD";
	case TOKEN_FLOAT_KEYWORD:
		return "TOKEN_FLOAT_KEYWORD";
	case TOKEN_DOUBLE_KEYWORD:
		return "TOKEN_DOUBLE_KEYWORD";

	case TOKEN_OPEN_PAREN:
		return "TOKEN_OPEN_PAREN";
	case TOKEN_CLOSE_PAREN:
		return "TOKEN_CLOSE_PAREN";
	case TOKEN_OPEN_BRACE:
		return "TOKEN_OPEN_BRACE";
	case TOKEN_CLOSE_BRACE:
		return "TOKEN_CLOSE_BRACE";
	case TOKEN_OPEN_BRACKET:
		return "TOKEN_OPEN_BRACKET";
	case TOKEN_CLOSE_BRACKET:
		return "TOKEN_CLOSE_BRACKET";
	case TOKEN_PLUS:
		return "TOKEN_PLUS";
	case TOKEN_MINUS:
		return "TOKEN_MINUS";
	case TOKEN_STAR:
		return "TOKEN_STAR";
	case TOKEN_SLASH:
		return "TOKEN_SLASH";
	case TOKEN_PERCENT:
		return "TOKEN_PERCENT";
	case TOKEN_EQUAL:
		return "TOKEN_EQUAL";
	case TOKEN_EXCLAMATION:
		return "TOKEN_EXCLAMATION";
	case TOKEN_LESS:
		return "TOKEN_LESS";
	case TOKEN_GREATER:
		return "TOKEN_GREATER";
	case TOKEN_COMMA:
		return "TOKEN_COMMA";
	case TOKEN_DOT:
		return "TOKEN_DOT";
	case TOKEN_COLON:
		return "TOKEN_COLON";
	case TOKEN_SEMICOLON:
		return "TOKEN_SEMICOLON";
	case TOKEN_APOSTROPHE:
		return "TOKEN_APOSTROPHE";
	case TOKEN_QUOTE:
		return "TOKEN_QUOTE";
	case TOKEN_STAR_STAR:
		return "TOKEN_STAR_STAR";
	case TOKEN_EQUAL_EQUAL:
		return "TOKEN_EQUAL_EQUAL";
	case TOKEN_NOT_EQUAL:
		return "TOKEN_NOT_EQUAL";
	case TOKEN_LESS_OR_EQUAL:
		return "TOKEN_LESS_OR_EQUAL";
	case TOKEN_GREATER_OR_EQUAL:
		return "TOKEN_GREATER_OR_EQUAL";
	case TOKEN_ARROW:
		return "TOKEN_ARROW";
	case TOKEN_COLON_COLON:
		return "TOKEN_COLON_COLON";
	case TOKEN_AT:
		return "TOKEN_AT";

	case TOKEN_IDENTIFIER:
		return "TOKEN_IDENTIFIER";
	case TOKEN_INTEGER:
		return "TOKEN_INTEGER";
	case TOKEN_FLOAT:
		return "TOKEN_FLOAT";
	case TOKEN_DOUBLE:
		return "TOKEN_DOUBLE";
	case TOKEN_STRING:
		return "TOKEN_STRING";
	case TOKEN_CHARACTER:
		return "TOKEN_CHARACTER";
	case TOKEN_UNKNOWN:
		return "TOKEN_UNKNOWN";
	case TOKEN_EOF:
		return "TOKEN_EOF";
	case TOKEN_KEYWORD_COUNT:
	default:
		break;
	}

	UNREACHABLE;
}

bool is_token_type_a_type_token(TokenType type)
{
	return type > TOKEN_FOREIGN_KEYWORD && type < TOKEN_OPEN_PAREN;
}
