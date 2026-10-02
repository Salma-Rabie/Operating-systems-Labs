[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/NwRY1vBU)
# Mini Shell Project

## Overview
This project implements a simple UNIX-like shell program called **myshell**. It allows users to execute basic commands, use input/output redirection, create command pipelines, and run processes in the background. Some parts of the project (like `tokenizer.cc`, `tokenizer.h`, and `command.h`) were provided, while the main logic for **parsing** and **executing** commands, along with the **Makefile**, were implemented in this work.

## Features
- Execute UNIX commands (e.g., `ls`, `cat`, `pwd`, etc.)
- Handle input/output redirection (`>`, `<`, `>>`)
- Support command pipelines using `|`
- Redirect error output using `2>` and `>>&`
- Support background execution using `&`
- Built-in commands:
  - `cd` — change the working directory
  - `exit` — exit the shell
- Logs terminated child processes into `log.txt` with timestamps
- Ignores `Ctrl+C` to prevent the shell from being terminated
- Automatically clears the log file when the shell starts

## File Structure
 -`command.cc`: Contains the main shell logic including the `parse()` and `execute()` implementations. 
 -`command.h`:  Declares the `Command` and `SimpleCommand` structures used by the shell. 
 -`tokenizer.cc`: Handles tokenizing user input into commands, arguments, and operators. 
 -`tokenizer.h`: Declares token types and the tokenizer function. 
 -`Makefile`: Used to build the project into an executable called `myshell`. 

## How to Compile and Run
1. Open a terminal in the project directory.
2. Compile the project using: make
3. Run the shell using: ./myshell
4. The prompt `myshell>` will appear, allowing you to type and execute commands.

## Example Commands
```bash
ls -l
cat < input.txt > output.txt
ls | grep .cpp
sleep 5 &
cd /home/user
exit
```

## Implementation Details
1-The **parse()** function :
The parse() function takes a list of tokens from the tokenizer and builds a structured representation of the command line.

    -It starts by resetting any previous command data.

    -It combines quoted strings into single arguments (e.g., "hello world" becomes one token instead of two).

    -Each token is analyzed:
        -Command and arguments are inserted into the current SimpleCommand.
        -Redirection symbols (>, >>, <, etc.) are detected, and the next token is treated as the filename for input or output.
        -Pipes (|) indicate the end of one command and the start of another.
        -Background symbol (&) marks the command to run without waiting for it to finish.

    -After parsing, the Command object is fully populated with all commands, arguments, and redirections.

    -Finally, it calls the execute() function to run the command(s).

This function ensures syntax correctness (like checking that every redirection operator has a filename afterward) and gracefully reports syntax errors.


2-The **execute()** function:
The execute() function is responsible for running the parsed command(s). It manages process creation, redirection, and logging.

    -Built-in Commands:
        -If the command is cd, it changes the current directory in the shell process itself.
        -If the command is exit, it prints a goodbye message and terminates the shell.

    -External Commands:
        -For normal commands, the shell forks a new child process.
        -Pipes are created between commands when necessary.
        -Each child process sets up its input/output redirection using dup2() and then executes the command using execvp().
        -The parent process waits for all foreground children to finish.

    -Logging: After each child finishes, the shell writes a message to log.txt with the process ID, exit status, and a timestamp. 
        -Example log entry:[2025-10-26 13:45:23] Child 1234 exited with status 0

    -Background Execution: If a command ends with &, the shell does not wait for it and prints its PID so the user knows it’s running in the background.

## Cleaning the Project
To remove compiled files and start fresh write in terminal: make clean

## Example Commands
echo Hello World
ls -l > output.txt
cat < input.txt | grep data > result.txt
sleep 5 &
cd ..
exit

## Logging Behavior
Each time the shell runs, it starts with an empty log.txt file. Every time a child process terminates, its completion details are appended to the file. This ensures the file only contains logs for the current session.

## Makefile Description
The Makefile compiles all .cc and .h files into object files and links them into one executable myshell.

    -make → builds the project
    -make clean → removes all object files and the executable


## Notes
- Provided: `tokenizer.h`, `tokenizer.cc`, `command.h`.
- Implemented: `parse()` and `execute()` functions in `command.cc`, plus the `Makefile`.
- Error handling was added for missing files, invalid syntax, and redirection issues.