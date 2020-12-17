//-------------------------------------------------------------
//
// Copyright(c) 2020 Infinera Corporation. All right reserved.
//
//-------------------------------------------------------------

#ifndef CHM6_BOARD_MS_SRC_CHM6BOARDDRIVER_H_
#define CHM6_BOARD_MS_SRC_CHM6BOARDDRIVER_H_

#include <iostream>
#include <string>
#include <mutex>
#include <memory>

#include "board_defs.h"

#include "SimpleLog.h"

#include "RegIfFactory.h"
#include "MfgEeprom.h"
#include "board_init_util.h"
#include "board_common_driver.h"

#include "Tmp112.h"
#include "SacModule.h"

using namespace std;
using namespace boardMs;

class BoardDriver
{
public:

    BoardDriver();

    ~BoardDriver();

    /*
     * Read PFGA PL I2C Bus4 SKICK MFG MFG EEPROM
     * cFpgaI2cBus0Offset = 0x0F00;
     * CAV24C02 = 7�b1010100
     */
    void GetEqptInventory(Chm6EqptInventory& inv);

    /*
     * Run diagnostic tests
     */
    void RunDiag();

    /*
     * 3.3.1 LED Test
     * SW can force LED test via 0x00 CNTL_REG register
     * (Set Bit 5, FORCE_LAMP_TEST_EN to HIGH).
     * Command sent from Controller card to host board,
     * HB translates this signal to the two MZ cards;
     * then, all LEDs start to blink.
     *
     * 4.2.1 Register: 0x00 CNTL_REG
     * 5 RW FORCE_LAMP_TEST_EN 0x0
     * FORCE_LAMP_TEST_EN - SW Force Lamp Test, active HIGH
     */
    int LedLocationTest(bool doTest);

    int SetFruFaultLedState(LedStateType ledState);

    int GetFruFaultLedState(LedStateType& ledState);

    int SetFruActiveLedState(LedStateType ledState);

    int GetFruActiveLedState(LedStateType& ledState);

