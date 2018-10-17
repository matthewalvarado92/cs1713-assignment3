/**********************************************************************
p3abc123.c  by LastName, FirstName
(change the previous line for your abc123 ID and your name)
Purpose:
    This program reads user information and a command file.  It
    processes the commands against the user information.
    This is the file containing functions written by the students.
 
Input:
 User:   same as Programming Assignment #2
 
 Command:
        This is different from the previous assignment.
        The file contains text in the form of command.
 
    VEH FILL szUserId1 dPPG iMiles dGallons
            This command will cause processFillup to process the data
    VEH FIX szUserId dPPG iMiles dGallons
            Change the vehicle data by removing a fillup from the data:
               - change the user’s vehicle data and reduce the fillups by 1
               - don’t modify a user with 0 fillups
            This functionality should be placed in the processFillupFix function.
            If a user doesn't exist, show a warning.
    USER NEWVEH szUserId szMake szModel
             Change the make and model of this user’s car and reset the average MPG,
                average PPG, and number of fillups to zero.
                If the user doesn't exist, show a warning.
    USER SHOW szUserId
             Show all the user information for this one user. If the user doesn't exist,
             show a warning.

Notes:
    This file contains functions:
       getUsers - retrieves the users and places them in an array for the specified file.
       sortUsers - sorts the user array by user ID
       printUsers - prints the user array's contents
       processVehCommand - processes the subcommands associated with the VEH command
       processUserCommand - processes the subcommands associated with the USER command
       searchUsers - using a binary search, finds the specified user in the userM array.
       processFillup - processes a fillup data record, checking the updating the user
            information base on the data provided
       processFillupFix - processes a fix to user information, checking the data and
            possibly changing the PPG and MPG data for the user.
  
**********************************************************************/
// If compiling using visual studio, tell the compiler not to give its warnings
// about the safety of scanf and printf
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cs1713p3.h"


/******************** getUsers ********************************************
int getUsers(User userM[])
Purpose:
    takes input from a text file stream, parses it into vehicle and
    user data, and populates the array userM array with the users
Parameters:
    O   User    userM[]         // array populated with user data
    I   char *pszUserFileName   // name of the file containing the user data
 Return:
    int iUserCnt, the number of users read from the file
Notes:
    If there is an error reading from the file or the data read from
 the file is poorly formatted, this function will report the error
 and terminate the program
    The file contains two different lines of data for each user:
    - User Vehicle Information:
        o One line per user (separated by commas)
            szMake  szModel  dMpg  dPpg  iNumFillups
            10s      10s     lf    lf    d
    - User Contact Information:
        o One line per user (separated by commas)
            szUserId  szEmail  szFullName  szPhone  szLoginName
            6s        30s      20s         13s      12s
        o Use caution when trying to convert szFullName.
          Although it is a maximum of 20 characters, it may
          contain spaces; therefore, you cannot simply use %20s.

**************************************************************************/
int getUsers(User userM[], char *pszUserFileName)
{
    char szInputBuffer[MAX_LINE_SIZE];   // some memory to read lines from file into
    char *pszNextLine = NULL;            // a pointer to the next line to be parsed
    int iScanfCount = 0;                 // number of valid conversions for sscanf
    int i = 0;                    // number of users obtained from file
    FILE *pFileUsers;                    // Stream Input for User data
    
    /* open the Users stream data file */
    if (pszUserFileName == NULL)
        exitError(ERR_MISSING_SWITCH, "-u");

    pFileUsers = fopen(pszUserFileName, "r");
    if (pFileUsers == NULL)
        exitError(ERR_USER_FILENAME, pszUserFileName);
    
    // get user informstion unctil EOF
    // fgets() returns NULL when EOF is reached

    /***** your code *******/
    while (fgets(szInputBuffer,MAX_LINE_SIZE, pFileUsers) != NULL) {
        if (szInputBuffer[0] == '\n')
            continue;
            iScanfCount = sscanf(szInputBuffer, "%10s %10s %lf %lf %d\n"
                , userM[i].szMake
                , userM[i].szModel
                , &userM[i].dAvgMPG
                , &userM[i].dAvgPPG
                , &userM[i].iNumFillups);
            if (iScanfCount < 5)
            {
                char szErrorMessage[] = "\nError: The scan count is less than 5" ;
                exitError(ERR_USER_CONTACT_DATA,szErrorMessage);
            }
            fgets(szInputBuffer, MAX_LINE_SIZE, pFileUsers);
            iScanfCount = sscanf(szInputBuffer, "%6s,%[^,],%[^,],%[^,],%[^[\n,]]"
                , userM[i].szUserId
                , userM[i].szEmailAddr
                , userM[i].szFullName
                , userM[i].szPhone
            , userM[i].szLoginName);
            if (iScanfCount < 5){
                char szErrorMessage[] = "\nError: The scan count is less than 5" ;
                exitError(ERR_USER_CONTACT_DATA,szErrorMessage);
            }
            i++;
    }

    
    
    fclose(pFileUsers);
    return i;
}

