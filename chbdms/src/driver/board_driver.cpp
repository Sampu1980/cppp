//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include <bitset>
#include <chrono>
#include <ios>
#include <iostream>
#include <string>
#include <thread>
#include <boost/thread.hpp>

#include "board_driver.h"
#include "InfnLogger.h"
#include "EepromHdr.h"
#include "EepromTlvArea.h"
#include "RegIfException.h"

BoardDriver::BoardDriver()
    : mspFpgaPlRegIf(nullptr)
    , mspFpgaPlMiscIf(nullptr)

    , mspFpgaPlI2c0RegIf(nullptr)
    , mspFpgaPlI2c0VolSeqIf(nullptr)
    , mspFpgaPlI2c0_3_3VPwrSupplyIf(nullptr)
    , mspFpgaPlI2c0_0_8VPwrSupplyIf(nullptr)
    , mspFpgaPlI2c0TempSensorIf(nullptr)

    , mspFpgaPlI2c1RegIf(nullptr)
    , mspFpgaPlI2c1VolSeqIf(nullptr)
    , mspFpgaPlI2c1_3_3VPwrSupplyIf(nullptr)
    , mspFpgaPlI2c1_0_8VPwrSupplyIf(nullptr)
    , mspFpgaPlI2c1TempSensorIf(nullptr)

    , mspFpgaPlI2c2RegIf(nullptr)
    , mspBottomMzFPC402_1RegIf(nullptr)
    , mspBottomMzFPC402_2RegIf(nullptr)
    , mspBottomMzIoExpIf(nullptr)
    , mspBottomMzSi5394Drv(nullptr)

    , mspFpgaPlI2c3RegIf(nullptr)
    , mspTopMzFPC402_1RegIf(nullptr)
    , mspTopMzFPC402_2RegIf(nullptr)
    , mspTopMzIoExpIf(nullptr)
    , mspTopMzSi5394Drv(nullptr)

    , mspFpgaPlI2c4RegIf(nullptr)
    , mspFpgaPlI2c4InletTempSensorIf(nullptr)
    , mspFpgaPlI2c4OutletTempSensorIf(nullptr)
    , mspMfgEepromDrvRegIf(nullptr)
    , mupMfgEepromDrv(nullptr)

    , mspFpgaPlMdioIf(nullptr)

    , mupTopMezzTmp112(nullptr)
    , mupBottomMezzTmp112(nullptr)
    , mupInletTmp112(nullptr)
    , mupOutletTmp112(nullptr)

    , mspFpgaPsI2c0PwrSeqIf(nullptr)

    , mBoardState(boardMs::EQPT_STATE_UNKNOWN)
    , mspBoardInitUtil(make_shared<BoardInitUtil>())
    , mColdRestartDcoDelaySec(-1)
    , mupLog(std::make_unique<SimpleLog::Log>(2000))
{
    CreateRegIf();

    mspBoardCmnDrv = make_shared<BoardCommonDriver>(false, false);

    boost::thread(boost::bind(
            &BoardDriver::MonitorStatus, this
            )).detach();

    std::ostringstream  log;
    log << " Created!";
    AddLog(__func__, __LINE__, log.str());
    INFN_LOG(SeverityLevel::info) << log.str();
}

BoardDriver::~BoardDriver()
{
}

void BoardDriver::GetEqptInventory(Chm6EqptInventory& inv)
{
    uint8* binBuf = new uint8[cFpgaSkickMfgEepromSize];

    mupMfgEepromDrv->GetEepromBin(binBuf);

    // Parse
    EepromHdr eepromHdr;
    eepromHdr.DecodeBuffer(binBuf, cFpgaSkickMfgEepromSize);

    EepromTlvArea eepromTlvArea;
    eepromTlvArea.DecodeBuffer(binBuf + eepromHdr.GetTlvStartOffset(),
            eepromHdr.GetTlvEndOffset() - eepromHdr.GetTlvStartOffset());

    {
        FinalAssSerNoTlv *pTlv = eepromTlvArea.GetFinalAssSerNoTlvPtr();
        if (pTlv)
            inv.SerialNumber = std::string(pTlv->GetFinalAssSerNo());
        else
            inv.SerialNumber = std::string("N/A");
    }

    {
        FinalAssPartNoTlv *pTlv = eepromTlvArea.GetFinalAssPartNoTlvPtr();
        if (pTlv)
            inv.PartNumber = std::string(pTlv->GetFinalAssPartNo());
        else
            inv.PartNumber = std::string("N/A");
    }

    {
        VendorTlv *pTlv = eepromTlvArea.GetVendorTlvPtr();
        if (pTlv)
            inv.VendorName = std::string(pTlv->GetName());
        else
            inv.VendorName = std::string(eepromHdr.GetVendorInfo());
    }

    {
        MfgFinalAssRevTlv *pTlv = eepromTlvArea.GetMfgFinalAssRevTlvPtr();
        if (pTlv)
            inv.HardwareRevision = std::string(pTlv->GetFinalRev());
        else
            inv.HardwareRevision = std::string("N/A");
    }

    {
        MfgDateTlv *pTlv = eepromTlvArea.GetMfgDateTlvPtr();
        if (pTlv)
            inv.ManufactureDate = std::string(pTlv->GetMfgDate());
        else
            inv.ManufactureDate = std::string("N/A");
    }

    // TODO: firmwareVersion
    {
        inv.FirmwareVersion = std::string("N/A");
    }

    {
        CleiCodeTlv *pTlv = eepromTlvArea.GetCleiCodeTlvPtr();
        if (pTlv)
            inv.CleiCode = std::string(pTlv->GetCleiCode());
        else
            inv.CleiCode = std::string("N/A");
    }

    {
        PonTlv *pTlv = eepromTlvArea.GetPonTlvPtr();
        if (pTlv)
            inv.Pon = std::string(pTlv->GetPon());
        else
            inv.Pon = std::string("N/A");
    }

    inv.InsertionDate = std::string("Not available");

    delete[] binBuf;
}

/*
 * Run diagnostic tests
 */
void BoardDriver::RunDiag()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::RunDiag() ...";
}

/*
 * Command sent from Controller card to host board,
 * HB translates this signal to the two MZ cards;
 * then, all LEDs start to blink.
 *
 * There are total of 39 LEDs on the FRU face plate.
 * 3 FRU LEDs: Green � Power LED, Red � Fault LED, Y/G � Status LED
 * 4 Line LEDs (2 LEDs per Line): RED � LOS, Y/G � Active LED
 * 2 LEDS (Y/G, RED) for each QSFP-28/QSFP-DD (Active is bi-color Y/G, LOS is red).
 */
