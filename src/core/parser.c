#include "parser.h"

#include "diagnostics.h"

extern ArenaAllocator declaration_allocator; // from compiler_internal.h
extern HashMap type_table;                   // from compiler_internal.h

#define YELLOW_HIGHLIGHT(text)   ANSI_COLOR_YELLOW text ANSI_COLOR_RED
#define UNEXPECTED_TOKEN_MESSAGE "Unexpected token '" YELLOW_HIGHLIGHT("%s") "' found!"

static Declaration invalid_declaration = {.kind = DECLARATION_INVALID};

Parser parser_create(Context* context, Lexer* lexer)
{
	Parser parser;
	parser.context = context;
	parser.lexer   = lexer;

	return parser;
}

void parser_advance(Parser* parser)
{
	lexer_try_get_next_token(parser->lexer);

	Token token = parser->lexer->current_token;

	TRACE("Consuming token:  <Token id=\"%i\" type=\"%s\", value=\"%s\" />\n", parser->lexer->token_count,
	      token_type_to_enum_stringified(token.type), token.lexeme);

	if (token.type == TOKEN_EOF)
		return;
}

bool parser_expect(Parser* parser, TokenType expected_type)
{
	Token token = parser->lexer->current_token;

	if (token.type != expected_type)
	{
		parser_report_error(
		    &token.source_span,
		    "Unexpected token '" YELLOW_HIGHLIGHT("%s") "' found! Token '" YELLOW_HIGHLIGHT("%s") "' was expected!",
		    token.lexeme, token_type_to_string(expected_type));

		return false;
	}

	return true;
}

bool parser_advance_and_expect(Parser* parser, TokenType expected_type)
{
	parser_advance(parser);

	return parser_expect(parser, expected_type);
}

void recover_from_error(Parser* parser)
{
	// TRACE("ERROR happened \n");

	parser_advance(parser);

	while (parser->lexer->current_token.type != TOKEN_EOF)
	{
		switch (parser->lexer->current_token.type)
		{
		case TOKEN_FUNCTION_KEYWORD:
			TRACE("ERROR recovery \n");
			return;
		default:
			parser_advance(parser);
			break;
		}
	}
}

void parser_report_error(SourceSpan* location, const char* message, ...)
{
	va_list list;
	va_start(list, message);
	diagnostics_verror_along_span(location, message, list);
	va_end(list);
}

bool parse_identifier(Parser* parser, const char** identifier)
{
	if (parser->lexer->current_token.type != TOKEN_IDENTIFIER)
	{
		parser_report_error(&parser->lexer->current_token.source_span,
		                    "Expected an identifier and received '" YELLOW_HIGHLIGHT("%s") "'",
		                    parser->lexer->current_token.lexeme);
		return false;
	}

	*identifier = parser->lexer->current_token.lexeme;

	parser_advance(parser); // consume the identifier

	return true;
}

bool parse_type(Parser* parser, Type* type)
{
	TypeKind* kind = (TypeKind*)hash_map_get_value(&type_table, parser->lexer->current_token.lexeme);
	if (kind == nullptr)
	{
		parser_report_error(
		    &parser->lexer->current_token.source_span,
		    "Expected a valid return type and received '" YELLOW_HIGHLIGHT(
		        "%s") "'. Standard types include " YELLOW_HIGHLIGHT("void") ", " YELLOW_HIGHLIGHT("int32") ", etc.",
		    parser->lexer->current_token.lexeme);

		return false;
	}

	parser_advance(parser); // consume the type

	return true;
}

bool parse_parameters(Parser* parser, Declaration** parameters)
{
	if (!parser_expect(parser, TOKEN_OPEN_PAREN))
		return false;

	parser_advance(parser); // consume (

	if (!parser_expect(parser, TOKEN_CLOSE_PAREN))
		return false;

	parser_advance(parser); // consume )

	return true;
}

bool parse_function_signature(Parser* parser, FunctionSignature* signature)
{
	parser_advance(parser); // consume fn

	if (!parse_type(parser, &signature->return_type))
		return false;

	if (!parse_identifier(parser, &signature->name))
		return false;

	if (!parse_parameters(parser, signature->parameters))
		return false;

	return true;
}

Declaration* parse_top_level_statement(Parser* parser)
{
	Declaration* new_declaration = &invalid_declaration;

	Lexer* lexer = parser->lexer;

	switch (lexer->current_token.type)
	{
	case TOKEN_FUNCTION_KEYWORD:
		new_declaration       = arena_allocator_allocate(&declaration_allocator, sizeof(Declaration));
		new_declaration->kind = DECLARATION_FUNCTION;

		if (!parse_function_signature(parser, &new_declaration->function.signature))
			return &invalid_declaration;

		// parse_function_body();

		vector_push(parser->context->functions_declarations, new_declaration);

		break;
	default:
		parser_report_error(&lexer->current_token.source_span,
		                    UNEXPECTED_TOKEN_MESSAGE " A top-level statement was expected e.g. a " YELLOW_HIGHLIGHT(
		                        "function") " or a " YELLOW_HIGHLIGHT("variable") " declaration!",
		                    lexer->current_token.lexeme);
		break;
	}

	return new_declaration;
}

void parser_parse(Parser* parser)
{
	Lexer* lexer = parser->lexer;

	parser_advance(parser);

	while (lexer->current_token.type != TOKEN_EOF)
	{
		Declaration* decl = parse_top_level_statement(parser);

		if (decl->kind == DECLARATION_INVALID)
		{
			recover_from_error(parser);
		}
	}
}
