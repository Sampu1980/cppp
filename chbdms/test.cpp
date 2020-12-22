//
//  src_cc/Trc/TrcSlogRelay.cpp
//
// Copyright (c) 2013-2015 Infinera Corporation
// All rights reserved.
//

#include <Trc/TrcSlogRelay.h>
#include <Trc/ZTrcTls.h>
#include <Trc/trcIfPrivate.h>

#include <Util/ZFixedOstream.h>
#include <stdio.h>



TrcSlogRelay::TrcSlogRelay() : OsThread("TrcSlogRelay")
         , mIsRelayActive(false)
         , mQueueFullCount(0)
         , mDropCount(0)
{
}

void TrcSlogRelay::Activate ()
{
    OsMutexGuard g(mLock);
    mIsRelayActive = true;
    //PrintSlogMsg(ZTrcTls::GetMyTLS(), "########## TRCLOGGER ACTIVATED ##########");
}

void TrcSlogRelay::Halt()
{
    OsMutexGuard g(mLock);
    mIsRelayActive = false;
    //PrintSlogMsg(ZTrcTls::GetMyTLS(), "########## TRCLOGGER HALTED ##########");

    // Wake up relay thread to do the jobs in the queue
    mQueSem.Give();
}

void TrcSlogRelay::Dump( std::ostream &os )
{
    OsMutexGuard g(mLock);
    os << "Trace Logger :: \n\tSEPARATE THREAD=[" << (int) mIsRelayActive << "]"
          "\n\tInQueue Size=" << mQueInUse.size()
       << "\n\tFreeQueue Size=" << mFreeQue.size()
       << "\n\tDropCount=" << mDropCount
       << "\n\tQueueFull=" << mQueueFullCount << std::endl;
}

// Queues up request for later consumption of relay thread
void TrcSlogRelay::Submit( ZTrcTls * myTLS, const char * pMsg )
{
    OsMutexGuard g(mLock);

    // Limit Queue size.
    // If the free queue size croses the MAX size, then delete the extra buffers
    // and bring back the queue to regular size.
    if (mFreeQue.size() > TRCMSG_FREE_QUEUE_HIGH_WATERMARK)
    {
        Discard( mFreeQue, mFreeQue.size() - TRCMSG_FREE_QUEUE_LOW_WATERMRK );
    }

    // If the in-use-size reaches MAX limit, drop the request
    if (mQueInUse.size() >= TRCMSG_INUSE_QUEUE_MAX_SIZE)
    {
        mDropCount++;
        return;
    }

    // Check if requests were dropped previously.
    if (mDropCount > 0)
    {
        // Increment the number of times queue has filled up
        mQueueFullCount++;

        char fullMsg[ 120 ];
        ZFixedOstream  xStrm( fullMsg, sizeof(fullMsg) );

        // Insert log entry.
        xStrm << "##########  QUEUE-FULL [" << mQueueFullCount << "], LOGS DROPPED ["
              << mDropCount << " ] ###########";

        // Submit queue full warning first.
        SubmitLogRequest( myTLS, xStrm.cstr() );

        mDropCount = 0;
    }

    // Enqueue actual request
    SubmitLogRequest( myTLS, pMsg );
}

void TrcSlogRelay::Run()
{
    while (1)
    {
        mQueSem.Take();

        LoggerQueue tmpQ;

        ZTRY
        {
            // Lock only while copying the queue.
            {
                OsMutexGuard g(mLock);
                std::swap( mQueInUse, tmpQ );
            }

            while (!tmpQ.empty())
            {
                // Process each log request.
                LogMsg *msg = tmpQ.front();

                LogMessage(msg);

                // Remove the element from tmpQ
                tmpQ.pop();
            }
        }
        ZCATCH (ZException &ex)
        {
            // Enqueue exception message in the slog data
            char buff[ 200 ];
            ZFixedOstream  xStrm( buff, sizeof(buff) );

            xStrm << "##########  TrcSlogRelay::Run EXCEPTION " << ex.What()
                  << " LOGS DROPPED [" << mQueInUse.size() << " ] ###########";

            ZTrc::PrintSlogMsg( ZTrcTls::GetMyTLS(), xStrm.cstr() );

            // If any exception happens, clear up the in-use queue.
            Discard( mQueInUse, mQueInUse.size() );
            Discard( tmpQ, tmpQ.size() );
        }
    }
}