int BoardDriver::LedLocationTest(bool doTest)
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::LedLocationTest() " << std::boolalpha << doTest;

    try
    {
        uint32 data = mspFpgaPlRegIf->Read32(cFpgaMasterCntlReg);

        data &= ~(cFpgaCntlReg_LOCATION_TEST_EN_mask);

        if (doTest)
        {
            data |= cFpgaCntlReg_LOCATION_TEST_EN_mask;
        }

        mspFpgaPlRegIf->Write32(cFpgaMasterCntlReg, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process LedLocationTest failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    return 0;
}

int BoardDriver::SetFruFaultLedState(LedStateType ledState)
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetFruFaultLedState() ...";

    std::lock_guard<std::mutex> guard(mFruFaultLedLock);

    bool isValidColor = false;
    uint32 colorBits = 0b000;

    switch(ledState)
    {
        case RED:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_FAULT_SOLID_RED_bits;
            break;
        case FLASHING_RED:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_FAULT_FLASHING_RED_bits;
            break;
        case OFF:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_FAULT_OFF_bits;
            break;
        default:
            isValidColor = false;
            break;
    }

    if (!isValidColor)
    {
        INFN_LOG(SeverityLevel::warning) << "BoardDriver::SetFruFaultLedState(): invalid Fru FAULT LED state: " << boardMs::LedStateTypeToStr(ledState);
        return -1;
    }

    uint32 regOffset = boardMs::cFpgaLedReg3_FRU_FAULT_LED_Addr;

    uint32 mask = boardMs::cFpgaLedReg3_FRU_FAULT_mask;

    uint32 regBitPos = cFpgaLedReg3_FRU_FAULT_bitpos;

    try
    {
        uint32 data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= ~mask;
        data |= (colorBits << regBitPos);
        mspFpgaPlMiscIf->Write32(regOffset, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process Fru FAULT LED failed!. Ex: " << e.GetError();
        return -1;
    }

    INFN_LOG(SeverityLevel::info) << "Set Fru FAULT LED to " << boardMs::LedStateTypeToStr(ledState);

    INFN_LOG(SeverityLevel::info) << "regOffset: 0x" << std::hex << regOffset << std::dec
                   << " mask: 0b" << std::bitset<32>(mask)
                   << " regBitPos: " << std::dec << regBitPos
                   << " colorBits: 0b" << std::bitset<3>(colorBits);

    return 0;
}

int BoardDriver::GetFruFaultLedState(LedStateType& ledState)
{
    std::lock_guard<std::mutex> guard(mFruFaultLedLock);

    uint32 regOffset = boardMs::cFpgaLedReg3_FRU_FAULT_LED_Addr;

    uint32 mask = boardMs::cFpgaLedReg3_FRU_FAULT_mask;

    uint32 regBitPos = cFpgaLedReg3_FRU_FAULT_bitpos;

    uint32 data = 0;
    try
    {
        data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= mask;
        data >>= regBitPos;
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process Fru FAULT failed!. Ex: " << e.GetError();
        return -1;
    }

    if (data == cFpgaLedReg_FRU_FAULT_SOLID_RED_bits)
    {
        ledState = RED;
    }
    else if (data == cFpgaLedReg_FRU_FAULT_FLASHING_RED_bits)
    {
        ledState = FLASHING_RED;
    }
    else
    {
        ledState = OFF;
    }

//    INFN_LOG(SeverityLevel::info) << "Get Fru FAULT LED is: " << LedStateTypeToStr(ledState);

    return 0;
}

int BoardDriver::SetFruActiveLedState(LedStateType ledState)
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetFruActiveLedState() ...";

    std::lock_guard<std::mutex> guard(mFruActiveLedLock);

    bool isValidColor = false;
    uint32 colorBits = 0b000;

    switch(ledState)
    {
        case GREEN:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_ACTIVE_SOLID_GREEN_bits;
            break;
        case FLASHING_GREEN:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_ACTIVE_FLASHING_GREEN_bits;
            break;
        case YELLOW:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_ACTIVE_SOLID_YELLOW_bits;
            break;
        case FLASHING_YELLOW:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_ACTIVE_FLASHING_YELLOW_bits;
            break;
        case OFF:
            isValidColor = true;
            colorBits = boardMs::cFpgaLedReg_FRU_ACTIVE_OFF_bits;
            break;
        default:
            isValidColor = false;
            break;
    }

    if (!isValidColor)
    {
        INFN_LOG(SeverityLevel::warning) << "BoardDriver::SetFruActiveLedState(): invalid Fru ACTIVE LED state: " << boardMs::LedStateTypeToStr(ledState);
        return -1;
    }

    uint32 regOffset = boardMs::cFpgaLedReg3_FRU_ACTIVE_LED_Addr;

    uint32 mask = boardMs::cFpgaLedReg3_FRU_ACTIVE_mask;

    uint32 regBitPos = cFpgaLedReg3_FRU_ACTIVE_bitpos;

    try
    {
        uint32 data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= ~mask;
        data |= (colorBits << regBitPos);
        mspFpgaPlMiscIf->Write32(regOffset, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process Fru ACTIVE LED failed!. Ex: " << e.GetError();
        return -1;
    }

    INFN_LOG(SeverityLevel::info) << "Set Fru ACTIVE LED to " << boardMs::LedStateTypeToStr(ledState);

    INFN_LOG(SeverityLevel::info) << "regOffset: 0x" << std::hex << regOffset << std::dec
                   << " mask: 0b" << std::bitset<32>(mask)
                   << " regBitPos: " << std::dec << regBitPos
                   << " colorBits: 0b" << std::bitset<3>(colorBits);

    return 0;
}

int BoardDriver::GetFruActiveLedState(LedStateType& ledState)
{
    std::lock_guard<std::mutex> guard(mFruActiveLedLock);

    uint32 regOffset = boardMs::cFpgaLedReg3_FRU_ACTIVE_LED_Addr;

    uint32 mask = boardMs::cFpgaLedReg3_FRU_ACTIVE_mask;

    uint32 regBitPos = cFpgaLedReg3_FRU_ACTIVE_bitpos;

    uint32 data = 0;
    try
    {
        data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= mask;
        data >>= regBitPos;
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) <<"Error: Process Fru ACTIVE LED failed!. Ex: " << e.GetError();
        return -1;
    }

    if (data == cFpgaLedReg_FRU_ACTIVE_SOLID_GREEN_bits)
    {
        ledState = GREEN;
    }
    else if (data == cFpgaLedReg_FRU_ACTIVE_FLASHING_GREEN_bits)
    {
        ledState = FLASHING_GREEN;
    }
    else if (data == cFpgaLedReg_FRU_ACTIVE_SOLID_YELLOW_bits)
    {
        ledState = YELLOW;
    }
    else if (data == cFpgaLedReg_FRU_ACTIVE_FLASHING_YELLOW_bits)
    {
        ledState = FLASHING_YELLOW;
    }
    else
    {
        ledState = OFF;
    }

//    INFN_LOG(SeverityLevel::info) << "Get Fru ACTIVE LED is: " << LedStateTypeToStr(ledState);

    return 0;
}

/*
 * 2 LEDS (Y/G, RED) for each QSFP-28/QSFP-DD (Active is bi-color Y/G, LOS is red).
 */
int BoardDriver::SetMezzQsfpLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType ledState)
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetMezzQsfpLedState() ...";

    std::lock_guard<std::mutex> guard(mQsfgLedLock);

    bool isValidColor = false;
    uint32 colorBits = 0b000;

    if (ledType == QSFP_ACTIVE)
    {
        switch(ledState)
        {
            case GREEN:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_ACTIVE_SOLID_GREEN_bits;
                break;
            case FLASHING_GREEN:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_ACTIVE_FLASHING_GREEN_bits;
                break;
            case YELLOW:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_ACTIVE_SOLID_YELLOW_bits;
                break;
            case FLASHING_YELLOW:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_ACTIVE_FLASHING_YELLOW_bits;
                break;
            case OFF:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_ACTIVE_OFF_bits;
                break;
            default:
                isValidColor = false;
                break;
        }

        if (!isValidColor)
        {
            INFN_LOG(SeverityLevel::warning) << "BoardDriver::SetMezzQSFPLedState(): invalid QSFP ACTIVE LED state: " << boardMs::LedStateTypeToStr(ledState) << std::endl;
            return -1;
        }
    }
    else if (ledType == QSFP_LOS)
    {
        switch(ledState)
        {
            case RED:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_LOS_SOLID_RED_bits;
                break;
            case FLASHING_RED:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_LOS_FLASHING_RED_bits;
                break;
            case OFF:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_QSFP_LOS_OFF_bits;
                break;
            default:
                isValidColor = false;
                break;
        }

        if (!isValidColor)
        {
            INFN_LOG(SeverityLevel::warning) << "BoardDriver::SetMezzQSFPLedState(): invalid QSFP LOS LED state: " << boardMs::LedStateTypeToStr(ledState) << std::endl;
            return -1;
        }
    }

    uint32 regOffset = boardMs::cFpgaLedReg_QSFP_LED_Addr[ledType][portId];

    uint32 mask = boardMs::cFpgaLedReg_QSFP_mask[ledType][portId];

    uint32 regBitPos = boardMs::cFpgaLedReg_QSFP_bitpos[ledType][portId];

    try
    {
        uint32 data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= ~mask;
        data |= (colorBits << regBitPos);
        mspFpgaPlMiscIf->Write32(regOffset, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process QSFP LED failed! portId=" << portId << ". Ex: " << e.GetError();
        return -1;
    }

    INFN_LOG(SeverityLevel::info) << "Set QSFP LED on "
                   << boardMs::QSFPPortIdToStr(portId)   << " "
                   << boardMs::QSFPLedTypeToStr(ledType) << " "
                   << " to " << boardMs::LedStateTypeToStr(ledState);

    INFN_LOG(SeverityLevel::info) << "regOffset: 0x" << std::hex << regOffset << std::dec
                   << " mask: 0b" << std::bitset<32>(mask)
                   << " regBitPos: " << std::dec << regBitPos
                   << " colorBits: 0b" << std::bitset<3>(colorBits);

    return 0;
}

