//
//  src_ne/component/Util/Backtrace.cpp
//
// Copyright (c) 2003-2009 Infinera Corporation
// All rights reserved.
//

#include <Util/Backtrace.h>
#include <Util/BaseInfo.h>
#include <Util/TracebackStack.h>
#include <Util/FixedTextBuffer.h>
#include <Util/SysLog.h>
#include <Trc/trcIf.h>
#include <OsEncap/OsThread.h>
#include <iomanip>
#include <unistd.h>
#include "Q2LinAtomic.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/procfs.h>
#include <sys/neutrino.h>
#include <ucontext.h>
#include <atomic.h>
#ifdef LINUX_COMPILE
#include <execinfo.h>
#include <cxxabi.h>
#include <execinfo.h>
#include <dlfcn.h>
#endif

extern char* __progname;

extern "C" { void __my_thread_exit(); }

#ifdef Z_arm   // LINUX_BROKEN
const unsigned int shared_lib_addr_prefix = 0xB0000000;
const unsigned int shared_lib_addr_mask   = 0xF0000000;
#endif

#if defined (Z_x86) && !defined(Z_sim) 
const unsigned int shared_lib_addr_prefix = 0xB0000000;
const unsigned int shared_lib_addr_mask   = 0xF0000000;
#endif

#ifdef Z_sim
const unsigned int shared_lib_addr_prefix = 0xB0000000;
const unsigned int shared_lib_addr_mask   = 0xF0000000;
#endif

#ifdef Z_ppc
const unsigned int shared_lib_addr_prefix = 0xF0000000;
const unsigned int shared_lib_addr_mask   = 0xF0000000;
#endif



unsigned long g_main_frame_addr = 0;
char gBacktraceBuffer[ 30000 ];

//static
util::Backtrace::StackLimitEntry util::Backtrace::msStackLimits[];

using namespace std;

namespace component_Util_Backtrace_cpp
{

    //----------------------------------------------------------------------
    //
    // static initializer to open /dev/console on startup
    // to be used in the moment of great need.
    //
    class DevConsole
    {
      public:

        DevConsole() : mFd(-1)
        {
            mFd = open("/dev/console", O_WRONLY);
            if (mFd == -1)
            {
                printf("%s: could not open /dev/console, errorno %d\n",
                        __progname, errno);
            }
        }

        ~DevConsole() { if (-1 != mFd) close(mFd); }

        void Write(const char* buf, size_t len)
        {
            // write to stdout first
            (void) write(1, buf, len);

            // and to console
            if (mFd == -1)
            {
                mFd = open("/dev/console", O_WRONLY);
            }

            if (mFd != -1)
            {
                (void) write(mFd, buf, len);
            }
        }

        bool IsOpen() { return mFd != -1; }

        int GetFd() { return mFd; }

      private:
        int mFd;
    };

    DevConsole gDevConsole;

    //----------------------------------------------------------------------

    // Helper function
    //
    bool StackEnd(unsigned long frameAddr)
    {
        return ((frameAddr == 0ul) ||
                (frameAddr == g_main_frame_addr) ||
                (frameAddr == OsThread::ThreadFrameAddress()));
    }
}
using namespace component_Util_Backtrace_cpp;


/////////////////////////////////////////////////////////////////////////////
std::ostream& operator<< (std::ostream& os, const util::Backtrace& bt)
{
    using namespace util;
    std::ios_base::fmtflags restoreFlags = os.flags();

    os << "Backtrace:" << std::endl;

    size_t numOfFrames = bt.mNumFrames;
    if (numOfFrames > util::Backtrace::MAX_FRAMES)
    {
        numOfFrames = util::Backtrace::MAX_FRAMES;
    }

    for (size_t i=0; i < numOfFrames; ++i)
    {
        Dl_info info;
        const char* sname = "";
        const char* fname = "";
        if ( (((bt.mBacktrace[i] &
            shared_lib_addr_mask) == shared_lib_addr_prefix)) &&
             dladdr( (void*)(bt.mBacktrace[i]), &info))
        {
            sname = info.dli_sname;
            fname = info.dli_fname;
            if (!fname)
            {
               fname = "";
            }
            os << setfill('0') ;
            os << "[" << __progname << ":" << bt.mThreadId << ":"
               << bt.mThreadName << "] 0x" << setw(8)<< std::hex << bt.mBacktrace[i]
               << std::dec
               << ": " << sname
               << "+0x" << bt.mBacktrace[i]-(uint32)info.dli_saddr
               << " (" << fname << ")" << std::endl;
        }
        else
        {
            sname = "";
            fname = "";
            os << setfill('0') ;
            os << "[" << __progname << ":" << bt.mThreadId << ":"
               << bt.mThreadName << "] 0x" << setw(8)<< std::hex << bt.mBacktrace[i]
               << std::dec << std::endl;
        }
    }

    (void) os.flags(restoreFlags);
    return os;
}


