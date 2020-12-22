//
//
// ResourceHandlerBatchThread.h
//
//
// Copyright(c) Infinera 2002-2018
//
//

#include <EHCommon/ResourceHandlerBatchThread.h>

ResourceHandlerBatchThread::ResourceHandlerBatchThread(const TimerProperty &prop,
													   const TimerJobIf &job)
{
	// Create a Joinable timer thread
	mpThreadedTimer = new ThreadedTimer(prop, job, false, true);

}

ResourceHandlerBatchThread::~ResourceHandlerBatchThread()
{
	Destroy();
}

void ResourceHandlerBatchThread::Start()
{
	mpThreadedTimer->Start();
}

void ResourceHandlerBatchThread::Destroy()
{
	mpThreadedTimer->Destroy();
}

void ResourceHandlerBatchThread::Activate(bool bInvokeJobImmediately)
{
	mpThreadedTimer->Activate(bInvokeJobImmediately);
}

void ResourceHandlerBatchThread::Deactivate(bool bInvokeJobImmediately)
{
	mpThreadedTimer->Deactivate(bInvokeJobImmediately);
}

