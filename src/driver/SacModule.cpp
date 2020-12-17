/*
 * SacModule.cpp
 *
 */

#include "SacModule.h"
#include "RegIfException.h"
#include "InfnLogger.h"

SacModule::SacModule(std::string name, RegIf *pIntf)
  : mName(name)
  , mFpgaRegIf(pIntf)
  , mIsTxEnabled(false)
  , mIsRxEnabled(false)
  , mTotalFrameErrCount(0)
  , mTotalCrcErrCount(0)
  , mTotalRxClkErrCount(0)
  , mReeableRx(false)
{
    INFN_LOG(SeverityLevel::info) << mName << " Created!";
}

SacModule::~SacModule()
{
}

int SacModule::SetSacModuleEnable(bool isTxEnable, bool isRxEnable)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint32 data = 0;

    try
    {
        data = mFpgaRegIf->Read32(cFpgaSacControlRegOffset);

        data &= ~cSacControlTxEnableMask;
        if (isTxEnable == true)
        {
            data |= cSacControlTxEnableMask;
        }

        data &= ~cSacControlRxEnableMask;
        if (isRxEnable == true)
        {

            data |= cSacControlRxEnableMask;
        }

        mFpgaRegIf->Write32(cFpgaSacControlRegOffset, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: SetSacModuleEnable() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    mIsTxEnabled = isTxEnable;
    mIsRxEnabled = isRxEnable;

    INFN_LOG(SeverityLevel::info) << mName << " mIsTxEnabled: " << std::boolalpha << mIsTxEnabled
                   << " mIsRxEnabled: " << std::boolalpha << mIsRxEnabled;

    return 0;
}

int SacModule::SetSacModuleTxEnable(bool isTxEnable)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint32 data = 0;

    try
    {
        data = mFpgaRegIf->Read32(cFpgaSacControlRegOffset);

        data &= ~cSacControlTxEnableMask;
        if (isTxEnable == true)
        {
            data |= cSacControlTxEnableMask;
        }

        mFpgaRegIf->Write32(cFpgaSacControlRegOffset, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: SetSacModuleTxEnable() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    mIsTxEnabled = isTxEnable;

    INFN_LOG(SeverityLevel::info) << mName << " mIsTxEnabled: " << std::boolalpha << mIsTxEnabled;

    return 0;
}

int SacModule::SetSacModuleRxEnable(bool isRxEnable)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint32 data = 0;

    try
    {
        data = mFpgaRegIf->Read32(cFpgaSacControlRegOffset);

        data &= ~cSacControlRxEnableMask;
        if (isRxEnable == true)
        {

            data |= cSacControlRxEnableMask;
        }

        mFpgaRegIf->Write32(cFpgaSacControlRegOffset, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: SetSacModuleRxEnable() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    mIsRxEnabled = isRxEnable;

    INFN_LOG(SeverityLevel::info) << mName << " mIsRxEnabled: " << std::boolalpha << mIsRxEnabled;

    return 0;
}

int SacModule::GetSacModuleEnableState(bool& isTxEnable, bool& isRxEnable)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint32 data = 0;

    try
    {
        data = mFpgaRegIf->Read32(cFpgaSacControlRegOffset);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: GetSacModuleEnableState() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    if ((data & cSacControlTxEnableMask) != 0)
    {
        isTxEnable = true;
    }
    else
    {
        isTxEnable = false;
    }

    if ((data & cSacControlRxEnableMask) != 0)
    {
        isRxEnable = true;
    }
    else
    {
        isRxEnable = false;
    }

    return 0;
}

int SacModule::GetSacModuleStatus(bool& isFrameErr, bool& isCrcErr, bool& isRxClkErr)
{
    std::lock_guard<std::recursive_mutex> guard(mLock);

    uint32 data = 0;

    try
    {
        data = mFpgaRegIf->Read32(cFpgaSacStatusRegOffset);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << mName << " Error: GetSacModuleStatus() failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    if ((data & cSacStatusFrameErrMask) != 0)
    {
        isFrameErr = true;
        if (mTotalFrameErrCount ==0)
        {
            INFN_LOG(SeverityLevel::info) << "Frame error is set!";
        }
        mTotalFrameErrCount++;
    }
    else
    {
        isFrameErr = false;
        if (mTotalFrameErrCount != 0)
        {
            INFN_LOG(SeverityLevel::info) << "Frame error is cleared! Previous TotalFrameErrCount = " << mTotalFrameErrCount;
            mTotalFrameErrCount = 0;
        }
    }

    if ((data & cSacStatusCrcErrMask) != 0)
    {
        isCrcErr = true;
        if (mTotalCrcErrCount == 0)
        {
            INFN_LOG(SeverityLevel::debug) << "CRC error is set!";
        }
        mTotalCrcErrCount++;
    }
    else
    {
        isCrcErr = false;
        if (mTotalCrcErrCount != 0)
        {
            INFN_LOG(SeverityLevel::debug) << "CRC error is cleared! Previous TotalCrcErrCount = " << mTotalCrcErrCount;
            mTotalCrcErrCount = 0;
        }
    }

    if ((data & cSacStatusRxSclkErrMask) != 0)
    {
        isRxClkErr = true;
        if (mTotalRxClkErrCount == 0)
        {
            INFN_LOG(SeverityLevel::info) << "RX clock error is set!";
        }
        mTotalRxClkErrCount++;
    }
    else
    {
        isRxClkErr = false;
        if (mTotalRxClkErrCount != 0)
        {
            INFN_LOG(SeverityLevel::info) << "RX clock error is cleared! Previous TotalRxClkErrCount = " << mTotalRxClkErrCount;
            mTotalRxClkErrCount = 0;
            mReeableRx = true;
        }
    }

    return 0;
}

int SacModule::CheckRedoSacModuleRxEnable()
{
    bool isFrameErr = false;
    bool isCrcErr = false;
    bool isRxClkErr = false;

    int r1 = GetSacModuleStatus(isFrameErr, isCrcErr, isRxClkErr);
    if (r1 != 0)
    {
        INFN_LOG(SeverityLevel::info) << "GetSacModuleStatus() failed!";
        return r1;
    }

    int r2 = 0;
    if (mReeableRx == true)
    {
        int r2 = SetSacModuleRxEnable(true);
        if (r2 != 0)
        {
            INFN_LOG(SeverityLevel::info) << "SetSacModuleRxEnable() failed!";

        }
        else
        {
            INFN_LOG(SeverityLevel::info) << "SetSacModuleRxEnable() re-enabled RX!";
            mReeableRx = false;
        }
    }
    return r2;
}

void SacModule::Dump(std::ostream& out)
{
    out << std::endl << "================== SAC Module Enable Configure  ================== " << std::endl;

    out << "\tConfig mIsTxEnabled    : " << std::boolalpha << mIsTxEnabled << std::endl;
    out << "\tConfig mIsRxEnabled    : " << std::boolalpha << mIsRxEnabled << std::endl;

    out << std::endl << "================== SAC Module Current Enable States ================== " << std::endl;

    bool isTxEnable = false;
    bool isRxEnable = false;

    int r1 = GetSacModuleEnableState(isTxEnable, isRxEnable);
    if (r1 == 0)
    {

        out << "\tisTxEnabled    : " << std::boolalpha << isTxEnable << std::endl;
        out << "\tisRxEnabled    : " << std::boolalpha << isRxEnable << std::endl;
    }
    else
    {
        out << "GetSacModuleEnableState() failed!" << std::endl;
        INFN_LOG(SeverityLevel::info) << mName << " GetSacModuleEnableState() failed!";
    }

    out << std::endl << "================== SAC Module Current Errors ================== " << std::endl;

    bool isFrameErr = false;
    bool isCrcErr = false;
    bool isRxClkErr = false;

    int r2 = GetSacModuleStatus(isFrameErr, isCrcErr, isRxClkErr);
    if (r2 == 0)
    {
        out << "\tisFrameErr    : " << std::boolalpha << isFrameErr  << std::endl;
        out << "\tisCrcErr      : " << std::boolalpha << isCrcErr    << std::endl;
        out << "\tisRxSclkErr   : " << std::boolalpha << isRxClkErr << std::endl;

        out << std::endl << "================== SAC Module Accumulated Errors ================== " << std::endl;

        out << "\tmTotalFrameErrCount    : " << mTotalFrameErrCount << std::endl;
        out << "\tmTotalCrcErrCount      : " << mTotalCrcErrCount   << std::endl;
        out << "\tmTotalRxClkErrCount    : " << mTotalRxClkErrCount << std::endl << std::endl;
        out << "\tmReeableRx             : " << mReeableRx          << std::endl << std::endl;
    }
    else
    {
        out << "GetSacModuleStatus() failed!" << std::endl;
        INFN_LOG(SeverityLevel::info) << mName << " GetSacModuleStatus() failed!";
    }
}
