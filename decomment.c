#include <stdio.h>
#include <stdlib.h>

/* Definining enumerations for the different states in the DFA */
enum State 
{
    START, /* Default state (not in a comment, string literal, or character literal) */
    START_COMMENT, /* State for the potential start for a comment (detected '/') */
    STRING_LITERAL, /* State for the inside of a string literal */
    CHARACTER_LITERAL, /* State for the inside of a character literal */
    IN_COMMENT, /* State for the inside of a comment */
    END_COMMENT, /* State for the potential end for a comment (detected '*') */
    ESCAPE /* State for the inside of an escape sequence */
};

/* Declaring global variables to track state and line numbers */
static int lineNumber = 1; /* Keeps track of the current line number */
static int newLineInCommentCount = 0; /* Keeps track of the number of newlines inside comments */
static int startOfCommentLineNumber = 0; /* Stores the starting line number of a comment (for the unterminated comment error message) */
static enum State CURRENT_STATE = START; /* Tracks the current state */
static enum State PREVIOUS_STATE = START; /* Tracks the previous state */

/* This function handles a character in the start state. 
If a forward slash is detected, it transitions to the start comment state. If 
    a double quote is detected, it transitions to the string literal state. If a single quote is detected, it transitions to the character
    literal state. If a escape character is detected, the number of lines is incremented and the state is not changed. Otherwise, write the
    character stdout. The previous state is set to this start state and the next character is returned. */
static int handleStartState (int c)
{
    /*if (c == '/' && PREVIOUS_STATE == END_COMMENT)
    {
        CURRENT_STATE = IN_COMMENT;
    }
    else */if (c == '/') /* Start of a comment */
    {
        CURRENT_STATE = START_COMMENT;
    }
    else if (c == '\"') /* Start of a string literal */
    {
        CURRENT_STATE = STRING_LITERAL;
        putchar(c);
    }
    else if (c == '\'') /* Start of a character literal */
    {
        CURRENT_STATE = CHARACTER_LITERAL;
        putchar(c);
    }
    else if (c == '\n') /* New line character */
    {
        putchar(c);
        lineNumber++; 
    }
    else if (c == '*' && PREVIOUS_STATE == END_COMMENT)
    {
        CURRENT_STATE = END_COMMENT;
    }
    else 
    {
        putchar(c);
    }
    PREVIOUS_STATE = START;
    return getchar();   
}

/* This function handles a character in the start comment state. If a asterisk is detected, it transitions to the in comment state and 
    stores the current line number as the line number of the start of the comment. If a forward slash is detected, it writes the forward 
    slash to stdout. If a escape character is detected, it writes a forward slash for the last character (which was a 
    incomplete start to a comment) to stdout, writes the current newline to stdout, increments the number of lines, and transitions back to
    the start state since it was not a complete start to a comment. Otherwise, it writes a forward slash for the last character (which was 
    a incomplete start to a comment) to stdout, writes the current character to stdout, transitions to the string literal, charaacter 
    literal, or start states depending on if the current character was a double quote, single quote, or any other character. The previous
    state is set to this start comment state and the next character is returned. */
static int handleStartCommentState (int c)
{
    if (c == '*') /* Full start of a comment */
    {
        CURRENT_STATE = IN_COMMENT; 
        startOfCommentLineNumber = lineNumber;
    }
    else if (c == '/')
    {
        putchar ('/');
    }
    else if (c == '\n')
    {
        putchar ('/');
        putchar (c);
        lineNumber++;
        CURRENT_STATE = START;
    }
    else 
    {
        putchar ('/'); 
        putchar (c);
        if (c == '\"')
        {
            CURRENT_STATE = STRING_LITERAL;
        }
        else if (c == '\'')
        {
            CURRENT_STATE = CHARACTER_LITERAL;
        }
        else
        {
            CURRENT_STATE = START;
        }
    }
    PREVIOUS_STATE = START_COMMENT;
    return getchar();      
}


static int handleStringLiteralState (int c)
{
    if (c == '\"') /* End of string literal */
    {
        CURRENT_STATE = START;
        putchar(c);
    }
    else if (c == '\\') /* Start of escape character */
    {
        putchar (c);
        CURRENT_STATE = ESCAPE;
    }
    else if (c == '\n') /* New line character */
    {
        putchar (c);
        lineNumber++; /* Increment total number of lines */
    }
    else
    {
        putchar (c);
    }
    PREVIOUS_STATE = STRING_LITERAL;
    return getchar();
}

static int handleCharacterLiteralState (int c)
{
    if (c == '\'') 
    {
        CURRENT_STATE = START;
        putchar(c);
    }
    else if (c == '\\') 
    {
        putchar (c);
        CURRENT_STATE = ESCAPE;
    }
    else if (c == '\n') 
    {
        putchar(c);
        lineNumber++;
    }
    else 
    {
        putchar(c);
    }
    PREVIOUS_STATE = CHARACTER_LITERAL;
    return getchar();
} 

static int handleInCommentState (int c)
{
    if (c == '*') 
    {
        CURRENT_STATE = END_COMMENT;
    }
    else if (c == '\n') 
    {
        newLineInCommentCount++;
    }

    PREVIOUS_STATE = IN_COMMENT;
    return getchar();
}

static int handleEndCommentState (int c)
{
    int i;
    
    if (c == '/')  
    {
        CURRENT_STATE = START;
        putchar (' ');
        
        for (i = 0; i < newLineInCommentCount; i++)  
        {
            putchar('\n');  
            lineNumber++;
        }
        newLineInCommentCount = 0;     
    }
    else if (c == '\n')
    {
        CURRENT_STATE = IN_COMMENT; 
        newLineInCommentCount++;
    }
    else if (c != '*')
    {
        CURRENT_STATE = IN_COMMENT;
    }

    PREVIOUS_STATE = END_COMMENT;
    return getchar();
}

static int handleEscapeState (int c)
{
    putchar(c);
    CURRENT_STATE = PREVIOUS_STATE;
    PREVIOUS_STATE = ESCAPE;
    return getchar();
}

int main (void)
{
    int c = getchar();

    while (c != EOF) 
    {
        switch (CURRENT_STATE) 
        {
            case START:
                c = handleStartState(c);
                break;
            case START_COMMENT:
                c = handleStartCommentState(c);
                break;
            case STRING_LITERAL:
                c = handleStringLiteralState(c);
                break;
            case CHARACTER_LITERAL:
                c = handleCharacterLiteralState(c);
                break;
            case IN_COMMENT:
                c = handleInCommentState(c);
                break;
            case END_COMMENT:
                c = handleEndCommentState(c);
                break;
            case ESCAPE:
                c = handleEscapeState(c);
                break;
        }
    }

    if (CURRENT_STATE == START_COMMENT)
    {
        putchar ('/');
    }
    else if (CURRENT_STATE == IN_COMMENT || CURRENT_STATE == END_COMMENT) 
    {
        int i = 0;
        putchar (' ');
        for (i = 0; i < newLineInCommentCount; i++)  
        {
            putchar('\n');  
            lineNumber++;
        }
        newLineInCommentCount = 0;
        fprintf (stderr, "Error: line %d: unterminated comment\n", startOfCommentLineNumber);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