int BoardDriver::GetMezzQsfpLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType& ledState)
{
    std::lock_guard<std::mutex> guard(mQsfgLedLock);

    uint32 regOffset = boardMs::cFpgaLedReg_QSFP_LED_Addr[ledType][portId];

    uint32 mask = boardMs::cFpgaLedReg_QSFP_mask[ledType][portId];

    uint32 regBitPos = boardMs::cFpgaLedReg_QSFP_bitpos[ledType][portId];

    uint32 data = 0;

    try
    {
        data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= mask;
        data >>= regBitPos;
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process QSFP LED failed! portId=" << portId << ". Ex: " << e.GetError();
        return -1;
    }

    if (ledType == QSFP_ACTIVE)
    {
        if (data == cFpgaLedReg_QSFP_ACTIVE_SOLID_GREEN_bits)
        {
            ledState = GREEN;
        }
        else if (data == cFpgaLedReg_QSFP_ACTIVE_FLASHING_GREEN_bits)
        {
            ledState = FLASHING_GREEN;
        }
        else if (data == cFpgaLedReg_QSFP_ACTIVE_SOLID_YELLOW_bits)
        {
            ledState = YELLOW;
        }
        else if (data == cFpgaLedReg_QSFP_ACTIVE_FLASHING_YELLOW_bits)
        {
            ledState = FLASHING_YELLOW;
        }
        else
        {
            ledState = OFF;
        }
    }
    else if (ledType == QSFP_LOS)
    {
        if (data == cFpgaLedReg_QSFP_LOS_SOLID_RED_bits)
        {
            ledState = RED;
        }
        else if (data == cFpgaLedReg_QSFP_LOS_FLASHING_RED_bits)
        {
            ledState = FLASHING_RED;
        }
        else
        {
            ledState = OFF;
        }
    }

//    INFN_LOG(SeverityLevel::info) << "Get QSFP LED on "
//                   << boardMs::QSFPPortIdToStr(portId)   << " "
//                   << boardMs::QSFPLedTypeToStr(ledType) << " "
//                   << " state is: " << boardMs::LedStateTypeToStr(ledState);

    return 0;
}

/*
 * 4 Line LEDs (2 LEDs per Line): RED � LOS, Y/G � Active LED
 */
int BoardDriver::SetMezzLineLedState(LineId lineId, LineLedType ledType, LedStateType ledState)
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetMezzLineLedState() ...";

    std::lock_guard<std::mutex> guard(mLineLedLock);

    bool isValidColor = false;
    uint32 colorBits = 0b000;

    if (ledType == LINE_ACTIVE)
    {
        switch(ledState)
        {
            case GREEN:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_ACTIVE_SOLID_GREEN_bits;
                break;
            case FLASHING_GREEN:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_ACTIVE_FLASHING_GREEN_bits;
                break;
            case YELLOW:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_ACTIVE_SOLID_YELLOW_bits;
                break;
            case FLASHING_YELLOW:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_ACTIVE_FLASHING_YELLOW_bits;
                break;
            case OFF:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_ACTIVE_OFF_bits;
                break;
            default:
                isValidColor = false;
                break;
        }

        if (!isValidColor)
        {
            INFN_LOG(SeverityLevel::warning) << "BoardDriver::SetMezzLINELedState(): invalid LINE ACTIVE LED state: " << boardMs::LedStateTypeToStr(ledState) << std::endl;
            return -1;
        }
    }
    else if (ledType == LINE_LOS)
    {
        switch(ledState)
        {
            case RED:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_LOS_SOLID_RED_bits;
                break;
            case FLASHING_RED:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_LOS_FLASHING_RED_bits;
                break;
            case OFF:
                isValidColor = true;
                colorBits = boardMs::cFpgaLedReg_LINE_LOS_OFF_bits;
                break;
            default:
                isValidColor = false;
                break;
        }

        if (!isValidColor)
        {
            INFN_LOG(SeverityLevel::warning) << "BoardDriver::SetMezzLINELedState(): invalid LINE LOS LED state: " << boardMs::LedStateTypeToStr(ledState) << std::endl;
            return -1;
        }
    }

    uint32 regOffset = boardMs::cFpgaLedReg_LINE_LED_Addr[ledType][lineId];

    uint32 mask = boardMs::cFpgaLedReg_LINE_mask[ledType][lineId];

    uint32 regBitPos = boardMs::cFpgaLedReg_LINE_bitpos[ledType][lineId];

    try
    {
        uint32 data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= ~mask;
        data |= (colorBits << regBitPos);
        mspFpgaPlMiscIf->Write32(regOffset, data);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process LINE LED failed! Ex: " << e.GetError();
        return -1;
    }

    INFN_LOG(SeverityLevel::info) << "Set LINE LED on "
                   << boardMs::LineIdToStr(lineId)   << " "
                   << boardMs::LINELedTypeToStr(ledType) << " "
                   << " to " << boardMs::LedStateTypeToStr(ledState);

    INFN_LOG(SeverityLevel::info) << "regOffset: 0x" << std::hex << regOffset << std::dec
                   << " mask: 0b" << std::bitset<32>(mask)
                   << " regBitPos: " << std::dec << regBitPos
                   << " colorBits: 0b" << std::bitset<3>(colorBits);

    return 0;
}

int BoardDriver::GetMezzLineLedState(LineId lineId, LineLedType ledType, LedStateType& ledState)
{
    std::lock_guard<std::mutex> guard(mLineLedLock);

    uint32 regOffset = boardMs::cFpgaLedReg_LINE_LED_Addr[ledType][lineId];

    uint32 mask = boardMs::cFpgaLedReg_LINE_mask[ledType][lineId];

    uint32 regBitPos = boardMs::cFpgaLedReg_LINE_bitpos[ledType][lineId];

    uint32 data = 0;
    try
    {
        data = mspFpgaPlMiscIf->Read32(regOffset);
        data &= mask;
        data >>= regBitPos;
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process LINE LED failed! Ex: " << e.GetError();
        return -1;
    }

    if (ledType == LINE_ACTIVE)
    {
        if (data == cFpgaLedReg_LINE_ACTIVE_SOLID_GREEN_bits)
        {
            ledState = GREEN;
        }
        else if (data == cFpgaLedReg_LINE_ACTIVE_FLASHING_GREEN_bits)
        {
            ledState = FLASHING_GREEN;
        }
        else if (data == cFpgaLedReg_LINE_ACTIVE_SOLID_YELLOW_bits)
        {
            ledState = YELLOW;
        }
        else if (data == cFpgaLedReg_LINE_ACTIVE_FLASHING_YELLOW_bits)
        {
            ledState = FLASHING_YELLOW;
        }
        else
        {
            ledState = OFF;
        }
    }
    else if (ledType == LINE_LOS)
    {
        if (data == cFpgaLedReg_LINE_LOS_SOLID_RED_bits)
        {
            ledState = RED;
        }
        else if (data == cFpgaLedReg_LINE_LOS_FLASHING_RED_bits)
        {
            ledState = FLASHING_RED;
        }
        else
        {
            ledState = OFF;
        }
    }

//    INFN_LOG(SeverityLevel::info) << "Get LINE LED on "
//                   << boardMs::LineIdToStr(lineId) << " "
//                   << boardMs::LINELedTypeToStr(ledType)   << " "
//                   << " state is: " << boardMs::LedStateTypeToStr(ledState);

    return 0;
}

