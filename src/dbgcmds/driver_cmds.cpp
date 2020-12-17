//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#include <cli/cli.h>
#include <boost/function.hpp>
#include <iomanip>
#include "driver_cmds.h"

using namespace cli;
using namespace boost;

DriverCmds::DriverCmds(BoardDriver& driver)
    : driver(driver)
{}

/*
 * CLI commands to dump internal logs
 */
void DriverCmds::DumpLog(std::ostream& out)
{
    driver.DumpLog(out);
}

void DriverCmds::DumpStatus(std::ostream& out, std::string instr)
{
    if (instr == std::string("tmp")
     || instr == std::string("tmp_all")
     || instr == std::string("led")
     || instr == std::string("sac"))
    {
        driver.DumpStatus(out, instr);
    }
    else
    {
        out << "Not supported command. Valid commands:" << std::endl
            << "\ttmp\n\ttmp_all\n\tled\n\tsac"<< std::endl;
    }
}

void DriverCmds::ResetLog(std::ostream& out)
{
    driver.ResetLog(out);
}

/*
 * CLI commands to access RegIf
 */
void DriverCmds::FpgaRead(std::ostream& out, std::string offsetStr)
{
    uint64 offset = strtol(offsetStr.c_str(), NULL, 16);

    uint32 data = driver.FpgaRead(offset);

    out << "FpgaRead: offset = 0x" << std::hex << offset
       << " data = 0x" << data << std::dec << std::endl;
}

void DriverCmds::FpgaWrite(std::ostream& out, std::string offset_str, std::string data_str)
{
    uint64 offset = strtol(offset_str.c_str(), NULL, 16);
    uint64 data   = strtol(data_str.c_str(), NULL, 16);

    driver.FpgaWrite(offset, data);

    out << "FpgaWrite: offset = 0x" << std::hex << offset
       << " data = 0x" << data << std::dec << std::endl;
}

/*
 * I2C bus2 bottom Mezz IO Expander read
 */
void DriverCmds::PlI2c2IoxRead (std::ostream& out, std::string strOffset)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);

    uint8 data = driver.PlI2c2IoxRead(offset);

    out << "Bottom Mezz IoxRead: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * I2C bus2 bottom Mezz IO Expander write
 */
void DriverCmds::PlI2c2IoxWrite (std::ostream& out, std::string strOffset, std::string strData)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);
    uint64 data = std::strtoul(strData.c_str(), NULL, 16);

    driver.PlI2c2IoxWrite(offset, data);

    out << "Bottom Mezz IoxWrite: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * I2C bus3 top Mezz IO Expander read
 */
void DriverCmds::PlI2c3IoxRead (std::ostream& out, std::string strOffset)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);

    uint8 data = driver.PlI2c3IoxRead(offset);

    out << "Top Mezz IoxRead: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * I2C bus3 top Mezz IO Expander write
 */
void DriverCmds::PlI2c3IoxWrite (std::ostream& out, std::string strOffset, std::string strData)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);
    uint64 data = std::strtoul(strData.c_str(), NULL, 16);

    driver.PlI2c3IoxWrite(offset, data);

    out << "Top Mezz IoxWrite: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * I2C bus2 bottom Mezz card Si5394 - ClockFanOut read
 */
void DriverCmds::I2c2PllRead (std::ostream& out, std::string strOffset)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);

    uint8 data = driver.I2c2PllRead(offset);

    out << "Bottom Mezz PllRead: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * I2C bus2 bottom Mezz card Si5394 - ClockFanOut write
 */
void DriverCmds::I2c2PllWrite (std::ostream& out, std::string strOffset, std::string strData)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);
    uint64 data = std::strtoul(strData.c_str(), NULL, 16);

    driver.I2c2PllWrite(offset, data);

    out << "Bottom Mezz PllWrite: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * Bottom Mezz card Si5394 configure
 */
void DriverCmds::I2c2PllConfig (std::ostream& out)
{
    driver.I2c2PllConfig();

    out << "Bottom Mezz PllConfig done. " << std::endl;
}

/*
 * I2C bus3 top Mezz card Si5394 - ClockFanOut read
 */
void DriverCmds::I2c3PllRead (std::ostream& out, std::string strOffset)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);

    uint8 data = driver.I2c3PllRead(offset);

    out << "Top Mezz PllRead: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * I2C bus3 top Mezz card Si5394 - ClockFanOut write
 */
void DriverCmds::I2c3PllWrite (std::ostream& out, std::string strOffset, std::string strData)
{
    uint64 offset = std::strtoul(strOffset.c_str(), NULL, 16);
    uint64 data = std::strtoul(strData.c_str(), NULL, 16);

    driver.I2c3PllWrite(offset, data);

    out << "Top Mezz PllWrite: offset = 0x" << std::hex << offset
       << " data = 0x" << (uint32)data << std::dec << std::endl;
}

/*
 * Top Mezz card Si5394 configure
 */
void DriverCmds::I2c3PllConfig (std::ostream& out)
{
    driver.I2c3PllConfig();

    out << "Top Mezz PllConfig done. " << std::endl;
}

void DriverCmds::MdioRead (std::ostream& out, std::string bus_str, std::string mdio_addr_str, std::string offset_str)
{
    uint64 bussel    = strtol(bus_str.c_str(), NULL, 10);
    uint64 mdio_addr = strtol(mdio_addr_str.c_str(), NULL, 16);
    uint64 offset    = strtol(offset_str.c_str(), NULL, 16);

    uint16 data = driver.MdioRead(bussel, mdio_addr, offset);

    out << "MdioRead: bus = " << bussel
        << " mdio_addr = 0x" << std::hex << mdio_addr
        << " offset = 0x" << offset
        << " data = 0x" << data << std::dec << std::endl;
}

void DriverCmds::MdioWrite (std::ostream& out, std::string bus_sel, std::string mdio_addr_str,
                            std::string offset_str, std::string data_str)

