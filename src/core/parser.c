#include "parser.h"

#include "core/token.h"
#include "diagnostics.h"
#include "utils/defines.h"

extern ArenaAllocator declaration_allocator; // from compiler_internal.h
extern ArenaAllocator statement_allocator;   // from compiler_internal.h
extern ArenaAllocator expression_allocator;  // from compiler_internal.h
extern HashMap type_table;                   // from compiler_internal.h
extern Context global_context;               // from compiler_internal.h

#define UNEXPECTED_TOKEN_MESSAGE "Unexpected token '" YHRT("%s") "' found!"
#define OK_OR_RET_FALSE(x) \
	if (!x)                \
	return false

static Declaration invalid_declaration = {.kind = DECLARATION_INVALID};
static Statement invalid_statement     = {.kind = DECLARATION_INVALID};
static Expression invalid_expression   = {.kind = EXPRESSION_INVALID};

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

inline Token parser_get_current_token(Parser* parser)
{
	return parser->lexer->current_token;
}

inline TokenType parser_get_token_type(Parser* parser)
{
	return parser_get_current_token(parser).type;
}

Token parser_get_token_and_advance(Parser* parser)
{
	Token token = parser_get_current_token(parser);

	parser_advance(parser);

	return token;
}

bool parser_expect(Parser* parser, TokenType expected_type)
{
	Token token = parser_get_current_token(parser);

	if (token.type != expected_type)
	{
		parser_report_error(&token.source_span,
		                    "Unexpected token '" YHRT("%s") "' found! Token '" YHRT("%s") "' was expected!",
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
	case TOKEN_EQUAL:
		return 1;
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
		                    "Expected an identifier and received '" YHRT("%s") "'",
		                    parser->lexer->current_token.lexeme);
		return false;
	}

	*identifier = parser->lexer->current_token.lexeme;

	parser_advance(parser); // consume the identifier

	return true;
}

// <type>
bool parse_type(Parser* parser, Type** type)
{
	Type** mapped_type = (Type**)hash_map_get_value(&type_table, parser->lexer->current_token.lexeme);
	if (mapped_type == nullptr)
	{
		parser_report_error(&parser->lexer->current_token.source_span,
		                    "Expected a valid return type and received '" YHRT("%s") "'. Standard types include " YHRT(
		                        "void") ", " YHRT("int") ", etc.",
		                    parser->lexer->current_token.lexeme);

		return false;
	}

	*type = *mapped_type;

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
	Token function_token = parser_get_token_and_advance(parser); // consume fn

	OK_OR_RET_FALSE(parse_type(parser, &signature->return_type));
	OK_OR_RET_FALSE(parse_identifier(parser, &signature->name));
	OK_OR_RET_FALSE(parse_parameters(parser, signature->parameters));

	Token close_paren_token = parser->lexer->previous_token;

	signature->source_span = extend_span_with_token(function_token.source_span, close_paren_token.source_span);

	return true;
}

Expression* parse_integer_constant_expression(Parser* parser)
{
	Type** mapped_type = (Type**)hash_map_get_value(&type_table, token_type_to_string(TOKEN_INT_KEYWORD));

	Expression* expression         = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind               = EXPRESSION_CONSTANT;
	expression->constant.int_value = atoi(parser->lexer->current_token.lexeme);
	expression->type               = *mapped_type;
	expression->constant.type      = CONSTANT_TYPE_INT;
	expression->resolve_status     = RESOLVE_STATUS_RESOLVED;
	expression->source_span        = parser->lexer->current_token.source_span;

	parser_advance(parser); // consume the number

	return expression;
}

Expression* parse_float_constant_expression(Parser* parser)
{
	Type** mapped_type = (Type**)hash_map_get_value(&type_table, token_type_to_string(TOKEN_FLOAT_KEYWORD));

	Expression* expression           = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind                 = EXPRESSION_CONSTANT;
	expression->constant.float_value = (float)atof(parser->lexer->current_token.lexeme);
	expression->type                 = *mapped_type;
	expression->constant.type        = CONSTANT_TYPE_FLOAT;
	expression->resolve_status       = RESOLVE_STATUS_RESOLVED;
	expression->source_span          = parser->lexer->current_token.source_span;

	parser_advance(parser); // consume the number

	return expression;
}

