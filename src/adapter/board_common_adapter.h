/*
 * board_common_adapter.h
 *
 *  Created on: 9/1, 2020
 *
 */

#ifndef CHM6_BOARD_MS_SRC_CHM6BOARDCOMMONADAPTER_IMPL_H_
#define CHM6_BOARD_MS_SRC_CHM6BOARDCOMMONADAPTER_IMPL_H_

#include <iostream>
#include <string>
#include <mutex>
#include <boost/ptr_container/ptr_vector.hpp>

#include "board_defs.h"
#include "types.h"
#include "board_driver.h"

#include "SimpleLog.h"

#include "board_adapter_if.h"

namespace boardAda
{

using namespace boardMs;


class BoardCommonAdapter : public BoardAdapterIf
{
public:

    BoardCommonAdapter();

    ~BoardCommonAdapter();

    std::string GetAid() { return mAid; }

    std::string getName(){ return mName; }

    /*
     * Board fault
     */
    board_fault_ptr_vec& GetBoardFault();

    /*
     * Board PM
     */

    virtual board_pm_ptr_vec& GetBoardPm();


    upgradable_device_ptr_vec& GetUpgradableDevices();

    // CLI commands
    virtual void DumpLog(std::ostream &os);

    virtual void DumpStatus(std::ostream &os, std::string cmd);

    virtual void ResetLog( std::ostream &os );

    virtual void DumpUpgradableDevices( std::ostream &os );

    virtual void DumpFaults(std::ostream &os, uint32 faultId);

    virtual void DumpFaults(std::ostream &os, std::string faultName);

    virtual void SetFaultSim(std::ostream &os, uint32 faultId,
                             bool isSimEn, bool isTripped);

    virtual void SetFaultSim(std::ostream &os, std::string faultNm,
                             bool isSimEn, bool isTripped);

    virtual void PrintMaxFaultNum(std::ostream &os);

    virtual void DumpPms(std::ostream &os);

    virtual void DumpLedStates(std::ostream &os);

    virtual void SetAccessFaultSim(std::ostream &os,
                                   std::string strfaultId,
                                   bool bSetFault);

protected:

    virtual void InitUpgradeableDevices();

    virtual void AddLog(const std::string &func,
                        uint32 line, const std::string &text);

    uint32 LookupFaultId(std::string faultName);

    std::string     mAid;
    std::string     mName;

    hostBoardActionType mLastBoardAction;

    // Board status
    Chm6EqptState mEqptState;

    Chm6EqptInventory  mEqptInv;

    // LED states
    LedStateType mPowerLedState;

    LedStateType mFaultLedState;

    LedStateType mActiveLedState;

    upgradable_device_ptr_vec mvUpgradableDevices;

    // Host board's faults and two Mezz cards' faults
    board_fault_ptr_vec mvBoardFaults;

    // Host board's PMs and two Mezz cards' PMs
    board_pm_ptr_vec mvBoardPms;

    // port LED states
    port_led_ptr_vec mvPortLedStates;

    // Line LED states
    line_led_ptr_vec mvLineLedStates;

    bool mIsLedLampTestOn;

    bool mIsLedLocTestOn;

    SimpleLog::Log*  mpLog;
    mutable std::mutex    mLogLock;

private:
  const uint32 StrToFaultId( RT_AccessFaultsMap& afMap,
                             std::string strfaultId );

  void DoSetAccessFaultSim( std::ostream &os,
                            std::string strfaultId,
                            bool bSetFault );

};

} // namespace boardAda

#endif /* CHM6_BOARD_MS_SRC_CHM6BOARDCOMMONADAPTER_IMPL_H_ */
