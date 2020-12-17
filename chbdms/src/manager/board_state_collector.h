/*
 * board_state_collector.h
 *
 *  Created on: Mar 12, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_CPP_BOARD_STATE_COLLECTOR_H_
#define CHM6_BOARD_MS_SRC_CPP_BOARD_STATE_COLLECTOR_H_

#include <boost/function.hpp>
#include <boost/thread.hpp>

class BoardStateCollectWorker
{
public:

    BoardStateCollectWorker()
    : mPm_strobe_ready(false) {}

    virtual ~BoardStateCollectWorker() {}

    virtual void CollectFaults() {}

    virtual void CollectPm() {}

    virtual void CollectState() {}

protected:

    friend class BoardStateCollector;

    boost::condition_variable mPm_strobe_cond;

    boost::mutex mPm_strobe_cond_mutex;

    bool mPm_strobe_ready;
};

class BoardStateCollector
{
public:

    BoardStateCollector(BoardStateCollectWorker& worker,
                        const std::string &name,
                        int collectStateInterval,
                        int collectFaultInterval,
                        bool first_run);

    ~BoardStateCollector();

    // Called when timer expired
    // Give tick and collect fault and Pm to update redis
    void Collect();

private:

    bool IsFirstRun();

    void CollectBoardFaults();

    void CollectBoardPm();

    void CollectBoardStatus();

    BoardStateCollectWorker& mrCollectWorker;

    boost::thread mThrFaults;

    boost::thread mThrPm;

    boost::thread mThrState;

    std::string mName;

    int mCollectStateInterval;

    int mCollectFaultInterval;

    bool mFirstRun;

    bool mThrdExit;
};

#endif /* CHM6_BOARD_MS_SRC_CPP_BOARD_STATE_COLLECTOR_H_ */
