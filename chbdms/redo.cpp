//=========================================================================
//
// src_ne/component/Util/ZExcHdlrTrace.cpp
//
//    Copyright (c) 2005-2015 Infinera Corporation
//        All rights reserved.
//
//=========================================================================

#include <Util/ZExcHdlrTrace.h>
#include <Util/ParanoidPtr.h>
#include <Util/ZFixedOstream.h>
#include <Util/SysLog.h>


#include <sys/mman.h>
#include <sys/neutrino.h>

#ifndef Z_LINUX
#include <iomanip.h>
#else
#include <iomanip>
using namespace std;
#endif
#include <atomic.h>                 // atomic_add_value



using namespace std;

// Magic values
static const int ZEXC_HDLR_NOT_INITIALIZED      =  0x34433443;
static const int ZEXC_FORCE_SRAM_INITIALIZATION =  0x66600666;

static unsigned int  gZExcHdlrInitLvl = 0;
static int   gZExcHdlrInitialized  = ZEXC_HDLR_NOT_INITIALIZED;
static int   gZExcForceSramInit    = 0;
static uint32 gResetReason;
static uint32 gResetStatus;


// Compute characteristics of the area in SRAM that ZExcHdlrTrace.cpp
// can write into.

//*******************************************************************
namespace ZExcHdlrTraceN
{
    static const unsigned CURRENT_ZEXC_TRACE_VERSION = 1;

    /*
     *  This EV_LOG_SIZE has to be a POWER-OF-2 NUMBER.
     *  See how the index is always incremented and the
     *  lower significant bits are used.
     */
    namespace _SramLayout_Ver1
    {
        // VERY important to have a power-of-2 value of the index,
        // because there is a contiguous increment of a counter
        // and we use the lower bits.
        //
        static const unsigned EV_LOG_SIZE          = 0x40;

        static const unsigned EV_LOG_SIZE_LSB_MASK = EV_LOG_SIZE - 1;

        struct ExcLoggedEvent         // (size = 3 * 4 = 12 bytes)
        {
            ZExcHdlrTrace::EventType   mEvType;
            int                        mIntData1;
            time_t                     mTimeStamp;
        };


        // VERY important to have a power-of-2 value of the index,
        // because there is a contiguous increment of a counter
        // and we use the lower bits.
        //
        static const unsigned CPLD_REASONS_LOG_SIZE   = 8;

        /**
         *  Overlaid structure onto CPCSRAM ExcHdlrTrace[] as defined in :
         *      sramPublic.h
         */
        struct SramExcLogArea_Ver1         // size = 4 + 3*4*64 = 772
        {
            unsigned  mEventIndex;

            ExcLoggedEvent  mEventVector[ EV_LOG_SIZE ];
        } ;

        struct CpldReasonsArea    // size = 4 +  1*8 + 4*8 = 44
        {

            unsigned  mCpldReasonIndex;

            unsigned char mCpldReasonWords  [ CPLD_REASONS_LOG_SIZE ] ;

            time_t  mCpldReasonTimeStamps   [ CPLD_REASONS_LOG_SIZE ] ;

            char BootromCorruption2[0x148];
        } ;


        struct SkReasonsArea      // size = 4 +  4*8 + 4*8 = 68
        {

            unsigned  mSkReasonIndex;

            uint32 mSkReasonWords  [ CPLD_REASONS_LOG_SIZE ] ;

            time_t  mSkReasonTimeStamps   [ CPLD_REASONS_LOG_SIZE ] ;

            char xtnReserved[0x138];
        } ;

        union SramReasonsArea_Ver1
        {
            CpldReasonsArea   CpldInfo;
            SkReasonsArea     SkInfo;
        };

        struct WdLogOptionsArea {
            char options[1];
        };

    } ;

    // WE ARE USING VER1
    //
    typedef _SramLayout_Ver1::ExcLoggedEvent  ExcLoggedEvent ;

    typedef _SramLayout_Ver1::SramExcLogArea_Ver1  SramExcLogArea ;

    typedef _SramLayout_Ver1::SramReasonsArea_Ver1  SramReasonsArea ;
    typedef _SramLayout_Ver1::CpldReasonsArea       CpldReasonsArea ;
    typedef _SramLayout_Ver1::SkReasonsArea         SkReasonsArea ;
    typedef _SramLayout_Ver1::WdLogOptionsArea      WdLogOptsArea;