/*
 * GX FRU ACTIVE LED "Flashing GREEN" shall be used when module is warm booting to align to Groove behavior.
 * GX FRU FAULT LED "Flashing RED" shall be used when module is cold booting.
 */
void BoardDriver::ResetFruLedsOnReboot(bool isColdBoot)
{
    if (!isColdBoot)
    {
        INFN_LOG(SeverityLevel::info) << "Before warm boot: SetFruActiveLedState to FLASHING_GREEN";
        SetFruActiveLedState(boardMs::FLASHING_GREEN);
    }
}

/*
 * Check if LED lamp or location tests are under going
 *
 * 4.16 Section: 0xc000 KAH_REGISTERS
 * 4.16.18 Register: 0xc0a0 RX_KAH_STATUS
 * 24 RO LED_TEST 0x0
 * 30 RO LOC_TEST 0x0
 *
 * The Lamp test is ON when led_test=1
 * The Location test is ON when led_test=0 and loc_test=1
 */
int BoardDriver::CheckLedLampLocTestState(bool& isLampTestOn, bool& isLocTestOn)
{
//    INFN_LOG(SeverityLevel::info) << "BoardDriver::CheckLedLampLocTestState() ...";

    uint32 data = 0;

    try
    {
        data = mspFpgaPlRegIf->Read32(cFpgaRxKahStatusReg);
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Process CheckLedLampLocTestState failed!" << ". Ex: " << e.GetError();
        return -1;
    }

    if ((data & cFpgaRaxKahStatus_LED_LAMP_TEST_mask) == cFpgaRaxKahStatus_LED_LAMP_TEST_mask)
    {
        isLampTestOn = true;
        isLocTestOn  = false;
    }
    else if ((data & cFpgaRaxKahStatus_LED_LOC_TEST_mask) == cFpgaRaxKahStatus_LED_LOC_TEST_mask)
    {
        isLampTestOn = false;
        isLocTestOn  = true;
    }
    else
    {
        isLampTestOn = false;
        isLocTestOn  = false;
    }

    return 0;
}

/*
 * Set delay time before cold restart DCO
 */
void BoardDriver::SetColdRestartDcoDelay(uint32 delaySeconds)
{
    INFN_LOG(SeverityLevel::info) << "Delay to cold restart DCO after " << delaySeconds << " seconds.";

    mColdRestartDcoDelaySec = delaySeconds;
}

/*
 * Blue Gecko as slave to PS on PS I2C1
 * sends High level signal to the Power Sequencer (ADM1066) on PS I2C0
 */
void BoardDriver::PowerCycleAllFrus()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::PowerCycleAllFrus() ...";
}

/*
 * Broadcom�s BCM53161/BCM53162 - Ethernet 2.5GE switches
 * On power up via SPI2 interface by PS
 */
void BoardDriver::ConfigureBCMSwitch()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::ConfigureBCMSwitch() ...";
}

/*
 * Broadcom�s BCM53161/BCM53162 - Ethernet 2.5GE switches
 * Reset via SPI2 interface by PS
 */
void BoardDriver::ResetBCMSwitch()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::ResetBCMSwitch() ...";
}

/* Configure USB interface
 * eUSB 8GB flash module (INFN p/n 672-0017-001)
 * controlled by PS
 */
void BoardDriver::ConfigureUsbInterface()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::ConfigureUsbInterface() ...";
}

/*
 * Update HW related status/faults to FDR
 */
void BoardDriver::UpdateHwStatusToFDR()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::UpdateHwStatusToFDR() ...";
}

float32 BoardDriver::GetPm(BoardPmId id)
{
    float32 data = 0;

    switch(id)
    {
        case OUTLET_TEMPERATURE:
            data = GetOutletTemperature();
            break;
        default:
            break;
    }

    return data;
}

void BoardDriver::SetInletTmpLowLimit(float llim)
{
    mupInletTmp112->SetTmpLowLimit(llim);
}

void BoardDriver::SetInletTmpHighLimit(float hlim)
{
    mupInletTmp112->SetTmpHighLimit(hlim);
}

void BoardDriver::SetOutletTmpLowLimit(float llim)
{
    mupOutletTmp112->SetTmpLowLimit(llim);
}

void BoardDriver::SetOutletTmpHighLimit(float hlim)
{
    mupOutletTmp112->SetTmpHighLimit(hlim);
}

void BoardDriver::SetTopMezzTmpLowLimit(float llim)
{
    mupTopMezzTmp112->SetTmpLowLimit(llim);
}

void BoardDriver::SetTopMezzTmpHighLimit(float hlim)
{
    mupTopMezzTmp112->SetTmpHighLimit(hlim);
}

void BoardDriver::SetBottomMezzTmpLowLimit(float llim)
{
    mupBottomMezzTmp112->SetTmpLowLimit(llim);
}

void BoardDriver::SetBottomMezzTmpHighLimit(float hlim)
{
    mupBottomMezzTmp112->SetTmpHighLimit(hlim);
}

/*
 * Reset whole CHM6
 * Blue Gecko GMCU_PS_POR_L signal controls the Power-On-Reset (POR)
 * for the whole CHM6, including the ICE6 DCO and both MZs
 */
void BoardDriver::FruReset()
{

}

/*
 * Reset flash
 * Blue Gecko GMCU_QSPI_RESET_L
 */
void BoardDriver::FlashReset()
{

}

/*
 * Reset DCO
 * GMCU_ZYNQ_SRST_L
 */
bool ResetDcoMezzCards()
{
    return true;
}

/*
 * Reset Mess cards by power off and release them from reset
 * Mezz PL I2C bus
 */
bool ResetMessCards()
{
    return true;
}

/*
 * Set IDs from controller configuration
 */
void BoardDriver::SetSlotId(uint32 slotId)
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetSlotId() is called !!!"
         << "slotId: "  << slotId;
}

void BoardDriver::SetSledId(uint32 sledId)
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetSledId() is called !!!"
         << "sledId: "  << sledId;
}

bool BoardDriver::SetRebootDco()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetRebootDco() is called !!!";

    return true;
}

bool BoardDriver::SetRebootCpu()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetRebootCpu() is called !!!";

    return true;
}

bool BoardDriver::IsColdBoot()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::IsColdBoot() is called !!!";

    return false;
}

bool BoardDriver::IsOnBench()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::IsOnBench() is called !!!";

    return true;
}

bool BoardDriver::IsChassisDiag()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::IsChassisDiag() is called !!!";

    return true;
}

std::string BoardDriver::GetFwVersion()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::GetFwVersion() is called !!!";

    std::string fwVer = "fw_version: v1.0";

    return fwVer;
}

// Set update FW
void BoardDriver::UpdateFw()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::UpdateFw() is called !!!";
}

// Start and stop device
bool BoardDriver::SetStart()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetStart() is called !!!";

    return true;
}

bool BoardDriver::SetStop()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetStop() is called !!!";

    return true;
}

// Fault
uint64 BoardDriver::GetFault()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::GetFault() is called !!!";

    return 1;
}

void BoardDriver::SetFault()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::SetFault() is called !!!";
}

void BoardDriver::ClearFault()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::ClearFault() is called !!!";
}

void BoardDriver::UpdateFaultLed()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::UpdateFaultLed() is called !!!";
}

/*
 * Update PM
 * No PM requested now
 */
void BoardDriver::UpdatePm()
{
    INFN_LOG(SeverityLevel::info) << "BoardDriver::UpdatePm() is called !!!";
}

/*
 * Verify all interfaces are working � must do after warm reboot
 */
int CheckDeviceHealth()
{
    return 0;
}


/*
 * CLI commands to dump internal logs
 */
void BoardDriver::DumpLog(std::ostream &os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardDriver.DumpLog >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    std::lock_guard<std::mutex> guard(mLogLock);

    if (mupLog)
    {
        mupLog->DisplayLog( os );
    }
    else
    {
        os << "BoardDriver Log is not created!" << std::endl;
    }

}

