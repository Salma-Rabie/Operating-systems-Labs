
#ifndef command_h
#define command_h

struct SimpleCommand // we divide the original big complex command to multiple simple commands
{
	int _numberOfAvailableArguments;
	int _numberOfArguments;
	char **_arguments; //array of pointers to arguments that contains at its end null

	SimpleCommand(); //constucter
	void insertArgument(char *argument); // to insert an argument to the array of arguments 
};

struct Command // conatains all the information of the big command and the numbers of everything in it
{
	int _numberOfAvailableSimpleCommands;
	int _numberOfSimpleCommands;
	SimpleCommand **_simpleCommands; // array of pointers of simple commands if there is pipes |
	char *_outFile;
	char *_inputFile;
	char *_errFile;
	int  _out_error;
	int _background;
	int _append;

	void prompt(); //reads lines in a loop and calls parse()
	void print(); //shows the parsed command table for debugging.
	void execute(); //execte the commands by making the child execute
	void clear(); // frees memory allocated to arguments, files, and resets counters

	Command(); //constructer 
	void insertSimpleCommand(SimpleCommand *simpleCommand);// to insert a simple command into the array of simplecommands

	static Command _currentCommand; // a global place to store the command being built/executed.
	static SimpleCommand *_currentSimpleCommand;
};

#endif
