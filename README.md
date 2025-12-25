# MiniLang – A Small Dynamic Language in C

MiniLang is a small, dynamically-typed interpreted programming language written in C.
The project focuses on **language runtime design**, **AST evaluation**, and **polymorphic objects**, with the long-term goal of evolving into a **bytecode VM–based interpreter** suitable for constrained systems (e.g. RV32).

This project is educational but intentionally designed with **real language implementation principles**.

---

## Features (Current)

- Dynamically typed objects:
  - `real` (double)
  - `bool`
  - `string`
- Polymorphic object system using **C vtables**
- AST-based interpreter (`eval`)
- Expressions:
  - Arithmetic (`+`, `*`, etc.)
  - Type-dependent operator behavior
- Variables and assignments
- Block execution
- Global environment (`Env_t`)
- Built-in `dump` (`?x`) for debugging
- AST visualization using **Graphviz DOT**

---

## Design Overview

### Object System

All runtime values are represented by:

```c
typedef struct Object_t {
    ObjectType type;
    Vtable_t* vt;
    void* data;
} Object_t;