/******************** sortUsers **************************************
    void sortUsers(User UserM[], int iUserCnt)
Purpose:

Parameters:
 
Returns:
 
Notes:
 
**************************************************************************/
void sortUsers(User userM[], int iUserCnt)
{
    /**** your code ******/
   int i ,j;
   int bChange; //boolean
   User userTemp;
   
   for(i = 0; i < (iUserCnt-1); i++)
   {
       bChange = FALSE;
        for(j = 1; j < (iUserCnt -i -1); j++)
        {
            if(strcmp(userM[j+1].szUserId, userM[j].szUserId) < 0)
            {
                userTemp = userM[j+1];
                userM[j+1] = userM[j];
                userM[j] = userTemp;
                bChange = TRUE;
            }
        }
        if( ! bChange)
            break;
   }

    
    
}

/********************processUserCommand *****************************
    void processUserCommand(User userM[], int iUserCnt
     , char *pszSubCommand, char *pszRemainingInput)
Purpose:
    Processes the subcommands associated with the USER command:
    USER UPDVEH szUserId szMake szModel
             Change the make and model of this user’s car and reset the average MPG,
                average PPG, and number of fillups to zero.
                If the user doesn't exist, show a warning.
    USER SHOW szUserId
             Show all the user information for this one user. If the user doesn't exist,
             show a warning.
Parameters:
    I/O User userM[]                  Array of users
    I   int   iUserCnt                Number of elements in userM[]
    I   char  *pszSubCommand          Should be NEWVEH or SHOW
    I   char  *pzRemainingInput       Points to the remaining characters in the input
                                      line (i.e., the characters that following the
                                      subcommand).
Notes:

**************************************************************************/
void processUserCommand(User userM[], int iUserCnt
                        , char *pszSubCommand, char *pszRemainingInput)
{
    int iScanfCnt = 0;
    char szUserId[7];           // temporary User Id
    char szMake[11];            // temporary Make
    char szModel[11];           // temporary Model
    int i;                      // subscript of a user in the user array

    
    // Determine what to do based on the subCommand
    if (strcmp(pszSubCommand, "UPDVEH") == 0)
    {
        // get the szUserId and szMake and szModel
        // your codei
        iScanfCnt = sscanf(pszRemainingInput, "%6s %10s %10s", szUserId, szMake, szModel);
            i = searchUsers(userM, iUserCnt, szUserId);
            //userM[i].szMake = szMake;
            memcpy(userM[i].szMake,szMake,sizeof(szMake));
            //userM[i].szModel = szModel;
            memcpy(userM[i].szModel, szModel, sizeof(szModel));
            userM[i].dAvgMPG = 0.0;
            userM[i].dAvgPPG = 0.0;
            userM[i].iNumFillups = 0;

        
        // Check for bad data
        if (iScanfCnt < 3)
            exitError(ERR_USER_NEWVEH_DATA, pszRemainingInput);
        
        // your code which uses searchUsers and updates the paid amount

    }
    else if (strcmp(pszSubCommand, "SHOW") == 0)
    {
        // get the szUserId
        iScanfCnt = sscanf(pszRemainingInput, "%6s", szUserId);
        
        // Check for bad data
        if (iScanfCnt < 1)
            exitError(ERR_USER_SHOW_DATA, pszRemainingInput);
        
        // find the user
        i = searchUsers(userM, iUserCnt, szUserId);
        if (i < 0)
        {
            printf(WARN_USER_NOT_FOUND, szUserId);
            return;
        }
        
        // your code
        printf("USER SHOW %s", userM[i].szUserId);
        printf("  %-7s %-10s %-10s  %6.2lf  %6.2lf\n", userM[i].szUserId, userM[i].szMake, userM[i].szModel, userM[i].dAvgMPG, userM[i].dAvgPPG);
        printf("     %-20s %-13s   %-12s %-30s\n", userM[i].szFullName, userM[i].szPhone, userM[i].szLoginName, userM[i].szEmailAddr );
    }
    else printf(WARN_USER_SUB_COMMAND, pszSubCommand);
}


/********************processVehCommand *****************************
    void processVehCommand(user userM[], int iuserCnt
         , char *pszSubCommand, char *pszRemainingInput)
Purpose:
    Processes the subcommands associated with the VEH command:
        VEH FILL szUserId dPPG iMiles dGallons
            uses processFillup to process this command
        VEH FIX szUserId dPPG iMiles dGallons
            Change the vehicle data by removing a fillup from the data:
               - change the user’s vehicle data and reduce the fillups by 1
               - don’t modify a user with 0 fillups
            Note that the users must exist in the user array.  If not, show a warning.
 
Parameters:
    I/O user userM[]                  Array of users
    I   int   iuserCnt                Number of elements in userM[]
    I   char  *pszSubCommand          Should be CHANGE or SHOW
    I   char  *pzRemainingInput       Points to the remaining characters in the input
                                      line (i.e., the characters that following the
                                      subcommand).
Notes:

**************************************************************************/
void processVehCommand(User userM[], int iUserCnt
                       , char *pszSubCommand, char *pszRemainingOutput)
{
    Fillup fillup;
    int iScanfCount;
    
    // Determine what to do based on the subCommand
    // your code
    iScanfCount = sscanf(pszRemainingOutput,"%s %s %lf %d %lf"
    , pszSubCommand
    , fillup.szUserId
    , &fillup.dPPG
    , &fillup.iMiles
    , &fillup.dGallons);

    if(strcmp(pszSubCommand,"FIX"))
        processFillupFix(fillup, userM, iUserCnt);
    else if(strcmp(pszSubCommand, "FILL"))
        processFillup(fillup, userM, iUserCnt);
    
    
    
}