void BoardDriver::DumpStatus(std::ostream &os, std::string cmd)
{
    os << "BoardDriver::DumpStatus(): cmd = " << cmd << std::endl;

    if (cmd == std::string("tmp"))
    {
        os << "<<<<<<<<<<<<<<<<<<< Tmp112.DumpTmp >>>>>>>>>>>>>>>>>>>>>>" << std::endl << std::endl;

        mupInletTmp112->DumpTmp(os);
        mupOutletTmp112->DumpTmp(os);
        mupTopMezzTmp112->DumpTmp(os);
        mupBottomMezzTmp112->DumpTmp(os);
    }

    else if (cmd == std::string("tmp_all"))
    {
        os << "<<<<<<<<<<<<<<<<<<< Tmp112.DumpAll >>>>>>>>>>>>>>>>>>>>>>" << std::endl << std::endl;

        mupInletTmp112->DumpAll(os);
        mupOutletTmp112->DumpAll(os);
        mupTopMezzTmp112->DumpAll(os);
        mupBottomMezzTmp112->DumpAll(os);
    }

    else if (cmd == std::string("led"))
    {
        DumpLedStates(os);
    }

    else if (cmd == std::string("sac"))
    {
        mupSacModule->Dump(os);
    }
}

void BoardDriver::ResetLog( std::ostream &os )
{
    os << "<<<<<<<<<<<<<<<<<<< BoardDriver.ResetLog >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    std::lock_guard<std::mutex> guard(mLogLock);

    if (mupLog)
    {
        mupLog->ResetLog();
        os << "BoardDriver Log has been reset!" << std::endl;
    }
}

/*
 * CLI commands to access RegIf
 */
uint32 BoardDriver::FpgaRead(uint32 offset)
{
    return (mspFpgaPlRegIf->Read32(offset));
}

void BoardDriver::FpgaWrite(uint32 offset, uint32 data)
{
    mspFpgaPlRegIf->Write32(offset, data);
}

/*
 * I2C bus2 bottom Mezz IO Expander read
 */
uint8 BoardDriver::PlI2c2IoxRead (uint32 offset)
{
    return (mspBottomMzIoExpIf->Read8(offset));
}

/*
 * I2C bus2 bottom Mezz IO Expander write
 */
void BoardDriver::PlI2c2IoxWrite (uint32 offset, uint8 data)
{
    mspBottomMzIoExpIf->Write8(offset, data);
}

/*
 * I2C bus3 top Mezz IO Expander read
 */
uint8 BoardDriver::PlI2c3IoxRead (uint32 offset)
{
    return (mspTopMzIoExpIf->Read8(offset));
}

/*
 * I2C bus3 top Mezz IO Expander write
 */
void BoardDriver::PlI2c3IoxWrite (uint32 offset, uint8 data)
{
    mspTopMzIoExpIf->Write8(offset, data);
}

/*
 * I2C bus2 bottom Mezz card Si5394 - ClockFanOut read
 */
uint8 BoardDriver::I2c2PllRead (uint32 offset)
{
    return (mspBottomMzSi5394Drv->Read(offset));
}

/*
 * I2C bus2 bottom Mezz card Si5394 - ClockFanOut write
 */
void BoardDriver::I2c2PllWrite (uint32 offset, uint8 data)
{
    mspBottomMzSi5394Drv->Write(offset, data);
}

/*
 * Bottom Mezz card Si5394 configure
 */
void BoardDriver::I2c2PllConfig ()
{
    mspBottomMzSi5394Drv->Configure(msSi5394_RegList);
}

/*
 * I2C bus3 top Mezz card Si5394 - ClockFanOut read
 */
uint8 BoardDriver::I2c3PllRead (uint32 offset)
{
    return (mspTopMzSi5394Drv->Read(offset));
}

/*
 * I2C bus3 top Mezz card Si5394 - ClockFanOut write
 */
void BoardDriver::I2c3PllWrite (uint32 offset, uint8 data)
{
    mspTopMzSi5394Drv->Write(offset, data);
}

/*
 * Top Mezz card Si5394 configure
 */
void BoardDriver::I2c3PllConfig ()
{
    mspTopMzSi5394Drv->Configure(msSi5394_RegList);
}

uint16 BoardDriver::MdioRead (uint8 bus_sel, uint16 mdio_addr, uint64 offset)
{
    return (mspFpgaPlMdioIf->Read16(bus_sel, mdio_addr, offset));
}

void BoardDriver::MdioWrite (uint8 bus_sel, uint16 mdio_addr,
                uint64 offset, uint16 data)
{
    mspFpgaPlMdioIf->Write16(bus_sel, mdio_addr, offset, data);
}

uint16 BoardDriver::I2cRead8 (uint8 bus_sel, uint16 phy_addr, uint64 offset)
{
    uint16 data = 0xde;

    switch ( bus_sel )
    {
        case 0:
        {
            mspFpgaPlI2c0RegIf->SetDevAddr(phy_addr);
            data = mspFpgaPlI2c0RegIf->Read8(offset);
            break;
        }

        case 1:
        {
            mspFpgaPlI2c1RegIf->SetDevAddr(phy_addr);
            data = mspFpgaPlI2c1RegIf->Read8(offset);
            break;
        }

        case 2:
        {
            mspFpgaPlI2c2RegIf->SetDevAddr(phy_addr);
            data = mspFpgaPlI2c2RegIf->Read8(offset);
            break;
        }

        case 3:
        {
            mspFpgaPlI2c3RegIf->SetDevAddr(phy_addr);
            data= mspFpgaPlI2c3RegIf->Read8(offset);
            break;
        }

        case 4:
        {
            mspFpgaPlI2c4RegIf->SetDevAddr(phy_addr);
            data= mspFpgaPlI2c4RegIf->Read8(offset);
            break;
        }

        default:
        {
            std::cout << "Read: Invalid bus# " << (uint16)bus_sel << std::endl;
            break;
        }
    }

    return data;
}

void BoardDriver::I2cWrite8 (uint8 bus_sel, uint16 phy_addr,
               uint64 offset, uint16 data)
{
    switch ( bus_sel )
    {
        case 0:
        {
            mspFpgaPlI2c0RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c0RegIf->Write8(offset, data);
            break;
        }

        case 1:
        {
            mspFpgaPlI2c1RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c1RegIf->Write8(offset, data);
            break;
        }

        case 2:
        {
            mspFpgaPlI2c2RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c2RegIf->Write8(offset, data);
            break;
        }

        case 3:
        {
            mspFpgaPlI2c3RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c3RegIf->Write8(offset, data);
            break;
        }

        case 4:
        {
            mspFpgaPlI2c4RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c4RegIf->Write8(offset, data);
            break;
        }

        default:
        {
            std::cout << "Read: Invalid bus# " << (uint16)bus_sel << std::endl;
            return;
        }
    }
}

uint16 BoardDriver::I2cRead16 (uint8 bus_sel, uint16 phy_addr, uint64 offset)
{
    uint16 data = 0xde;

    switch ( bus_sel )
    {
        case 0:
        {
            mspFpgaPlI2c0RegIf->SetDevAddr(phy_addr);
            data = mspFpgaPlI2c0RegIf->Read16(offset);
            break;
        }

        case 1:
        {
            mspFpgaPlI2c1RegIf->SetDevAddr(phy_addr);
            data = mspFpgaPlI2c1RegIf->Read16(offset);
            break;
        }

        case 2:
        {
            mspFpgaPlI2c2RegIf->SetDevAddr(phy_addr);
            data = mspFpgaPlI2c2RegIf->Read16(offset);
            break;
        }

        case 3:
        {
            mspFpgaPlI2c3RegIf->SetDevAddr(phy_addr);
            data= mspFpgaPlI2c3RegIf->Read16(offset);
            break;
        }

        case 4:
        {
            mspFpgaPlI2c4RegIf->SetDevAddr(phy_addr);
            data= mspFpgaPlI2c4RegIf->Read16(offset);
            break;
        }

        default:
        {
            std::cout << "Read: Invalid bus# " << (uint16)bus_sel << std::endl;
            break;
        }
    }

    return data;
}

