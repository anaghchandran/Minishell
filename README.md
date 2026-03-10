# 🐚 Mini Shell

A lightweight **Linux command-line shell** built from scratch in C. Supports command execution, piping, and Unix signal handling — replicating core functionality of bash/sh.

---

## 📌 Features

- Execute any Linux command (`ls`, `pwd`, `grep`, etc.)
- **N-level command piping** (`cmd1 | cmd2 | cmd3 | ...`)
- Signal handling — `Ctrl+C` (SIGINT) and `Ctrl+Z` (SIGTSTP)
- Process management using `fork()`, `execvp()`, `wait()`
- Clean command parsing and tokenization

---

## 🛠️ Tech Stack

| Component | Details |
|---|---|
| Language | Advanced C |
| OS | Linux (Ubuntu) |
| System Calls | fork(), execvp(), wait(), pipe() |
| IPC | Pipe |
| Signals | SIGINT, SIGTSTP |
| Build | GCC, Makefile |

---

## 🏗️ How It Works

```
User Input
    |
    v
Command Parser (tokenize input)
    |
    v
Pipe Detection
    |
   / \
  /   \
No    Yes
Pipe  Pipe
 |      |
fork() fork() chain
execvp() with pipe() fd
wait()  redirect
    \   /
     \ /
      v
Output to Terminal
```

---

## 📂 Project Structure

```
Minishell/
├── main.c          # Shell loop and input handling
├── execute.c       # Command execution logic
├── execute.h
├── pipe.c          # Pipe handling for chained commands
├── pipe.h
├── signals.c       # Signal handler setup
├── signals.h
└── Makefile
```

---

## 🚀 How to Run

```bash
git clone https://github.com/anaghchandran/Minishell.git
cd Minishell
make
./minishell
```

### Example Usage

```bash
$ ls -l | grep ".c" | wc -l
$ pwd
$ echo "Hello from Minishell"
```

---

## 💡 Key Learnings

- Linux process creation and management
- Inter-process communication using pipes
- Signal handling in Unix systems
- N-level pipe chaining logic

---

## 👤 Author

**Anagh CS** — Embedded Systems Engineer  
[GitHub](https://github.com/anaghchandran) | [LinkedIn](https://www.linkedin.com/in/anagh-chandran)
