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

/* This function handles a character in the start state. If a forward slash is detected, it transitions to the start comment state. If 
    a double quote is detected, it transitions to the string literal state. If a single quote is detected, it transitions to the character
    literal state. If a escape character is detected, the number of lines is incremented and the state is not changed. Otherwise, write the
    character stdout. The previous state is set to this start state and the next character is returned. */
static int handleStartState (int c)
{
    if (c == '/') /* Start of a comment */
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
    else /* Regular character */
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
    if (c == '*') /* Complete start of a comment */
    {
        CURRENT_STATE = IN_COMMENT; 
        startOfCommentLineNumber = lineNumber;
    }
    else if (c == '/') /* Another forward slash, could be the start of a comment */
    {
        putchar ('/');
    }
    else if (c == '\n') /* New line character */
    {
        putchar ('/');
        putchar (c);
        lineNumber++;
        CURRENT_STATE = START;
    }
    else /* Incomplete comment, any other character */
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

/* This function handles a character in the string literal state. If a double quote is detected, it writes the double quote to stdout
    and transitions back to the default start state because the string literal is closed. If a backwards slash is detected, it writes the
    backwards slash to stdout and transitions it to the escape state. If a newline character is detected, it writes the newline to stdout
    and increments the line number while remaining in the string literal state. Otherwise, it writes the character to stdout and remains in
    the string literal state. The previous state is set to this string literal state and the next character is returned. */
static int handleStringLiteralState (int c)
{
    if (c == '\"') /* End of string literal */
    {
        putchar(c);
        CURRENT_STATE = START;
    }
    else if (c == '\\') /* Start of escape character */
    {
        putchar (c);
        CURRENT_STATE = ESCAPE;
    }
    else if (c == '\n') /* New line character */
    {
        putchar (c);
        lineNumber++; 
    }
    else /* Any other character */
    {
        putchar (c);
    }

    PREVIOUS_STATE = STRING_LITERAL;
    return getchar();
}

/* This function handles a character in the character literal state. If a single quote is detected, it writes the single quote to stdout
    and transitions back to the default start state because the string literal is closed. If a backwards slash is detected, it writes the
    backwards slash to stdout and transitions it to the escape state. If a newline character is detected, it writes the newline to stdout
    and increments the line number while remaining in the character literal state. Otherwise, it writes the character to stdout and remains 
    in the character literal state. The previous state is set to this character literal state and the next character is returned. */
static int handleCharacterLiteralState (int c)
{
    if (c == '\'') /* End of character literal */
    {
        putchar(c);
        CURRENT_STATE = START;
    }
    else if (c == '\\') /* Start of escape character */
    {
        putchar (c);
        CURRENT_STATE = ESCAPE;
    }
    else if (c == '\n') /* New line character */
    {
        putchar(c);
        lineNumber++;
    }
    else /* Any other character */
    {
        putchar(c);
    }
    
    PREVIOUS_STATE = CHARACTER_LITERAL;
    return getchar();
} 

/* This function handles a character in the in comment state. If an asterisk is detected, the comment is potentially ending, so it 
    transitions to the end comment state. If a newline character is detected, it increments the number of new lines inside the comment.
    Otherwise, the character is not written to stdout and it remains in the in comment state. The previous state is set to this in comment 
    state and the next character is returned. */
static int handleInCommentState (int c)
{
    if (c == '*') /* Potential end of comment */
    {
        CURRENT_STATE = END_COMMENT;
    }
    else if (c == '\n') /* New line inside comment */
    {
        newLineInCommentCount++;
    }

    PREVIOUS_STATE = IN_COMMENT;
    return getchar();
}

/* This function handles a character in the end comment state. If a forward slash is detected, it is the end of the comment, so it first
    writes a space character to stdout in place of the comment, then prints the number, if any, of new lines inside the comment, resets 
    the count of the number of new lines inside the comment, and transitions back to the default start state. If a newline character is 
    detected, it is an incomplete comment ending, so it transitions back to the in comment state and increments the number of new lines 
    inside the comment. If any other character that isn't an asterisk is detected, it is also an incomplete comment ending so goes back 
    to the in comment state (if it were an asterisk, the comment is still potentially ending so it would remain in the end comment state).
    The previous state is set to this end comment state and the next character is returned. */
static int handleEndCommentState (int c)
{
    int i; /* Initializing i at the top of the block */
    
    if (c == '/') /* Complete end of the comment */
    {
        putchar (' ');

        for (i = 0; i < newLineInCommentCount; i++)  
        {
            putchar('\n');  
            lineNumber++;
        }
        newLineInCommentCount = 0;
        CURRENT_STATE = START;
    }
    else if (c == '\n') /* New line character */
    {
        CURRENT_STATE = IN_COMMENT; 
        newLineInCommentCount++;
    }
    else if (c != '*') /* Any character that isn't an asterisk */
    {
        CURRENT_STATE = IN_COMMENT;
    }

    PREVIOUS_STATE = END_COMMENT;
    return getchar();
}

/* This function handles a character in the escape state. It writes escaped character to the stdout as the character is devoid of any 
    special meaning and transitions back to the stage it was originally in. The previous state is set to this escape state and the next 
    character is returned. */
static int handleEscapeState (int c)
{
    putchar(c);
    CURRENT_STATE = PREVIOUS_STATE;
    PREVIOUS_STATE = ESCAPE;
    return getchar();
}

/* This function reads text from stdin character by character, removing and replacing comments with spaces, handling unterminated comments,
    and handling nested newlines within comments. If the last character was a forward slash, it deals with it by writing it to stdout. 
    It returns EXIT_SUCESS if there were no errors and unterminated comments. If an unterminated comment is found, it returns EXIT_FAILURE 
    and writes to stderr with an unterminated comment error and the line number of where the unterminated comment begins. */
int main (void)
{
    int c = getchar();

    while (c != EOF) /* No more characters */
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