/////////////////////////////////////////////////////////////////////////////
void util::Backtrace::Capture()
{
    mNumFrames = 0;
    mThreadName[0] = 0;

    if (OsThread::Current())
    {
        if (OsThread::Current()->GetName())
        {
            const char *myThrName = OsThread::Current()->GetName();
            if (myThrName)
            {
                unsigned origNameLen = strlen( myThrName );
                if ( origNameLen >= sizeof(mThreadName) )
                {
                    memcpy( mThreadName, myThrName, sizeof(mThreadName) -1 );
                    mThreadName[ sizeof(mThreadName) -1 ] = '\0';
                }
                else
                {
                    strncpy( mThreadName, myThrName, sizeof(mThreadName) );
                }
            }
        }
    }

    mThreadId = pthread_self();

#ifdef Z_QNX
#ifdef Z_ppc

    // on PPC just walk the stack, this __builtin stuff doesn't
    // work right
    unsigned long* fp = (unsigned long*)__builtin_frame_address(0);

    while ((mNumFrames < MAX_FRAMES) && (fp) && (*fp))
    {
        mBacktrace[mNumFrames++] = fp[1];
        fp = (unsigned long*)*fp;
    }

#endif // Z_ppc

#ifdef Z_sim

#define GETRA(xx_level) \
    do { \
    unsigned long ra; \
    unsigned long fa = (unsigned long)__builtin_frame_address(xx_level); \
    if (StackEnd(fa)) return; \
    ra = (unsigned long)__builtin_return_address(xx_level); \
    mBacktrace[mNumFrames++] = ra; \
    if (ra == (unsigned long)&__my_thread_exit) return; \
    } while (0)

    // skip this frame?
    GETRA(0);
    GETRA(1);   GETRA(2);   GETRA(3);   GETRA(4);   GETRA(5);
    GETRA(6);   GETRA(7);   GETRA(8);   GETRA(9);   GETRA(10);
    GETRA(11);  GETRA(12);  GETRA(13);  GETRA(14);  GETRA(15);
    GETRA(16);  GETRA(17);  GETRA(18);  GETRA(19);  GETRA(20);
    GETRA(21);  GETRA(22);  GETRA(23);  GETRA(24);  GETRA(25);
    GETRA(26);  GETRA(27);  GETRA(28);  GETRA(29);  GETRA(30);  GETRA(31);

#endif // Z_sim QNX
#endif //Z_QNX

#ifdef Z_LINUX
        mNumFrames = backtrace((void**)mBacktrace, MAX_FRAMES);
#endif
}  /* Capture */