{
    uint64 bussel    = strtol(bus_sel.c_str(), NULL, 10);
    uint64 mdio_addr = strtol(mdio_addr_str.c_str(), NULL, 16);
    uint64 offset    = strtol(offset_str.c_str(), NULL, 16);
    uint64 data      = strtol(data_str.c_str(), NULL, 16);

    driver.MdioWrite(bussel, mdio_addr, offset, data);

    out << "MdioWrite: bus = " << bussel
        << " mdio_addr = 0x" << std::hex << mdio_addr
        << " offset = 0x" << offset
        << " data = 0x" << data << std::dec << std::endl;
}

void DriverCmds::I2cRead8 (std::ostream& out, std::string bus_str, std::string phy_addr_str, std::string offset_str)
{
    uint64 bussel    = strtol(bus_str.c_str(), NULL, 10);
    uint64 phy_addr = strtol(phy_addr_str.c_str(), NULL, 16);
    uint64 offset    = strtol(offset_str.c_str(), NULL, 16);

    uint16 data = driver.I2cRead8(bussel, phy_addr, offset);

    out << "I2cRead8: bus = " << bussel
        << " phy_addr = 0x" << std::hex << phy_addr
        << " offset = 0x" << offset
        << " data = 0x" << data << std::dec << std::endl;
}

void DriverCmds::I2cWrite8 (std::ostream& out, std::string bus_sel, std::string phy_addr_str,
                           std::string offset_str, std::string data_str)
{
    uint64 bussel    = strtol(bus_sel.c_str(), NULL, 10);
    uint64 phy_addr = strtol(phy_addr_str.c_str(), NULL, 16);
    uint64 offset    = strtol(offset_str.c_str(), NULL, 16);
    uint64 data      = strtol(data_str.c_str(), NULL, 16);

    driver.I2cWrite8(bussel, phy_addr, offset, data);

    out << "I2cWrite8: bus = " << bussel
        << " phy_addr = 0x" << std::hex << phy_addr
        << " offset = 0x" << offset
        << " data = 0x" << data << std::dec << std::endl;
}

void DriverCmds::I2cRead16 (std::ostream& out, std::string bus_str, std::string phy_addr_str, std::string offset_str)
{
    uint64 bussel    = strtol(bus_str.c_str(), NULL, 10);
    uint64 phy_addr = strtol(phy_addr_str.c_str(), NULL, 16);
    uint64 offset    = strtol(offset_str.c_str(), NULL, 16);

    uint16 data = driver.I2cRead16(bussel, phy_addr, offset);

    out << "I2cRead16: bus = " << bussel
        << " phy_addr = 0x" << std::hex << phy_addr
        << " offset = 0x" << offset
        << " data = 0x" << data << std::dec << std::endl;
}

void DriverCmds::I2cWrite16 (std::ostream& out, std::string bus_sel, std::string phy_addr_str,
                           std::string offset_str, std::string data_str)
{
    uint64 bussel    = strtol(bus_sel.c_str(), NULL, 10);
    uint64 phy_addr = strtol(phy_addr_str.c_str(), NULL, 16);
    uint64 offset    = strtol(offset_str.c_str(), NULL, 16);
    uint64 data      = strtol(data_str.c_str(), NULL, 16);

    driver.I2cWrite16(bussel, phy_addr, offset, data);

    out << "I2cWrite16: bus = " << bussel
        << " phy_addr = 0x" << std::hex << phy_addr
        << " offset = 0x" << offset
        << " data = 0x" << data << std::dec << std::endl;
}

/*
 * CLI commands for MFG EEPROM
 */

void DriverCmds::ImportMfgEepromFromBinFile(std::ostream& out, std::string filename)
{
    driver.ImportMfgEepromFromBinFile(out, filename);
}

void DriverCmds::ExportMfgEepromToBinFile(std::ostream& out, std::string filename)
{
    driver.ExportMfgEepromToBinFile(out, filename);
}

void DriverCmds::ReadFromMfgEeprom(std::ostream& out)
{
    driver.ReadFromMfgEeprom(out);
}

void DriverCmds::WriteToMfgEeprom(std::ostream& out)
{
    driver.WriteToMfgEeprom(out);
}

void DriverCmds::DumpEepromBin(std::ostream& out)
{
    driver.DumpEepromBin(out);
}

void DriverCmds::DumpEepromFields(std::ostream& out, int level)
{
    driver.DumpEepromFields(out, level);
}

void DriverCmds::DumpEepromBinBufSrcName(std::ostream& out)
{
    driver.DumpEepromBinBufSrcName(out);
}

/*
 * CLI commands for LED test
 */
void DriverCmds::SetFruActiveLedState(std::ostream& out, int ledState)
{
    if ( (ledState != GREEN)
      && (ledState != FLASHING_GREEN)
      && (ledState != YELLOW)
      && (ledState != FLASHING_YELLOW)
      && (ledState != OFF) )
    {
        out << "Invalid Fru ACTIVE LED state.\n"
                "Valid state: \n"
                "\t1 - LED OFF\n"
                "\t2 - LED YELLOW\n"
                "\t3 - LED FLASHING_YELLOW\n"
                "\t4 - LED GREEN\n"
                "\t5 - LED FLASHING_GREEN" << std::endl;
        return;
    }

    int ret = driver.SetFruActiveLedState(LedStateType(ledState));
    if (ret == 0)
    {
        out << "SetFruActiveLedState() is succeeded! " << std::endl;
        out << "Set Fru ACTIVE LED to " << boardMs::LedStateTypeToStr((LedStateType)ledState) << std::endl;
    }
    else
    {
        out << "SetFruActiveLedState() is failed! " << std::endl;
    }
}

void DriverCmds::GetFruActiveLedState(std::ostream& out)
{
    boardMs::LedStateType ledState;
    int ret = driver.GetFruActiveLedState(ledState);
    if (ret == 0)
    {
        out << "GetFruActiveLedState() is succeeded! " << std::endl;
        out << "Get Fru ACTIVE LED: " << boardMs::LedStateTypeToStr(ledState) << std::endl;
    }
    else
    {
        out << "GetFruActiveLedState() is failed! " << std::endl;
    }
}

