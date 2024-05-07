# Custom Mini Shell 

**Introduction:**
Welcome to our Mini Shell project! This documentation provides an overview of the technical aspects and key features of our implementation.

**Project Overview:**
Our Mini Shell is a command-line interface designed to execute shell commands with enhanced functionality, leveraging the power of Lex and Yacc for parsing and processing user input. The project emphasizes efficiency, scalability, and adherence to best practices in shell scripting and system-level programming.

**Key Features:**

1. **Lex & Yacc Integration:** 
   - Utilizes Lex and Yacc for robust command parsing, enabling support for complex grammar rules such as pipes, redirections, and background tasks.

2. **Process Management:**
   - Implements efficient process management techniques, including fork(), execvp(), and waitpid(), ensuring proper execution and synchronization of commands.

3. **File Redirection:**
   - Implements file redirection capabilities using dup2(), facilitating seamless redirection of input/output streams for enhanced control and flexibility.

4. **Pipeline Support:**
   - Supports command pipelines for chaining multiple commands together, enhancing data flow and enabling efficient data processing.

**Technical Expertise Demonstrated:**

- **Shell Scripting Proficiency:** Demonstrates expertise in shell scripting principles, showcasing precise command parsing and execution tailored to meet diverse user requirements.

- **System-Level Programming:** Exhibits proficiency in system-level programming, including process management, file I/O operations, and signal handling, ensuring robust performance and reliability.

- **Design Elegance:** Emphasizes an elegant design approach focused on optimizing performance, scalability, and maintainability, reflecting a commitment to technical excellence.

**Technical Stack:**

- **Languages:** C, Lex, Yacc
- **Tools/Technologies:** Unix shell scripting, Process management, File I/O, Signal handling

## Getting Started 🚀

### Prerequisites :toolbox:

Ensure you have the following installed on your system:

* Linux OS: Ubuntu or any Debian-based distribution.
* C Compiler
* make
* bison and flex

To install prerequisites on a Debian-based system, run:

```sh
sudo apt update && sudo apt install gcc make bison flex
```

### Building and Running :green_circle:

1. Clone this repository.
2. Navigate to the project directory.
3. Run `make`.

```sh
make
```

### Project Structure 📁

| File Name       | Description                                            |
|-----------------|--------------------------------------------------------|
| `command.cc`    | Implementation of shell commands.                     |
| `command.h`     | Structure definition for shell commands.               |
| `shell.l`       | Lexical structure definition using lex.                |
| `shell.y`       | Grammar and syntax definition using yacc.              |
| `Makefile`      | Build automation script.                               |
| `error.txt`     | Log file capturing error messages during execution.    |
| `log.txt`       | Log file containing execution details.                 |



## Contribution Guidelines :clinking_glasses:

Contributions are welcome! Feel free to fork this repository, make changes, and submit a pull request. Please adhere to the [Contribution Guidelines](CONTRIBUTING.md).