/////////////////////////////////////////////////////////////////////////////
void util::Backtrace::CaptureFromOsSignal( void *pContext )
{
#ifdef Z_ppc
    mThreadId = pthread_self();
    mThreadName[0] = 0;

    if (OsThread::Current())
    {
        const char *myThrName = OsThread::Current()->GetName();
        if (myThrName)
        {
            unsigned origNameLen = strlen( myThrName );
            if ( origNameLen >= sizeof(mThreadName) )
            {
                memcpy( mThreadName, myThrName, sizeof(mThreadName) -1 );
                mThreadName[ sizeof(mThreadName) -1 ] = '\0';
            }
            else
            {
                strncpy( mThreadName, myThrName, sizeof(mThreadName) );
            }
        }
    }

    mNumFrames = 0;

    // Register set for faulting thread is uctx->uc_mcontext.cpu
    // Specific fields are in <cpu>/context.h (ex. ppc/context.h)
#ifndef Z_LINUX
    ucontext_t  *uctx = (ucontext_t *)pContext;
    PPC_CPU_REGISTERS  *regs = &uctx->uc_mcontext.cpu ;
    uint32_t *fp  = (uint32_t *)GET_REGSP(regs);
    uint32_t *lr  = (uint32_t *)(regs)->lr;

    uint32_t *fp_crash  = fp;

    uint32_t *crashIP  = ((uint32_t *)GET_REGIP(regs));

    // First frame of the thread
    uint32_t *stackLimit = OsThread::Current() ?
                        (uint32_t *) OsThread::ThreadFrameAddress() :
                        (uint32_t *) g_main_frame_addr;
    if (stackLimit == 0)
    {
        // This thread does not know where its first frame is, so we
        // assume we know - NNDS threads fall in this category.
        // Assume 64K from where we are.
        stackLimit = (uint32_t *)((unsigned char*)fp + 512 * 1024);
    }

    // Top instruction pointer best captured by the QNX signal handler.
    mBacktrace[ mNumFrames++ ] = (unsigned long)crashIP;
    mBacktrace[ mNumFrames++ ] = (unsigned long)lr;

    if (fp && (fp < stackLimit))
    {
        // Go to second frame
        fp = (uint32_t *)( *fp_crash ) ;
    }

    while ((mNumFrames < util::Backtrace::MAX_FRAMES) && (fp) && (*fp))
    {
        mBacktrace[ mNumFrames++ ] = fp[1];
        uint32_t *nextfp = (uint32_t *)(*fp);

        // Validate the frame pointer
        if ((nextfp <= fp) || (nextfp > stackLimit))
        {
            break; // out of bounds
        }
        fp = nextfp;
    }
#endif // !Z_LINUX

#else
//#pragma message "PPC_CPU_REGISTERS should be mapped to linux specific representation"
#endif // Z_ppc

}  /* CaptureFromOsSignal */


/////////////////////////////////////////////////////////////////////////////
void util::Backtrace::Dump(bool capture)
{
    static char buf[16384];
    buf[0] = 0;

    if (capture)
    {
        Capture();
    }

#if defined(QNX) 
    char *cp = buf;
    char *limit = buf + sizeof(buf) - 2;
    *limit = '\0';

    char *my_prog_name    = __progname;
    char *my_thread_name  = mThreadName;
    int   my_thread_id    = mThreadId;

    for (size_t i=0; i < mNumFrames; ++i)
    {
        int cnt = snprintf( cp, limit - cp,
                           "[%s:%d:%s] 0x%p: \n", my_prog_name, my_thread_id,
                           my_thread_name, (unsigned char*)mBacktrace[i]);
        if (cnt <= 0)
            break;

        cp += cnt;

        if (cp > limit)
            break;
     }

    gDevConsole.Write(buf, strlen(buf));

    buf[0] = 0;
    strncpy(buf,"-----------------------\n",sizeof(buf)-2);

    // second time with symbols for shared lib

    if (mNumFrames > MAX_FRAMES)
    {
        mNumFrames = MAX_FRAMES;
    }

    for (size_t i=0; i < mNumFrames; ++i)
    {
        Dl_info info;
        const char* sname = "";
        const char* fname = "";

        if ( (((mBacktrace[i] & shared_lib_addr_mask) == shared_lib_addr_prefix)) &&
        dladdr( (void*)(mBacktrace[i]), &info))
        {
            sname = info.dli_sname;
            fname = info.dli_fname;
            if (!fname)
            {
                fname = "";
            }

            int catPos = strlen(buf);
            snprintf(buf+catPos, sizeof(buf)-catPos,
                     "[%s:%d:%s] 0x%p: %s+0x%lx (%s)\n",
                     __progname, mThreadId, mThreadName,
                     (unsigned char*)mBacktrace[i], sname,
                     (mBacktrace[i]-(uint32)info.dli_saddr), fname);
        }
        else
        {
            sname = "";
            fname = "";
            int catPos = strlen(buf);
            snprintf(buf+catPos, sizeof(buf)-catPos,
                     "[%s:%d:%s] 0x%p: \n",
                     __progname, mThreadId, mThreadName,
                     (unsigned char*)mBacktrace[i]);
        }
    }

    gDevConsole.Write(buf, strlen(buf));

#else // LINUX_COMPILE
    {
        ostringstream os;
        os <<  "build: " << BaseInfo::GetEngBldId() << endl;
        void *array[MAX_FRAMES];
        size_t size;

        size = backtrace(array, MAX_FRAMES);

        // taken from http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes

        // get void*'s for all entries on the stack
        size = backtrace(array, 15);

        // print out all the frames to stderr
        backtrace_symbols_fd(array, size, STDERR_FILENO);
        for(int i=0; i < size; i++)
        {
            TRC_SMSG(all,1,array[i]);
            Dl_info info;
            if( dladdr(array[i],&info) == 0)
            {
                os <<  __FILE__ << ": " << array[i] << endl;
            }
            else
            {
                char *demangled = NULL;
                int status = -1;
                demangled = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
                if(status == 0)
                {
                    os <<  __FILE__ << ": " << array[i] << ": " << demangled << endl;
                }
                else
                {
                    os <<  __FILE__ << ": " << array[i]
                            << ": " << info.dli_fname << ": " << info.dli_sname << endl;
                }
                free(demangled);
            }
        }
        /* not using syslog console since it does not print the thread name */
        TRC_SMSG(all,1,os.str());
        cout << os.str() << endl;
    }
#endif

}  // Backtrace::Dump