void DriverCmds::SetFruFaultLedState(std::ostream& out, int ledState)
{
    if ( (ledState != RED)
      && (ledState != FLASHING_RED)
      && (ledState != OFF) )
    {
        out << "Invalid Fru FAULT LED state.\n"
                "Valid state: \n"
                        "\t1 - LED OFF\n"
                        "\t6 - LED RED\n"
                        "\t7 - LED FLASHING_RED\n" << std::endl;
        return;
    }

    int ret = driver.SetFruFaultLedState(LedStateType(ledState));
    if (ret == 0)
    {
        out << "SetFruFaultLedState() is succeeded! " << std::endl;
        out << "Set Fru FAULT LED to " << boardMs::LedStateTypeToStr((LedStateType)ledState) << std::endl;
    }
    else
    {
        out << "SetFruFaultLedState() is failed! " << std::endl;
    }
}

void DriverCmds::GetFruFaultLedState(std::ostream& out)
{
    boardMs::LedStateType ledState;
    int ret = driver.GetFruFaultLedState(ledState);
    if (ret == 0)
    {
        out << "GetFruFaultLedState() is succeeded! " << std::endl;
        out << "Get Fru FAULT LED: " << boardMs::LedStateTypeToStr(ledState) << std::endl;
    }
    else
    {
        out << "GetFruFaultLedState() is failed! " << std::endl;
    }
}

void DriverCmds::SetMezzQsfpLedState(std::ostream& out, int portId, int ledType, int ledState)
{
    if (portId >= boardMs::MAX_QSFP_NUM)
    {
        out << "Invalid Mezz QSFP ID. Valid input(0 based): 0 ~ 15" << std::endl;
        return;
    }

    if (ledType >= boardMs::MAX_QSFP_LED_TYPE)
    {
        out << "Invalid Mezz QSFP LED type. Valid input: 0 - Active LED; 1 - Los LED" << std::endl;
        return;
    }

    if ((QSFPLedType)ledType == QSFP_ACTIVE)
    {
        if ( (ledState != GREEN)
          && (ledState != FLASHING_GREEN)
          && (ledState != YELLOW)
          && (ledState != FLASHING_YELLOW)
          && (ledState != OFF) )
        {
            out << "Invalid Mezz QSFP ACTIVE LED state.\n"
                    "Valid state: \n"
                    "\t1 - LED OFF\n"
                    "\t2 - LED YELLOW\n"
                    "\t3 - LED FLASHING_YELLOW\n"
                    "\t4 - LED GREEN\n"
                    "\t5 - LED FLASHING_GREEN" << std::endl;
            return;
        }
    }

    else if ((QSFPLedType)ledType == QSFP_LOS)
    {
        if ( (ledState != RED)
          && (ledState != FLASHING_RED)
          && (ledState != OFF) )
        {
            out << "Invalid Mezz QSFP LOS LED state.\n"
                    "Valid state: \n"
                            "\t1 - LED OFF\n"
                            "\t6 - LED RED\n"
                            "\t7 - LED FLASHING_RED\n" << std::endl;
            return;
        }
    }

    int ret = driver.SetMezzQsfpLedState((QSFPPortId)portId, (QSFPLedType)ledType, (LedStateType)ledState);
    if (ret == 0)
    {
        out << "SetMezzQSFPLedState() is succeeded! " << std::endl;
        out << "Set LED on "
            << boardMs::QSFPPortIdToStr((QSFPPortId)portId)   << " "
            << boardMs::QSFPLedTypeToStr((QSFPLedType)ledType) << " "
            << " to " << boardMs::LedStateTypeToStr((LedStateType)ledState) << std::endl;
    }
    else
    {
        out << "SetMezzQSFPLedState() is failed! " << std::endl;
    }
}

void DriverCmds::GetMezzQsfpLedState(std::ostream& out, int portId, int ledType)
{
    if (portId >= boardMs::MAX_QSFP_NUM)
    {
        out << "Invalid Mezz QSFP num. Valid input(0 based): 0 ~ 7" << std::endl;
        return;
    }

    if (ledType >= boardMs::MAX_QSFP_LED_TYPE)
    {
        out << "Invalid Mezz QSFP LED type. Valid input: 0 - Active LED; 1 - Los LED" << std::endl;
        return;
    }

    boardMs::LedStateType state;

    int ret = driver.GetMezzQsfpLedState(QSFPPortId(portId), QSFPLedType(ledType), state);

    if (ret == 0)
    {
        out << "GetMezzQsfpLedState() is succeeded! " << std::endl;
        out << "Get QSFP LED on "
            << boardMs::QSFPPortIdToStr((QSFPPortId)portId)   << " "
            << boardMs::QSFPLedTypeToStr((QSFPLedType)ledType) << " "
            << " state is: " << boardMs::LedStateTypeToStr(state)
            << std::endl;
    }
    else
    {
        out << "GetMezzQsfpLedState() is failed! " << std::endl;
    }
}

void DriverCmds::SetMezzLineLedState(std::ostream& out, int lineId, int ledType, int ledState)
{
    if (lineId >= boardMs::MAX_LINE_NUM)
    {
        out << "Invalid Line ID num. Valid input: 0 - Line_1; 1 - Line_2" << std::endl;
        return;
    }

    if (ledType >= boardMs::MAX_LINE_LED_TYPE)
    {
        out << "Invalid Mezz LINE LED type. Valid input: 0 - Active LED; 1 - Los LED" << std::endl;
        return;
    }

    if ((LineLedType)ledType == LINE_ACTIVE)
    {
        if ( (ledState != GREEN)
          && (ledState != FLASHING_GREEN)
          && (ledState != YELLOW)
          && (ledState != FLASHING_YELLOW)
          && (ledState != OFF) )
        {
            out << "Invalid LINE ACTIVE LED state.\n"
                    "Valid state: \n"
                    "\t1 - LED OFF\n"
                    "\t2 - LED YELLOW\n"
                    "\t3 - LED FLASHING_YELLOW\n"
                    "\t4 - LED GREEN\n"
                    "\t5 - LED FLASHING_GREEN" << std::endl;
            return;
        }
    }

    else if ((LineLedType)ledType == LINE_LOS)
    {
        if ( (ledState != RED)
          && (ledState != FLASHING_RED)
          && (ledState != OFF) )
        {
            out << "Invalid Mezz LINE LOS LED state.\n"
                    "Valid state: \n"
                            "\t1 - LED OFF\n"
                            "\t6 - LED RED\n"
                            "\t7 - LED FLASHING_RED\n" << std::endl;
            return;
        }
    }

    int ret = driver.SetMezzLineLedState((LineId)lineId, (LineLedType)ledType, (LedStateType)ledState);
    if (ret == 0)
    {
        out << "SetMezzLineLedState() is succeeded! " << std::endl;
        out << "Set LINE LED on "
            << boardMs::LineIdToStr((LineId)lineId) << " "
            << boardMs::LINELedTypeToStr((LineLedType)ledType) << " "
            << " to " << boardMs::LedStateTypeToStr((LedStateType)ledState)
            << std::endl;
    }
    else
    {
        out << "SetMezzLineLedState() is failed! " << std::endl;
    }
}

