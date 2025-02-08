#include <stdio.h>
#include <stdlib.h>


enum State 
{
    START, 
    START_COMMENT, 
    STRING_LITERAL, 
    CHARACTER_LITERAL, 
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
        CURRENT_STATE = CHARACTER_LITERAL;
        putchar(c);
    }
    else if (c == '\n') /* New line character */
    {
        putchar(c);
        lineNumber++; /* Increment total number of lines */
    }
    /*
    else if (c == '*' && PREVIOUS_STATE == START_COMMENT)
    {
        CURRENT_STATE = IN_COMMENT;
    }*/
    else 
    {
        CURRENT_STATE = START; /* technically do nothing */
        putchar(c);
    }
    PREVIOUS_STATE = START;
    return getchar();   
}

static int handleStartCommentState (int c)
{
    if (c == '*') /* Full start of a comment */
    {
        CURRENT_STATE = IN_COMMENT; 
        PREVIOUS_STATE = START_COMMENT;
    }
    else if (c == '/')
    {
        putchar ('/');
        
        CURRENT_STATE = START_COMMENT;
        PREVIOUS_STATE = START_COMMENT;
    }
    else if (c == '\n')
    {
        putchar ('/');
        putchar (c);
        lineNumber++;
        CURRENT_STATE = START;
        PREVIOUS_STATE = START_COMMENT;
    }
    else /* Uncomplete start of a comment*/
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
        else if (c == '\n')
        {
            lineNumber++;
        }
        else
        {
            CURRENT_STATE = START;
        }
        PREVIOUS_STATE = START_COMMENT;
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
        CURRENT_STATE = ESCAPE;
    }
    else if (c == '\n') 
    {
        putchar(c);
        lineNumber++;
    }
    else 
    {
        CURRENT_STATE = CHARACTER_LITERAL;
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
    else 
    {
        CURRENT_STATE = IN_COMMENT;
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
    else if (c == '*')
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
    else if (CURRENT_STATE == ESCAPE)
    {
        putchar ('\\');
    }
    else if (CURRENT_STATE == IN_COMMENT || CURRENT_STATE == END_COMMENT) 
    {
        printf ("Error: line %d: unterminated comment\n", lineNumber);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