/////////////////////////////////////////////////////////////////////////////
// This function captures IP, SP and LR registers from a given thread.
//
// Returns 0 if registers were captured.
// Returns 1 if the thread is found dead, but its resources
//           have not been released.
// Returns 2 if the thread id is invalid or has status retrieval issues.
//
int GetThreadRegisters( int fd, pthread_t tid,
                        uintptr_t *ip, uintptr_t *sp, uintptr_t *lr )
{
#if defined(QNX)
#if defined Z_ppc
    procfs_greg regs;
    int err;

    err = devctl( fd, DCMD_PROC_CURTHREAD, &tid, sizeof tid, 0 );
    if ( err != EOK )
    {
        return 2;
    }

    procfs_status  status;
    status.tid = tid;

    err = devctl(fd, DCMD_PROC_TIDSTATUS, &status, sizeof status, 0);
    if (err != EOK)
    {
        return 2;
    }

    if (STATE_DEAD == status.state)
    {
        return 1;
    }

    err = devctl( fd, DCMD_PROC_GETGREG, &regs, sizeof regs, 0 );
    if ( err != EOK )
    {
        return 1;
    }

    *ip = PPC_GET_REGIP(&regs.ppc);
    *sp = PPC_GET_REGSP(&regs.ppc);
    *lr = regs.ppc.lr;
#endif
    return 0;

#else // Z_ppc && !Z_LINUX

#ifdef Z_LINUX
#pragma message "Find Linux equivalent of setting and getting proc registers"
#endif
    return 2;

#endif // Z_ppc && !Z_LINUX

}  /* GetThreadRegisters */


/////////////////////////////////////////////////////////////////////////////
void UnwindThreadStack( OneTraceback & rOneTb,
                        uint32_t * stack_ptr , int ip , int lr )
{
#ifdef Z_ppc
    uint32_t *prev_frame_ptr = stack_ptr;

    // First frame of the thread
    uint32_t *stackLimit = 0;
    int tid = rOneTb.GetId();

    if ( 1 == tid )      // Thread 1 ... we know
    {
        stackLimit = (uint32_t *) g_main_frame_addr;
    }
    else
    {
        stackLimit = (uint32_t *)util::Backtrace::GetStackLimitForThread( tid );
    }

    if (stackLimit == 0)
    {
        // This thread does not know where its first frame is, but
        // we need a worst case estimate. NNDS threads fall in
        // the category of threads with big stacks.
        stackLimit = (uint32_t *)
                     ((unsigned char*)stack_ptr + 512 * 1024);
    }

    // Push first the current instruction pointer
    rOneTb.AddWord( ip );

    // Push the LR and ignore the * (stack_ptr + 1) value
    // in the top frame
    rOneTb.AddWord( lr );

    uint32_t * frame_ptr = stack_ptr;

    while( frame_ptr && rOneTb.IsSpaceAvail() )
    {
        frame_ptr = (uint32_t *) *frame_ptr;

        if ( 0 == frame_ptr )
        {
            // Appears to be a healthy bottom of stack.
            break;
        }

        if ( ( frame_ptr < prev_frame_ptr )
                ||
             ( stackLimit && ( frame_ptr > stackLimit ) )
           )
        {
            // Current frame ptr is too risky to be dereferenced.
            break;
        }

        rOneTb.AddWord( *(frame_ptr + 1) );
    }
#endif  // Z_ppc

}  /* UnwindThreadStack */