void DriverCmds::GetMezzLineLedState(std::ostream& out, int lineId, int ledType)
{
    if (lineId >= boardMs::MAX_LINE_NUM)
    {
        out << "Invalid Line ID num. Valid input: 0 - Line_1; 1 - Line_2" << std::endl;
        return;
    }

    if (ledType >= boardMs::MAX_LINE_LED_TYPE)
    {
        out << "Invalid Mezz LINE LED type. Valid input: 0 - Active LED; 1 - Los LED" << std::endl;
        return;
    }

    boardMs::LedStateType state;

    int ret = driver.GetMezzLineLedState(LineId(lineId), LineLedType(ledType), state);
    if (ret == 0)
    {
        out << "GetMezzLineLedState() is succeeded! " << std::endl;
        out << "Get LINE LED on "
            << boardMs::LineIdToStr((LineId)lineId) << " "
            << boardMs::LINELedTypeToStr((LineLedType)ledType)   << " "
            << " state is: " << boardMs::LedStateTypeToStr(state)
            << std::endl;
    }
    else
    {
        out << "GetMezzLineLedState() is failed! " << std::endl;
    }
}

void DriverCmds::DoLedLocationTest(std::ostream& out, int doTest)
{
    driver.LedLocationTest((bool)doTest);

    out << "DoLedLocationTest: " << doTest << std::endl;
}

void DriverCmds::DumpLedRegs(std::ostream& out)
{
    driver.DumpLedRegs(out);
}

void DriverCmds::DumpLed(std::ostream& out)
{
    driver.DumpLedStates(out);
}

/*
 * CLI commands for FPGA, IO Exp and other devices
 */
void DriverCmds::DumpFpgaMiscStatusReg(std::ostream& out)
{
    uint32 regVal;
    int errCode = driver.GetBrdCmnDriver()->GetFpgaMiscStatus(regVal);

    if (errCode)
    {
        out << "Err " << errCode << " while accessing register" << endl;
        return;
    }

    out << "Fpga Misc Status: 0x" << std::hex << regVal << std::dec << endl;
}

void DriverCmds::DumpMezzIoExpInput(std::ostream& out, uint32 mezzId)
{
    if (mezzId > MEZZ_BRD_BTM)
    {
        out << "Invalid input. mezzId = " << mezzId << endl;
        out << "*** Usage:  mezzId: 0 for Top Mezz Board, 1 for Bottom Mezz Board" << endl;
        return;
    }

    uint32 regVal;
    int errCode = driver.GetBrdCmnDriver()->GetFpgaIoExpInput((mezzBoardIdType)mezzId, regVal);

    if (errCode)
    {
        out << "Err " << errCode << " while accessing register" << endl;
        return;
    }

    out << "Mezz IO Expander Input: 0x" << std::hex << regVal << std::dec << endl;
}

/*
 * CLI commands for host ADM1066 power sequencer
 */
void DriverCmds::ReadManid(std::ostream& out)
{
    driver.ReadManid(out);
}

void DriverCmds::ReadRevid(std::ostream& out)
{
    driver.ReadRevid(out);
}

void DriverCmds::ReadMark1(std::ostream& out)
{
    driver.ReadMark1(out);
}

void DriverCmds::ReadMark2(std::ostream& out)
{
    driver.ReadMark2(out);
}

void DriverCmds::ReadByte(std::ostream& out)
{
    driver.ReadByte(out);
}

void DriverCmds::ReadByteData(std::ostream& out, std::string command)
{
    uint8 commandCode = std::stoul(command, NULL, 16);

    driver.ReadByteData(out, commandCode);
}

void DriverCmds::ReadWordData(std::ostream& out, std::string command)
{
    uint8 commandCode = std::stoul(command, NULL, 16);

    driver.ReadWordData(out, commandCode);
}

void DriverCmds::ReadBlockData(std::ostream& out, std::string command)
{
    uint8 commandCode = std::stoul(command, NULL, 16);

    driver.ReadBlockData(out, commandCode);
}

void DriverCmds::WriteByte(std::ostream& out, std::string value)
{
    uint8 val = std::stoul(value, NULL, 16);

    driver.WriteByte(out, val);
}

void DriverCmds::WriteByteData(std::ostream& out, std::string command, std::string value)
{
    uint32 cmdCode = std::stoul(command, NULL, 16);
    uint32 val = std::stoul(value, NULL, 16);

    out << "cmdCode = " << command
        << " ;val = " << value
        << std::endl;

    driver.WriteByteData(out, cmdCode, val);
}

void DriverCmds::WriteWordData(std::ostream& out, std::string command, std::string value)
{
    uint8 commandCode = std::stoul(command, NULL, 16);

    uint16 val = std::stoul(value, NULL, 16);

    driver.WriteWordData(out, commandCode, val);
}

void DriverCmds::DumpHostADM1066Revision(std::ostream& out)
{
    driver.DumpHostADM1066Revision(out);
}

void DriverCmds::DumpHostAdm1066Eeprom(std::ostream& out)
{
    driver.DumpHostAdm1066Eeprom(out);
}

