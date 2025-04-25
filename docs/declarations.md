# 📘 Declarations in the _Wandelt_ Language

This document defines the syntax, structure, and semantics of **declarations** in the _Wandelt_ programming language. Declarations introduce named program elements such as functions, variables, constants, and types available for use in the program.

---

## 🧩 Overview of Declaration Types

| Kind     | Description                                  |
| -------- | -------------------------------------------- |
| Function | Declares a reusable block of executable code |

> 💡 This document will be updated as new kinds of declarations are introduced.

---

## 🔧 Function Declarations

### 📌 Syntax

```plaintext
fn <return_type> <function_name>(<parameters>?)
{
    <body>
}
```

### 🧪 Examples

```c
fn int32 main()
{
    return 0;
}

fn int32 add(int32 a, int32 b)
{
    return a + b;
}
```

### 🧱 Components

| Part              | Meaning                                                        |
| ----------------- | -------------------------------------------------------------- |
| `fn`              | Keyword introducing a function declaration                     |
| `<return_type>`   | The type of value the function returns (e.g., `int32`, `void`) |
| `<function_name>` | The identifier for the function                                |
| `<parameters>`    | (Optional) A comma-separated list of typed parameters          |
| `<body>`          | A block of statements enclosed in `{}`                         |

### 📐 Semantics

- The function must return a value of the declared return type. If the return type is `void`, no value is returned.
- Execution starts at the function named `main`.
- Function names must be unique within the same scope.
- Parameters are optional; a function can be declared without any parameters.

---
