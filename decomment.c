#include <stdio.h>
#include <stdlib.h>


enum State 
{
    START, 
    START_COMMENT, 
    STRING_LITERAL, 
    CHAR_LITERAL, 
    IN_COMMENT, 
    END_COMMENT,
    ESCAPE
};

static int lineNumber = 1;
static int newLineInCommentCount = 0;
static enum State CURRENT_STATE = START;
static enum State PREVIOUS_STATE = START;

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
        CURRENT_STATE = CHAR_LITERAL;
        putchar(c);
    }
    else if (c == '\n') /* New line character */
    {
        putchar(c);
        lineNumber++; /* Increment total number of lines */
    }
    else 
    {
        CURRENT_STATE = START; /* technically do nothing */
        putchar(c);
    }
    return getchar();   
}

static int handleStartCommentState (int c)
{
    
    if (c == '*') /* Full start of a comment */
    {
        CURRENT_STATE = IN_COMMENT; 
    }
    else /* Uncomplete start of a comment, previous '/' gets treated as a regular character */
    {
        putchar ('/'); 
        putchar (c);
        CURRENT_STATE = START;
    }
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
        PREVIOUS_STATE = STRING_LITERAL;
        CURRENT_STATE = ESCAPE;
    }
    else if (c == '\n') /* New line character */
    {
        putchar (c);
        CURRENT_STATE = STRING_LITERAL;
        lineNumber++; /* Increment total number of lines */
    }
    else
    {
        CURRENT_STATE = STRING_LITERAL; /* technically do nothing */
        putchar (c);
    }
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
        PREVIOUS_STATE = CHAR_LITERAL;
        CURRENT_STATE = ESCAPE;
    }
    else if (c == '\n') 
    {
        putchar(c);
        lineNumber++;
    }
    else 
    {
        CURRENT_STATE = CHAR_LITERAL;
        putchar(c);
    }
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
    else 
    {
        CURRENT_STATE = IN_COMMENT;
    }
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
    else  
    {    
        CURRENT_STATE = IN_COMMENT;  
    }

    

    return getchar();
}

static int handleEscapeState (int c)
{
    putchar(c);
    CURRENT_STATE = PREVIOUS_STATE;
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
            case CHAR_LITERAL:
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

    if (CURRENT_STATE == IN_COMMENT || CURRENT_STATE == END_COMMENT) 
    {
        printf ("Error: line %d: unterminated comment\n", lineNumber);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