///////////////////////////////////////////////////////////////////////////////

boost::function< void (DriverCmds*, std::ostream&) > cmdDumpDriverLog = &DriverCmds::DumpLog;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdDumpDriverStatus = &DriverCmds::DumpStatus;
boost::function< void (DriverCmds*, std::ostream&) > cmdResetDriverLog = &DriverCmds::ResetLog;

// regif commands
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdFpgaRead = &DriverCmds::FpgaRead;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string) > cmdFpgaWrite = &DriverCmds::FpgaWrite;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdPlI2c2IoxRead = &DriverCmds::PlI2c2IoxRead;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string) > cmdPlI2c2IoxWrite = &DriverCmds::PlI2c2IoxWrite;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdPlI2c3IoxRead = &DriverCmds::PlI2c3IoxRead;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string) > cmdPlI2c3IoxWrite = &DriverCmds::PlI2c3IoxWrite;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdI2c2PllRead = &DriverCmds::I2c2PllRead;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string) > cmdI2c2PllWrite = &DriverCmds::I2c2PllWrite;
boost::function< void (DriverCmds*, std::ostream&) > cmdI2c2PllConfig = &DriverCmds::I2c2PllConfig;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdI2c3PllRead = &DriverCmds::I2c3PllRead;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string) > cmdI2c3PllWrite = &DriverCmds::I2c3PllWrite;
boost::function< void (DriverCmds*, std::ostream&) > cmdI2c3PllConfig = &DriverCmds::I2c3PllConfig;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string, std::string) > cmdMdioRead = &DriverCmds::MdioRead;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string, std::string, std::string) > cmdMdioWrite = &DriverCmds::MdioWrite;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string, std::string) > cmdI2cRead8 = &DriverCmds::I2cRead8;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string, std::string, std::string) > cmdI2cWrite8 = &DriverCmds::I2cWrite8;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string, std::string) > cmdI2cRead16 = &DriverCmds::I2cRead16;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string, std::string, std::string) > cmdI2cWrite16 = &DriverCmds::I2cWrite16;

// MFG EEPROM commands
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdImportMfgEepromFromBinFile = &DriverCmds::ImportMfgEepromFromBinFile;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdExportMfgEepromToBinFile = &DriverCmds::ExportMfgEepromToBinFile;
boost::function< void (DriverCmds*, std::ostream&) > cmdReadFromMfgEeprom = &DriverCmds::ReadFromMfgEeprom;
boost::function< void (DriverCmds*, std::ostream&) > cmdWriteToMfgEeprom = &DriverCmds::WriteToMfgEeprom;
boost::function< void (DriverCmds*, std::ostream&) > cmdDumpEepromBin = &DriverCmds::DumpEepromBin;
boost::function< void (DriverCmds*, std::ostream&, int) > cmdDumpEepromFields = &DriverCmds::DumpEepromFields;
boost::function< void (DriverCmds*, std::ostream&) > cmdDumpEepromBinBufSrcName = &DriverCmds::DumpEepromBinBufSrcName;

// LED test commands
boost::function< void (DriverCmds*, std::ostream&, int) > cmdSetFruActiveLedState = &DriverCmds::SetFruActiveLedState;
boost::function< void (DriverCmds*, std::ostream&) > cmdGetFruActiveLedState = &DriverCmds::GetFruActiveLedState;
boost::function< void (DriverCmds*, std::ostream&, int) > cmdSetFruFaultLedState = &DriverCmds::SetFruFaultLedState;
boost::function< void (DriverCmds*, std::ostream&) > cmdGetFruFaultLedState = &DriverCmds::GetFruFaultLedState;
boost::function< void (DriverCmds*, std::ostream&, int, int, int) > cmdSetMezzQsfpLedState = &DriverCmds::SetMezzQsfpLedState;
boost::function< void (DriverCmds*, std::ostream&, int, int) > cmdGetMezzQsfpLedState = &DriverCmds::GetMezzQsfpLedState;
boost::function< void (DriverCmds*, std::ostream&, int, int, int) > cmdSetMezzLineLedState = &DriverCmds::SetMezzLineLedState;
boost::function< void (DriverCmds*, std::ostream&, int, int) > cmdGetMezzLineLedState = &DriverCmds::GetMezzLineLedState;
boost::function< void (DriverCmds*, std::ostream&, int) > cmdDoLedLocationTest = &DriverCmds::DoLedLocationTest;
boost::function< void (DriverCmds*, std::ostream&) > cmdDumpLedRegs = &DriverCmds::DumpLedRegs;
boost::function< void (DriverCmds*, std::ostream&) > cmdDumpLed = &DriverCmds::DumpLed;

// FPGA Utility and Wrappers
boost::function< void (DriverCmds*, std::ostream&) > cmdDumpFpgaMiscStatusReg= &DriverCmds::DumpFpgaMiscStatusReg;
boost::function< void (DriverCmds*, std::ostream&, uint32) > cmdDumpMezzIoExpInput= &DriverCmds::DumpMezzIoExpInput;

// Host ADM1066 commands
boost::function< void (DriverCmds*, std::ostream&) > cmdReadManid = &DriverCmds::ReadManid;
boost::function< void (DriverCmds*, std::ostream&) > cmdReadRevid = &DriverCmds::ReadRevid;
boost::function< void (DriverCmds*, std::ostream&) > cmdReadMark1 = &DriverCmds::ReadMark1;
boost::function< void (DriverCmds*, std::ostream&) > cmdReadMark2 = &DriverCmds::ReadMark2;
boost::function< void (DriverCmds*, std::ostream&) > cmdReadByte = &DriverCmds::ReadByte;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdReadByteData = &DriverCmds::ReadByteData;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdReadWordData = &DriverCmds::ReadWordData;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdReadBlockData = &DriverCmds::ReadBlockData;
boost::function< void (DriverCmds*, std::ostream&, std::string) > cmdWriteByte = &DriverCmds::WriteByte;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string) > cmdWriteByteData = &DriverCmds::WriteByteData;
boost::function< void (DriverCmds*, std::ostream&, std::string, std::string) > cmdWriteWordData = &DriverCmds::WriteWordData;
boost::function< void (DriverCmds*, std::ostream&) > cmdDumpHostADM1066Revision = &DriverCmds::DumpHostADM1066Revision;
boost::function< void (DriverCmds*, std::ostream&) > cmdDumpHostAdm1066Eeprom = &DriverCmds::DumpHostAdm1066Eeprom;

