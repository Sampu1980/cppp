//
//
// OsDynamicThreadPoolWorker.cpp
//
// This is an adaptation of the OsThreadPoolWorker class (fully backward compatible)
// where the number of threads dynamically grows to the allocated maximum
// and dynamically shrinks back to the specified minimum. It is very useful
// in situations where it is not required to preallocate a large number of threads 
// and to grow them dynamically only under certain situations. 
//
// The Infinera Dynamic Thread Pool Worker
//
// Copyright(c) Infinera 2003-2018
//

#include <Util/ZAssert.h>
#include <ApplProxy/OsDynamicThreadPoolWorker.h>
#include <ApplProxy/OsDynamicThreadPool.h>

OsDynamicThreadPoolWorker::OsDynamicThreadPoolWorker(const char*   pName,
                                       int           prio,
                                       int           stacksize,
                                       OsDynamicThreadPool& rPool)
: OsThread(pName, prio, stacksize, true),
  mrPool(rPool),
  mbExit(false),
  mpWorkItem (0)
{
}

OsDynamicThreadPoolWorker::~OsDynamicThreadPoolWorker()
{
}

void
OsDynamicThreadPoolWorker::Exit()
{
    mbExit = true;

	OsMutexGuard guard (mMutex);
	if (mpWorkItem)
	{
		mpWorkItem->TerminateRun ();
	}
}

void
OsDynamicThreadPoolWorker::Run()
{
    while(1)
    {
        if (mbExit)
        {
            break;
        }

        DynamicWorkItem *pWorkItem = mrPool.GetNextWork();
		
		mMutex.Lock ();
		mpWorkItem = pWorkItem;
		mMutex.Unlock ();
        
		if (mpWorkItem)
        {
			mrPool.DecrementNumberIdleThreads (); // Decrement the idle thread count
            mpWorkItem->Run();

			mMutex.Lock ();
            mpWorkItem->Destroy();
			mpWorkItem = 0;
			mMutex.Unlock ();

			mrPool.IncrementNumberIdleThreads (); // Increment the idle thread count

			if ((mrPool.IsNumberThreadsMoreThanMinimum () == true) && 
				(mrPool.GetNumberWorkItems () == 0)) // If there is an excess of idle threads, exit this thread as long as there are no pending work items
			{
				mrPool.DecrementNumberIdleThreads (); // Decrement the idle thread count
				break;
			}
        }
    }
	mrPool.RemoveWorkerThread (this); // Remove this worker thread from the list of threads maintained by the thread pool
}

