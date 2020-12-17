/*
 * SacModule.h
 *
 */

#ifndef CHM6_BOARD_MS_SRC_DRIVER_FPGASACMODULE_H_
#define CHM6_BOARD_MS_SRC_DRIVER_FPGASACMODULE_H_

#include <mutex>

#include "types.h"
#include "FpgaRegIf.h"

const uint32 cFpgaSacModuleOffset = 0x17000;

const uint32 cFpgaSacOwnRegOffset = cFpgaSacModuleOffset + 0x0;

const uint32 cFpgaSacPeerRegOffset = cFpgaSacModuleOffset + 0x800;

const uint32 cFpgaSacControlRegOffset = cFpgaSacModuleOffset + 0x1000;

/*
 * TX_ENABLE - When 1, Transmitter will send data from OWN_REG RAM to DCO
 */
const uint32 cSacControlTxEnableMask = 0x1 << 4;

/*
 * RX_ENABLE - When 1, Receiver will get data from DCO and store into PEER_REG RAM.
 * SW writes 1 to set RX_ENABLE register, and clock detection scheme clears
 * this register in case of absense of SAC serial clock.
 */
const uint32 cSacControlRxEnableMask = 0x1 << 5;

const uint32 cSacConfigRegOffset = cFpgaSacModuleOffset + 0x1004;

/*
 * Config TIMEOUT - The maximum number of idle frames on SAC bus (33-bit zero frames).
 * If TIMEOUT is reached, assert error (FRAME_ERR).
 */
const uint32 cSacConfigTimeoutMask = 0xff;

const uint32 cFpgaSacStatusRegOffset = cFpgaSacModuleOffset + 0x1010;

/*
 * STATUS_REG FRAME_ERR: Frame error, active high.
 */
const uint32 cSacStatusFrameErrMask = 0x1;

/*
 * STATUS_REG CRC_ERR: CRC error, active high.
 */
const uint32 cSacStatusCrcErrMask = 0x1 << 1;

/*
 * STATUS_REG RX_SCLK_ERR: SAC RX clock error, active high.
 */
const uint32 cSacStatusRxSclkErrMask = 0x1 << 2;

//const uint32 cFpgaSacMaskRegOffset = 0x1030;

class SacModule
{
public:
    SacModule(std::string name, RegIf *pIntf);

    ~SacModule();

    int SetSacModuleEnable(bool isTxEnable, bool isRxEnable);

    int SetSacModuleTxEnable(bool isTxEnable);

    int SetSacModuleRxEnable(bool isRxEnable);

    int GetSacModuleEnableState(bool& isTxEnable, bool& isRxEnable);

    int GetSacModuleStatus(bool& isFrameErr, bool& isCrcErr, bool& isRxClkErr);

    int CheckRedoSacModuleRxEnable();

    void Dump(std::ostream& out);

private:

    std::string mName;

    RegIf* mFpgaRegIf;

    bool mIsTxEnabled;

    bool mIsRxEnabled;

    uint32 mTotalFrameErrCount;

    uint32 mTotalCrcErrCount;

    uint32 mTotalRxClkErrCount;

    bool mReeableRx;

    std::recursive_mutex  mLock;
};


#endif /* CHM6_BOARD_MS_SRC_DRIVER_FPGASACMODULE_H_ */