    /*
     * 2 LEDS (Y/G, RED) for each QSFP-28/QSFP-DD (Active is bi-color Y/G, LOS is red).
     */
    int SetMezzQsfpLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType ledState);

    int GetMezzQsfpLedState(QSFPPortId portId, QSFPLedType ledType, LedStateType& ledState);

    /*
     * 4 Line LEDs (2 LEDs per Line): RED � LOS, Y/G � Active LED
     */
    int SetMezzLineLedState(LineId lineId, LineLedType ledType, LedStateType ledState);

    int GetMezzLineLedState(LineId lineId, LineLedType ledType, LedStateType& ledState);

    /*
     * GX FRU ACTIVE LED "Flashing GREEN" shall be used when module is warm booting to align to Groove behavior.
     * GX FRU FAULT LED "Flashing RED" shall be used when module is cold booting.
     */
    void ResetFruLedsOnReboot(bool isColdBoot);

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
    int CheckLedLampLocTestState(bool& isLampTestOn, bool& isLocTestOn);

    /*
     * Set delay time before cold restart DCO
     */
    void SetColdRestartDcoDelay(uint32 delaySeconds);

    /*
     * Blue Gecko as slave to PS on PS I2C1
     * sends High level signal to the Power Sequencer (ADM1066) on PS I2C0
     */
    void PowerCycleAllFrus();

    /*
     * Broadcom�s BCM53161/BCM53162 - Ethernet 2.5GE switches
     * On power up via SPI2 interface by PS
     */
    void ConfigureBCMSwitch();

    /*
     * Broadcom�s BCM53161/BCM53162 - Ethernet 2.5GE switches
     * Reset via SPI2 interface by PS
     */
    void ResetBCMSwitch();

    /* Configure USB interface
     * eUSB 8GB flash module (INFN p/n 672-0017-001)
     * controlled by PS
     */
    void ConfigureUsbInterface(); // ?? How ??

    /*
     * Update HW related status/faults to FDR
     */
    void UpdateHwStatusToFDR(); // ?? Are these done by SW ??

    float32 GetPm(BoardPmId id);

    /*
     * Inlet temp: PL SKICK I2C
     */
    void SetInletTmpLowLimit(float llim);

    void SetInletTmpHighLimit(float hlim);

    /*
     * Outlet temp: PL SKICK I2C
     */
    void SetOutletTmpLowLimit(float llim);

    void SetOutletTmpHighLimit(float hlim);


    void SetTopMezzTmpLowLimit(float llim);

    void SetTopMezzTmpHighLimit(float hlim);


    void SetBottomMezzTmpLowLimit(float llim);

    void SetBottomMezzTmpHighLimit(float hlim);

    /*
     * Reset whole CHM6
     * Blue Gecko GMCU_PS_POR_L signal controls the Power-On-Reset (POR)
     * for the whole CHM6, including the ICE6 DCO and both MZs
     */
    void FruReset();

    /*
     * Reset flash
     * Blue Gecko GMCU_QSPI_RESET_L
     */
    void FlashReset();

    /*
     * Reset DCO
     * GMCU_ZYNQ_SRST_L
     */
    bool ResetDcoMezzCards();

    /*
     * Reset Mess cards by power off and release them from reset
     * Mezz PL I2C bus
     */
    bool ResetMessCards();

    /*
     * Set IDs from controller configuration
     */
    void SetSlotId(uint32 slotId);

    void SetSledId(uint32 sledId);

    bool SetRebootDco();

    bool SetRebootCpu();

    bool IsColdBoot();

    bool IsOnBench();

    bool IsChassisDiag();

    std::string GetFwVersion();

    // Set update FW
    void UpdateFw();

    // Start and stop device
    bool SetStart();

    bool SetStop();

    // Fault
    uint64 GetFault();

    void SetFault();

    void ClearFault();

    void UpdateFaultLed();

    /*
     * Update PM
     * No PM requested now
     */
    void UpdatePm();

    /*
     * Verify all interfaces are working � must do after warm reboot
     */
    int CheckDeviceHealth();

    /*
     * CLI commands to dump internal logs
     */
    void DumpLog(std::ostream &os);

    void DumpStatus(std::ostream &os, std::string cmd);

    void ResetLog( std::ostream &os );


    /*
     * CLI commands to access RegIf
     */
    uint32 FpgaRead(uint32 offset);

    void FpgaWrite(uint32 offset, uint32 data);

    /*
     * I2C bus2 bottom Mezz IO Expander read
     */
    uint8 PlI2c2IoxRead (uint32 offset);

    /*
     * I2C bus2 bottom Mezz IO Expander write
     */
    void PlI2c2IoxWrite (uint32 offset, uint8 data);

    /*
     * I2C bus3 top Mezz IO Expander read
     */
    uint8 PlI2c3IoxRead (uint32 offset);

    /*
     * I2C bus3 top Mezz IO Expander write
     */
    void PlI2c3IoxWrite (uint32 offset, uint8 data);

    /*
     * I2C bus2 bottom Mezz card Si5394 - ClockFanOut read
     */
    uint8 I2c2PllRead (uint32 offset);

    /*
     * I2C bus2 bottom Mezz card Si5394 - ClockFanOut write
     */
    void I2c2PllWrite (uint32 offset, uint8 data);

    /*
     * Bottom Mezz card Si5394 configure
     */
    void I2c2PllConfig ();

    /*
     * I2C bus3 top Mezz card Si5394 - ClockFanOut read
     */
    uint8 I2c3PllRead (uint32 offset);

    /*
     * I2C bus3 top Mezz card Si5394 - ClockFanOut write
     */
    void I2c3PllWrite (uint32 offset, uint8 data);

    /*
     * Top Mezz card Si5394 configure
     */
    void I2c3PllConfig ();

    uint16 MdioRead (uint8 bus_sel, uint16 mdio_addr, uint64 offset);

    void MdioWrite (uint8 bus_sel, uint16 mdio_addr,
                    uint64 offset, uint16 data);

    uint16 I2cRead8 (uint8 bus_sel, uint16 phy_addr, uint64 offset);

    void I2cWrite8 (uint8 bus_sel, uint16 phy_addr,
                   uint64 offset, uint16 data);

    uint16 I2cRead16 (uint8 bus_sel, uint16 phy_addr, uint64 offset);

    void I2cWrite16 (uint8 bus_sel, uint16 phy_addr,
                   uint64 offset, uint16 data);


    shared_ptr<BoardCommonDriver> GetBrdCmnDriver() { return mspBoardCmnDrv; }

    /*
     * CLI commands for MFG EEPROM
     */
    void ImportMfgEepromFromBinFile(std::ostream& out, std::string& filename);

    void ExportMfgEepromToBinFile(std::ostream& out, std::string& filename);

    void ReadFromMfgEeprom(std::ostream& out);

    void WriteToMfgEeprom(std::ostream& out);

    void DumpEepromBin(std::ostream& out);

    void DumpEepromFields(std::ostream& out, int level);

    void DumpEepromBinBufSrcName(std::ostream& out);

    /*
     * CLI commands for LED
     */
    void DumpLedRegs(std::ostream& out);

    void DumpLedStates(std::ostream& out);

    /*
     * CLI commands for host ADM1066 power sequencer
     *
     * Name     Address Value   Function
     * MANID    0xF4    0x41    Manufacturer ID for Analog Devices
     * REVID    0xF5    0x02    Silicon revision
     * MARK1    0xF6    0x00    Software brand
     * MARK2    0xF7    0x00    Software brand
     */
    void ReadManid(std::ostream& out);

    void ReadRevid(std::ostream& out);

    void ReadMark1(std::ostream& out);

    void ReadMark2(std::ostream& out);

    void ReadByte(std::ostream& out);

    void ReadByteData(std::ostream& out, uint8 command);

    void ReadWordData(std::ostream& out, uint8 command);

    void ReadBlockData(std::ostream& out, uint8 command);

    void WriteByte(std::ostream& out, uint8 value);

    void WriteByteData(std::ostream& out, uint8 command, uint8 value);

    void WriteWordData(std::ostream& out, uint8 command, uint16 value);

    /*
     * Host ADM1066 reversion @ EEPROM byte 0xf900
     */
    void DumpHostADM1066Revision(std::ostream& out);

    void DumpHostAdm1066Eeprom(std::ostream& out);

