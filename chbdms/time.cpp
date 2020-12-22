//
// src_cc/Util/TimerMgr.cpp
//
// Use an instance of this class if you wish to have a consolidated
// timer management thread (instead of one thread per timer as in 
// ThreadedTimer) and have jobs executed as and when required
// Ensure however that the "job" and calling task is thread-safe. 
// For purpose of efficiency only Timers with a minimum granularity 
// in the order of seconds can be managed by this class (to be fixed 
// later on). For millisec timers, use ThreadedTimer class.
// Depending on the definition of the contained classes used by TimerJobIf, 
// code is flexible enough that the job can even be executed in a seperate 
// thread if required - in this case it is strongly recommended that
// protection locks be used in the ExecuteJob () and Destroy () methods!!!
//
// Steps to use this class (see samples in .cpp file under #ifdef TEST_TIMER_MGR):
// ===============================================================================
// 1. For jobs to be executed inline
//		a) Define a class XXX as "class XXX: public virtual TimerJobIf"
//         and implement the Copy (), Destroy (), ExecuteJob () and
//         Output () pure virtual methods
//		b) In your code, create a stack instance of TimerProperty class and
//         of XXX class
//      c) Create a stack instance of TimerMgrJob class giving the instances 
//         created in step (b) as input to the ctor. 
//		d) Create an allocated memory instance of the TimerMgr class giving
//         the maximum number of jobs permitted. For each instance of (c),
//         call TimerMgr::AddJob () or TimerMgr::RemoveJob () to add or remove
//         a timer job respectively. This can be done even after TimerMgr::Start ()
//         is called
//      e) For the TimerMgr class instance in (c), call the Start() method.
//         The timer will call the jobs inline as per the timer properties specified
//
// 2. For jobs to be executed in a seperate thread
//		a) Define a class XXX as "class XXX: public virtual TimerJobIf"
//         and implement the Copy (), Destroy (), ExecuteJob () and
//         Output () pure virtual methods
//         Also define a class XXXThread as "class XXXThread: public virtual OsThread"
//         and implement the Run () method
//
//         It is recommended that XXX::ExecuteJob () create an allocated memory instance 
//         of XXXThread followed by a call to XXXThread::Start ()
//
//		b) In your code, create a stack instance of TimerProperty class and
//         of XXX class
//      c) Create a stack instance of TimerMgrJob class giving the instances 
//         created in step (b) as input to the ctor. 
//		d) Create an allocated memory instance of the TimerMgr class giving
//         the maximum number of jobs permitted. For each instance of (c),
//         call TimerMgr::AddJob () or TimerMgr::RemoveJob () to add or remove
//         a timer job respectively. This can be done even after TimerMgr::Start ()
//         is called
//      e) For the TimerMgr class instance in (c), call the Start() method.
//         The timer will call the jobs inline as per the timer properties specified
//
// $Author: nsubramaniam $
//
// Copyright(c) Infinera 2003
//
//

#include <Util/TimerMgr.h>
#include <OsEncap/OsCountingSemaphore.h>
#include <OsEncap/OsMutexGuard.h>

// TimerMgrJob
TimerMgrJob::TimerMgrJob ()
: mId (0), mpProp (0), mpJob (0)
{
}

TimerMgrJob::TimerMgrJob (const TimerMgrJob & job)
: mId (job.mId), mpProp (0), mpJob (0)
{
	if (job.mpProp)
	{
		mpProp = new TimerProperty (*(job.mpProp));
	}

	if (job.mpJob)
	{
		mpJob = (job.mpJob)->Copy ();
	}
}

TimerMgrJob::~TimerMgrJob ()
{
	if (mpJob)
	{
		mpJob->Destroy ();
		mpJob = NULL;
	}

	if (mpProp)
	{
		delete mpProp;
		mpProp = NULL;
	}
}

TimerMgrJob::TimerMgrJob (const TimerProperty & prop, 
						  const TimerJobIf & job)
						  : mId (0), mpProp (0), mpJob (0)
{
	mpProp = new TimerProperty (prop);
	mpJob = ((TimerJobIf *)&job)->Copy ();
}

std::ostream & operator << (std::ostream & os, const TimerMgrJob & job)
{
	os << "TimerMgrJob::mId = " << job.mId << endl;
	if (job.mpProp)
	{
		os << *(job.mpProp);
	}
	if (job.mpJob)
	{
		(job.mpJob)->Output (os);
	}
	return os;
}

// TimerMgr
TimerMgr::TimerMgr ()
: OsThread (""), mMaxJobs (0), mpJobList (0), mpJobExpiryList (0), mLastJobId (0)
{
}