    // Pointers to SRAM are broken into TWO integers to avoid
    // a stray pointer READ operation pick up the value of
    // the pointer of SRAM and then use it.
    //
    ParanoidPtr<SramExcLogArea>    gptr_ZExcArea  ;
    ParanoidPtr<SramReasonsArea>   gptr_ZCpldArea ;
    ParanoidPtr<WdLogOptsArea>     gptr_WdLogOptsArea ;

}  // namespace ZExcHdlrTraceN

using namespace ZExcHdlrTraceN;

//*******************************************************************
void ZExcCannotInitialize()
{
    abort();
}

//*******************************************************************
//static
void ZExcHdlrTrace::ResetClean()
{
    Initialize();

    memset( & *gptr_ZExcArea ,  0, sizeof(*gptr_ZExcArea)  );

    memset( & *gptr_ZCpldArea,  0, sizeof(*gptr_ZCpldArea) );

    memset( & *gptr_WdLogOptsArea,  0, sizeof(*gptr_WdLogOptsArea) );

    ZExcHdlrTrace::RecordEvent( ZExcHdlrTrace::ZEXC_TRACE_WIPED, 0 );
}

//*******************************************************************
//  This method only establishes that the running process will have
//  pre-mapped a virtual pointer to the SRAM area.
//*******************************************************************
//static
void ZExcHdlrTrace::Initialize()
{
    if ( ~ ZEXC_HDLR_NOT_INITIALIZED == gZExcHdlrInitialized)
    {
        return ;
    }

    int prevLvl = atomic_add_value( &gZExcHdlrInitLvl , 1 );

    if (prevLvl != 0)
    {
        ZExcCannotInitialize();
    }

    gptr_ZExcArea  =  reinterpret_cast<SramExcLogArea*>
                      (new char [ sizeof(_SramLayout_Ver1::SramExcLogArea_Ver1) ]);

    gptr_ZCpldArea =  reinterpret_cast<SramReasonsArea*>
                      (new char [ sizeof(_SramLayout_Ver1::SramReasonsArea_Ver1) ]);

    gptr_WdLogOptsArea = reinterpret_cast<WdLogOptsArea*>
                      (new char [ sizeof(_SramLayout_Ver1::SramReasonsArea_Ver1) ]);

    gZExcHdlrInitialized =    ~ ZEXC_HDLR_NOT_INITIALIZED ;

    // Just for debugging.  We can set the global variable
    // to cause the reset of the contents of SRAM.  We should
    // not commit to Perforce changes to variable gZExcForceSramInit.
    if (ZEXC_FORCE_SRAM_INITIALIZATION == gZExcForceSramInit)
    {
        ResetClean();
    }
}

//*******************************************************************
//static
void ZExcHdlrTrace::RecordEvent( EventType ev , int intData1 )
{
    if (ZEXC_HDLR_NOT_INITIALIZED == gZExcHdlrInitialized)
    {
        Initialize();
    }
    SramExcLogArea *ptr_area =    & *gptr_ZExcArea ;

    // We use the index value before the increment takes place.
    int index = atomic_add_value( & ptr_area->mEventIndex, 1 );

    // Our index is pretty smart.  It only consists of some lower
    // significant bits of the <mEvenIndex> value.
    //
    index  &=  _SramLayout_Ver1::EV_LOG_SIZE_LSB_MASK ;

    ExcLoggedEvent *ptr_evt = & ptr_area->mEventVector[ index ] ;

    ptr_evt->mEvType    = ev;
    ptr_evt->mIntData1  = intData1;

    time( & ptr_evt->mTimeStamp );
}

//*******************************************************************
// The use of an atomic operation in this method is purely formal;
// only once in a boot-up session this function is expected to be called.

//static
void ZExcHdlrTrace::RecordBootReason( uint32 regValue, uint32 regStatus )
{
    return;
}

//*******************************************************************
//static
void ZExcHdlrTrace::RecordBootReasonTimestamp()
{
    return;
}


//*******************************************************************
//static
uint32 ZExcHdlrTrace::GetResetReason()
{
    return 0;
}

//*******************************************************************
//static
void ZExcHdlrTrace::AuditReminderOfResetReason()
{
    char auditMsg[ 90 ];
    ZFixedOstream  myStrm( auditMsg, sizeof(auditMsg) );

    myStrm << "Build id: UT Stubs"
           << ", CPLD status: 0x" << (void*)(int)gResetStatus
           << ", reset cause: 0x" << (void*)(int)gResetReason
           << " ";
    ZExcHdlrTrace::DumpResetCauseBitsInText( myStrm, gResetReason );
    SYSLOG( util::SEVERITY_AUDIT, myStrm.cstr() );
}


