#include "parser.h"

#include "core/token.h"
#include "diagnostics.h"

extern ArenaAllocator declaration_allocator; // from compiler_internal.h
extern ArenaAllocator statement_allocator;   // from compiler_internal.h
extern ArenaAllocator expression_allocator;  // from compiler_internal.h
extern HashMap type_table;                   // from compiler_internal.h
extern Context global_context;               // from compiler_internal.h

#define YELLOW_HIGHLIGHT(text)   ANSI_COLOR_YELLOW text ANSI_COLOR_RED
#define UNEXPECTED_TOKEN_MESSAGE "Unexpected token '" YELLOW_HIGHLIGHT("%s") "' found!"
#define OK_OR_RET_FALSE(x) \
	if (!x)                \
	return false

static Declaration invalid_declaration = {.kind = DECLARATION_INVALID};
static Statement invalid_statement     = {.type = DECLARATION_INVALID};
static Expression invalid_expression   = {.kind = EXPRESSION_INVALID};

static Type int32_type = {.kind = TYPE_KIND_INT_32};

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

bool try_advance(Parser* parser, TokenType type)
{
	if (parser->lexer->current_token.type == type)
	{
		parser_advance(parser);

		return true;
	}

	return false;
}

void recover_from_error(Parser* parser)
{
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
	global_context.error_count++;

	va_list list;
	va_start(list, message);
	diagnostics_verror_along_span(location, message, list);
	va_end(list);
}

#define TOKEN_BINARY_OPERATORS \
	case TOKEN_STAR:           \
	case TOKEN_SLASH:          \
	case TOKEN_PLUS:           \
	case TOKEN_MINUS

int8 parser_get_token_precedence(TokenType type)
{
	switch (type)
	{
	case TOKEN_STAR_STAR:
	case TOKEN_PERCENT:
		return 6;
	case TOKEN_STAR:
	case TOKEN_SLASH:
		return 5;
	case TOKEN_PLUS:
	case TOKEN_MINUS:
		return 4;
	case TOKEN_LESS:
	case TOKEN_LESS_OR_EQUAL:
	case TOKEN_GREATER:
	case TOKEN_GREATER_OR_EQUAL:
		return 3;
	case TOKEN_EQUAL_EQUAL:
	case TOKEN_NOT_EQUAL:
		return 2;
	default:
		return -1;
	}
}

// <identifier>
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

// <type>
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

	type->kind = *kind;

	parser_advance(parser); // consume the type

	return true;
}

// for now empty (), TODO: (<type> <identifier>, ...)
bool parse_parameters(Parser* parser, Declaration** parameters)
{
	OK_OR_RET_FALSE(parser_expect(parser, TOKEN_OPEN_PAREN));

	parser_advance(parser); // consume (

	OK_OR_RET_FALSE(parser_expect(parser, TOKEN_CLOSE_PAREN));

	parser_advance(parser); // consume )

	(void)parameters; // todo

	return true;
}

// fn <type> <identifier>(<type> <identifier>, ...)
bool parse_function_signature(Parser* parser, FunctionSignature* signature)
{
	parser_advance(parser); // consume fn

	OK_OR_RET_FALSE(parse_type(parser, &signature->return_type));
	OK_OR_RET_FALSE(parse_identifier(parser, &signature->name));
	OK_OR_RET_FALSE(parse_parameters(parser, signature->parameters));

	return true;
}

Expression* parse_literal_expression(Parser* parser)
{
	if (parser->lexer->current_token.type != TOKEN_NUMBER)
	{
		parser_report_error(&parser->lexer->current_token.source_span,
		                    "Expected a number and received '" YELLOW_HIGHLIGHT("%s") "'",
		                    parser->lexer->current_token.lexeme);
		return &invalid_expression;
	}

	Expression* expression        = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind              = EXPRESSION_LITERAL;
	expression->literal.int_value = atoi(parser->lexer->current_token.lexeme);
	expression->literal.type      = &int32_type; // TODO add type deduction

	parser_advance(parser); // consume the number

	return expression;
}

Expression* parse_grouped_expression(Parser* parser)
{
	OK_OR_RET_FALSE(parser_expect(parser, TOKEN_OPEN_PAREN));

	parser_advance(parser); // consume (

	Expression* expression       = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind             = EXPRESSION_GROUP;
	expression->group.expression = parse_expression(parser);

	if (expression->group.expression->kind == EXPRESSION_INVALID)
		return &invalid_expression;

	if (!parser_expect(parser, TOKEN_CLOSE_PAREN))
		return &invalid_expression;

	parser_advance(parser); // consume )

	return expression;
}

