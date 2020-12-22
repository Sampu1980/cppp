//
//  src_ne/component/Util/SysLog.cpp
//
// Copyright (c) 2003-2009 Infinera Corporation
// All rights reserved.
//

#include <Util/SysLog.h>
#include <Trc/ZTrcTls.h>
#include <Trc/trcIfPrivate.h>
#include <OsEncap/OsThread.h>
#include <OsEncap/OsFileSystem.h>
#include <Util/ConsoleOutThr.h>

#include <assert.h>
#include <stdio.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <time.h>

#include <iomanip>
using namespace std;
#include <stdarg.h>


extern char* __progname;

namespace Util {

    static const int OP_CODE= _SLOG_SETCODE(_SLOG_SYSLOG, 0);

    static int GetSeverity(util::Severity_t s)
    {
        int slog_s = _SLOG_DEBUG2;

        switch (s)
        {
            case util::SEVERITY_AUDIT:
            slog_s = _SLOG_CRITICAL;
            break;
            case util::SEVERITY_DEBUG:
            slog_s = _SLOG_DEBUG1;
            break;
            case util::SEVERITY_ERROR:
            slog_s = _SLOG_ERROR;
            break;
            case util::SEVERITY_FATAL:
            slog_s = _SLOG_CRITICAL;
            break;
            case util::SEVERITY_INFO:
            slog_s = _SLOG_INFO;
            break;
            case util::SEVERITY_WARNING:
            slog_s = _SLOG_WARNING;
            break;
        }

        return slog_s;
    }

    static const int  PROCESS_NAME_LENGTH       = 22;
    static const int  SPACES_AFTER_PROCESS_NAME =  2;

    void PopulateHdrText( ZTrcTls::ZLargeOneLiner & zslogStruct );
    void SysLogFormatted( int severityInt, char sevChar,
                          const char *pFmt, va_list ap );
    void SysLogText( int severityInt, char sevChar,
                     const char *src, int srcLen = 0 );

    ConsoleOutThr  * gpConsoleOutputThread = NULL;


}  // Util namespace

namespace util
{
    bool gbDoConsoleOutput = false;
    bool gbDoConsoleInited = false;
}

//////////////////////////////////////////////////////////////////////
//   Adds a NUL char at the end of the message, returns ptr to TLS
//   and returns pointer to the data for printing.
//
void SysLogZSlogAndConsole( util::Severity_t s, bool doConsole )
{
    ZTrcTls *pMyTLS = ZTrcTls::GetMyTLS();

    ZOstreamWithHdr & rStrm = pMyTLS->mSmsgStrm;

    rStrm << std::ends;

    const char *data = rStrm.c_str();

    bool enabledSlogs = ZTrc::IsSlogEnabled();

    if (enabledSlogs)
    {
        zslog_basic( Util::OP_CODE, Util::GetSeverity(s),
                     rStrm.c_hdr(), rStrm.size() );
    }

    if (doConsole || (false == enabledSlogs))
    {
        time_t curTime = time( NULL );
        char timeBuf[26];

        // Format provided by ctime_r is: "Tue May  7 10:40:27 2002\n\0"
        ctime_r( &curTime, timeBuf );

        // Skip the truncated and/or nicely formatted process name
        // that is embedded in the data.  For the console, we
        // do not need any padding.
        //
        data += Util::PROCESS_NAME_LENGTH + Util::SPACES_AFTER_PROCESS_NAME;

        if (0 == Util::gpConsoleOutputThread)
        {
            // This prints two lines.
            printf( "%s%s: %s\n", timeBuf ,
                                  __progname, data);
        }
        else
        {
            Util::gpConsoleOutputThread->Write( timeBuf );
            Util::gpConsoleOutputThread->Write( __progname );
            Util::gpConsoleOutputThread->Write( ": " );
            Util::gpConsoleOutputThread->Write( data );
            Util::gpConsoleOutputThread->Write( "\n" );
            Util::gpConsoleOutputThread->WakeUp();
        }
    }

    pMyTLS->Reset();
}

/////////////////////////////////////////////////////////////////////
//static
std::ostream  & util::StartMsgOstream()
{
    std::ostream  *pZOStrm = ZTrcTls::GetMsgOstream();

    *pZOStrm  << std::setiosflags(ios_base::left)
              << std::setw(Util::PROCESS_NAME_LENGTH) << std::setfill(' ')
              << __progname
          << std::setw(Util::SPACES_AFTER_PROCESS_NAME) << std::setfill(' ')
              << "" /* prints x num of spaces */ ;
    return *pZOStrm;
}