void TrcSlogRelay::Discard( LoggerQueue & rQueue, uint32 size )
{
    for (uint32 i = 0; i < size; i++)
    {
        LogMsg *tmp = rQueue.front();
        rQueue.pop();
        delete tmp;
    }
}


void TrcSlogRelay::SubmitLogRequest( ZTrcTls * myTLS, const char * pMsg )
{
    OsMutexGuard g(mLock);

    uint32 size = strlen( pMsg ) + 1;
    uint32 copied = 0;
    do
    {
        LogMsg *msg = NULL;
        if (!mFreeQue.empty())
        {
            msg = mFreeQue.front();
            mFreeQue.pop();
        }

        if (msg == NULL)
        {
            msg = new LogMsg();
        }

        uint32 length = size - copied;
        if (length >= sizeof(msg->mMsg))
        {
            length = sizeof(msg->mMsg) - 1;
        }

        // Capture data being deferred
        msg->Load( myTLS, pMsg + copied, length );

        copied += length;

        try
        {
            mQueInUse.push( msg );
        }
        catch ( std::exception &e )
        {
            // If the message didn't get queued then free
            // it and continue on.
            delete msg;
        }

    }
    while( copied < size );

    mQueSem.Give();
}


void TrcSlogRelay::LogMessage( LogMsg *log )
{
    // Copy the necessary stuff to relay thread's ZTrcTls.
    ZTrcTls *pMyTLS = ZTrcTls::GetMyTLS();

    memcpy(pMyTLS->mThreadName, log->mThreadName, sizeof(pMyTLS->mThreadName));

    pMyTLS->mSeverityChar = log->mSeverityChar;
    pMyTLS->mSeverityInt =  log->mSeverityInt;

    // ZNamePlusBasicMsg info
    memcpy(        pMyTLS->mHdrLine.mRestOfChars, log->mHdrLine.mRestOfChars,
            sizeof(pMyTLS->mHdrLine.mRestOfChars) );
    memcpy(      & pMyTLS->mHdrLine.mBuffHdr,   & log->mHdrLine.mBuffHdr,
            sizeof(pMyTLS->mHdrLine.mBuffHdr) );

    ZTrc::PrintSlogMsg( pMyTLS, log->mMsg );

    OsMutexGuard g( mLock );
    mFreeQue.push( log );
}

void TrcSlogRelay::FlushSlogRelayData(bool stop)
{
    OsMutexGuard g(mLock);

    if (stop) 
    {
        mIsRelayActive = false;
    }

    while (!mQueInUse.empty())
    {
        // Process each log request.
        LogMsg *msg = mQueInUse.front();

        LogMessage(msg);

        mQueInUse.pop();
    }
}


TrcSlogRelay::LogMsg::LogMsg()
: mSeverityChar('0')
, mSeverityInt(0)
{
    mMsg[0] = '\0';
    mThreadName[0] = '\0';
	memset(&mHdrLine.mBuffHdr, 0, sizeof(mHdrLine.mBuffHdr));
	memset(mHdrLine.mRestOfChars, 0, sizeof(mHdrLine.mRestOfChars));
}

void TrcSlogRelay::LogMsg::Load( ZTrcTls *myTls, const char *pMsg, uint32 length )
{
    // Extract info from myTls.
    memcpy( mThreadName, myTls->mThreadName, sizeof(mThreadName) );

    mSeverityChar = myTls->mSeverityChar;
    mSeverityInt =  myTls->mSeverityInt;

    memcpy(        mHdrLine.mRestOfChars, myTls->mHdrLine.mRestOfChars,
            sizeof(mHdrLine.mRestOfChars) );
    memcpy(      & mHdrLine.mBuffHdr,   & myTls->mHdrLine.mBuffHdr,
            sizeof(mHdrLine.mBuffHdr) );

    // data
    if (length >= sizeof(mMsg))
    {
        length = sizeof(mMsg) - 1;
    }
    memcpy( mMsg, pMsg, length );
    mMsg[ length ] = '\0';
}


////////////////////////////////////////////////////////////////////////////////////

// Guard protection that activates and de-activates the relay thread.

TrcSlogRelayGuard::TrcSlogRelayGuard() 
{
    // This will fault assert if the service is not active
    TrcSlogRelayService::Instance()->Activate();
}

TrcSlogRelayGuard::~TrcSlogRelayGuard()
{
    // This will fault assert if the service is not active
    TrcSlogRelayService::Instance()->Halt();
}