Expression* parse_double_constant_expression(Parser* parser)
{
	Type** mapped_type = (Type**)hash_map_get_value(&type_table, token_type_to_string(TOKEN_DOUBLE_KEYWORD));

	Expression* expression            = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind                  = EXPRESSION_CONSTANT;
	expression->constant.double_value = atof(parser->lexer->current_token.lexeme);
	expression->type                  = *mapped_type;
	expression->constant.type         = CONSTANT_TYPE_DOUBLE;
	expression->resolve_status        = RESOLVE_STATUS_RESOLVED;
	expression->source_span           = parser->lexer->current_token.source_span;

	parser_advance(parser); // consume the number

	return expression;
}

Expression* parse_character_constant_expression(Parser* parser)
{
	Type** mapped_type = (Type**)hash_map_get_value(&type_table, token_type_to_string(TOKEN_CHAR_KEYWORD));

	Expression* expression         = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind               = EXPRESSION_CONSTANT;
	expression->constant.int_value = parser->lexer->current_token.lexeme[0];
	expression->type               = *mapped_type;
	expression->constant.type      = CONSTANT_TYPE_INT;
	expression->resolve_status     = RESOLVE_STATUS_RESOLVED;
	expression->source_span        = parser->lexer->current_token.source_span;

	parser_advance(parser); // consume the character

	return expression;
}

Expression* parse_boolean_constant_expression(Parser* parser)
{
	Type** mapped_type = (Type**)hash_map_get_value(&type_table, token_type_to_string(TOKEN_BOOL_KEYWORD));

	Expression* expression          = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind                = EXPRESSION_CONSTANT;
	expression->constant.bool_value = parser->lexer->current_token.type == TOKEN_TRUE_KEYWORD;
	expression->type                = *mapped_type;
	expression->constant.type       = CONSTANT_TYPE_BOOL;
	expression->resolve_status      = RESOLVE_STATUS_RESOLVED;
	expression->source_span         = parser->lexer->current_token.source_span;

	parser_advance(parser); // consume the bool

	return expression;
}

Expression* parse_constant_expression(Parser* parser)
{
	switch (parser->lexer->current_token.type)
	{
	case TOKEN_INTEGER:
		return parse_integer_constant_expression(parser);
	case TOKEN_FLOAT:
		return parse_float_constant_expression(parser);
	case TOKEN_DOUBLE:
		return parse_double_constant_expression(parser);
	case TOKEN_CHARACTER:
		return parse_character_constant_expression(parser);
	case TOKEN_STRING:
		NOT_IMPLEMENTED;
		break;
	case TOKEN_TRUE_KEYWORD:
	case TOKEN_FALSE_KEYWORD:
		return parse_boolean_constant_expression(parser);
	default:
		break;
	}

	UNREACHABLE;
}

Expression* parse_grouped_expression(Parser* parser)
{
	OK_OR_RET_FALSE(parser_expect(parser, TOKEN_OPEN_PAREN));

	Token open_paren_token = parser_get_token_and_advance(parser); // consume (

	Expression* expression       = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind             = EXPRESSION_GROUP;
	expression->group.expression = parse_expression(parser);

	if (expression->group.expression->kind == EXPRESSION_INVALID)
		return &invalid_expression;

	if (!parser_expect(parser, TOKEN_CLOSE_PAREN))
		return &invalid_expression;

	Token close_paren_token = parser_get_token_and_advance(parser); // consume )

	expression->source_span = extend_span_with_token(open_paren_token.source_span, close_paren_token.source_span);

	return expression;
}