void InsertDriverCmds(unique_ptr< Menu > & driverMenu, DriverCmds& driverCmds)
{
    driverMenu -> Insert(
            "log",
            [&](std::ostream& out){ cmdDumpDriverLog(&driverCmds, out); },
            "Dump driver log" );

    driverMenu -> Insert(
            "status",
            [&](std::ostream& out, std::string instr){ cmdDumpDriverStatus(&driverCmds, out, instr); },
            "Dump driver status\n"
            "\tValid commands:\n"
            "\t\"tmp\": dump temperatures\n"
            "\t\"tmp_all\": dump all temperature related data\n"
            "\t\"led\": dump all LED states\n"
            "\t\"sac\": dump SAC bus states\n",
            {"command"} );

    driverMenu -> Insert(
            "resetlog",
            [&](std::ostream& out){ cmdResetDriverLog(&driverCmds, out); },
            "Reset driver log" );
}

void InsertRegIfCmds(unique_ptr< Menu > & subMenu_regif, DriverCmds& driverCmds)
{
    subMenu_regif -> Insert(
            "read",
            [&](std::ostream& out, std::string offset_str)
            {
                cmdFpgaRead(&driverCmds, out, offset_str);
            },
            "Read FPGA register", {"0xOffset"} );

    subMenu_regif -> Insert(
                "write",
                [&](std::ostream& out, std::string offset_str, std::string data_str)
                {
                    cmdFpgaWrite(&driverCmds, out, offset_str, data_str);
                },
                "Write FPGA register", {"0xoffset", "0xdata"} );

    subMenu_regif -> Insert(
                "PlI2c2IoxRead",
                [&](std::ostream& out, std::string offset)
                {
                    cmdPlI2c2IoxRead(&driverCmds, out, offset);
                },
                "Bottom Mezz card IO expender register read", {"0xoffset"} );

    subMenu_regif -> Insert(
                "PlI2c2IoxWrite",
                [&](std::ostream& out, std::string offset, std::string data)
                {
                    cmdPlI2c2IoxWrite(&driverCmds, out, offset, data);
                },
                "Bottom Mezz card IO expender register write", {"offset, data"} );

    subMenu_regif -> Insert(
                "PlI2c3IoxRead",
                [&](std::ostream& out, std::string offset)
                {
                    cmdPlI2c3IoxRead(&driverCmds, out, offset);
                },
                "Top Mezz card IO expender register read", {"0xoffset"} );

    subMenu_regif -> Insert(
                "PlI2c3IoxWrite",
                [&](std::ostream& out, std::string offset, std::string data)
                {
                    cmdPlI2c3IoxWrite(&driverCmds, out, offset, data);
                },
                "Top Mezz card IO expender register write", {"offset, data"} );

    subMenu_regif -> Insert(
                "I2c2PllRead",
                [&](std::ostream& out, std::string offset)
                {
                    cmdI2c2PllRead(&driverCmds, out, offset);
                },
                "Bottom Mezz card Si5394 register read", {"offset"} );

    subMenu_regif -> Insert(
                "I2c2PllWrite",
                [&](std::ostream& out, std::string offset, std::string data)
                {
                    cmdI2c2PllWrite(&driverCmds, out, offset, data);
                },
                "Bottom Mezz card Si5394 register write", {"offset,data"} );

    subMenu_regif -> Insert(
                "I2c2PllConfig",
                [&](std::ostream& out)
                {
                    cmdI2c2PllConfig(&driverCmds, out);
                },
                "Bottom Mezz card Si5394 configure");

    subMenu_regif -> Insert(
                "I2c3PllRead",
                [&](std::ostream& out, std::string offset)
                {
                    cmdI2c3PllRead(&driverCmds, out, offset);
                },
                "Top Mezz card Si5394 register read", {"offset"} );

    subMenu_regif -> Insert(
                "I2c3PllWrite",
                [&](std::ostream& out, std::string offset, std::string data)
                {
                    cmdI2c3PllWrite(&driverCmds, out, offset, data);
                },
                "Top Mezz card Si5394 register write", {"offset,data"} );

    subMenu_regif -> Insert(
                "I2c3PllConfig",
                [&](std::ostream& out)
                {
                    cmdI2c3PllConfig(&driverCmds, out);
                },
                "Top Mezz card Si5394 configure");

    subMenu_regif -> Insert(
            "mdioRd",
            [&](std::ostream& out, std::string bussel_str, std::string mdio_addr_str, std::string offset_str)
            {
                cmdMdioRead(&driverCmds, out, bussel_str, mdio_addr_str, offset_str);
            },
            "Read Gearbox MDIO register", {"bus sel", "0xMdio_addr", "0xoffset"} );

    subMenu_regif -> Insert(
            "mdioWr",
            [&](std::ostream& out, std::string bus_str, std::string mdio_addr_str,
               std::string offset_str, std::string data_str)
            {
                cmdMdioWrite(&driverCmds, out, bus_str, mdio_addr_str, offset_str, data_str);
            },
            "Write Gearbox MDIO register", {"bus sel", "0xMdio_addr", "0xoffset", "0xdata"} );

    subMenu_regif -> Insert(
            "i2cRd8",
            [&](std::ostream& out, std::string bus_str, std::string phy_addr_str, std::string offset_str)
            {

                cmdI2cRead8(&driverCmds, out, bus_str, phy_addr_str, offset_str);
            },
            "Read 8 bits from I2C register", {"bus sel", "0xphy_addr", "0xoffset"} );

    subMenu_regif -> Insert(
            "i2cWr8",
            [&](std::ostream& out, std::string bus_str, std::string phy_addr_str,
               std::string offset_str, std::string data_str)
            {
                cmdI2cWrite8(&driverCmds, out, bus_str, phy_addr_str, offset_str, data_str);
            },
            "Write 8 bits to I2C register", {"bus sel", "0xphy_addr", "0xoffset", "0xdata"} );

    subMenu_regif -> Insert(
            "i2cRd16",
            [&](std::ostream& out, std::string bus_str, std::string phy_addr_str, std::string offset_str)
            {

                cmdI2cRead16(&driverCmds, out, bus_str, phy_addr_str, offset_str);
            },
            "Read 16 bits from I2C register", {"bus sel", "0xphy_addr", "0xoffset"} );

    subMenu_regif -> Insert(
            "i2cWr16",
            [&](std::ostream& out, std::string bus_str, std::string phy_addr_str,
               std::string offset_str, std::string data_str)
            {
                cmdI2cWrite16(&driverCmds, out, bus_str, phy_addr_str, offset_str, data_str);
            },
            "Write 16 bits to I2C register", {"bus sel", "0xphy_addr", "0xoffset", "0xdata"} );
}

