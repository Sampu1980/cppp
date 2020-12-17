/*
 * board_state_collector.cpp
 *
 *  Created on: Mar 12, 2020
 */

#include <boost/bind.hpp>
#include <string>
#include <iostream>

#include "chm6/redis_adapter/application_servicer.h"
#include "InfnLogger.h"
#include "board_state_collector.h"

BoardStateCollector::BoardStateCollector(BoardStateCollectWorker& worker,
                                         const std::string &name,
                                         int collectStateInterval,
                                         int collectFaultInterval,
                                         bool first_run)
    : mrCollectWorker(worker)
    , mName(name)
    , mCollectStateInterval(collectStateInterval)
    , mCollectFaultInterval(collectFaultInterval)
    , mFirstRun(first_run)
    , mThrdExit(false)
{
}

BoardStateCollector::~BoardStateCollector()
{
    mThrdExit = true;
    try
    {
        boost::this_thread::sleep(boost::posix_time::seconds(2));
    }
    catch(...)
    {
        INFN_LOG(SeverityLevel::info) << "boost::condition_error exception thrown on sleep";
    }
}

void BoardStateCollector::Collect()
{
    mThrFaults = boost::thread(boost::bind(
            &BoardStateCollector::CollectBoardFaults, this
            ));

    mThrFaults.detach();

    mThrState = boost::thread(boost::bind(
            &BoardStateCollector::CollectBoardStatus, this
            ));

    mThrState.detach();
}

bool BoardStateCollector::IsFirstRun()
{
    if (mFirstRun)
    {
        mFirstRun = false;
    }
    return mFirstRun;
}

void BoardStateCollector::CollectBoardFaults()
{
    boost::posix_time::seconds workTime(mCollectFaultInterval);

     while (!mThrdExit)
    {
        mrCollectWorker.CollectFaults();

        boost::this_thread::sleep(workTime);
    }

    INFN_LOG(SeverityLevel::info) << "Board faults Worker: finished";
}

void BoardStateCollector::CollectBoardPm()
{
    while (true)
    {
        boost::unique_lock<boost::mutex> lock(mrCollectWorker.mPm_strobe_cond_mutex);

        while (!mrCollectWorker.mPm_strobe_ready)
        {
            mrCollectWorker.mPm_strobe_cond.wait(lock);
            if (mThrdExit)
            {
                break;
            }
        }

        mrCollectWorker.mPm_strobe_ready = false;
        mrCollectWorker.CollectPm();

        if (mThrdExit)
        {
            break;
        }
    }

    INFN_LOG(SeverityLevel::info) << "Board pm Worker: finished";
}

void BoardStateCollector::CollectBoardStatus()
{
    boost::posix_time::seconds workTime(mCollectStateInterval);

     while (!mThrdExit)
    {
        mrCollectWorker.CollectState();

        boost::this_thread::sleep(workTime);
    }

    INFN_LOG(SeverityLevel::info) << "Board status Worker: finished";
}