private:

    /*
     * Init CHM6 board after came up
     */
    int Initialize();

    /*
     * Create register interfaces
     */
    void CreateRegIf();

    /*
     * Period thread to monitor status
     */
    void MonitorStatus();

    float32 GetInletTemperature();

    float32 GetOutletTemperature();

    float32 GetTopMezzTemperature();

    float32 GetBottomMezzTemperature();

    /*
     * Cold restart DCO
     */
    int ColdRestartDco();

    void AddLog(const std::string &func, uint32 line, const std::string &text);

    shared_ptr<RegIf> mspFpgaPlRegIf;

    shared_ptr<FpgaMiscIf> mspFpgaPlMiscIf;
    /*
     * I2C[0] assigned to bottom MEZZ I2C[0]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c0RegIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c0VolSeqIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c0_3_3VPwrSupplyIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c0_0_8VPwrSupplyIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c0TempSensorIf;

    /*
     * I2C[1] assigned to top MEZZ I2C[1]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c1RegIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c1VolSeqIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c1_3_3VPwrSupplyIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c1_0_8VPwrSupplyIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c1TempSensorIf;

    /*
     * I2C[2] assigned to bottom MEZZ I2C[2]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c2RegIf;
    shared_ptr<FpgaI2cIf> mspBottomMzFPC402_1RegIf;
    shared_ptr<FpgaI2cIf> mspBottomMzFPC402_2RegIf;
    shared_ptr<FpgaI2cIf> mspBottomMzIoExpIf;
    shared_ptr<Si5394> mspBottomMzSi5394Drv;

    /*
     * I2C[3] assigned to top MEZZ I2C[3]
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c3RegIf;
    shared_ptr<FpgaI2cIf> mspTopMzFPC402_1RegIf;
    shared_ptr<FpgaI2cIf> mspTopMzFPC402_2RegIf;
    shared_ptr<FpgaI2cIf> mspTopMzIoExpIf;
    shared_ptr<Si5394> mspTopMzSi5394Drv;

    /*
     * I2C[4] assigned to SKICK I2C
     */
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c4RegIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c4InletTempSensorIf;
    shared_ptr<FpgaI2cIf> mspFpgaPlI2c4OutletTempSensorIf;
    shared_ptr<FpgaI2cIf> mspMfgEepromDrvRegIf;

    recursive_mutex mRMutexMfgEepromWrPretect;
    unique_ptr<MfgEeprom> mupMfgEepromDrv;

    /*
     * MDIO module contains a single driver (set of registers)
     * for up to 4 sets of MDIO interfaces or buses
     * CHM6 uses 2 sets of MDIO buses Currently
     */
    shared_ptr<FpgaMdioIf> mspFpgaPlMdioIf;

    /*
     * TMP112 temperature sensors on PL
     */
    unique_ptr<Tmp112> mupTopMezzTmp112;

    unique_ptr<Tmp112> mupBottomMezzTmp112;

    unique_ptr<Tmp112> mupInletTmp112;

    unique_ptr<Tmp112> mupOutletTmp112;

    /*
     * SAC bus on FPGA PL
     */

    unique_ptr<SacModule> mupSacModule;

    /*
     * RegIf for FPGA PS interface
     */
    shared_ptr<FpgaPsI2cIf> mspFpgaPsI2c0PwrSeqIf;

    Chm6EqptState mBoardState;

    shared_ptr<BoardInitUtil> mspBoardInitUtil;

    shared_ptr<BoardCommonDriver> mspBoardCmnDrv;

    mutable std::mutex  mFruActiveLedLock;

    mutable std::mutex  mFruFaultLedLock;

    mutable std::mutex  mQsfgLedLock;

    mutable std::mutex  mLineLedLock;

    sint32 mColdRestartDcoDelaySec;

    mutable std::mutex mFpgaLatchSrcRegLock;

    std::unique_ptr<SimpleLog::Log> mupLog;
    mutable std::mutex  mLogLock;
};

#endif /* CHM6_BOARD_MS_SRC_CHM6BOARDDRIVER_H_ */