void InsertMfgEepromCmds(unique_ptr< Menu > & subMenu_eeprom, DriverCmds& driverCmds)
{
    subMenu_eeprom -> Insert(
            "import",
            [&](std::ostream& out, std::string filename)
            {
                cmdImportMfgEepromFromBinFile(&driverCmds, out, filename);
            },
            "import EEPROM bin file to buffer", {"filename"} );

    subMenu_eeprom -> Insert(
            "export",
            [&](std::ostream& out, std::string filename)
            {
                cmdExportMfgEepromToBinFile(&driverCmds, out, filename);
            },
            "export EEPROM bin buffer to file", {"filename"} );

    subMenu_eeprom -> Insert(
            "read",
            [&](std::ostream& out)
            {
                cmdReadFromMfgEeprom(&driverCmds, out);
            },
            "read from EEPROM into buffer" );

    subMenu_eeprom -> Insert(
            "write",
            [&](std::ostream& out)
            {
                cmdWriteToMfgEeprom(&driverCmds, out);
            },
            "write from buffer into EEPROM" );

    subMenu_eeprom -> Insert(
            "dump_bin",
            [&](std::ostream& out)
            {
                cmdDumpEepromBin(&driverCmds, out);
            },
            "raw dump of buffer" );

    subMenu_eeprom -> Insert(
            "dump_fields",
            [&](std::ostream& out, int level)
            {
                cmdDumpEepromFields(&driverCmds, out, level);
            },
            "dump parsed fields of buffer", {"level"} );

    subMenu_eeprom -> Insert(
            "buffer_name",
            [&](std::ostream& out)
            {
                cmdDumpEepromBinBufSrcName(&driverCmds, out);
            },
            "dump name of buffer: EEPROM or imported file" );
}

void InsertLedCmds(unique_ptr< Menu > & subMenu_led, DriverCmds& driverCmds)
{
    subMenu_led -> Insert(
            "set_fru_active_led",
            [&](std::ostream& out, int ledType)
            {
                cmdSetFruActiveLedState(&driverCmds, out, ledType);
            },
            "Set Fru ACTIVE LED state.\n"
            "\tValid Active ledState:\n"
                    "\t\t1 - LED OFF\n"
                    "\t\t2 - LED YELLOW\n"
                    "\t\t3 - LED FLASHING_YELLOW\n"
                    "\t\t4 - LED GREEN\n"
                    "\t\t5 - LED FLASHING_GREEN\n",
            {"ledType"} );

    subMenu_led -> Insert(
            "get_fru_active_led",
            [&](std::ostream& out)
            {
                cmdGetFruActiveLedState(&driverCmds, out);
            },
            "Get Fru ACTIVE LED state.\n");

    subMenu_led -> Insert(
            "set_fru_fault_led",
            [&](std::ostream& out, int ledType)
            {
                cmdSetFruFaultLedState(&driverCmds, out, ledType);
            },
            "Set Fru FAULT LED state.\n"
            "\tValid Los ledState: \n"
                    "\t\t1 - LED OFF\n"
                    "\t\t6 - LED RED\n"
                    "\t\t7 - LED FLASHING_RED\n",
            {"ledType"} );

    subMenu_led -> Insert(
            "get_fru_fault_led",
            [&](std::ostream& out)
            {
                cmdGetFruFaultLedState(&driverCmds, out);
            },
            "Get Fru FAULT LED state.\n");

    subMenu_led -> Insert(
            "set_qsfp_led",
            [&](std::ostream& out, int portId, int ledType, int ledState)
            {
                cmdSetMezzQsfpLedState(&driverCmds, out, portId, ledType, ledState);
            },
            "Set Mezzanine card's QSFP LED state.\n"
            "\tValid \"portId\"(0 based): 0 ~ 15\n"
            "\tValid \"ledType\": 0 - Active LED; 1 - Los LED\n"
            "\tValid Active ledState:\n"
                    "\t\t1 - LED OFF\n"
                    "\t\t2 - LED YELLOW\n"
                    "\t\t3 - LED FLASHING_YELLOW\n"
                    "\t\t4 - LED GREEN\n"
                    "\t\t5 - LED FLASHING_GREEN\n"
            "\tValid Los ledState: \n"
                    "\t\t1 - LED OFF\n"
                    "\t\t6 - LED RED\n"
                    "\t\t7 - LED FLASHING_RED\n",
            {"portId", "ledType", "ledState"} );

    subMenu_led -> Insert(
            "get_qsfp_led",
            [&](std::ostream& out, int portId, int ledType)
            {
                cmdGetMezzQsfpLedState(&driverCmds, out, portId, ledType);
            },
            "Get Mezzanine card's QSFP LED state.\n"
            "\tValid \"portId\"(0 based): 0 ~ 15\n"
            "\tValid \"ledType\": 0 - Active LED; 1 - Los LED\n",
            {"portId", "ledType"} );

    subMenu_led -> Insert(
            "set_line_led",
            [&](std::ostream& out, int lineId, int ledType, int ledState)
            {
                cmdSetMezzLineLedState(&driverCmds, out, lineId, ledType, ledState);
            },
            "Set Mezzanine card's LINE LED state.\n"
            "\tValid \"lineId\": 0 - LINE_1; 1 - LINE_2.\n"
            "\tValid \"ledType\": 0 - Active LED; 1 - Los LED\n"
            "\tValid Active ledState:\n"
                    "\t\t1 - LED OFF\n"
                    "\t\t2 - LED YELLOW\n"
                    "\t\t3 - LED FLASHING_YELLOW\n"
                    "\t\t4 - LED GREEN\n"
                    "\t\t5 - LED FLASHING_GREEN\n"
            "\tValid Los ledState: \n"
                    "\t\t1 - LED OFF\n"
                    "\t\t6 - LED RED\n"
                    "\t\t7 - LED FLASHING_RED\n",
            {"lineId", "ledType", "ledState"} );

    subMenu_led -> Insert(
            "get_line_led",
            [&](std::ostream& out, int lineId, int ledType)
            {
                cmdGetMezzLineLedState(&driverCmds, out, lineId, ledType);
            },
            "Get Mezzanine card's LINE LED state.\n"
            "\tValid \"lineId\": 0 - LINE_1; 1 - LINE_2.\n"
            "\tValid \"ledType\": 0 - Active LED; 1 - Los LED\n",
            {"lineId", "ledType"} );

    subMenu_led -> Insert(
            "location_test",
            [&](std::ostream& out, int doTest)
            {
                cmdDoLedLocationTest(&driverCmds, out, doTest);
            },
            "Start or stop LED location test.\n"
            "\tValid \"doTest\": 0 - stop test; 1 or bigger - start test.\n",
            {"doTest"} );

    subMenu_led -> Insert(
            "dump_led_regs",
            [&](std::ostream& out){ cmdDumpLedRegs(&driverCmds, out); },
            "Dump LED registers" );

    subMenu_led -> Insert(
            "dump_led",
            [&](std::ostream& out){ cmdDumpLed(&driverCmds, out); },
            "Dump LED states" );
}