/////////////////////////////////////////////////////////////////////
//static
void util::SysLogDumpStream(util::Severity_t s)
{
    SysLogZSlogAndConsole( s , false /* no console */ );
}

/////////////////////////////////////////////////////////////////////
//static
void util::ConsoleDumpStream(util::Severity_t s)
{
    SysLogZSlogAndConsole( s , true /* do console */ );
}

////////////////////////////////////////////////////////////////////
void util::SysLog(util::Severity_t s, const string& msg)
{
    (void) slogf( Util::OP_CODE, Util::GetSeverity(s), "%s\t %s",
                 __progname, msg.c_str());
}

/////////////////////////////////////////////////////////////////////
void util::ConsoleLog(util::Severity_t s, const string& msg)
{
    SysLog( s, msg );
    ConsoleTxtOnly( msg.c_str() );
}

/////////////////////////////////////////////////////////////////////
void util::ConsoleTxtOnly( const char *text )
{
    time_t curTime = time( NULL );
    char timeBuf[26];

    ctime_r( &curTime, timeBuf );

    if (0 == Util::gpConsoleOutputThread)
    {
        printf( "(%ld) %s", curTime, timeBuf );
        printf( "%s: %s\n", __progname, text );
    }
    else
    {
        char someBuff[ 120 ];
        snprintf( someBuff, sizeof(someBuff), "(%ld) %s", (long int)curTime, timeBuf );
        Util::gpConsoleOutputThread->Write( someBuff );
        Util::gpConsoleOutputThread->Write( __progname );
        Util::gpConsoleOutputThread->Write( ": " );
        Util::gpConsoleOutputThread->Write( text );
        Util::gpConsoleOutputThread->Write( "\n" );
        Util::gpConsoleOutputThread->WakeUp();
    }
}

/////////////////////////////////////////////////////////////////////
void Util::PopulateHdrText( ZTrcTls::ZLargeOneLiner & zslogStruct )
{
    const char *threadIdStr = OsThread::Current() ?
                              OsThread::Current()->GetName() : "0";

    char *ownerName = zslogStruct.mBuffHdr.mThreadChars;
    size_t dstOwnerNameSize = sizeof( zslogStruct.mBuffHdr.mThreadChars );

#if defined(QNX) || defined(LINUX_COMPILE)
    // Overflow by one character is desired to avoid leaving a
    // NUL char.  This is why the spaces follow the data in the
    // format string below.
    //
    // We are intentionally overwriting the end of the ownerName
    // buffer.  mBuffHdr has some room to spare.
    (void) snprintf( ownerName, dstOwnerNameSize + 1,
                     "%.*s:%s                     ",
                     (int)ZTrcTls::PROGNAME_LEN, __progname, threadIdStr );
#else
    // We overflow on purpose
    sprintf( ownerName, ".*s                  ",
             dstOwnerNameSize - 5, threadIdStr);
#endif

    zslogStruct.mBuffHdr.mSpace1[0]  = ' ';
    zslogStruct.mBuffHdr.mSpaces2[0] = ' ';
    zslogStruct.mBuffHdr.mSpaces2[1] = ' ';
    zslogStruct.mBuffHdr.mSevChar[0] = '?';
}

/////////////////////////////////////////////////////////////////////
void Util::SysLogFormatted( int severityInt, char sevChar,
                            const char *pFmt, va_list ap )
{
    ZTrcTls::ZLargeOneLiner  oneLineStruct;

    Util::PopulateHdrText( oneLineStruct );
    oneLineStruct.mBuffHdr.mSevChar[0] = sevChar;

#if defined(QNX) || defined(LINUX_COMPILE)
    int toPrint = vsnprintf( oneLineStruct.mRestOfChars,
                             sizeof(oneLineStruct.mRestOfChars),
                             pFmt, ap );
    toPrint +=  sizeof(oneLineStruct.mBuffHdr) + 1 /* EndOfString */ ;

    zslog_basic( _SLOG_SETCODE(_SLOG_SYSLOG, 0) , severityInt,
                 &oneLineStruct, toPrint );
#else
    strncpy( oneLineStruct.mRestOfChars,
             "No support for this target", 26 );
#endif
}

/////////////////////////////////////////////////////////////////////
void util::SysLogDebugF( const char *pFmt, ... )
{
    va_list ap;
    va_start( ap, pFmt );
    Util::SysLogFormatted( _SLOG_DEBUG1, 'D', pFmt, ap );
    va_end(ap);
}

void util::SysLogAuditF( const char *pFmt, ... )
{
    va_list ap;
    va_start( ap, pFmt );
    Util::SysLogFormatted( _SLOG_NOTICE, 'A', pFmt, ap );
    va_end(ap);
}

