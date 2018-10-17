/**********************************************************************
cs1713p3Driver.c
Purpose:
    This program reads user information and a command file.   It
    processes the commands against the user information.
    This is the driver code for the routines written by the students.
Command Parameters:
    p3 -u userFileName -c commandFileName
Input:
    User    same as Programming Assignment #2 although there might be different data.

    Command This is different from the previous assignment.  The file contains
    text in the form of commands.
        GAME RESULT szUserId1 szUserId2 iScore1 iScore2
            This should use processGame to process this
        GAME FIX szUserId1 szUserId2 iOldScoreUser1 iOldScoreUser2 iNewScore1 iNewScore2
            if the winner changes:
               - decrement the old winner's iWin the old loser's iLoss
               - increment the new winner's iWin the new loser's iLoss
            Otherwise, don't change the wins and losses.
            This functionality is in the processGameFix function.
        TEAM PAID szUserId dAmount
             Add the amount to this user's dPaidAmount.
        TEAM SHOW szUserId
             Show all the user information for this one user.
Results:
    Prints the Users prior to sorting
    Prints the Users after sorting.
    Processes the commands (see above) and shows any errors.
    Prints the resulting Users
Returns:
    0  normal
    -1 invalid command line syntax
    -2 show usage only
    -3 error during processing, see stderr for more information

Notes:
    p3 -?       will provide the usage information.  In some shells,
                you will have to type p3 -\?
**********************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cs1713p3.h"

// prototypes for functions with code in this file
void processCommandSwitches(int argc, char *argv[], char **ppszUserFileName
    , char **ppszCommandFileName);
void processCommands(User userM[], int iUserCount, char *pszCommandFileName);
int getSimpleToken(char szInput[], int *piBufferPosition, char szToken[]);

#define MAX_TOKEN_SIZE 20

int main(int argc, char *argv[])
{
    User      userM[MAX_USERS];             // array of user items
    int       iUserCnt;                     // number of items in userM;
    char      *pszUserFileName = NULL;      // User file name
    char      *pszCommandFileName = NULL;   // Command file name

    processCommandSwitches(argc, argv, &pszUserFileName, &pszCommandFileName);
    printf("this was open\n");
    // get the user, print it, sort it, and print the sorted
    iUserCnt = getUsers(userM, pszUserFileName);
    printUsers("Initial Users", userM, iUserCnt);
    
    sortUsers(userM, iUserCnt);
    
    printUsers("Sorted Users", userM, iUserCnt);
    printf("\n");

    // process the command input file
    processCommands(userM, iUserCnt, pszCommandFileName);

    // print the user after processing the commandfile
    printUsers("\nProcessed Users", userM, iUserCnt);
    printf("\n");
    return 0;
}

/******************** processCommands **************************************
    void processCommands(User userM[], int iUserCnt
        , char *pszCommandFileName)
Purpose:
    Reads the Command file to process commands.  There are two major
    types of commands:  GAME and TEAM.
Parameters:
    I/O User  userM[]                 array of user
    I   int   iUserCnt                number of elements in userM[]
    I   char  *pszCommandFileName   command file name
Notes:
    This function ßßopens and closes the Command file.
**************************************************************************/
void processCommands(User userM[], int iUserCnt
     , char *pszCommandFileName)
{
    FILE *pfileCommand;                     // stream Input containing commands
    char szInputBuffer[MAX_LINE_SIZE];      // input buffer for a single text line
    char szCommand[MAX_TOKEN_SIZE+1];       // command
    char szSubCommand[MAX_TOKEN_SIZE+1];    // subcommand
    int bGotToken;                          // TRUE if getSimpleToken got a token
    int iBufferPosition;                    // This is used by getSimpleToken to
                                            // track parsing position within input buffer
    
    // open the Command stream data file
    if (pszCommandFileName == NULL)
        exitError(ERR_MISSING_SWITCH, "-c");

    pfileCommand = fopen(pszCommandFileName, "r");
    if (pfileCommand == NULL)
        exitError(ERR_COMMAND_FILENAME, pszCommandFileName);
    
    /* get command data until EOF
    ** fgets returns null when EOF is reached.
    */
    while (fgets(szInputBuffer, MAX_LINE_SIZE, pfileCommand) != NULL)
    {
        printf("%s", szInputBuffer);        // includes \n in the data
        iBufferPosition = 0;                // reset buffer position

        // get the command
        bGotToken = getSimpleToken(szInputBuffer, &iBufferPosition, szCommand);

        // see what the command is
        if (bGotToken && strcmp(szCommand, "USER") == 0)
        {   // USER command
            // get the sub comand
            bGotToken = getSimpleToken(szInputBuffer, &iBufferPosition, szSubCommand);
            if (bGotToken)
                processUserCommand(userM, iUserCnt, szSubCommand
                , &szInputBuffer[iBufferPosition]);    // address past the subcommand
            else exitError(ERR_USER_SUB_COMMAND, " missing subcommand");
        }
        else  if (bGotToken && strcmp(szCommand, "VEH") == 0)
        {   // VEH command
            bGotToken = getSimpleToken(szInputBuffer, &iBufferPosition, szSubCommand);
            if (bGotToken)
                processVehCommand(userM, iUserCnt, szSubCommand
                    , &szInputBuffer[iBufferPosition]);
            else exitError(ERR_VEH_SUB_COMMAND, " missing subcommand");
        }
        else
            exitError(ERR_INVALID_COMMAND, szCommand);
    }
    fclose(pfileCommand);
}

 /******************** getSimpleToken **************************************
 int getSimpleToken(char szInput[], int *piBufferPosition, char szToken[])
 Purpose:
    Returns the next token in a string.  The delimiter for the token is a
    space, a newline or the end of the string.  To help with a
    subsequent call, it also returns the next position in the buffer.
Parameters:
    I   char szInput[]          input buffer to be parsed
    I/O int *piBufferPosition   Position to begin looking for the next token.
                                This is also used to return the next
                                position to look for a token (which will
                                follow the delimiter).
    O   char szToken[]          Returned token.
Returns:
    Functionally:
        TRUE - a token is returned
        FALSE - no more tokens
    iBufferPosition parm - the next position for parsing
    szToken parm - the returned token.  If not found, it will be an empty string.
Notes:
    - If the token is larger than the szToken parm, we return a truncated value.
**************************************************************************/
int getSimpleToken(char szInput[], int *piBufferPosition, char szToken[])
{
    int iDelimPos;                      // found position of delim
    int iCopy;                          // number of characters to copy
    char szDelims[20] = " \n";          // delimiters
    
    // check for past the end of the string
    if (*piBufferPosition >= (int) strlen(szInput))
    {
        szToken[0] = '\0';              // mark it empty
        return FALSE;                   // no more tokens
    }

    // get the position of the first delim, relative to the iBufferPosition
    iDelimPos = (int)(strcspn(&szInput[*piBufferPosition], szDelims));

    // see if we have more characters than target token, if so, trunc
    if (iDelimPos > MAX_TOKEN_SIZE)
        iCopy = MAX_TOKEN_SIZE;             // truncated size
    else
        iCopy = iDelimPos;
    
    // copy the token into the target token variable
    memcpy(szToken, &szInput[*piBufferPosition], iCopy);
    szToken[iCopy] = '\0';              // null terminate

    // advance the position
    *piBufferPosition += iDelimPos + 1;
    return TRUE;
}