//*******************************************************************
//static
char const *ZExcHdlrTrace::EvName( EventType ev )
{
    #define  CASE_EV_NAME(XName)  case XName: name = #XName ; break

    char *name  = "BAD_CASE_EVENT";

    switch (ev)
    {
        CASE_EV_NAME(NO_EVENT);
        CASE_EV_NAME(BAD_EVENT_NUMBER);
        CASE_EV_NAME(ZEXC_TRACE_WIPED);

        CASE_EV_NAME(ZASSERT_REASON_SET);
        CASE_EV_NAME(ZASSERT_MSG_TO_SLOG);
        CASE_EV_NAME(ZASSERT_TO_ABORT);
        CASE_EV_NAME(ZWATCHED_ALL_PROCESSES_NON_CRITICAL);
        CASE_EV_NAME(ZWATCHED_ALL_PROCESSES_RESTORED);

        CASE_EV_NAME(WDOG_SIGPWR_SHUTDOWN_HANDLED);
        CASE_EV_NAME(WDOG_SIG_X_HANDLED);
        CASE_EV_NAME(WDOG_TO_EXIT);
        CASE_EV_NAME(WDOG_FDR_STORE_START);
        CASE_EV_NAME(WDOG_FDR_STORE_DONE);
        CASE_EV_NAME(WDOG_FDR_MISSED_AT_LINE_NUM);
        CASE_EV_NAME(WDOG_DEBUG_JUMPER_PRESENT);
        CASE_EV_NAME(WDOG_FIFO_TO_NANNY_ERROR);
        CASE_EV_NAME(WDOG_NANNY_SOCKET_ERROR);
        CASE_EV_NAME(WDOG_INTR_HDL_LINE_NUM);
        CASE_EV_NAME(WDOG_INTERRUPT_RAW);
        CASE_EV_NAME(WDOG_UECC_INTR);
        CASE_EV_NAME(WDOG_MAIN_LOOP_FALL_THRU);
        CASE_EV_NAME(WDOG_FAILURE_HANDLER);
        CASE_EV_NAME(WDOG_DBG_SW_JUMPER_EXIT);
        CASE_EV_NAME(WDOG_TEMP_INTERRUPT);
        CASE_EV_NAME(WDOG_UNSTABLE_HW_STROBE);
        CASE_EV_NAME(WDOG_INVALID_EXIT);
        CASE_EV_NAME(WDOG_SIGTERM_HANDLED);
        CASE_EV_NAME(WDOG_TOP_CPU_USRS_CAPTURE);
        CASE_EV_NAME(WDOG_READ_ERROR);
        CASE_EV_NAME(WDOG_FORCED_FALL_THRU);
        CASE_EV_NAME(WDOG_CPLD_STATUS_VIA_INTR);
        CASE_EV_NAME(WDOG_STROBE_STOPPED);
        CASE_EV_NAME(WDOG_PROC_SCAN_ISSUE);
        CASE_EV_NAME(WDOG_CPU_USAGE_ISSUE);
        CASE_EV_NAME(WDOG_CORRUPT_VAR_DETECED);
        CASE_EV_NAME(WDOG_USB_VBUS_OC_INTR);
        CASE_EV_NAME(WDOG_ECC_SBERR_COUNT);
        CASE_EV_NAME(WDOG_PARITY_INTR);
        CASE_EV_NAME(WDOG_DBG_SLOG_FLUSH);
        CASE_EV_NAME(WDOG_SIGNAL_IGNORED);

        CASE_EV_NAME(FDR_TRACE_WRITE);
        CASE_EV_NAME(FDR_TRACE_WR_ERROR);
        CASE_EV_NAME(FDRRECORDER_INITIALIZED_OK);
        CASE_EV_NAME(FDRRECORDER_THROW_LINENO);
        CASE_EV_NAME(FDRRECORDER_ZERROR_LINENO);
        CASE_EV_NAME(FDRRECORDER_WR_START);
        CASE_EV_NAME(FDRRECORDER_WR_STEP_LINENO);

        CASE_EV_NAME(OSPROCESS_BAD_BT);
        CASE_EV_NAME(BOOTROM_BOOTSTATUS_ISSUE);

        CASE_EV_NAME(FDRCPC_ZERROR_LINENO);
        CASE_EV_NAME(FDRCPC_INITIALIZED_OK);
        CASE_EV_NAME(FDRHDL_ERROR_LINENO);

        default:  name = "UNKNOWN OR DEPRECATED EVENT" ;  break;
    }
    return name;

    #undef   CASE_EV_NAME
}

