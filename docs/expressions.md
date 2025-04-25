# 📘 Expressions in the _Wandelt_ Language

Expressions are the **core building blocks of computation** in the _Wandelt_ language. They produce values and are used in variable assignments, return statements, function calls, and control flow conditions.

---

## 🧩 Overview of Expression Types

| Expression Type      | Description                              | Example         |
| -------------------- | ---------------------------------------- | --------------- |
| **Literal**          | Represents a fixed value                 | `42`, `"hello"` |
| **Binary Operation** | Combines two values with an operator     | `a + b`         |
| **Unary Operation**  | Applies an operator to a single operand  | `-x`, `!flag`   |
| **Function Call**    | Calls a function with arguments          | `add(1, 2)`     |
| **Parenthesized**    | Groups expressions to control precedence | `(a + b) * c`   |
| **Identifier**       | Refers to a named variable or parameter  | `x`, `count`    |

> 💡 This document will be updated as new kinds of expressions are introduced.

---

## 🔢 Literal Expressions

Literals directly represent constant values in code.

### Examples

```c
42         // integer
3.14       // floating-point
true       // boolean
"hello"    // string
```

---

## ➕ Binary Expressions

Combine two sub-expressions with an operator.

### Syntax

```plaintext
<left sub-expressions> <operator> <right sub-expressions>
```

### Supported Operators

| Operator | Meaning        | Notes |
| -------- | -------------- | ----- |
| `+`      | Addition       |       |
| `-`      | Subtraction    |       |
| `*`      | Multiplication |       |
| `/`      | Division       |       |
| `%`      | Modulus        |       |
| `==`     | Equality       |       |
| `!=`     | Inequality     |       |
| `<, >`   | Comparisons    |       |
| `<=, >=` | Comparisons    |       |

### Example

```c
a * (b + 3)
```

---

## ➖ Unary Expressions

A unary operator applies to a single expression.

### Syntax

```plaintext
<operator> <expression>
```

### Supported Operators

| Operator | Meaning     |
| -------- | ----------- |
| `-`      | Negation    |
| `!`      | Logical NOT |

### Example

```c
!flag
-x
```

---

## 📞 Function Call Expressions

Call a previously defined function with arguments.

### Syntax

```plaintext
<function_name>(<arg1>, <arg2>, ...)
```

### Example

```c
add(5, 3)
```

- The number and type of arguments must match the function's declaration.

---

## 🧠 Identifier Expressions

Refer to a declared variable or parameter.

### Example

```c
count
```

- The identifier must be in scope and must be previously declared.

---

## () Parenthesized Expressions

Used to **group** expressions and **override operator precedence**.

### Example

```c
(a + b) * c
```
