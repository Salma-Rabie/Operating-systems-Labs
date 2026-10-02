#ifndef tokenizer_h
#define tokenizer_h 


#include <string>
#include <vector>

enum TokenType
{
    TOKEN_COMMAND,    
    TOKEN_ARGUMENT,  //thing that is not command or one of the redirections or pipe or excute background. ex: name of file   
    TOKEN_REDIRECT,  
    TOKEN_APPEND,   
    TOKEN_INPUT,     
    TOKEN_ERROR,    
    TOKEN_PIPE,       
    TOKEN_BACKGROUND, 
    TOKEN_REDIRECT_AND_ERROR,
    TOKEN_EOF,       // it must be in the end of the array of tokens
};

struct Token
{
    TokenType type; // the type of the word
    std::string value; //the word itself is the value
};

std::vector<Token> tokenize(const std::string &input); //takes the input line and returns a list (vector) of tokens

#endif