//*******************************************************************
void dump_ascii_buffer(std::ostream& os, char *buffer, int buffer_size)
{
    char *cursor             =  buffer;
    char *boundary_of_buffer =  buffer + buffer_size;

    while (cursor < boundary_of_buffer)
    {
        char ch = *cursor++;

        if (0 == ch)
        {
            ch  = '*';
        }
        else if (0 == isprint(ch) && (ch != '\n') && (ch != '\r'))
        {
            ch = '.';
        }
        os << ch;
    }
}

//*******************************************************************
void capture_ascii_buffer(char *dst, char *buffer, int buffer_size)
{
    char *cursor             =  buffer;
    char *boundary_of_buffer =  buffer + buffer_size;

    while (cursor < boundary_of_buffer)
    {
        char ch = *cursor++;

        if (0 == ch)
        {
            ch  = '*';
        }
        else if (0 == isprint(ch) && (ch != '\n') && (ch != '\r'))
        {
            ch = '.';
        }
        *dst++ = ch;
    }
}

//*******************************************************************
//static
std::ostream& ZExcHdlrTrace::DumpResetCauseBitsInText( std::ostream& os ,
                                                       uint32 reason )
{
    //  This algorithm traverses the bits in the CPU_RST_CAUSE register
    //  and if any bits are set, then more than one message will be output.

    os << "UT Stubs: unknown reset bit";
    return os ;

}  // DumpResetCauseBitsInText


//*******************************************************************
//  Dumps contents of the SRAM area from oldest to newest record.
//*******************************************************************
//static
void ZExcHdlrTrace::Dump( std::ostream& os, int detailLevel )
{
    Initialize();

    if (ZEXC_HDLR_NOT_INITIALIZED == gZExcHdlrInitialized)
    {
        os << "ZExcHdlrTrace: COULD NOT BE INITIALIZED!!\n" ;
        return;
    }

    // std::ios_base::fmtflags oldFlags = os.flags();
    // char oldFillChar = os.fill();

    // End of dump of Exc Events
    SramExcLogArea *ptr_exc_area =    & *gptr_ZExcArea ;

    const unsigned max_index = _SramLayout_Ver1::EV_LOG_SIZE ;

    int indx      = ptr_exc_area->mEventIndex;    // Index into oldest slot

    unsigned k;                                   // Tells end of circular list

    os << resetiosflags(ios_base::dec) ;

    for ( k = 0 ; k < max_index ; ++k )
    {
        int useful_index =  ( indx++ ) &
                            _SramLayout_Ver1::EV_LOG_SIZE_LSB_MASK ;

        ExcLoggedEvent *ptr_evt = & ptr_exc_area->mEventVector[ useful_index ] ;

        EventType   evt_id    = ptr_evt->mEvType ;
        int         evt_datum = ptr_evt->mIntData1 ;
        time_t      evt_stamp = ptr_evt->mTimeStamp ;

        if ((evt_id == 0) && (evt_datum == 0) && (evt_stamp == 0))
        {
            continue ;     // No useful info
        }

        // Compute local time out of the saved time stamp.
        //
        struct tm   struct_tm_value;

        localtime_r( &evt_stamp , &struct_tm_value );

        char date_strg[100];

        (void)strftime(date_strg, sizeof(date_strg),
                       "%Y/%m/%d %H:%M:%S ", &struct_tm_value);

        os << date_strg
           << "E_"  << std::dec << std::setw(3) << std::setfill('0')
                    << std::right << static_cast<unsigned>( evt_id )
                    << std::setfill(' ') << " " << std::setw(40)
                    << std::left << EvName(evt_id)
           << " 0x" << std::hex << evt_datum
           << std::dec << std::endl;
    }
    // End of dump of Exc Events
    os << std::endl;

}  // Dump


//static
bool ZExcHdlrTrace::IsCpcFdrThrottleEnabled(){
    Initialize();

    return (false); 
}

//static
void ZExcHdlrTrace::SetCpcFdrThrottleEnabled()
{
    Initialize();

    // clear that bit alone. Throttling is ON by default.
}

//static
void ZExcHdlrTrace::ClearCpcFdrThrottleEnabled(){
    Initialize();

    // set that bit alone.
}