/////////////////////////////////////////////////////////////////////////////
//static
char * util::Backtrace::DumpTracebacksOfAllThreads( void * context ,
                                                    char * buffer ,
                                                    size_t buffLen ,
                                                    bool   toAuditLog ,
                                                    bool   toConsole )
{
    int errorTids = 0;
    pthread_t tid = 1;

    // Do this thread first
    util::Backtrace myBt;
    char tmpBuffer[120];

    if ( context ) {
        myBt.CaptureFromOsSignal( context );
    }
    else {
        myBt.Capture();
    }

    if (NULL == buffer)
    {
        buffer = gBacktraceBuffer;
        buffLen = sizeof(gBacktraceBuffer);
    }

    TracebackStack tbStack( buffer, buffLen );

    pthread_t myTid = pthread_self();

    int fd = open( "/proc/self/as", O_RDONLY );
    if ( fd == -1 ) {
        TRC_SMSG(audit, 1, "self sys info access failed. errno=" << errno );
        return NULL;
    }

    int holdRslt = ThreadCtl(_NTO_TCTL_THREADS_HOLD, 0);
    if ( -1 == holdRslt )
    {
        ZAUDIT_F( ("_NTO_TCTL_THREADS_HOLD failed. errno=%d", errno) );
    }

    for ( ; errorTids < 6 ; ++tid )
    {
        if (tid == myTid)
        {
            continue;
        }
        uintptr_t ip, sp, lr;

        int threadStatus = GetThreadRegisters( fd, tid, &ip, &sp, &lr );

        if ( threadStatus > 0 )
        {
            if ( threadStatus > 1 )
            {
                ++errorTids;
            }
            continue;
        }
        errorTids = 0;

        OneTraceback oneTb;
        oneTb.SetId( tid );

        UnwindThreadStack( oneTb, (uint32_t *)sp , (int)ip, (int)lr );
        tbStack.InsertTb( oneTb );
    }

    // ASCII Output is now buffered into the same buffer,
    // in the area that was not used by the tbStack object.
    FixedTextBuffer  outTextBuff( buffer + tbStack.BufferUsage(),
                                  buffLen - tbStack.BufferUsage() );

    outTextBuff.Write( "\n" );
    outTextBuff.Write( __progname );
    outTextBuff.Write( "\n<MultiThreadTraceback> {" );
    outTextBuff.Write( __progname );
    outTextBuff.Write( ": Traceback Stack reported from thread #" );

    // This thread's traceback is printed first
    snprintf( tmpBuffer, sizeof(tmpBuffer),
              "%d\n"
              "[%d] ", (int) myTid, (int) myTid );
    outTextBuff.Write( tmpBuffer );

    if (myBt.mNumFrames > MAX_FRAMES)
    {
        myBt.mNumFrames = MAX_FRAMES;
    }

    for ( size_t i = 0 ; i < myBt.mNumFrames ;  )
    {
        snprintf( tmpBuffer, sizeof(tmpBuffer), "%lx", myBt.mBacktrace[i] );
        outTextBuff.Write( tmpBuffer );
        ++i;
        if (i < myBt.mNumFrames)
        {
            outTextBuff.Write( "," );
        }
    }
    outTextBuff.Write( "\n" );

    // Print of all other threads
    tbStack.Print( &outTextBuff );

    tbStack.PrintDllInfo( &outTextBuff );

    outTextBuff.Write( "</MultiThreadTraceback> of " );
    outTextBuff.Write( __progname );

    int releaseRslt = ThreadCtl(_NTO_TCTL_THREADS_CONT, 0);

    if ( -1 == releaseRslt )
    {
        ZAUDIT_F( ("_NTO_TCTL_THREADS_CONT failed. errno=%d", errno) );
    }

    if (toAuditLog)
    {
        ZAUDIT_TXT( outTextBuff.GetPtrToData() );
    }

    if (toConsole)
    {
        util::ConsoleTxtOnly( outTextBuff.GetPtrToData() );
    }

    close(fd);

    return outTextBuff.GetPtrToData();
}  /* DumpTracebacksOfAllThreads */