Expression* parse_cast_expression(Parser* parser)
{
	Expression* expression     = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
	expression->kind           = EXPRESSION_CAST;
	expression->resolve_status = RESOLVE_STATUS_UNRESOLVED; // cast_kind must be resolved by sema

	Token type_token = parser->lexer->current_token;

	if (!parse_type(parser, &expression->cast.cast_to))
		return &invalid_expression;

	expression->type = expression->cast.cast_to;

	if (!parser_expect(parser, TOKEN_OPEN_PAREN))
		return &invalid_expression;

	parser_advance(parser); // consume (

	expression->cast.expression = parse_expression(parser);

	if (expression->cast.expression->kind == EXPRESSION_INVALID)
		return &invalid_expression;

	if (!parser_expect(parser, TOKEN_CLOSE_PAREN))
		return &invalid_expression;

	Token close_paren_token = parser_get_token_and_advance(parser); // consume )

	expression->source_span = extend_span_with_token(type_token.source_span, close_paren_token.source_span);

	return expression;
}

Expression* parse_primary_expression(Parser* parser)
{
	switch (parser->lexer->current_token.type)
	{
	case TOKEN_INTEGER:
	case TOKEN_FLOAT:
	case TOKEN_DOUBLE:
	case TOKEN_CHARACTER:
	case TOKEN_STRING:
	case TOKEN_TRUE_KEYWORD:
	case TOKEN_FALSE_KEYWORD:
		return parse_constant_expression(parser);
	case TOKEN_OPEN_PAREN:
		return parse_grouped_expression(parser);
	case TOKEN_IDENTIFIER:
		Expression* expression      = arena_allocator_allocate(&expression_allocator, sizeof(Expression));
		expression->kind            = EXPRESSION_IDENTIFIER;
		expression->identifier.name = parser->lexer->current_token.lexeme;
		expression->source_span     = parser->lexer->current_token.source_span;

		parser_advance(parser); // consume the identifier

		return expression;
	TOKEN_TYPE_KINDS:
		return parse_cast_expression(parser);
		break;
	default:
		parser_report_error(&parser->lexer->current_token.source_span,
		                    "Expected an " YHRT("expression") ", but received a '" YHRT("%s") "'",
		                    parser->lexer->current_token.lexeme);

		return &invalid_expression;
	}
}

BinaryOperator token_type_to_binary_operator(TokenType type)
{
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
	default:
		ASSERT(false, "Invalid token type for binary operator");
		return BINARY_OPERATOR_INVALID;
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
		binary->binary.operator= token_type_to_binary_operator(operator);
		binary->binary.right = rhs;
		binary->source_span  = extend_span_with_token(lhs->source_span, rhs->source_span);

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
	Token return_token = parser_get_token_and_advance(parser); // consume return

	Statement* statement = arena_allocator_allocate(&statement_allocator, sizeof(Statement));
	statement->kind      = STATEMENT_RETURN;

	// if no expression, just return;
	if (parser_get_token_type(parser) != TOKEN_SEMICOLON)
	{
		statement->return_.expression = parse_expression(parser);

		if (statement->return_.expression->kind == EXPRESSION_INVALID)
			return &invalid_statement;
	}

	if (!parser_expect(parser, TOKEN_SEMICOLON))
		return &invalid_statement;

	Token semicolon_token = parser_get_token_and_advance(parser); // consume ;

	statement->source_span = extend_span_with_token(return_token.source_span, semicolon_token.source_span);

	return statement;
}

Statement* parse_variable_declaration_statement(Parser* parser)
{
	Token type_token = parser->lexer->current_token;

	Statement* statement = arena_allocator_allocate(&statement_allocator, sizeof(Statement));
	statement->kind      = STATEMENT_DECLARATION;

	statement->declaration.declaration       = arena_allocator_allocate(&declaration_allocator, sizeof(Declaration));
	statement->declaration.declaration->kind = DECLARATION_VARIABLE;
	statement->declaration.declaration->resolve_status = RESOLVE_STATUS_UNRESOLVED;

	if (!parse_type(parser, &statement->declaration.declaration->variable.type))
		return &invalid_statement;

	if (!parser_expect(parser, TOKEN_IDENTIFIER))
		return &invalid_statement;

	statement->declaration.declaration->variable.name = parser->lexer->current_token.lexeme;

	parser_advance(parser); // consume the identifier

	if (parser_get_token_type(parser) != TOKEN_EQUAL)
	{
		parser_report_error(&parser->lexer->current_token.source_span,
		                    "Variables must be initialized, when declared! Expected an assignment operator '=' after "
		                    "the variable name '" YHRT("%s") "'.",
		                    statement->declaration.declaration->variable.name);

		return &invalid_statement;
	}

	parser_advance(parser); // consume the equal sign

	statement->declaration.declaration->variable.initializer = parse_expression(parser);

	if (statement->declaration.declaration->variable.initializer->kind == EXPRESSION_INVALID)
		return &invalid_statement;

	if (!parser_expect(parser, TOKEN_SEMICOLON))
		return &invalid_statement;

	Token semicolon_token = parser_get_token_and_advance(parser); // consume ;

	statement->source_span = extend_span_with_token(type_token.source_span, semicolon_token.source_span);

	return statement;
}