Expression* parse_primary_expression(Parser* parser)
{
	switch (parser->lexer->current_token.type)
	{
	case TOKEN_NUMBER:
		return parse_literal_expression(parser);
	case TOKEN_OPEN_PAREN:
		return parse_grouped_expression(parser);
	default:
		parser_report_error(
		    &parser->lexer->current_token.source_span,
		    "Expected an " YELLOW_HIGHLIGHT("expression") ", but received a '" YELLOW_HIGHLIGHT("%s") "'",
		    parser->lexer->current_token.lexeme);
		return &invalid_expression;
	}
}

Expression* parse_expression_rhs(Parser* parser, Expression* lhs, int8 min_precedence)
{
	while (true)
	{
		Token current_token = parser->lexer->current_token;
		int8 precedence     = parser_get_token_precedence(current_token.type);

		// Stop if not an operator or precedence too low
		if (precedence < min_precedence)
			return lhs;

		TokenType operator= current_token.type;
		parser_advance(parser); // consume the operator

		// Parse right-hand side
		Expression* rhs = parse_primary_expression(parser);
		if (rhs->kind == EXPRESSION_INVALID)
			return &invalid_expression;

		// Check for next operator with higher precedence
		int8 next_precedence = parser_get_token_precedence(parser->lexer->current_token.type);
		if (next_precedence > precedence)
		{
			rhs = parse_expression_rhs(parser, rhs, precedence + 1);
			if (rhs->kind == EXPRESSION_INVALID)
			{
				return &invalid_expression;
			}
		}

		Expression* binary  = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
		binary->kind        = EXPRESSION_BINARY;
		binary->binary.left = lhs;
		binary->binary.operator= operator;
		binary->binary.right = rhs;

		// Continue with the binary expression as the new left-hand side
		lhs = binary;
	}
}

Expression* parse_expression(Parser* parser)
{
	Expression* lhs = parse_primary_expression(parser);

	if (lhs->kind == EXPRESSION_INVALID)
		return &invalid_expression;

	return parse_expression_rhs(parser, lhs, 0);
}

Statement* parse_return_statement(Parser* parser)
{
	parser_advance(parser); // consume return

	Statement* statement          = arena_allocator_allocate(&statement_allocator, sizeof(Statement));
	statement->type               = STATEMENT_RETURN;
	statement->return_.expression = parse_expression(parser);

	if (statement->return_.expression->kind == EXPRESSION_INVALID)
		return &invalid_statement;

	if (!parser_expect(parser, TOKEN_SEMICOLON))
	{
		return &invalid_statement;
	}

	parser_advance(parser); // consume ;

	return statement;
}

Statement* parse_statement(Parser* parser)
{
	// TRACE("Type %s \n", parser->lexer->current_token.lexeme);
	switch (parser->lexer->current_token.type)
	{
	case TOKEN_RETURN_KEYWORD:
		return parse_return_statement(parser);

	case TOKEN_EOF:
		parser_report_error(&parser->lexer->current_token.source_span,
		                    "Reached the end of the file when expecting a statement.");

		return &invalid_statement;
	default:
		parser_report_error(&parser->lexer->current_token.source_span,
		                    UNEXPECTED_TOKEN_MESSAGE " A statement was expected!", parser->lexer->current_token.lexeme);

		return &invalid_statement;
	}
}

Statement* parse_compound_statement(Parser* parser)
{
	if (!parser_expect(parser, TOKEN_OPEN_BRACE))
		return &invalid_statement;

	parser_advance(parser); // consume {

	Statement* statement = arena_allocator_allocate(&statement_allocator, sizeof(Statement));
	statement->type      = STATEMENT_COMPOUND;

	Statement** last_ptr = &statement->compound.first;

	while (!try_advance(parser, TOKEN_CLOSE_BRACE)) // process till the end of }
	{
		Statement* inner = parse_statement(parser);
		if (inner->type == STATEMENT_INVALID)
			return &invalid_statement;

		*last_ptr = inner;
		last_ptr  = &inner->next;
	}

	return statement;
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

		Statement* body = parse_compound_statement(parser);
		if (body->type == STATEMENT_INVALID)
			return &invalid_declaration;

		new_declaration->function.body = body;

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
