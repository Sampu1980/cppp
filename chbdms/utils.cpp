
// $Id: //swdepot/main/src_ne/diag/testHandler/utils.cpp#3 $
// $Change: 61672 $
// $DateTime: 2004/05/27 14:41:41 $
//
// $Author: jjiang $
//
// Copyright(c) Infinera 2003
//
//
/*
 * Define a unique identifier for this source file.
 */
#define _UTILS_C_

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <ZSys.h>
#include <Trc/trcIf.h>
#include <Util/ZTrace.h>

#include "Ics/MsgListenerIf.h"
#include "NvUtil/NvReader.h"
#include "NvUtil/NvWriter.h"
#include "Ics/NvBufMsg.h"

//global variables for Nv defined in diagShell.cpp
extern NvReader *rdrDiag;
extern NvWriter *nvwDiag;

std::ostream  *logStream = NULL;

void setLogStream (std::ostream  *os)
{
    logStream = os;
}

extern "C" {

#include "diagtypes.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "types.h"
#include "global.h"
#include "utils.h"
#include "stdarg.h"


#define SLAVE_ROLE 0
extern int  my_role;

//int cmdMaskCmpMemW(int argc, char **argv);

//int nv_print (*NvReader *rdr, NvWriter& nvw )
int nv_printf (char *outString)
{
   nvwDiag->Append("ReturnedStr", outString);

   return ZOK;
}

int is_terminator(char *c)
{
    int flag;

    switch((int)*c)
    {
        case '\0':
        case '\r':
        case ';':
            flag = TRUE;   /* is a terminator */
            break;
        default:
            flag = FALSE;  /* is NOT a terminator */
            break;
   }
   return(flag);
}

int is_separator(char *c)
{
    int flag;

    switch ((int)*c)
    {
        case '\0':
        case ' ':
        case '\r':
        case ';':
        case '/':
            flag = TRUE;              /* is a terminator */
            break;
        default:
            flag = FALSE;             /* is NOT a terminator */
            break;
   }
   return(flag);
}

#define __ (char)255
static const char hexTable[256] = {
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, __, __, __, __, __, __,
  __, 10, 11, 12, 13, 14, 15, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, 10, 11, 12, 13, 14, 15, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
  __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
};
#undef __

/*
** a_con_bin(c) - convert an ascii char to its binary representation
*/
int a_con_bin(char c)
{
   return hexTable[(int)c];
}


/*
** atob(str,ptrnum,base) convert an ascii string to an integer.
**  this function is non-standard and has the following
**  characteristic:
**
**      bases: hex(0x) decimal()
**      sign: recocognizes leading '-' sign
**      base: specifies the radix
**  returns:
**      pointer to the characrer in the string that
**      terminated the conversion
*/

char *ator (char *str, int *ptrnum, int base)
{
    int num;
    int minus;
    char *end_str;

    num = 0;
    while (isspace(*str))  // Remove leading spaces
        str++;

    if (*str == '-' )
    {
        minus = 1;
        str++;             // Skip the minus sign
    }
    else
         minus = 0;

    if (*str == '0' )      // Number in the format 0x1234 or 0d1234
    {
       switch (str[1])
       {
            case 'x':
            case 'X':
                end_str = atoui (str + 2, (U32*)&num,16);
                break;
            case 'd':
            case 'D':
                end_str = atoui (str + 2, (U32*)&num,10);
                break;
            default:
                end_str = atoui (str, (U32*)&num, base);
                break;
        }
    }
    else
       end_str = atoui (str, (U32*)&num, base );

    if (minus)
    {
        if (num < 0)
            diagPrintf(V0, "Signed Overflow");
        else
            num = -num;
    }

    *ptrnum = num;
    return (end_str);
}

char *atob (char *str, int *ptrnum, int base)
{
    int num;
    char *end_str;

    end_str = ator (str, &num, base);

    *ptrnum = num;

    return (end_str);
}

/*
** atoui(str,ptrnum,base) - converts a ascii string to an unsigned long integer
**             value - returns a count of the chars in
**             string. Will return a -1 if there is an overflow
**             or an illegal character for the base specified
**  entry:
**      char *str - pointer to ascii string
**      unsigned *ptrnum - pointer to unsigned int
**      unsigned base - the base
**
**  return:
**      current str pointer (char *str)
*/
char *atoui (char *str, U32 *ptrnum, int base)
{

    U32 next_nib;
    U32 accum, tmp;
    U32 max_accum;

    if (base == 0) {
        return (str);
    }

    max_accum = 0xffffffff / base;

    accum = 0;
    while ((next_nib = a_con_bin(*str)) < (U32)base)
    {
        if (accum > max_accum)
        {
            diagPrintf(V0, "Integer Overflow: accum=%d > max_accum=%d, next_nib=%d\n", accum, max_accum, next_nib);
            break;
        }
        tmp = (accum*base) + next_nib;

        if (tmp < accum)
        {
            diagPrintf(V0, "Integer Overflow: tmp=%d < accum=%d, next_nib=%d\n", tmp, accum, next_nib);
            break;
        }
        accum = tmp;
        str++;
    }
    *ptrnum = accum;
    return(str);
}

/*
** askmore() asks the question MORE? (any key/n)
**  returns FALSE if the user answers n for no
**      or TRUE if the user wants more
*/
int askmore(void)
{
    char c;

        diagPrintf(V0, "    \n--MORE-- (any key to continue/n|q)? ");
        c = getchar() & 0x7f;
        diagPrintf(V0, "   \n");
#ifdef OLD_CODE
        if(c == 'n' || c == 'N' || c == 'q' || c == 'Q' || c == '\r')
#else
        if(c == 'n' || c == 'N' || c == 'q' || c == 'Q')
#endif
            return(FALSE);
        else
            return(TRUE);
}

//
//    This function works like printf except it depends on the verbosity level. The global
//  verbosity is set by the command "setp verb=xxx" and shown by "showp" command.  When
//  verbosity value is less than or equal to the global verbosity, i.e. gDiagFlags.nVerbosity
//  then diagPrint behaves like print.  Otherwise nothing will be printed.  By changing the
//  global verbosity, we can control what is printed and what is not printed out.  The
//  following is the guideline for verbosity levels used in a test
//
//
//    V1: test name, progressing information.  For example
//        "Testing memory from 0x0010000 to 0x0100000"
//
//    V2: to display error messages intended for users.  For example
//        "Error: data miscompared at 0x10000 act=0x1234 exp=0x4321"
//
//    V3: to display progressing information.  For example
//        "Testing chip 1: OK, chip 2= failed ..."
//
//    V4: Trace data.  For example at the beginning of a routine we want to display all
//        the passing parameters in diagPktCompare:
//        "diagPktCompare, x=1, y=2, z=3"
//
//    V5: More detail trace data if needed, for example
//        "Waiting for interrupt from Hissa ..."
//
//    By default, the global verbosity in gDiagFlags.nVerbosity is set to V3 so Diag will
//    display test names, progressing information and error messages if any.  When this
//    is set to 1, Diag just displays test name and PASS/FAIL status with no error messages.
//

/*
 * Note: All calls to debugPrintf() on slave CID process cause a
 * message to be sent to the master so the master can display it.
*/
//int nv_print (NvWriter* nvw,  char *outString)

void diagPrintf (int verbosity, char *string, ...)
{
    char       buf [1024];
    va_list    ap;
#if 0
    FILE*      outFile = NULL;
#endif


    if (verbosity <= gDiagFlags.nVerbosity || gDiagFlags.logFile && verbosity <= V2 || verbosity == AUDIT)
    {
        va_start (ap, string);
        vsprintf (buf, string, ap);
        va_end (ap);

        if (gDiagFlags.logFile && verbosity <= V2)
           TRC_MSG(bpost,1, (buf));

        if (verbosity == AUDIT)
        {
            TRC_SMSG(audit, 1, (buf));
            verbosity = V1;
        }

        if (verbosity <= gDiagFlags.nVerbosity)

        if (logStream == NULL || verbosity <= V2)
        {
            if (nvwDiag == NULL)
	        {
	            // global variablr "nvwDiag", normal
                fflush(stdout);
                printf (buf);   // If not Common User version, just display msg.
                fflush(stdout);
            }
            else       //DBI
                nv_printf(buf);
        }

        if (logStream != NULL)
        {
            (*logStream) << buf << std::endl;
        }

#if 0
       //output to error log file
        if (gDiagFlags.logFile)
        {
            if ((outFile = fopen(fileName, "a")) != NULL)
            {
                fprintf(outFile, buf);
                fclose(outFile);
            }
        }
#endif
    }
}

/*
//exec script
int execScript( const char* filename )
{
   FILE* pFile = NULL;
   char* pBuffer = NULL;
   char  sCommand[21];
   U32	nValue = 0;
   U32	nLineCount = 0;
   U8	   nCommandLen = 0;
   int   nIndex = 0;
   char* t_argv[6];

   pFile = fopen( filename, "r" );
   if ( pFile == NULL ) {
      diagPrintf(V1,"Error: Can't open file: %s\n", filename);
      return FAIL;
   }

   pBuffer = malloc(1024);

   while ( !feof(pFile) )
   {
      sCommand[0]='\0';
      for (nIndex=1; nIndex<6; nIndex++)
         t_argv[nIndex][0] = '\0';

      if ( fgets( pBuffer, 1024, pFile ) == NULL )
         continue;
      nLineCount++;

      sscanf(pBuffer, "%s%s%s%s%s%s", sCommand, t_argv[1], t_argv[2], t_argv[3], t_argv[4], t_argv[5]);
      diagPrintf( V2,"%s %s %s %s %s %s\n", sCommand, t_argv[1], t_argv[2], t_argv[3], t_argv[4], t_argv[5]);

      nCommandLen = strlen(sCommand);

      if ( nCommandLen == 0)
         continue;

      for ( nIndex = 0; nIndex < nCommandLen; nIndex++ )
      {
         sCommand[nIndex] = toupper(sCommand[nIndex]);
      }

      if ( strcmp(sCommand, "SMB") == 0 )
      {
         if (strlen(t_argv[3])==0)
            cmdDiagWriteMemB(3, t_argv);
         else
            cmdDiagWriteMemB(4, t_argv);
      }
      else if ( strcmp(sCommand, "SMW") == 0 )
      {
         if (strlen(t_argv[3])==0)
            cmdDiagWriteMemW(3, t_argv);
         else
            cmdDiagWriteMemW(4, t_argv);
      }
      else if ( strcmp(sCommand, "SML") == 0 )
      {
         if (strlen(t_argv[3])==0)
            cmdDiagWriteMemL(3, t_argv);
         else
            cmdDiagWriteMemL(4, t_argv);
      }
      else if ( strcmp(sCommand, "DMB") == 0 )
      {
         if (strlen(t_argv[2])==0)
            cmdDiagReadMemB(2, t_argv);
         else
            cmdDiagReadMemB(3, t_argv);
      }
      else if ( strcmp(sCommand, "DMW") == 0 )
      {
         if (strlen(t_argv[2])==0)
            cmdDiagReadMemW(2, t_argv);
         else
            cmdDiagReadMemW(3, t_argv);
      }
      else if ( strcmp(sCommand, "DML") == 0 )
      {
         if (strlen(t_argv[2])==0)
            cmdDiagReadMemL(2, t_argv);
         else
            cmdDiagReadMemL(3, t_argv);
      }
      else if ( strcmp(sCommand, "CW") == 0 )
      {
         if (strlen(t_argv[3])==0)
            cmdMaskCmpMemW(3, t_argv);
         else if (strlen(t_argv[4])==0)
            cmdMaskCmpMemW(4, t_argv);
         else
            cmdMaskCmpMemW(5, t_argv);
      }
      else if ( strcmp(sCommand, "DELAY") == 0 )
      {
         if (strlen(t_argv[1])==0) {
		      printf( "Too few arguments.\n" );
  		      printf( "Usage: delay <value>\n" );
         } else {
            atoui(t_argv[1], &nValue, 16);
            delay(nValue);
         }
      }

      else if ( sCommand[0] == '*' )   //comment line starting with *
         continue;

      else if ( strcmp(sCommand, "TEST") == 0 )
      {
         for (nIndex=1; nIndex<6; nIndex++) {
            if (strlen(t_argv[nIndex])==0)
               break;
         }
         test_cmd(nIndex, t_argv);
      }
      else if ( strcmp(sCommand, "SETP") == 0 )
      {
         for (nIndex=1; nIndex<6; nIndex++) {
            if (strlen(t_argv[nIndex])==0)
               break;
         }
         setParaCmd(nIndex, t_argv);
      }
      else if ( strcmp(sCommand, "SHOWP") == 0 )
      {
         for (nIndex=1; nIndex<6; nIndex++) {
            if (strlen(t_argv[nIndex])==0)
               break;
         }
         showParaCmd(nIndex, t_argv);
      }

      //add more commands here
      else
      {
         diagPrintf( V2, "Error: Unsupported Cmd : %s \n", sCommand );
      }
   }

   free(pBuffer);
   fclose(pFile );

   return PASS;
}
*/

/*
int cmdExecScript (int argc, char **argv)
{

#ifdef CID_COMMON_USER
   if (i_am_master()) {
        issue_cid_cmd(argc, argv);    // If Master, Pass this command on to slaves
        if (!master_runs_test())      // Should the master execute this command as well ?
            return (PASS);
   }
#endif

    if ( argc < 2 )
    {
      diagPrintf( V2, "Executes the EST VisionClick Style Scripts!\n" );
      diagPrintf( V2, "Executes the EST VisionClick Style Scripts!\n" );
      diagPrintf( V2, "Usage : %s <filename>\n", argv[0] );
      return FAIL;
    }
    if ( execScript( argv[1] ) != PASS )
    {
      diagPrintf( V2, "Unable to Execute File (%s)!\n", argv[0] );
      return FAIL;
    }

    return PASS;
}
      diagPrintf( V2, "Executes the EST VisionClick Style Scripts!\n" );
      cout <<'
*/


} //extern "C"

