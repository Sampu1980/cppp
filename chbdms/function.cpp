//
//
// ConnMngrPublisher.cpp
//
//
// $Id$ 
// $Change$ 
// $DateTime$ 
//
// $Author$ 
//
// Copyright(c) Infinera 2002

#include <ConnMngr/ConnMngrPublisher.h>
#include <Ics/IcsLocalDomainService.h>
#include <Ics/NvBufMsg.h>
#include <SysCommon/HwCfg.h>
#include <OsEncap/OsMutexGuard.h>
#include <iostream>
#include <memory>
#include <Ics/IcsDomain.h>
#include <SysCommon/HwCfg.h>
#include <Trc/trcIf.h>

using namespace std;

bool ConnMngrEventListener::OnBeforeIssueSent(const char* pNs, Msg* pMsg)
{
	TRC_SMSG(all, 4, __FUNCTION__ << " NameSpace: " << pNs << endl << "Msg: " << pMsg << endl);
	return true;
}

void ConnMngrEventListener::OnAfterIssueSent(const char* pNs, Msg* pMsg)
{
	TRC_SMSG(all, 4, __FUNCTION__ << " NameSpace: " << pNs << endl << "Msg: " << pMsg << endl);
	return;
}

void ConnMngrEventListener::OnReliableStatus(const IcsPubStatus& rStatus)
{
#if defined( USE_NDDS )
	TRC_SMSG(all, 4, __FUNCTION__ << " Namespace: " << rStatus.GetNamespace() << " Event: " << rStatus.GetPublicationEvent() <<
			 "Number of unreliable subs: " << rStatus.GetNumOfSubsUnreliable() << " Reliable Subs: " << rStatus.GetNumOfSubsReliable()
			 << " UnAcked: " << rStatus.GetNumOfUnackedIssues() << endl);
#endif
}

ConnMngrPublisher::ConnMngrPublisher()
{
    mPubNc = NULL;
    mPubSc = NULL;
    mPubNc = new IcsPublication(IcsLocalDomainService::Instance(), 
                                ConnMngrUtil::ConnMngrNcNs(), NULL, &mEventListener);
    if(mPubNc == NULL)
    {
        //TODO : Assert
    }

    mPubSc = new IcsPublication(IcsLocalDomainService::Instance(), ConnMngrUtil::ConnMngrScNs(), NULL, &mEventListener);
    if(mPubSc == NULL)
    {
        //TODO: Assert
    }

    mNcSubPresent = false;
    cout << "ConnMngrPublisher Installed" << endl;
}

void 
ConnMngrPublisher::SendConnUpdate(ConnMngrUtil::ConnStatus status, uint32 shelfId)
{
    OsMutexGuard guard(mLock);

    if((shelfId == 0) && !IsNcSubscriberPresent())
    {
        if(TestNcSubscription())
            mNcSubPresent = true;
        else
            return;           
    }

    std::auto_ptr<NvBufMsg> ptr_msg(new NvBufMsg());

    string nvName = ConnMngrUtil::GetNvNameString(status);

    NvWriter& nvw = ptr_msg->OpenWriter(nvName.c_str());

    nvw.Append("shelfId", shelfId);

    ZTRY
    {
        if(shelfId != 0)
        {
            mPubSc->Send(ptr_msg.release());
        }
        else
        {
        	mPubNc->Send(ptr_msg.release());
        }
    }

    ZCATCH_BASE(ex)
    {
        ZASSERT_STRM(0, __FUNCTION__, "SendConnUpdate failed");
    } 
}

bool
ConnMngrPublisher::IsNcSubscriberPresent()
{
    return mNcSubPresent;
}

bool
ConnMngrPublisher::TestNcSubscription()
{
    // TODO: change the implementation to verify the exact Subscribers.
    return (mPubNc->SubscriptionWait(1000, 0, 2) == ZOK);
}