void TimerMgr::Destroy ()
{
	delete this;
}

TimerMgr::~TimerMgr ()
{
	mMutex.Lock ();

	if (mpJobList)
	{
		std::map<unsigned long, TimerMgrJob *>::iterator itr = 
			mpJobList->begin ();

		while (itr!= mpJobList->end ())
		{
			delete (*itr).second;
			itr++;
		}
		mpJobList->clear ();
		delete mpJobList;
		mpJobList = NULL;
	}

	if (mpJobExpiryList)
	{
		mpJobExpiryList->clear ();
		delete mpJobExpiryList;
		mpJobExpiryList = NULL;
	}

	mMutex.Unlock ();
}

TimerMgr::TimerMgr (unsigned int maxJobs)
: OsThread (""), mMaxJobs (maxJobs), mpJobList (0), mpJobExpiryList (0), mLastJobId (0)
{
	mpJobList = new std::map<unsigned long, TimerMgrJob *>();
	mpJobExpiryList = new std::map<unsigned long, unsigned int>();
}

TimerMgr::TimerMgr (const TimerMgr & timerMgr)
:	OsThread ("", ((TimerMgr *)&timerMgr)->GetPriority ()),
	mMaxJobs (timerMgr.mMaxJobs), 
	mpJobList (0), 
	mpJobExpiryList (0),
	mLastJobId (timerMgr.mLastJobId)
{
	((TimerMgr *)&timerMgr)->mMutex.Lock ();

	if (timerMgr.mpJobList)
	{
		mpJobList = new std::map<unsigned long, TimerMgrJob *> ();
		if (mpJobList)
		{
			std::map<unsigned long, TimerMgrJob *>::iterator itr = 
				timerMgr.mpJobList->begin ();

			while (itr++ != timerMgr.mpJobList->end ())
			{
				// Make a deep copy
				(*mpJobList)[(*itr).first] = new TimerMgrJob (*((*itr).second));
			}
		}
	}

	if (timerMgr.mpJobExpiryList)
	{
		mpJobExpiryList = new std::map<unsigned long, unsigned int> ();
		if (mpJobExpiryList)
		{
			std::map<unsigned long, unsigned int>::iterator itr = 
				timerMgr.mpJobExpiryList->begin ();

			while (itr++ != timerMgr.mpJobExpiryList->end ())
			{
				// Make a deep copy
				(*mpJobExpiryList)[(*itr).first] = (*itr).second;
			}
		}
	}

	((TimerMgr *)&timerMgr)->mMutex.Unlock ();
}

unsigned long TimerMgr::AddJob (const TimerMgrJob & job)
{
	mMutex.Lock ();

	if (!mpJobList || !mpJobExpiryList || mpJobList->size () == mMaxJobs)
	{
		mMutex.Unlock ();
		return 0;
	}

	TimerMgrJob* pJob = new TimerMgrJob (job);
	if (!pJob)
	{
		mMutex.Unlock ();
		return 0;
	}
	// Generate job id
	pJob->mId = ++mLastJobId;

	(*mpJobList)[pJob->mId] = pJob;
	// Normalize everything to seconds - to be fixed later
	(*mpJobExpiryList)[pJob->mId] = pJob->mpProp->mTmoSecs + pJob->mpProp->mTmoMsecs/1000;
	
	mMutex.Unlock ();
	return pJob->mId;
}

int TimerMgr::RemoveJob (unsigned long id)
{
    OsMutexGuard guard( mMutex );

	if (!mpJobList || !mpJobExpiryList)
    {
        return 0;
    }

	std::map<unsigned long, TimerMgrJob*>::iterator item =
		                                mpJobList->find (id);
	if (item != mpJobList->end ())
	{
		delete (*item).second;		
	}
	mpJobList->erase (id);

	mpJobExpiryList->erase (id);

	return 1;
}