void BoardDriver::I2cWrite16 (uint8 bus_sel, uint16 phy_addr,
               uint64 offset, uint16 data)
{
    switch ( bus_sel )
    {
        case 0:
        {
            mspFpgaPlI2c0RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c0RegIf->Write16(offset, data);
            break;
        }

        case 1:
        {
            mspFpgaPlI2c1RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c1RegIf->Write16(offset, data);
            break;
        }

        case 2:
        {
            mspFpgaPlI2c2RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c2RegIf->Write16(offset, data);
            break;
        }

        case 3:
        {
            mspFpgaPlI2c3RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c3RegIf->Write16(offset, data);
            break;
        }

        case 4:
        {
            mspFpgaPlI2c4RegIf->SetDevAddr(phy_addr);
            mspFpgaPlI2c4RegIf->Write16(offset, data);
            break;
        }

        default:
        {
            std::cout << "Read: Invalid bus# " << (uint16)bus_sel << std::endl;
            return;
        }
    }
}

/*
 * CLI commands for MFG EEPROM
 */
void BoardDriver::ImportMfgEepromFromBinFile(std::ostream& out, std::string& filename)
{
    mupMfgEepromDrv->ImportFromBinFile(filename.c_str());

    out << "Imported file: " << filename
        << " into buffer: " << mupMfgEepromDrv->GetEepromBinBufSrcName() << std::endl;
}

void BoardDriver::ExportMfgEepromToBinFile(std::ostream& out, std::string& filename)
{
    mupMfgEepromDrv->ExportToBinFile(filename.c_str());
    out << "Exported buffer: " << mupMfgEepromDrv->GetEepromBinBufSrcName()
        << " to file: " << filename  << std::endl;
}

void BoardDriver::ReadFromMfgEeprom(std::ostream& out)
{
    mupMfgEepromDrv->ReadFromEeprom();
    mupMfgEepromDrv->DumpEepromBinBufSrcName(out);
}

void BoardDriver::WriteToMfgEeprom(std::ostream& out)
{
    mupMfgEepromDrv->DumpEepromBinBufSrcName(out);
    mupMfgEepromDrv->WriteToEeprom();
}

void BoardDriver::DumpEepromBin(std::ostream& out)
{
    mupMfgEepromDrv->DumpEepromBin(out);
}

void BoardDriver::DumpEepromFields(std::ostream& out, int level)
{
    mupMfgEepromDrv->DumpEepromFields(out, level);
}

void BoardDriver::DumpEepromBinBufSrcName(std::ostream& out)
{
    mupMfgEepromDrv->DumpEepromBinBufSrcName(out);
}

/*
 * CLI commands for LED
 */
void BoardDriver::DumpLedRegs(std::ostream& os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardDriver.DumpLedRegs >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    os << "name" << "\t\tOffset" << "\tmask" << "\t\t\t\t\tbitpos" << std::endl;

    os << "Fru Active LED"
       << "\t0x" << std::hex << cFpgaLedReg3_FRU_ACTIVE_LED_Addr
       << "\t0b" << std::bitset<32>(cFpgaLedReg3_FRU_ACTIVE_mask)
       << "\t" <<  std::dec << cFpgaLedReg3_FRU_ACTIVE_bitpos
       << std::endl;

    os << "Fru Fault LED"
       << "\t0x" << std::hex << cFpgaLedReg3_FRU_FAULT_LED_Addr
       << "\t0b" << std::bitset<32>(cFpgaLedReg3_FRU_FAULT_mask)
       << "\t" <<  std::dec << cFpgaLedReg3_FRU_FAULT_bitpos
       << std::endl;

    os << std::endl << "QSFP ACTIVE LED:" << std::endl;
    os << "name" << "\t\tOffset" << "\tmask" << "\t\t\t\t\tbitpos" << std::endl;

    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        os << boardMs::QSFPPortIdToStr(QSFPPortId(i))
           << "\t\t0x" << std::hex << cFpgaLedReg_QSFP_LED_Addr[QSFP_ACTIVE][QSFPPortId(i)]
           << "\t0b" << std::bitset<32>(cFpgaLedReg_QSFP_mask[QSFP_ACTIVE][QSFPPortId(i)])
           << "\t" << std::dec << cFpgaLedReg_QSFP_bitpos[QSFP_ACTIVE][QSFPPortId(i)]
           << std::endl;
    }

    os << std::endl << "QSFP LOS LED:" << std::endl;
    os << "name" << "\t\tOffset" << "\tmask" << "\t\t\t\t\tbitpos" << std::endl;

    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        os << boardMs::QSFPPortIdToStr(QSFPPortId(i))
           << "\t\t0x" << std::hex << cFpgaLedReg_QSFP_LED_Addr[QSFP_LOS][QSFPPortId(i)]
           << "\t0b" << std::bitset<32>(cFpgaLedReg_QSFP_mask[QSFP_LOS][QSFPPortId(i)])
           << "\t" << std::dec << cFpgaLedReg_QSFP_bitpos[QSFP_LOS][QSFPPortId(i)]
           << std::endl;
    }

    os << std::endl << "LINE ACTIVE LED:" << std::endl;
    os << "name" << "\t\tOffset" << "\tmask" << "\t\t\t\t\tbitpos" << std::endl;

    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        os << boardMs::LineIdToStr(LineId(i))
           << "\t\t0x" << std::hex << cFpgaLedReg_LINE_LED_Addr[LINE_ACTIVE][LineId(i)]
           << "\t0b" << std::bitset<32>(cFpgaLedReg_LINE_mask[LINE_ACTIVE][LineId(i)])
           << "\t" << std::dec << cFpgaLedReg_LINE_bitpos[LINE_ACTIVE][LineId(i)]
           << std::endl;
    }

    os << std::endl << "LINE LOS LED:" << std::endl;
    os << "name" << "\t\tOffset" << "\tmask" << "\t\t\t\t\tbitpos" << std::endl;

    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        os << boardMs::LineIdToStr(LineId(i))
           << "\t\t0x" << std::hex << cFpgaLedReg_LINE_LED_Addr[LINE_LOS][LineId(i)]
           << "\t0b" << std::bitset<32>(cFpgaLedReg_LINE_mask[LINE_LOS][LineId(i)])
           << "\t" << std::dec << cFpgaLedReg_LINE_bitpos[LINE_LOS][LineId(i)]
           << std::endl;
    }
}

void BoardDriver::DumpLedStates(std::ostream& os)
{
    os << "<<<<<<<<<<<<<<<<<<< BoardDriver.DumpLedStates >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    bool isLampTestOn = false;
    bool isLocTestOn = false;

    CheckLedLampLocTestState(isLampTestOn, isLocTestOn);

    os << "isLampTestOn     : " << std::boolalpha << isLampTestOn << std::endl;
    os << "isLocationTestOn : " << std::boolalpha << isLocTestOn << std::endl << std::endl;

    os << "Below LED states are LED state registers' values. Do NOT reflect real LED colors during lamp and location tests." << std::endl;

    os << "name" << "\t\tState" << std::endl;

    LedStateType state;

    GetFruActiveLedState(state);
    os << "Fru Act LED: \t" << LedStateTypeToStr(state) << std::endl;

    GetFruFaultLedState(state);
    os << "Fru Los LED: \t" << LedStateTypeToStr(state) << std::endl;

    os << std::endl << "QSFP ACTIVE LED:" << std::endl;
    os << "name" << "\t\tState" << std::endl;

    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        GetMezzQsfpLedState(QSFPPortId(i), QSFP_ACTIVE, state);

        os << boardMs::QSFPPortIdToStr(QSFPPortId(i))
           << "\t\t" << LedStateTypeToStr(state) << std::endl;
    }

    os << std::endl << "QSFP LOS LED:" << std::endl;
    os << "name" << "\t\tState" << std::endl;

    for (uint32 i = 0; i < MAX_QSFP_NUM; ++i)
    {
        GetMezzQsfpLedState(QSFPPortId(i), QSFP_LOS, state);

        os << boardMs::QSFPPortIdToStr(QSFPPortId(i))
           << "\t\t" << LedStateTypeToStr(state) << std::endl;
    }

    os << std::endl << "LINE ACTIVE LED:" << std::endl;
    os << "name" << "\t\tState" << std::endl;

    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        GetMezzLineLedState(LineId(i), LINE_ACTIVE, state);

        os << boardMs::LineIdToStr(LineId(i))
           << "\t\t" << LedStateTypeToStr(state) << std::endl;
    }

    os << std::endl << "LINE LOS LED:" << std::endl;
    os << "name" << "\t\tState" << std::endl;

    for (uint32 i = 0; i < MAX_LINE_NUM; ++i)
    {
        GetMezzLineLedState(LineId(i), LINE_LOS, state);

        os << boardMs::LineIdToStr(LineId(i))
           << "\t\t" << LedStateTypeToStr(state) << std::endl;
    }
}

