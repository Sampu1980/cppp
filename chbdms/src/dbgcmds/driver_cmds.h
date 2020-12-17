//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_TOM_MS_SRC_DBGCMDS_DRIVERCMDS_H_
#define CHM6_TOM_MS_SRC_DBGCMDS_DRIVERCMDS_H_

#include "board_driver.h"

class DriverCmds
{
public:
    DriverCmds(BoardDriver& driver);
    ~DriverCmds() {}

    /*
     * CLI commands to dump internal logs
     */
    void DumpLog(std::ostream& out);

    void DumpStatus(std::ostream& out, std::string instr);

    void ResetLog(std::ostream& out);

    /*
     * CLI commands to access RegIf
     */
    void FpgaRead(std::ostream& out, std::string offsetStr);

    void FpgaWrite(std::ostream& out, std::string offset_str, std::string data_str);

    /*
     * I2C bus2 bottom Mezz IO Expander read
     */
    void PlI2c2IoxRead (std::ostream& out, std::string strOffset);

    /*
     * I2C bus2 bottom Mezz IO Expander write
     */
    void PlI2c2IoxWrite (std::ostream& out, std::string strOffset, std::string strData);

    /*
     * I2C bus3 top Mezz IO Expander read
     */
    void PlI2c3IoxRead (std::ostream& out, std::string strOffset);

    /*
     * I2C bus3 top Mezz IO Expander write
     */
    void PlI2c3IoxWrite (std::ostream& out, std::string strOffset, std::string strData);

    /*
     * I2C bus2 bottom Mezz card Si5394 - ClockFanOut read
     */
    void I2c2PllRead (std::ostream& out, std::string strOffset);

    /*
     * I2C bus2 bottom Mezz card Si5394 - ClockFanOut write
     */
    void I2c2PllWrite (std::ostream& out, std::string strOffset, std::string strData);

    /*
     * Bottom Mezz card Si5394 configure
     */
    void I2c2PllConfig (std::ostream& out);

    /*
     * I2C bus3 top Mezz card Si5394 - ClockFanOut read
     */
    void I2c3PllRead (std::ostream& out, std::string strOffset);

    /*
     * I2C bus3 top Mezz card Si5394 - ClockFanOut write
     */
    void I2c3PllWrite (std::ostream& out, std::string strOffset, std::string strData);

    /*
     * Top Mezz card Si5394 configure
     */
    void I2c3PllConfig (std::ostream& out);

    void MdioRead (std::ostream& out, std::string bus_sel, std::string mdio_addr, std::string offset_str);

    void MdioWrite (std::ostream& out, std::string bus_sel, std::string mdio_addr,
            std::string offset, std::string data);

    void I2cRead8 (std::ostream& out, std::string bus_sel, std::string phy_addr, std::string offset);

    void I2cWrite8 (std::ostream& out, std::string bus_sel, std::string phy_addr,
            std::string offset, std::string data);

    void I2cRead16 (std::ostream& out, std::string bus_sel, std::string phy_addr, std::string offset);

    void I2cWrite16 (std::ostream& out, std::string bus_sel, std::string phy_addr,
            std::string offset, std::string data);

    /*
     * CLI commands for MFG EEPROM
     */

    void ImportMfgEepromFromBinFile(std::ostream& out, std::string filename);

    void ExportMfgEepromToBinFile(std::ostream& out, std::string filename);

    void ReadFromMfgEeprom(std::ostream& out);

    void WriteToMfgEeprom(std::ostream& out);

    void DumpEepromBin(std::ostream& out);

    void DumpEepromFields(std::ostream& out, int level=0);

    void DumpEepromBinBufSrcName(std::ostream& out);

    /*
     * CLI commands for LED test
     */
    void SetFruActiveLedState(std::ostream& out, int ledState);

    void GetFruActiveLedState(std::ostream& out);

    void SetFruFaultLedState(std::ostream& out, int ledState);

    void GetFruFaultLedState(std::ostream& out);

    void SetMezzQsfpLedState(std::ostream& out, int portId, int ledType, int ledState);

    void GetMezzQsfpLedState(std::ostream& out, int portId, int ledType);

    void SetMezzLineLedState(std::ostream& out, int lindId, int ledType, int ledState);

    void GetMezzLineLedState(std::ostream& out, int lindId, int ledType);

    void DoLedLocationTest(std::ostream& out, int doTest);

    void DumpLedRegs(std::ostream& out);

    void DumpLed(std::ostream& out);

    /*
     * CLI commands for FPGA, IO Exp and other devices
     */
    void DumpFpgaMiscStatusReg(std::ostream& out);
    void DumpMezzIoExpInput(std::ostream& out, uint32 mezzId);

    /*
     * CLI commands for host ADM1066 power sequencer
     */
    void ReadManid(std::ostream& out);

    void ReadRevid(std::ostream& out);

    void ReadMark1(std::ostream& out);

    void ReadMark2(std::ostream& out);

    void ReadByte(std::ostream& out);

    void ReadByteData(std::ostream& out, std::string command);

    void ReadWordData(std::ostream& out, std::string command);

    void ReadBlockData(std::ostream& out, std::string command);

    void WriteByte(std::ostream& out, std::string value);

    void WriteByteData(std::ostream& out, std::string command, std::string value);

    void WriteWordData(std::ostream& out, std::string command, std::string value);

    void DumpHostADM1066Revision(std::ostream& out);

    void DumpHostAdm1066Eeprom(std::ostream& out);

private:
    BoardDriver& driver;
};

#endif /* CHM6_TOM_MS_SRC_DBGCMDS_DRIVERCMDS_H_ */
