# 🖥️ CoreShell — OS-Inspired Shell with Paging & Scheduling (C)

A custom command-line shell written in C that simulates core operating system concepts including **process scheduling, paging, memory management, and page replacement**.

---

## 🚀 Features

### 🧠 Command Interpreter

Supports the following commands:

* `help`
* `quit`
* `set VAR STRING`
* `print VAR`
* `run SCRIPT.TXT`
* `exec p1 [p2] [p3] POLICY`
* `my_ls`
* `echo STRING | $VAR`

---

### 🔁 Process Scheduling

Implements multiple scheduling algorithms:

* **FCFS (First Come First Serve)**
* **RR (Round Robin, quantum = 2)**
* **SJF (Shortest Job First)**
* **AGING (priority improvement over time)**

---

### 📄 Paging System

* Programs are divided into **pages of 3 lines**
* Pages are loaded into a **frame store**
* Each process maintains a **page table**
* Supports:

  * Page faults
  * Page replacement (victim selection)
  * Lazy loading

---

### 💾 Memory Management

* **Frame Store** (simulated physical memory)
* **Variable Store** (key-value shell memory)
* Configurable sizes:

  * Example: `Frame Store Size = 21`, `Variable Store Size = 10`

---

### ⚠️ Page Fault Handling

When memory is full and a new page must be loaded:

```text
Page fault! Victim page contents:
<old page contents>
End of victim page contents.
```

---

### 📂 File System Interaction

* `my_ls` → lists files in current directory (via system call)
* Programs are copied into a simulated disk directory:

```text
BackingStore/
```

---

## 🧱 Project Structure

```text
CoreShell/
│
├── shell.c            # Main shell loop
├── interpreter.c      # Command parsing & execution
├── kernel.c           # Scheduler + paging logic
├── pcb.c              # Process Control Block
├── cpu.c              # CPU simulation
├── shellmemory.c      # Memory (frames + variables)
│
├── *.h                # Header files
├── Makefile           # Build config
│
├── prog1 ... prog12   # Test programs
├── tc1 ... tc5        # Test cases
└── BackingStore/      # Simulated disk (runtime)
```

---

## ⚙️ Build & Run

### 🔧 Compile

```bash
make
```

### ▶️ Run shell

```bash
./mysh
```

---

## 🧪 Testing

### Run test cases

```bash
./mysh < tc1
./mysh < tc2
./mysh < tc3
./mysh < tc4
./mysh < tc5
```

### Compare output

```bash
./mysh < tc3 > output.txt
diff output.txt tc3_result
```

---

## 🧠 Key Concepts Demonstrated

* Virtual Memory (paging, page tables)
* Page Replacement (victim selection)
* CPU Scheduling algorithms
* Process lifecycle management (PCB)
* Memory abstraction & simulation
* System-level programming in C

---

## ⚠️ Notes

* Designed for educational purposes (Operating Systems simulation)
* Behavior depends on environment (requires Unix-like commands for `cp`, `rm`, `ls`)
* Works best in **MSYS2 / Git Bash / Linux**

---

## 🛠️ Future Improvements

* More realistic AGING implementation
* Improved page replacement strategies (LRU, FIFO)
* Dynamic memory resizing
* Cross-platform filesystem support
* Better error handling

---

## 👤 Author

**Jin Yang**
Computer Engineering @ York University

---

## 📜 License

This project is for educational use.