/******************** searchUsers *****************************
    int searchUsers(User userM[], int iUserCnt, char *pszMatchUserId)
Purpose:
    Using a binary search, it finds the specified user in the userM array.
Parameters:
    I   User userM[]                 Array of users
    I   int   iUserCnt               Number of elements in userM[]
    I   char *pszMatchUserId         User Id to find in the array
Returns:
    >= 0   subscript of where the match value was found
    -1     not found
Notes:

**************************************************************************/
int searchUsers(User userM[], int iUserCnt, char *pszMatchUserId)
{
    // your code
    int iLb = 0;
    int iUb = iUserCnt -1;
    int iMid;
    while(iLb < iUb)
    {
        iMid = (iLb + iUb) / 2;
        if(pszMatchUserId == userM[iMid].szUserId)
            return iMid;
        else if(pszMatchUserId < userM[iMid].szUserId)
            iUb = iMid -1;
        else
            iLb = iMid + 1;
    }


    
    return -1;        // not found
}

/*** include your processFillup code from Program #2.  Change it to use
    your binary search ***/
// your code
void processFillup(Fillup fillup, User userM[], int iUserCnt){
    int i;
    //printing out the infomation.
    //printf("  %s        %.2lf    %d  %5.1lf"  , fillup.szUserId, fillup.dPPG, fillup.iMiles, fillup.dGallons);
    i = searchUsers(userM, iUserCnt, fillup.szUserId);
     //computering the new averages and shit.
     if (i != -1){
         printf("\n");
         userM[i].dAvgMPG =( (userM[i].dAvgMPG*userM[i].iNumFillups)+(fillup.iMiles/fillup.dGallons)) / (userM[i].iNumFillups + 1);
         userM[i].dAvgPPG =( (userM[i].dAvgPPG * userM[i].iNumFillups) + fillup.dPPG ) / (userM[i].iNumFillups + 1);
         userM[i].iNumFillups++;
     }
}




// documentation required
void processFillupFix(Fillup fillup, User userM[], int iUserCnt)
{
    int i;
    // your code
    i = searchUsers(userM, iUserCnt, fillup.szUserId);

    if (i != -1)
    {
        printf("\n");
        userM[i].dAvgMPG =( (userM[i].dAvgMPG*userM[i].iNumFillups)-(fillup.iMiles/fillup.dGallons)) / (userM[i].iNumFillups - 1);
        userM[i].dAvgPPG =( (userM[i].dAvgPPG * userM[i].iNumFillups) - fillup.dPPG ) / (userM[i].iNumFillups - 1);
        userM[i].iNumFillups--;
    }
}
// documentation required
void printFillup(Fillup fillup)
{
    printf("%6s %11.2lf %6d %7.2lf\n"
           , fillup.szUserId
           , fillup.dPPG
           , fillup.iMiles
           , fillup.dGallons);
}


/******************** printUsers *****************************
   void printUsers(char szHeading[], User userM[], int iUserCnt)
Purpose:
    Prints user information.
Parameters:
    i char szHeading[]  string describing why the users are printed
                        (e.g., "Original User Information")
    i User userM[]      array of users
    i int iUserCnt      number of users
Notes:
    Prints two lines of text per user.
**************************************************************************/
void printUsers(char szHeading[], User userM[], int iUserCnt)
{
    int i;          // local counter
    // Print the heading
    printf("%s\n", szHeading);
    // print column headings
    printf("  %-7s %-10s %-10s  %-6s  %-6s\n", "UserID", "Make", "Model", "AvgMPG", "AvgPPG");
    printf("     %-20s %-13s   %-12s %-30s\n", "Name", "Phone", "login", "email");
    // iterate through the userM array
    for (i = 0; i < iUserCnt; i++)
    {
        printf("  %-7s %-10s %-10s  %6.2lf  %6.2lf\n", userM[i].szUserId, userM[i].szMake, userM[i].szModel, userM[i].dAvgMPG, userM[i].dAvgPPG);
        printf("     %-20s %-13s   %-12s %-30s\n", userM[i].szFullName, userM[i].szPhone, userM[i].szLoginName, userM[i].szEmailAddr );
    }
    printf("\n");
}

