#include "token.h"

const char* token_type_to_string(TokenType type)
{
	switch (type)
	{
	case TOKEN_FUNCTION_KEYWORD:
		return "fn";
	case TOKEN_RETURN_KEYWORD:
		return "return";
	case TOKEN_INT32_KEYWORD:
		return "int32";
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
	case TOKEN_IDENTIFIER:
		return "identifier";
	case TOKEN_NUMBER:
		return "number";
	case TOKEN_STRING:
		return "string";
	case TOKEN_CHARACTER:
		return "character";
	case TOKEN_UNKNOWN:
		return "unknown";
	case TOKEN_EOF:
		return "EOF";
	default:
		ASSERT(false, "Invalid token! %i", type);
		break;
	}
}
