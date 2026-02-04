# DataBase_Engine_in_C
A minimal in-memory database engine written in C, inspired by SQLite and MySQL internals. Supports insert, select, delete, and a REPL-based SQL-like interface.


# Mini Database Engine in C

A minimal **in-memory database engine written in C**, inspired by how real-world databases like **SQLite** and **MySQL** work internally.

This project is designed as a learning-focused implementation that demonstrates **database engine fundamentals**, including command parsing, row storage, execution pipelines, and error handling — all written from scratch in **pure C**.

---

## Features

- Interactive REPL (Read–Eval–Print Loop)
- SQL-like commands:
  - `insert <id> <username> <email>`
  - `select`
  - `delete <id>`
- Fixed-size row storage
- Safe input parsing (buffer overflow protection)
- Clean execution pipeline with result codes
- In-memory table management
- Written in standard C (portable and lightweight)

---

## Why This Project?

Most tutorials show *how to use* databases.  
This project focuses on **how databases are built internally**.

By building a small database engine from scratch, this project explores:

- How rows are stored in memory
- How commands are parsed and executed
- How execution results are propagated
- How databases separate logic from presentation
- How low-level systems are structured in C

This makes it ideal for:
- Systems programming learners
- Computer science students
- Backend engineers curious about database internals
- Anyone preparing for low-level or C-based interviews

---

## Getting Started

### Requirements
- GCC or any C compiler
- Windows, Linux, or macOS

### Build
```sh
gcc main.c -o mini_db
```

### RUN
```sh
/.main_db
```
