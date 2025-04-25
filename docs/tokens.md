# Recognized Tokens

This document describes the tokens recognized by the lexer, as defined in `core/token.h`. Tokens are categorized into keywords, single character tokens, double character tokens and any other complementary ones.

## Token Categories

### Keywords

Keywords are reserved words used for specific language constructs.

| Token    | Token Type               | Description                     |
| -------- | ------------------------ | ------------------------------- |
| `fn`     | `TOKEN_FUNCTION_KEYWORD` | Starts a function declaration   |
| `return` | `TOKEN_RETURN_KEYWORD`   | Starts return statement         |
| `int32`  | `TOKEN_INT32_KEYWORD`    | Specifies a 32-bit integer type |

### Single Character Tokens

Single character tokens represent operators, punctuation, or delimiters.

| Token | Token Type            | Description                         |
| ----- | --------------------- | ----------------------------------- |
| `(`   | `TOKEN_OPEN_PAREN`    | Opening parenthesis                 |
| `)`   | `TOKEN_CLOSE_PAREN`   | Closing parenthesis                 |
| `{`   | `TOKEN_OPEN_BRACE`    | Opening brace                       |
| `}`   | `TOKEN_CLOSE_BRACE`   | Closing brace                       |
| `[`   | `TOKEN_OPEN_BRACKET`  | Opening bracket                     |
| `]`   | `TOKEN_CLOSE_BRACKET` | Closing bracket                     |
| `+`   | `TOKEN_PLUS`          | Addition operator                   |
| `-`   | `TOKEN_MINUS`         | Subtraction operator                |
| `*`   | `TOKEN_STAR`          | Multiplication operator             |
| `/`   | `TOKEN_SLASH`         | Division operator                   |
| `%`   | `TOKEN_PERCENT`       | Modulus operator                    |
| `=`   | `TOKEN_EQUAL`         | Assignment or equality operator     |
| `!`   | `TOKEN_EXCLAMATION`   | Logical NOT operator                |
| `<`   | `TOKEN_LESS`          | Less than operator                  |
| `>`   | `TOKEN_GREATER`       | Greater than operator               |
| `,`   | `TOKEN_COMMA`         | Comma for separating elements       |
| `.`   | `TOKEN_DOT`           | Member access or decimal point      |
| `:`   | `TOKEN_COLON`         | Type annotation or label separator  |
| `;`   | `TOKEN_SEMICOLON`     | Statement terminator                |
| `'`   | `TOKEN_APOSTROPHE`    | Single quote for character literals |
| `"`   | `TOKEN_QUOTE`         | Double quote for string literals    |

### Double Character Tokens

Double character tokens represent compound operators or special constructs.

| Token | Token Type               | Description                       |
| ----- | ------------------------ | --------------------------------- |
| `**`  | `TOKEN_STAR_STAR`        | Exponentiation operator           |
| `==`  | `TOKEN_EQUAL_EQUAL`      | Equality comparison operator      |
| `!=`  | `TOKEN_NOT_EQUAL`        | Inequality comparison operator    |
| `<=`  | `TOKEN_LESS_OR_EQUAL`    | Less than or equal to operator    |
| `>=`  | `TOKEN_GREATER_OR_EQUAL` | Greater than or equal to operator |
| `->`  | `TOKEN_ARROW`            | Arrow operator                    |
| `::`  | `TOKEN_COLON_COLON`      | Scope operator                    |

### Other Tokens

Other tokens represent identifiers, literals, or special cases.

| Token       | Token Type         | Description                               |
| ----------- | ------------------ | ----------------------------------------- |
| `somename1` | `TOKEN_IDENTIFIER` | User-defined names (variables, functions) |
| `12`        | `TOKEN_NUMBER`     | Numeric literals                          |
| `"string"`  | `TOKEN_STRING`     | String literals                           |
| `'c'`       | `TOKEN_CHARACTER`  | Character literals                        |
| N/A         | `TOKEN_UNKNOWN`    | Unrecognized or invalid token             |
| N/A         | `TOKEN_EOF`        | End of file marker                        |

## Notes

- The `TOKEN_UNKNOWN` type is used for unrecognized sequences that do not match any valid token.
- The `TOKEN_EOF` type marks the end of the input stream.

For more details, refer to the `token.h` file.
