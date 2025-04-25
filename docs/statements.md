# 📘 Statements in the _Wandelt_ Language

This document defines the syntax and semantics of **statements** in the _Wandelt_ programming language. Statements represent the building blocks of execution—they describe actions performed during the runtime of a program.

---

## 🧩 Overview of Statement Types

| Statement Type | Description                                      |
| -------------- | ------------------------------------------------ |
| `return`       | Exits a function and optionally provides a value |

> 💡 This document will be updated as new kinds of statements are introduced.

---

## 🔧 `return` Statement

### 📌 Syntax

```plaintext
return <expression>;
```

### 🧪 Examples

```c
return 42;

return a + b;
```

### 🧱 Components

| Part           | Description                                                                      |
| -------------- | -------------------------------------------------------------------------------- |
| `return`       | Keyword indicating function exit                                                 |
| `<expression>` | (Required) An expression that evaluates to a value of the function's return type |
| `;`            | Terminates the statement                                                         |

### 📐 Semantics

- Immediately exits the current function.
- Evaluates the expression and returns its result to the caller.
- The returned value must match the declared return type of the function.
- Execution after a `return` statement in the same block is **unreachable**.

### ⚠️ Rules

- Every non-void function **must** have a `return` statement in all control flow paths.
- `return` can only be used **inside** function bodies.
- `return` is **not** allowed in global or type-level contexts.