void util::SysLogErrorF(   const char *pFmt, ... )
{
    va_list ap;
    va_start( ap, pFmt );
    Util::SysLogFormatted( _SLOG_ERROR, 'E', pFmt, ap );
    va_end(ap);
}

void util::SysLogInfoF(    const char *pFmt, ... )
{
    va_list ap;
    va_start( ap, pFmt );
    Util::SysLogFormatted( _SLOG_INFO, 'I', pFmt, ap );
    va_end(ap);
}

void util::SysLogWarningF( const char *pFmt, ... )
{
    va_list ap;
    va_start( ap, pFmt );
    Util::SysLogFormatted( _SLOG_WARNING, 'W', pFmt, ap );
    va_end(ap);
}

/////////////////////////////////////////////////////////////////////
void Util::SysLogText( int severityInt, char sevChar,
                       const char *src, int srcLen )
{
    ZTrcTls::ZLargeOneLiner  oneLineStruct;

    Util::PopulateHdrText( oneLineStruct );
    oneLineStruct.mBuffHdr.mSevChar[0] = sevChar;

#if defined(QNX) || defined(LINUX_COMPILE)
    if (srcLen <= 0)
    {
        srcLen = strlen( src );
    }
    const int  cMaxLenPerSlog = sizeof(oneLineStruct.mRestOfChars) -1;
    oneLineStruct.mRestOfChars[ cMaxLenPerSlog ] = '\0';

    while (srcLen > 0)
    {
        char *dst = oneLineStruct.mRestOfChars;

        // Scan up to cMaxLenPerSlog contiguous bytes with complete sentences
        if (srcLen <= cMaxLenPerSlog)
        {
            memcpy( dst, src, srcLen );
            dst[ srcLen ] = '\0';
            int toPrint = srcLen + 1 + sizeof(oneLineStruct.mBuffHdr);

            zslog_basic( _SLOG_SETCODE(_SLOG_SYSLOG, 0) , severityInt,
                         &oneLineStruct, toPrint );
            return;
        }

        // Try to truncate to the last new-line char within
        // the range printable in a single call to zslog
        const char *scanBkwdStart = src + cMaxLenPerSlog;
        const char *scanBkwd = scanBkwdStart;
        while ( *scanBkwd != '\n' && scanBkwd > src )
            --scanBkwd;

        if (scanBkwd == src)            // No new line chars found
        {
            scanBkwd = scanBkwdStart;
        }

        int tmpSrcLen = scanBkwd - src;
        {
            memcpy( dst, src, tmpSrcLen );
            dst[ tmpSrcLen ] = '\0';
            int zsLen = tmpSrcLen + 1 + sizeof(oneLineStruct.mBuffHdr);

            zslog_basic( _SLOG_SETCODE(_SLOG_SYSLOG, 0) , severityInt,
                         &oneLineStruct, zsLen );
        }

        srcLen -= tmpSrcLen;
        src += tmpSrcLen;
    }
#else
    strncpy( oneLineStruct.mRestOfChars,
             "No support for this target", 26 );
#endif
}


/////////////////////////////////////////////////////////////////////
void util::SysLogDebug( const char *text )
{
    Util::SysLogText( _SLOG_DEBUG1, 'D', text );
}

void util::SysLogAudit( const char *text )
{
    Util::SysLogText( _SLOG_NOTICE, 'A', text );
}

void util::SysLogError(   const char *text )
{
    Util::SysLogText( _SLOG_ERROR, 'E', text );
}

void util::SysLogInfo( const char *text )
{
    Util::SysLogText( _SLOG_INFO, 'I', text );
}

void util::SysLogWarning( const char *text )
{
    Util::SysLogText( _SLOG_WARNING, 'W', text );
}

void util::SetConsoleLogging( )
{
    char ForceConsoleFile[] = "/f0/base/ConsoleOn.txt";
    char ForceConsoleFileAlt[] = "/dev/shmem/ConsoleOn.txt";

    if ( OsFileSystem::IsExist(ForceConsoleFile) ||
         OsFileSystem::IsExist(ForceConsoleFileAlt) )
    {
        // std::cout << "Enable SysLog Console" << std::endl;
        util::gbDoConsoleOutput = true;
    }
    util::gbDoConsoleInited = true;
}

void util::InitConsoleOutputThread( int threadPrio, size_t buffSize )
{
    ConsoleOutThr *p = new ConsoleOutThr( threadPrio, buffSize );
    ConsoleOutThrService::InstallInstance( p );
    Util::gpConsoleOutputThread = ConsoleOutThrService::Instance();
    Util::gpConsoleOutputThread->Start();
}