/*
 * CLI commands for host ADM1066 power sequencer
 */
void BoardDriver::ReadManid(std::ostream& out)
{
    std::ios_base::fmtflags f(out.flags());
    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadByteData(cAdm1066MANID);

    out << "Host board ADM1066 MANID(0xF4) = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << data << std::dec << std::endl;
    out.flags( f );
}

void BoardDriver::ReadRevid(std::ostream& out)
{
    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadByteData(cAdm1066REVID);

    std::ios_base::fmtflags f(out.flags());
    out << "Host board ADM1066 REVID(0xF5) = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << data << std::dec << std::endl;
    out.flags(f);
}

void BoardDriver::ReadMark1(std::ostream& out)
{
    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadByteData(cAdm1066MARK1);

    std::ios_base::fmtflags f(out.flags());
    out << "Host board ADM1066 MARK1(0xF6) = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << data << std::dec << std::endl;
    out.flags(f);
}

void BoardDriver::ReadMark2(std::ostream& out)
{
    std::ios_base::fmtflags f(out.flags());
    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadByteData(cAdm1066MARK2);

    out << "Host board ADM1066 MARK2(0xF7) = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << data << std::dec << std::endl;
    out.flags(f);
}

void BoardDriver::ReadByte(std::ostream& out)
{
    std::ios_base::fmtflags f(out.flags());
    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadByte();

    out << "Host board ADM1066 read_byte = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << data << std::dec << std::endl;
    out.flags(f); 
}

void BoardDriver::ReadByteData(std::ostream& out, uint8 command)
{
    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadByteData(command);

    out << "Host board ADM1066 read_byte_data: command = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)command
        << " data = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << data
        << std::dec << std::endl;
}

void BoardDriver::ReadWordData(std::ostream& out, uint8 command)
{
    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadWordData(command);

    out << "Host board ADM1066 read_word_data: command = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)command
        << " data = 0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << data
        << std::dec << std::endl;
}

void BoardDriver::ReadBlockData(std::ostream& out, uint8 command)
{
    uint8* value = new uint8[32]; // SMBus allows at most 32 bytes

    sint32 data = mspFpgaPsI2c0PwrSeqIf->ReadBlockData(command, value);

    std::ios_base::fmtflags f(out.flags());
    out << "Host board ADM1066 read_block_data: command = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)command
        << " number of bytes = " << std::dec << data << std::endl;

    for (int i=0; i < 32; ++i)
    {
        if ((i & 0x7) == 0)
        {
            out << "\n" << boost::format("%02x") % i << ": ";
        }

        out << boost::format("%02x") % (int)(value[i]) << " ";
    }

    out << std::endl << std::endl;
    out.flags(f);
}

void BoardDriver::WriteByte(std::ostream& out, uint8 value)
{
    mspFpgaPsI2c0PwrSeqIf->WriteByte(value);

    std::ios_base::fmtflags f(out.flags());
    out << "Host board ADM1066 write_byte value = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)value << std::dec << std::endl;
    out.flags(f);
}

void BoardDriver::WriteByteData(std::ostream& out, uint8 command, uint8 value)
{
    mspFpgaPsI2c0PwrSeqIf->WriteByteData(command, value);

    out << "Host board ADM1066 write_byte_data: command = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)command << std::dec
        << " value = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)value << std::dec
        << std::endl;
}

void BoardDriver::WriteWordData(std::ostream& out, uint8 command, uint16 value)
{
    mspFpgaPsI2c0PwrSeqIf->WriteWordData(command, value);

    out << "Host board ADM1066 write_word_data: command = 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << (int)command
        << " value = 0x" << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << (int)value
        << std::dec << std::endl;
}

/*
 * Host ADM1066 reversion @ EEPROM byte 0xf900
 */
void BoardDriver::DumpHostADM1066Revision(std::ostream& out)
{
    unsigned short eeprom_rev_addr = cAdm1066EepromRevAddr;

    mspFpgaPsI2c0PwrSeqIf->WriteByteData((eeprom_rev_addr & 0xff00) >> 8, eeprom_rev_addr & 0xff);
    uint32 ver = mspFpgaPsI2c0PwrSeqIf->ReadByte();

    out << "Host ADM1066 version: "  << ver << std::endl;
}

