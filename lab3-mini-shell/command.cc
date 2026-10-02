
/*
 * 
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <glob.h>
#include <wordexp.h>
#include <vector>
#include <string>
#include <cctype>
#include <iostream>
#include <sstream>
#include <errno.h>
#include "command.h"
#include "tokenizer.h"
#include <fstream>
#include <ctime>
#include <termios.h>



void parse(std::vector<Token> &tokens) 
{
    // reset the current global command
    Command &cmd = Command::_currentCommand;
    cmd.clear();

    // prepare first simple command
    SimpleCommand *curSimple = new SimpleCommand();
    TokenType redirectWaitingFor = TOKEN_EOF; // store which redirect type will be for the file after it
    
    // Step 1: Combine quoted tokens into one string
    std::vector<Token> mergedTokens;
    for (size_t i = 0; i < tokens.size(); ++i) {
        Token t = tokens[i];

        if (!t.value.empty() && t.value.front() == '"' && t.value.back() != '"') {
            // Start of a quoted string, but not closed yet
            std::string combined = t.value;

            // Keep adding tokens until we find one ending with '"'
            while (i + 1 < tokens.size()) {
                combined += " " + tokens[++i].value;
                if (!tokens[i].value.empty() && tokens[i].value.back() == '"') {
                    break;
                }
            }
            // Push back as one merged token (still same type)
            mergedTokens.push_back({TOKEN_ARGUMENT, combined});
        } 
        else {
            // Normal token
            mergedTokens.push_back(t);
        }
    }

    // Now use mergedTokens instead of tokens
    tokens = mergedTokens;
    

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        Token &t = tokens[i];

        if (t.type == TOKEN_EOF)
        {
            // finish last simple command if it has arguments
            if (curSimple->_numberOfArguments > 0)
            {
                cmd.insertSimpleCommand(curSimple);
                curSimple = new SimpleCommand();
            }
            break;
        }

        switch (t.type)
        {
        case TOKEN_COMMAND:
        case TOKEN_ARGUMENT:
        {
            // if we were waiting for a filename for a redirect, set that file
            if (redirectWaitingFor != TOKEN_EOF)
            {
                char *fname = strdup(t.value.c_str());
                if (redirectWaitingFor == TOKEN_REDIRECT)
                {
                    // '>' simple redirect
                    cmd._outFile = fname;
                    cmd._append = 0;
                }
                else if (redirectWaitingFor == TOKEN_APPEND)
                {
                    // '>>' append
                    cmd._outFile = fname;
                    cmd._append = 1;
                }
                else if (redirectWaitingFor == TOKEN_INPUT)
                {
                    cmd._inputFile = fname;
                }
                else if (redirectWaitingFor == TOKEN_ERROR)
                {
                    cmd._errFile = fname;
                    cmd._out_error = 0;
                }
                else if (redirectWaitingFor == TOKEN_REDIRECT_AND_ERROR)
                {
                    // treat as redirect stdout & stderr to the same file
                    cmd._outFile = fname;
                    cmd._errFile = fname;   
                    cmd._out_error = 1;
                    cmd._append = 0; // no append variant here
                }
                redirectWaitingFor = TOKEN_EOF;
            }
            else
            {
                // Regular argument or command not file name: insert into current simple command
                curSimple->insertArgument(strdup(t.value.c_str()));
            }
            break;
        }

        case TOKEN_PIPE:
        {
            // Check if there was a valid command before the pipe
            if (curSimple->_numberOfArguments == 0)
            {
                std::cerr << "Syntax error: pipe '|' cannot appear after an empty command.\n";
                free(curSimple->_arguments);
                free(curSimple);
                cmd.clear();

                return;// Stop parsing completely
            }


            // Check if there's another command after the pipe
            if (i + 1 >= tokens.size() || tokens[i + 1].type == TOKEN_EOF)
            {
                std::cerr << "Syntax error: pipe '|' cannot be the last token.\n";
                free(curSimple->_arguments);
                free(curSimple);
                cmd.clear();
                return; // Stop parsing completely
            }

            
            // Finish current command and start a new one if no error found
            cmd.insertSimpleCommand(curSimple);
            curSimple = new SimpleCommand();
            break;
        }

        case TOKEN_REDIRECT: // '>'
            redirectWaitingFor = TOKEN_REDIRECT;
            break;

        case TOKEN_APPEND: // '>>'
            redirectWaitingFor = TOKEN_APPEND;
            break;

        case TOKEN_INPUT: // '<'
            redirectWaitingFor = TOKEN_INPUT;
            break;

        case TOKEN_ERROR: // '2>'
            redirectWaitingFor = TOKEN_ERROR;
            break;

        case TOKEN_REDIRECT_AND_ERROR: // '>>&' 
            redirectWaitingFor = TOKEN_REDIRECT_AND_ERROR;
            break;

        case TOKEN_BACKGROUND: // '&'
            cmd._background = 1;
            break;

        default:
            break;
        }
    }


    if (redirectWaitingFor != TOKEN_EOF) //this means we reached the end of tokens but redirectWaitingFor is still set and this means filename is missing
    {
        std::cerr << "Syntax error: redirection operator missing filename\n";
        free(curSimple->_arguments);
        free(curSimple);
        cmd.clear();
        return;
    }

    // if the last simple command contains arguments, insert it
    if (curSimple->_numberOfArguments > 0)
    {
        cmd.insertSimpleCommand(curSimple);
    }
    else
    {
        // if last simple command is empty, free it
        free(curSimple->_arguments);
        free(curSimple);
    }

    
    // Finally execute the built command
    cmd.execute();
   
}

SimpleCommand::SimpleCommand() //allocates initial space for arguments
{
    _numberOfAvailableArguments = 5;
    _numberOfArguments = 0;
    _arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *)); //memory allocation for  argumnets
}

void SimpleCommand::insertArgument(char *argument) //doubles the array size when needed and stores the argument pointer
{
    if (_numberOfAvailableArguments == _numberOfArguments + 1)
    {
        _numberOfAvailableArguments *= 2;
        _arguments = (char **)realloc(_arguments,
                                      _numberOfAvailableArguments * sizeof(char *));
    }

    _arguments[_numberOfArguments] = argument;
    _arguments[_numberOfArguments + 1] = NULL; // must put null at the end of the simple command to make execute know when to stop

    _numberOfArguments++;
}

Command::Command()
{
    _numberOfAvailableSimpleCommands = 1;
    _simpleCommands = (SimpleCommand **)
        malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

    _numberOfSimpleCommands = 0;
    _outFile = 0;
    _inputFile = 0;
    _errFile = 0;
    _background = 0;
    _append=0;
    _out_error=0;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
    if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
    {
        _numberOfAvailableSimpleCommands *= 2;
        _simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
                                                    _numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
    }

    _simpleCommands[_numberOfSimpleCommands] = simpleCommand;
    _numberOfSimpleCommands++;
}

void Command::clear()
{
    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
            free(_simpleCommands[i]->_arguments[j]);
        }

        free(_simpleCommands[i]->_arguments);
        free(_simpleCommands[i]);
    }

    if (_outFile)
    {
        free(_outFile);
    }

    if (_inputFile)
    {
        free(_inputFile);
    }

    if (_errFile)
    {
        free(_errFile);
    }

    _numberOfSimpleCommands = 0;
    _outFile = 0;
    _inputFile = 0;
    _errFile = 0;
    _background = 0;
    _append=0;
    _out_error=0;
}

void Command::print()
{
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        printf("  %-3d ", i);
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
            printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
        }
    }

    printf("\n\n");
    printf("  Output       Input        Error        Err&Out       Background\n");
    printf("  ------------ ------------ ------------ ------------ ------------\n");
    printf("  %-12s %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
           _inputFile ? _inputFile : "default", _errFile ? _errFile : "default", _out_error == 1 ? _errFile : "default"
           ,_background ? "YES" : "NO");
    printf("\n\n");
}

void Command::execute() 
{
   // handle empty command
    if (_numberOfSimpleCommands == 0)
    {
        prompt();
        return;
    } 

    

    // check built-ins: only when there is a single simple command like cd and exit
    if (_numberOfSimpleCommands == 1)
    {
        SimpleCommand *sc = _simpleCommands[0];
        if (sc->_numberOfArguments > 0)
        {
            if (strcmp(sc->_arguments[0], "cd") == 0)
            {
                // cd built-in (no fork) because cd changes the current directory of the shell process itself, not a child 
                const char *dest = NULL;
                if (sc->_numberOfArguments >= 2)
                {
                    dest = sc->_arguments[1];
                }
                else
                {
                    dest = getenv("HOME");
                    if (!dest) dest = "/";
                }
                printf("Changing to directory '%s'\n", dest);
                if (chdir(dest) != 0)
                {
                    fprintf(stderr, "cd: %s: %s\n", dest, strerror(errno)); //this will happen if changing directory failed
                }
                else
                {
                    char cwd[1024];
                    if (getcwd(cwd, sizeof(cwd)) != NULL) // function that gets the current working directory
                    {
                        printf("You are now in %s\n", cwd);
                    }
                }
                clear();
                prompt();
                return;
            }
            else if (strcmp(sc->_arguments[0], "exit") == 0 && sc->_numberOfArguments == 1)
            {
                printf("Good bye!!\n");
                fflush(stdout);
                exit(0);
            }
            else if (strcmp(sc->_arguments[0], "exit") == 0 && sc->_numberOfArguments > 1)
            {
                std::cerr << "Syntax error\n";
                clear();
                return;

            }
        }
    }
    
    // Print parsed command table
    print();

    // Prepare pipes if we have more than one simple command
    int numCmds = _numberOfSimpleCommands;
    std::vector<int> pipefds; // store pipe file descriptors in pairs [r0, w0, r1, w1, r2, w2, ...]

    if (numCmds > 1) //If there’s more than one simple command we need pipes
    {
        pipefds.resize((numCmds - 1) * 2); // we need N-1 pipes if we have N commandes and Each pipe uses 2 file descriptors (read and write)
        for (int i = 0; i < (numCmds - 1); ++i) //Loop once per pipe to call pipe() 
        {
            if (pipe(pipefds.data() + i * 2) < 0)  //for pipe i, this effectively fills pipefds[2*i] and pipefds[2*i+1].
            {
                perror("pipe");
                // cleanup and return
                clear();
                prompt();
                return;
            }
        }
    }

    std::vector<pid_t> pids; //for child pids
    pids.reserve(numCmds);//make sure the vector has enough space to hold numCmds elements before we start adding them so noe reallocation happens

    // For each simple command, fork and set up I/O
    for (int i = 0; i < numCmds; ++i)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork"); // if fork fails break
            break;
        }

        if (pid == 0)
        {
            // Child process
            // Restore default SIGINT so child can be terminated by Ctrl-C when needed if run in foreground
            signal(SIGINT, SIG_DFL);

            // Setup input:
            if (i == 0)
            {
                if (_inputFile)
                {
                    int fd = open(_inputFile, O_RDONLY);
                    if (fd < 0)
                    {
                        fprintf(stderr, "Error opening input file %s: %s\n", _inputFile, strerror(errno));
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);//the program reads from fd instead of keyboard
                    close(fd);
                }
            }
            else
            {
                // Not first command: connect read end of previous pipe to stdin
                int readEnd = pipefds[(i - 1) * 2 + 0];
                dup2(readEnd, STDIN_FILENO);
            }

            // Setup output:
            if (i == numCmds - 1)
            {
                // last command
                if (_outFile)
                {
                    int fd;
                    if (_append)
                        fd = open(_outFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
                    else
                        fd = open(_outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                    if (fd < 0)
                    {
                        fprintf(stderr, "Error opening output file %s: %s\n", _outFile, strerror(errno));
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);//The program writes to fd instead of the terminal
                    close(fd);
                }
            }
            else
            {
                // Not last command: connect write end of current pipe to stdout
                int writeEnd = pipefds[i * 2 + 1];
                dup2(writeEnd, STDOUT_FILENO);
            }

            // Setup stderr redirection:
            if (_errFile)
            {
                int fd;
                // if out_error is set (meaning redirect both stdout and stderr to same file),
                // then stderr has already been set by outFile logic above if outFile==errFile.
                // But we still safely set stderr here:
                if (_append)
                    fd = open(_errFile, O_WRONLY | O_CREAT | O_APPEND, 0644);
                else
                    fd = open(_errFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (fd < 0)
                {
                    fprintf(stderr, "Error opening error file %s: %s\n", _errFile, strerror(errno));
                    exit(1);
                }
                dup2(fd, STDERR_FILENO); //The program’s errors go to fd instead of the terminal
                close(fd);
            }

            // Close all pipe fds in child (they have been duplicated where needed)
            for (size_t j = 0; j < pipefds.size(); ++j)
            {
                close(pipefds[j]);
            }

            // Execute the command
            SimpleCommand *sc = _simpleCommands[i];
            if (sc->_numberOfArguments == 0)
            {
                exit(0);
            }

            // prepare argv for execvp (already null-terminated in insertArgument)
            char **argv = sc->_arguments;

            execvp(argv[0], argv);
            // if execvp returns, there was an error
            fprintf(stderr, "%s: command not found or failed to execute: %s\n", argv[0], strerror(errno));
            exit(1);
        }
        else
        {
            // Parent process: track child pid
            pids.push_back(pid);

            // close pipe fds that parent doesn't need anymore
            if (numCmds > 1)
            {
                // close previous read end (i-1) and current write ends where appropriate
                if (i > 0)
                {
                    // close read end of previous pipe in parent
                    int prevRead = pipefds[(i - 1) * 2 + 0];
                    close(prevRead);
                }
                if (i < numCmds - 1)
                {
                    // close write end of this pipe in parent
                    int thisWrite = pipefds[i * 2 + 1];
                    close(thisWrite);
                }
            }
        }
    }

    // Parent: wait for children unless background
    if (!_background)
    {
         // wait for all child processes and log each termination
         int status;
        pid_t wpid; // pid_t is a type for process IDs
        for (pid_t cpid : pids)
        {
            do
            {
                wpid = waitpid(cpid, &status, 0);
            } while (wpid == -1 && errno == EINTR);

            // write the termination info to log.txt
            FILE *f = fopen("log.txt", "a");
            if (f)
            {
                time_t now = time(NULL); //gives the current time in seconds.
                char timestr[64]; 
                if (localtime(&now) != NULL)
                    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&now));
                else
                    snprintf(timestr, sizeof(timestr), "unknown-time");

                if (WIFEXITED(status))
                {
                    fprintf(f, "[%s] Child %d exited with status %d\n", timestr, (int)cpid, WEXITSTATUS(status)); //WEXITSTATUS gives the exit code
                }
                else if (WIFSIGNALED(status))
                {
                    fprintf(f, "[%s] Child %d killed by signal %d\n", timestr, (int)cpid, WTERMSIG(status));
                }
                else
                {
                    fprintf(f, "[%s] Child %d ended (status=%d)\n", timestr, (int)cpid, status);
                }
                fclose(f);
            }
        }
    }
    else
    {
        // background: do not wait, print background pid(s)
        if (!pids.empty())
        {
            printf("[");
            for (size_t i = 0; i < pids.size(); ++i)
            {
                printf("%d", pids[i]);
                if (i + 1 < pids.size()) printf(", ");
            }
            printf("] running in background\n");
            fflush(stdout);
        }
    }


    // Clean up and return to prompt
    clear();
    prompt();
}

void Command::prompt() //prints "myshell>" to start the terminal  and waits for the user's input,have the infinite while loop, call the tokenizer then give its output to parse
{
    fflush(stdout);
    std::string input;
    while (true)
    {
        printf("myshell>"); 
        std::getline(std::cin, input); //reads line
        std::vector<Token> tokens = tokenize(input);
        parse(tokens);
    }
}


Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;


int main()
{
    signal(SIGINT, SIG_IGN);

    // Disable echoing of control characters like ^C
    struct termios term; 
    tcgetattr(STDIN_FILENO, &term); //Gets current terminal settings.
    term.c_lflag &= ~ECHOCTL; // Disables printing control characters (like ^C, ^Z, etc.).
    tcsetattr(STDIN_FILENO, TCSANOW, &term);//Applies the change immediately.

    // Clear the log file each time the shell starts
    FILE *log = fopen("log.txt", "w");
    if (log) fclose(log);
    
    Command::_currentCommand.prompt();
    return 0;
}