/////////////////////////////////////////////////////////////////////////////
//static
void util::Backtrace::RegisterStackLimit( int threadId, void *stackLimit )
{
    size_t  maxIndex = sizeof(msStackLimits) / sizeof(msStackLimits[0]);
    StackLimitEntry * pGoodSpace = NULL;

    for ( size_t k = 0 ; k < maxIndex ; ++k )
    {
        StackLimitEntry * pEntry = & msStackLimits[k];

        if ( threadId == (int)( pEntry->tidSlot & ~cInUseBit ) )
        {
            // Slot found already associated to same tid
            // This could only happen if an OsThread dies without
            // deregistering their previous stack base pointer.
            pEntry->pStackLimit = stackLimit;

            if (pGoodSpace)
            {
                // Release the entry we had reserved
                atomic_clr( & pGoodSpace->tidSlot, cInUseBit );
            }
            return;
        }

        if ( (0 == (pEntry->tidSlot & cInUseBit)) && (NULL == pGoodSpace) )
        {
            // Try to grab first slot here
            unsigned prevValue = atomic_set_value( & pEntry->tidSlot,
                                                   cInUseBit );

            if ( cInUseBit & prevValue )
            {
                // We did not grab it; another thread did it.
                continue;
            }
            pGoodSpace = pEntry;
        }
    }

    if (NULL == pGoodSpace)
    {
        return;
    }

    pGoodSpace->pStackLimit = stackLimit;
    pGoodSpace->tidSlot = cInUseBit | threadId;

}  /* RegisterStackLimit */


/////////////////////////////////////////////////////////////////////////////
//static
void util::Backtrace::DeregisterStackLimit( int threadId )
{
    size_t  maxIndex = sizeof(msStackLimits) / sizeof(msStackLimits[0]);

    for (size_t k = 0 ; k < maxIndex ; ++k )
    {
        StackLimitEntry * pEntry = & msStackLimits[k];

        if ( threadId == (int)(pEntry->tidSlot & ~cInUseBit ) )
        {
            pEntry->pStackLimit = 0;
            pEntry->tidSlot     = 0;
            return;
        }
    }

}  /* DeregisterStackLimit */


/////////////////////////////////////////////////////////////////////////////
//static
void * util::Backtrace::GetStackLimitForThread( int threadId )
{
    size_t  maxIndex = sizeof(msStackLimits) / sizeof(msStackLimits[0]);

    for (size_t k = 0 ; k < maxIndex ; ++k )
    {
        StackLimitEntry * pEntry = & msStackLimits[k];

        if (0 == threadId)
        {
            if ( pEntry->tidSlot || pEntry->pStackLimit )
            {
                // This output is meant for debugging, given that zero
                // is an invalid thread id.
                std::cout << "(" << k
                      << ") TidSlot [" << (void*)(pEntry->tidSlot)
                      << "] InUse: " << (int)((pEntry->tidSlot >> 31) & 0x01)
                      << " limit: " << (void*)pEntry->pStackLimit << std::endl;
            }
            continue;
        }

        if ( threadId == (int)( pEntry->tidSlot & ~cInUseBit ) )
        {
            return pEntry->pStackLimit;
        }
    }
    return NULL;

}  /* GetStackLimitForThread */

void util::Backtrace::DumpSymbols()
{
#ifdef Z_LINUX
    void *array[100];
    char **strings;
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 100);

    strings = backtrace_symbols(array, size);
    if (strings == NULL) {
        return;
    }

    for (int i = 0; i < size; i++)
    {
        TRC_SMSG(all,1,strings[i]);
    }
    TRC_SMSG(all,1,"---------------------------------------------------------------------------");
    free(strings);
#endif
}