/******************** processCommandSwitches *****************************
    void processCommandSwitches(int argc, char *argv[]
        , char **ppszUserFileName
        , char **ppszCommandFileName)
Purpose:
    Checks the syntax of command line arguments and returns the filenames.
    If any switches are unknown, it exits with an error.
Parameters:
    I   int argc                        count of command line arguments
    I   char *argv[]                    array of command line arguments
    O   char **ppszUserFileName         user file name
    O   char **ppszCommandFileName      command file name
Notes:
    If a -? switch is passed, the usage is printed and the program exits
    with USAGE_ONLY.
    If a syntax error is encountered (e.g., unknown switch), the program
    prints a message to stderr and exits with ERR_COMMAND_LINE_SYNTAX.
**************************************************************************/

void processCommandSwitches(int argc, char *argv[], char **ppszUserFileName
    , char **ppszCommandFileName)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        // check for a switch
        if (argv[i][0] != '-')
            exitUsage(i, ERR_EXPECTED_SWITCH, argv[i]);
        // determine which switch it is
        switch (argv[i][1])
        {
        case 'u':                   // User File Name
            if (++i >= argc)
                exitUsage(i, ERR_MISSING_ARGUMENT, argv[i - 1]);
            // check for too long of a file anme
            else
                *ppszUserFileName = argv[i];
            break;
        case 'c':                   // Command File Name
            if (++i >= argc)
                exitUsage(i, ERR_MISSING_ARGUMENT, argv[i - 1]);
            else
                *ppszCommandFileName = argv[i];
            break;
        case '?':
            exitUsage(USAGE_ONLY, "", "");
            break;
        default:
            exitUsage(i, ERR_EXPECTED_SWITCH, argv[i]);
        }
    }
}

/******************** exitError *****************************
    void exitError(char *pszMessage, char *pszDiagnosticInfo)
Purpose:
    Prints an error message and diagnostic to stderr.  Exits with
    ERROR_PROCESSING.
Parameters:
    I char *pszMessage              error message to print
    I char *pszDiagnosticInfo       supplemental diagnostic information
Notes:
    This routine causes the program to exit.
**************************************************************************/
void exitError(char *pszMessage, char *pszDiagnosticInfo)
{
    fprintf(stderr, "Error: %s %s\n"
        , pszMessage
        , pszDiagnosticInfo);
    exit(ERROR_PROCESSING);
}

/******************** exitUsage *****************************
    void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
Purpose:
    If this is an argument error (iArg >= 0), it prints a formatted message
    showing which argument was in error, the specified message, and
    supplemental diagnostic information.  It also shows the usage. It exits
    with ERR_COMMAND_LINE_SYNTAX.

    If this is just asking for usage (iArg will be -1), the usage is shown.
    It exits with USAGE_ONLY.
Parameters:
    I int iArg                      command argument subscript
    I char *pszMessage              error message to print
    I char *pszDiagnosticInfo       supplemental diagnostic information
Notes:
    This routine causes the program to exit.
**************************************************************************/

void exitUsage(int iArg, char *pszMessage, char *pszDiagnosticInfo)
{
    if (iArg >= 0)
        fprintf(stderr, "Error: bad argument #%d.  %s %s\n"
        , iArg
        , pszMessage
        , pszDiagnosticInfo);
    fprintf(stderr, "p3 -u userFileName -c commandFileName\n");
    if (iArg >= 0)
        exit(-1);
    else
        exit(-2);
}