void InsertFpgaUtilCmds(unique_ptr< Menu > & subMenu_util, DriverCmds& driverCmds)
{
    subMenu_util -> Insert(
            "fpga_misc_status",
            [&](std::ostream& out)
            {
                cmdDumpFpgaMiscStatusReg(&driverCmds, out);
            },
            "Dump Fpga Misc Status Reg");

    subMenu_util -> Insert(
            "fpga_mezz_ioexp_in",
            [&](std::ostream& out, uint32 boardId)
            {
                cmdDumpMezzIoExpInput(&driverCmds, out, boardId);
            },
            "Dump Fpga Mezzanine Io Expander Inputs");
}

void InsertAdm1066Cmds(unique_ptr< Menu > & subMenu_adm1066, DriverCmds& driverCmds)
{
    subMenu_adm1066 -> Insert(
            "dump_manid",
            [&](std::ostream& out)
            {
                cmdReadManid(&driverCmds, out);
            },
            "dump host board ADM1066 register MANID. Expected value: 0x41" );

    subMenu_adm1066 -> Insert(
            "dump_revid",
            [&](std::ostream& out)
            {
                cmdReadRevid(&driverCmds, out);
            },
            "dump host board ADM1066 register REVID. Expected value: 0x02" );

    subMenu_adm1066 -> Insert(
            "dump_mark1",
            [&](std::ostream& out)
            {
                cmdReadMark1(&driverCmds, out);
            },
            "dump host board ADM1066 register MARK1. Expected value: 0x00" );

    subMenu_adm1066 -> Insert(
            "dump_mark2",
            [&](std::ostream& out)
            {
                cmdReadMark2(&driverCmds, out);
            },
            "dump host board ADM1066 register MARK2. Expected value: 0x00" );

    subMenu_adm1066 -> Insert(
            "read_byte",
            [&](std::ostream& out)
            {
                cmdReadByte(&driverCmds, out);
            },
            "read byte of host board ADM1066" );

    subMenu_adm1066 -> Insert(
            "read_byte_data",
            [&](std::ostream& out, std::string command)
            {
                cmdReadByteData(&driverCmds, out, command);
            },
            "read byte data of host board ADM1066",
            {"command code(hex)"} );

    subMenu_adm1066 -> Insert(
            "read_word_data",
            [&](std::ostream& out, std::string command)
            {
                cmdReadWordData(&driverCmds, out, command);
            },
            "read word data of host board ADM1066",
            {"command code(hex)"} );

    subMenu_adm1066 -> Insert(
            "read_block_data",
            [&](std::ostream& out, std::string command)
            {
                cmdReadBlockData(&driverCmds, out, command);
            },
            "read block data of host board ADM1066",
            {"command code(hex)"} );

    subMenu_adm1066 -> Insert(
            "write_byte",
            [&](std::ostream& out, std::string value)
            {
                cmdWriteByte(&driverCmds, out, value);
            },
            "write byte to host board ADM1066",
            {"value(hex)"} );

    subMenu_adm1066 -> Insert(
            "write_byte_data",
            [&](std::ostream& out, std::string command, std::string value)
            {
                cmdWriteByteData(&driverCmds, out, command, value);
            },
            "write byte data to host board ADM1066",
            {"command code(hex)", "value(hex)"} );

    subMenu_adm1066 -> Insert(
            "write_word_data",
            [&](std::ostream& out, std::string command, std::string value)
            {
                cmdWriteWordData(&driverCmds, out, command, value);
            },
            "write word data to host board ADM1066",
            {"command code(hex)", "value(hex)"} );

    subMenu_adm1066 -> Insert(
            "dump_host_adm1066_version",
            [&](std::ostream& out)
            {
                cmdDumpHostADM1066Revision(&driverCmds, out);
            },
            "dump host board ADM1066 version" );

    subMenu_adm1066 -> Insert(
            "dump_host_adm1066_eeprom",
            [&](std::ostream& out)
            {
                cmdDumpHostAdm1066Eeprom(&driverCmds, out);
            },
            "dump host board ADM1066 EEPROM 64 bytes" );
}