Statement* parse_expression_statement(Parser* parser)
{
	Token type_token = parser->lexer->current_token;

	Statement* statement             = arena_allocator_allocate(&statement_allocator, sizeof(Statement));
	statement->kind                  = STATEMENT_EXPRESSION;
	statement->expression.expression = parse_expression(parser);

	if (!parser_expect(parser, TOKEN_SEMICOLON))
		return &invalid_statement;

	Token semicolon_token = parser_get_token_and_advance(parser); // consume ;

	statement->source_span = extend_span_with_token(type_token.source_span, semicolon_token.source_span);

	return statement;
}

Statement* parse_statement(Parser* parser)
{
	switch (parser->lexer->current_token.type)
	{
	TOKEN_TYPE_KINDS:
		return parse_variable_declaration_statement(parser);

	case TOKEN_IDENTIFIER:
		return parse_expression_statement(parser);

	case TOKEN_RETURN_KEYWORD:
		return parse_return_statement(parser);

	case TOKEN_OPEN_BRACE:
		return parse_compound_statement(parser);

	case TOKEN_EOF:
		parser_report_error(
		    &parser->lexer->current_token.source_span,
		    "Reached the end of the file when expecting a statement. Did you forget to close a scope with '}'?");

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

	Token open_brace_token = parser_get_token_and_advance(parser); // consume {

	Statement* statement = arena_allocator_allocate(&statement_allocator, sizeof(Statement));
	statement->kind      = STATEMENT_COMPOUND;

	Statement** last_ptr = &statement->compound.first;

	while (!try_advance(parser, TOKEN_CLOSE_BRACE)) // process till the end of }
	{
		Statement* inner = parse_statement(parser);
		if (inner->kind == STATEMENT_INVALID)
			return &invalid_statement;

		*last_ptr = inner;
		last_ptr  = &inner->next;
	}

	Token close_brace_token = parser->lexer->previous_token;
	statement->source_span  = extend_span_with_token(open_brace_token.source_span, close_brace_token.source_span);

	return statement;
}

Declaration* parse_top_level_statement(Parser* parser)
{
	Declaration* new_declaration = &invalid_declaration;

	Lexer* lexer = parser->lexer;

	switch (lexer->current_token.type)
	{
	case TOKEN_FUNCTION_KEYWORD:
		new_declaration                 = arena_allocator_allocate(&declaration_allocator, sizeof(Declaration));
		new_declaration->kind           = DECLARATION_FUNCTION;
		new_declaration->resolve_status = RESOLVE_STATUS_UNRESOLVED;

		if (!parse_function_signature(parser, &new_declaration->function.signature))
			return &invalid_declaration;

		Statement* body = parse_compound_statement(parser);
		if (body->kind == STATEMENT_INVALID)
			return &invalid_declaration;

		new_declaration->function.body = body;
		new_declaration->source_span =
		    extend_span_with_token(new_declaration->function.signature.source_span, body->source_span);

		vector_push(parser->context->functions_declarations, new_declaration);

		break;
	default:
		parser_report_error(&lexer->current_token.source_span,
		                    UNEXPECTED_TOKEN_MESSAGE " A top-level statement was expected e.g. a " YHRT(
		                        "function") " or a " YHRT("variable") " declaration!",
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