void BoardDriver::DumpHostAdm1066Eeprom(std::ostream& out)
{
    out << "EEPROM 64 bytes from 0xf800 (byte by byte):\n";

    unsigned short eeprom_addr = cAdm1066EepromStartAddr;
    int value;

    for (int i=0; i<64; ++i, ++eeprom_addr)
    {
        mspFpgaPsI2c0PwrSeqIf->WriteByteData((eeprom_addr & 0xff00) >> 8, eeprom_addr & 0xff);
        value = mspFpgaPsI2c0PwrSeqIf->ReadByte();
        out << boost::format("%02x") % value << " ";
        if ((i & 0xf) == 0xf)
            out << std::endl;
    }

    out << std::dec << std::endl;;
    out << "EEPROM 256 bytes from 0xf800 (32-byte blocks):\n";

    unsigned char block[256];

    uint8 offset = 0;

    for (uint i = 0; i < 8; ++i)
    {
        mspFpgaPsI2c0PwrSeqIf->WriteByteData(0xf8, offset);
        mspFpgaPsI2c0PwrSeqIf->ReadBlockData(cAdm1066BlockReadCode, block+offset);
        offset += 0x20;
    }

    for (int i=0; i<256; ++i)
    {
        if ((i & 0xf) == 0)
        {
            out << "\n" << boost::format("%02x") % i << ": ";
        }
        out << boost::format("%02x") % (int)(block[i]) << " ";
    }
    out << std::endl << std::endl;

    out << "EEPROM 64 bytes from 0xf900 (byte by byte):" << std::endl;

    eeprom_addr = cAdm1066EepromStartAddr+256;

    for (int i=0; i<64; ++i, ++eeprom_addr)
    {
        mspFpgaPsI2c0PwrSeqIf->WriteByteData((eeprom_addr & 0xff00) >> 8, eeprom_addr & 0xff);
        value = mspFpgaPsI2c0PwrSeqIf->ReadByte();
        out << boost::format("%02x") % value << " ";
        if ((i & 0xf) == 0xf)
            out << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////

/*
 * Initialize CHM6 board after power up
 */
int BoardDriver::Initialize()
{
    if (IsColdBoot() )
    {
        return (mspBoardInitUtil->ColdInit());
    }
    else
    {
        return (mspBoardInitUtil->WarmInit());
    }
}

/*
 * Create register interfaces
 */
void BoardDriver::CreateRegIf()
{
    RegIfFactory* pFactory = new RegIfFactory();

	RegIfFactorySingleton::InstallInstance(pFactory);

    mspFpgaPlRegIf = pFactory->CreateFpgaPlRegIf();

    mspFpgaPlMiscIf = pFactory->CreateFpgaMiscRegIf();

    /*
     * I2C[0] assigned to bottom MEZZ I2C[0]
     */
    mspFpgaPlI2c0RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus0);

    mspFpgaPlI2c0VolSeqIf = pFactory->CreateFpgaPlI2c0VoltageSequencerRegIf();

    mspFpgaPlI2c0_3_3VPwrSupplyIf = pFactory->CreateFpgaPlI2c03_3VPwrSupplyRegIf();

    mspFpgaPlI2c0_0_8VPwrSupplyIf = pFactory->CreateFpgaPlI2c00_8VPwrSupplyRegIf();

    mspFpgaPlI2c0TempSensorIf = pFactory->CreateFpgaPlI2c0TempSensorRegIf();

    /*
     * I2C[1] assigned to top MEZZ I2C[1]
     */
    mspFpgaPlI2c1RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus1);

    mspFpgaPlI2c1VolSeqIf = pFactory->CreateFpgaPlI2c1VoltageSequencerRegIf();

    mspFpgaPlI2c1_3_3VPwrSupplyIf = pFactory->CreateFpgaPlI2c13_3VPwrSupplyRegIf();

    mspFpgaPlI2c1_0_8VPwrSupplyIf = pFactory->CreateFpgaPlI2c10_8VPwrSupplyRegIf();

    mspFpgaPlI2c1TempSensorIf = pFactory->CreateFpgaPlI2c1TempSensorRegIf();

    /*
     * I2C[2] assigned to bottom MEZZ I2C[2]
     */
    mspFpgaPlI2c2RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus2);

    mspBottomMzFPC402_1RegIf = pFactory->CreateBottomMzFPC402_1RegIf();

    mspBottomMzFPC402_2RegIf = pFactory->CreateBottomMzFPC402_2RegIf();

    mspBottomMzIoExpIf = pFactory->CreateBottomMzIOExpanderRegIf();

    mspBottomMzSi5394Drv = pFactory->CreateBottomMzSi5394Drv();

    /*
     * I2C[3] assigned to top MEZZ I2C[3]
     */
    mspFpgaPlI2c3RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus3);

    mspTopMzFPC402_1RegIf = pFactory->CreateTopMzFPC402_1RegIf();

    mspTopMzFPC402_2RegIf = pFactory->CreateTopMzFPC402_2RegIf();

    mspTopMzIoExpIf = pFactory->CreateTopMzIOExpanderRegIf();

    mspTopMzSi5394Drv = pFactory->CreateTopMzSi5394Drv();

    /*
     * I2C[4] assigned to SKICK I2C
     */
    mspFpgaPlI2c4RegIf = pFactory->CreateFpgaPlI2cRegIf(PlI2cBus4);

    mspFpgaPlI2c4InletTempSensorIf = pFactory->CreateFpgaPlI2c4InletTempSensorRegIf();

    mspFpgaPlI2c4OutletTempSensorIf = pFactory->CreateFpgaPlI2c4OutletTempSensorRegIf();

    mspMfgEepromDrvRegIf = pFactory->CreateFpgaPlI2c4MfgEepromRegIf();

    mupMfgEepromDrv = make_unique<MfgEeprom>(mspMfgEepromDrvRegIf.get(),
                                             mspFpgaPlMiscIf.get(),
                                             mRMutexMfgEepromWrPretect,
                                             cFpgaSkickMfgEepromSize);

    mspFpgaPlMdioIf = pFactory->CreateFpgaPlMdioRegIf();

    /*
     * TMP112 temperature sensors on PL
     */
    mupTopMezzTmp112 = make_unique<Tmp112>("Top Mezz Temperature Sensor",
                                            mspFpgaPlI2c1TempSensorIf.get());

    mupBottomMezzTmp112 = make_unique<Tmp112>("Bottom Mezz Temperature Sensor",
                                               mspFpgaPlI2c0TempSensorIf.get());

    mupInletTmp112 = make_unique<Tmp112>("Inlet Temperature Sensor",
                                          mspFpgaPlI2c4InletTempSensorIf.get());

    mupOutletTmp112 = make_unique<Tmp112>("Outlet Temperature Sensor",
                                           mspFpgaPlI2c4OutletTempSensorIf.get());

    mupOutletTmp112->SetTmpLowLimit(cOutLetTmpLLim);
    mupOutletTmp112->SetTmpHighLimit(cOutLetTmpHLim);

    /*
     * SAC bus on FPGA PL
     */

    mupSacModule = make_unique<SacModule>("SAC Module", mspFpgaPlRegIf.get());
    mupSacModule->SetSacModuleEnable(true, true);

    /*
     * RegIf for FPGA PS interface
     */
    mspFpgaPsI2c0PwrSeqIf = pFactory->CreateFpgaPsI2c0PwrSeqIf();
}

void BoardDriver::MonitorStatus()
{
    std::ostringstream  log;
    log << "Started status monitor thread ...";
    AddLog(__func__, __LINE__, log.str());
    INFN_LOG(SeverityLevel::info) << log.str();

    while (true)
    {
        // Monitor SRC bus RX clock error and re-enable RX
        mupSacModule->CheckRedoSacModuleRxEnable();

        // Check if cold restart DCO
        if (mColdRestartDcoDelaySec == 0)
        {
            int ret = ColdRestartDco();
            if (ret == 0)
            {
                mColdRestartDcoDelaySec = -1;
//                INFN_LOG(SeverityLevel::info) << " mColdRestartDcoDelaySec = " << mColdRestartDcoDelaySec;
            }
        }
        else if (mColdRestartDcoDelaySec > 0)
        {
//            INFN_LOG(SeverityLevel::info) << " mColdRestartDcoDelaySec = " << mColdRestartDcoDelaySec;
            mColdRestartDcoDelaySec--;
        }

        sleep(1);
    }
}

/*
 * Inlet temp: PL SKICK I2C
 */
float32 BoardDriver::GetInletTemperature()
{
    float32 tmp = 0;
    mupInletTmp112->GetTmperature(tmp);
    return tmp;
}

/*
 * Outlet temp: PL SKICK I2C
 */
float32 BoardDriver::GetOutletTemperature()
{
    float32 tmp = 0;
    mupOutletTmp112->GetTmperature(tmp);
    return tmp;
}

float32 BoardDriver::GetTopMezzTemperature()
{
    float32 tmp = 0;
    mupTopMezzTmp112->GetTmperature(tmp);
    return tmp;
}

float32 BoardDriver::GetBottomMezzTemperature()
{
    float32 tmp = 0;
    mupBottomMezzTmp112->GetTmperature(tmp);
    return tmp;
}

/*
 * Cold restart DCO
 */
int BoardDriver::ColdRestartDco()
{
    INFN_LOG(SeverityLevel::info) << " DCO cold restart ...";

    uint32 data = 0;
    try
    {
        mFpgaLatchSrcRegLock.lock();

        data = mspFpgaPlMiscIf->Read32(cFpgaLatchSrcReg);

        // Disable Power to DCO  and put it in reset...
        data &= ~(cFpgaLatchSrcReg_dco_reset_l_mask);
        data &= ~(cFpgaLatchSrcReg_dco_power_en_mask);

        mspFpgaPlMiscIf->Write32(cFpgaLatchSrcReg, data);
        mFpgaLatchSrcRegLock.unlock();
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Disable Power to DCO failed! Ex: " << e.GetError();
        return -1;
    }
    catch (...)
    {
        INFN_LOG(SeverityLevel::error) << "Disable Power to DCO caught exception!";
        return -1;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    try
    {
        mFpgaLatchSrcRegLock.lock();

        data = mspFpgaPlMiscIf->Read32(cFpgaLatchSrcReg);

        // Bring DCO out of reset and enable DCO power
        data |= cFpgaLatchSrcReg_dco_reset_l_mask;
        data |= cFpgaLatchSrcReg_dco_power_en_mask;

        mspFpgaPlMiscIf->Write32(cFpgaLatchSrcReg, data);
        mFpgaLatchSrcRegLock.unlock();
    }
    catch ( regIf::RegIfException &e )
    {
        INFN_LOG(SeverityLevel::error) << "Error: Bring DCO out of reset failed! Ex: " << e.GetError();
        return -1;
    }
    catch (...)
    {
        INFN_LOG(SeverityLevel::error) << "Bring DCO out of reset caught exception!";
        return -1;
    }

    return 0;
}

void BoardDriver::BoardDriver::AddLog(const std::string &func, uint32 line, const std::string &text)
{
    std::lock_guard<std::mutex> guard(mLogLock);

    ostringstream  os;
    os << "BoardDriver::" << func << ":" << line << ": " << text;

    if (mupLog)
    {
        mupLog->AddRecord( os.str().c_str() );
    }
}