void TimerMgr::Run()
{
	while (1)
	{
		// Poll the job list every one second
		OsCountingSemaphore tmoSem (1);
		tmoSem.Take (1000); // 1000ms = 1sec
		tmoSem.Give ();

		mMutex.Lock ();
		if (!mpJobList || !mpJobExpiryList)
		{
			mMutex.Unlock ();
			break;
		}

		std::map<unsigned long, unsigned int>::iterator expiryItr = 
			mpJobExpiryList->begin ();

		while (expiryItr != mpJobExpiryList->end ())
		{
			// Determine if the job is to be executed now
			if (--((*expiryItr).second) > 0)
			{
				// Go to next job
				expiryItr++;
				continue;
			}
			std::map<unsigned long, TimerMgrJob*>::iterator jobItr = 
				mpJobList->find ((*expiryItr).first);

			if (jobItr == mpJobList->end ())
			{
				// Go to next job
				expiryItr++;
				continue;				
			}

			TimerMgrJob* pTimerJob = (*jobItr).second;
			TimerProperty* pProp = pTimerJob->mpProp;
			TimerJobIf* pJob = pTimerJob->mpJob;

			if (pProp && pJob)
			{
				pJob->ExecuteJob ();

				if (pProp->mIsRecurring &&
				    (pProp->mMaxRecurrance == 0 || --(pProp->mMaxRecurrance) >= 1)
				   )
				{
					// Reset the original expiry value and continue
					(*expiryItr).second = pProp->mTmoSecs + pProp->mTmoMsecs/1000;
					expiryItr++;
					continue;
				} 
				else 
				{
					// Remove this job from the list. However note the following:
					// If the job is still executing in a seperate thread, the job subclass
					// should have protection lock in its ExecuteJob () and Destroy ()
					// methods to allow the job to complete
					std::map<unsigned long, unsigned int>::iterator curr = expiryItr;
					expiryItr++;

					mpJobList->erase ((*curr).first);
					mpJobExpiryList->erase (curr);
					continue;
				}
			}
			// Go to next job
			expiryItr++;
		}
		
		mMutex.Unlock ();
	}
}

ostream & operator << (ostream & os, const TimerMgr & timerMgr)
{
	os << "TimerMgr::mMaxJobs      = " << timerMgr.mMaxJobs << endl;
	os << "TimerMgr::mLastJobId    = " << timerMgr.mLastJobId << endl;
	if (timerMgr.mpJobList)
	{
		std::map<unsigned long, TimerMgrJob *>::iterator itr = 
			timerMgr.mpJobList->begin ();

		while (itr != timerMgr.mpJobList->end ())
		{
			os << *((*itr).second);
			itr++;
		}
	}
	return os;
}

/////////////////////////////////////////////////
//
// SAMPLE CODE BELOW
//
////////////////////////////////////////////////

#undef TEST_TIMER_MGR
#ifdef TEST_TIMER_MGR
// MyTimerJob
class MyTimerJob: public virtual TimerJobIf
{

	class MyTimerJobThread: public virtual OsThread
	{
	public:
		MyTimerJobThread (const char *pStr)
			: mpStr (strdup (pStr)), OsThread ("")
		{
		}

	protected:
		virtual void Run ()
		{
			cout << "MyTimerJobThread: " << mpStr << endl;
		}

	private:
		char* mpStr;
	};

public:
	char* mpStr;

	virtual MyTimerJob* Copy () 
	{
		return (new MyTimerJob (*this));
	}

	virtual void Destroy ()
	{
		cout << "Exiting MyTimerJob " << mpStr << " ..." << endl;
		delete this;
	}

	MyTimerJob ()
	: mpStr (0)
	{
	}

	MyTimerJob (char* strP)
		: mpStr (strdup (strP))
	{
	}

	MyTimerJob (const MyTimerJob & job)
		: mpStr (strdup (job.mpStr))
	{
	}

	virtual ~MyTimerJob () 
	{
		if (mpStr) delete mpStr;
	}

	virtual ostream & Output (ostream & os)
	{
		static int count = 1;
		os << mpStr << "[" << count++ << "]" << endl;
		return os;
	}

	virtual void ExecuteJob ()
	{
		if (1)
		{
			this->Output (cout);
		}
		else
		{
			MyTimerJobThread* pJobThr = new MyTimerJobThread (mpStr);
			pJobThr->Start ();
		}
	}
};

int main (int argc, char** argv)
{
	TimerProperty p1 (4, 0, 1, 3);
	MyTimerJob j1 ("1");
	TimerMgrJob mj1 (p1, j1);

	TimerProperty p2 (7, 0, 1, 5);
	MyTimerJob j2 ("2");
	TimerMgrJob mj2 (p2, j2);

	TimerMgr* pMgr = new TimerMgr (10);
	pMgr->Start (); 
	int jid1 = pMgr->AddJob (mj1);

	cout << *pMgr;

	while (1)
	{
		
		
		OsCountingSemaphore tmoSem1 (1);
		tmoSem1.Take (9000);

		if (pMgr) pMgr->RemoveJob (jid1);
		if (pMgr)
		{
			int jid2 = pMgr->AddJob (mj2);
		}

		tmoSem1.Give ();

		OsCountingSemaphore tmoSem2 (1);
		tmoSem2.Take (10000);
		
		if (pMgr)
		{
			pMgr->Destroy ();
			pMgr = NULL;
		}
		tmoSem2.Give ();
	}

	return 0;
}
#endif

