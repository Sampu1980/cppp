/*
 * Copyright (c) 2020 Infinera
 *
 * board_fault_defs.h
 *
 *  Created on: Aug 12, 2020
 */

#ifndef CHM6_BOARD_MS_SRC_COMMON_BOARD_FAULT_DEFS_H_
#define CHM6_BOARD_MS_SRC_COMMON_BOARD_FAULT_DEFS_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/ptr_container/ptr_vector.hpp>

#include "types.h"
#include "board_common_driver.h"    // alarm reg access
#include "RegIfException.h"

namespace boardMs
{
// Board fault data structures
typedef enum faultConditionType
{
    FAULT_UNKNOWN = 0,
    FAULT_SET,
    FAULT_CLEAR,
    MAX_FAULT_CON_NUM
}faultConditionType;

typedef enum BoardFaultId
{
    BOARD_FAULT_INVALID = -1,
    SAC_BUS_FAIL,
    DCO_ZYNQ_PS_LINK_DOWN,
    DCO_ZYNQ_PL_LINK_DOWN,
    DCO_NXP_PL_LINK_DOWN,
    DCO_ZYNQ_PS_LINK_CRC_ERRORS,
    DCO_ZYNQ_PL_LINK_CRC_ERRORS,
    DCO_NXP_PL_LINK_CRC_ERRORS,
    HOST_OCXO_FREQ_OOR,
    HOST_HS_FAIL,
    HOST_12V_UV,
    HOST_12V_OV,
    HOST_5V_UV,
    HOST_5V_OV,
    HOST_3_3V_UV,
    HOST_3_3V_OV,
    HOST_1_8V_UV,
    HOST_1_8V_OV,
    HOST_1_5V_UV,
    HOST_1_5V_OV,
    HOST_P10V_UV,
    HOST_P10V_OV,
    HOST_P1_0V_UV,
    HOST_P1_0V_OV,
    HOST_P0_85V_UV,
    HOST_P0_85V_OV,
    HOST_ZYNQ_PSPLL_1_2V_UV,
    HOST_ZYNQ_PSPLL_1_2V_OV,
    HOST_ZYNQ_AVCC_UV,
    HOST_ZYNQ_AVCC_OV,
    HOST_ZYNQ_AVTT_UV,
    HOST_ZYNQ_AVTT_OV,
    EUSB_ACCESS_FAIL,
    I2C_GMCU_ACCESS_FAIL,
    DDR_MEMHOT_FAIL,
    FPGA_LINK_DOWN,
    FPGA_PL_ACCESS_FAIL,
    INLET_TEMP_OOR,
    BOARD_TEMP_OORH,
    HOST_PWR_SEQ_ACCESS_FAIL,
    HOST_HS_ACCESS_FAIL,
    INLET_TEMP_ACCESS_FAIL,
    BOARD_TEMP_ACCESS_FAIL,
    MFG_EEPROM_ACCESS_FAIL,
    CAL_EEPROM_ACCESS_FAIL,

    TMZ_DSPLL_OOL,           // Top Mezz card faults
    TMZ_OCXO_LOS,
    TMZ_I2C_CLKGEN_I2C_FAIL,
    TMZ_TMP_OORH,
    TMZ_TMP_OORL,
    TMZ_TMP_I2C_FAIL,
    TMZ_3_3V_TMP_OORH,
    TMZ_3_3V_TMP_OORL,
    TMZ_3_3V_I2C_FAIL,
    TMZ_3_3V_VIN_OORL,
    TMZ_3_3V_VOUT_OOR,
    TMZ_1_8V_TMP_OORH,
    TMZ_1_8V_TMP_OORL,
    TMZ_1_8V_I2C_FAIL,
    TMZ_0_8V_VIN_OORL,
    TMZ_0_8V_VOUT_OOR,
    TMZ_TXDRV_VOUT_OOR,
    TMZ_H_TXDRV_VOUT_OOR,
    TMZ_AVS1_TMP_OORH,
    TMZ_AVS2_TMP_OORH,
    TMZ_AVS3_TMP_OORH,
    TMZ_AVS1_OC,
    TMZ_AVS2_OC,
    TMZ_AVS3_OC,
    TMZ_AVS1_ACCESS_FAIL,
    TMZ_AVS2_ACCESS_FAIL,
    TMZ_AVS3_ACCESS_FAIL,
    TMZ_AVS1_VIN_OORL,
    TMZ_AVS2_VIN_OORL,
    TMZ_AVS3_VIN_OORL,
    TMZ_AVS1_VOUT_OOR,
    TMZ_AVS2_VOUT_OOR,
    TMZ_AVS3_VOUT_OOR,
    TMZ_RETIMER1_FW_FAIL,
    TMZ_RETIMER2_FW_FAIL,
    TMZ_RETIMER3_FW_FAIL,
    TMZ_RETIMER1_AVS_FAIL,
    TMZ_RETIMER2_AVS_FAIL,
    TMZ_RETIMER3_AVS_FAIL,
    TMZ_RETIMER1_INGRESS_DCO_LOS_LOL,
    TMZ_RETIMER2_INGRESS_DCO_LOS_LOL,
    TMZ_RETIMER3_INGRESS_DCO_LOS_LOL,
    TMZ_RETIMER1_INGRESS_CLIENT_LOS_LOL,
    TMZ_RETIMER2_INGRESS_CLIENT_LOS_LOL,
    TMZ_RETIMER3_INGRESS_CLIENT_LOS_LOL,
    TMZ_RETIMER1_INGRESS_FAULT,
    TMZ_RETIMER2_INGRESS_FAULT,
    TMZ_RETIMER3_INGRESS_FAULT,
    TMZ_RETIMER1_LINK_DOWN,
    TMZ_RETIMER2_LINK_DOWN,
    TMZ_RETIMER3_LINK_DOWN,
    TMZ_RETIMER1_EGRESS_LOS_LOL,
    TMZ_RETIMER2_EGRESS_LOS_LOL,
    TMZ_RETIMER3_EGRESS_LOS_LOL,
    TMZ_RETIMER1_EGRESS_FAULT,
    TMZ_RETIMER2_EGRESS_FAULT,
    TMZ_RETIMER3_EGRESS_FAULT,
    TMZ_RETIMER1_EGRESS_LINK_DOWN,
    TMZ_RETIMER2_EGRESS_LINK_DOWN,
    TMZ_RETIMER3_EGRESS_LINK_DOWN,
    TMZ_RETIMER1_ACCESS_FAIL,
    TMZ_RETIMER2_ACCESS_FAIL,
    TMZ_RETIMER3_ACCESS_FAIL,
    TMZ_IOEXP_ACCESS_FAIL,
    TMZ_FPC1_ACCESS_FAIL,
    TMZ_FPC2_ACCESS_FAIL,
    TMZ_PWR_SEQ_ACCESS_FAIL,

    BMZ_DSPLL_OOL,              // Bottom Mezz card faults
    BMZ_OCXO_LOS,
    BMZ_I2C_CLKGEN_I2C_FAIL,
    BMZ_TMP_OORH,
    BMZ_TMP_OORL,
    BMZ_TMP_I2C_FAIL,
    BMZ_3_3V_TMP_OORH,
    BMZ_3_3V_TMP_OORL,
    BMZ_3_3V_I2C_FAIL,
    BMZ_3_3V_VIN_OORL,
    BMZ_3_3V_VOUT_OOR,
    BMZ_1_8V_TMP_OORH,
    BMZ_1_8V_TMP_OORL,
    BMZ_1_8V_I2C_FAIL,
    BMZ_0_8V_VIN_OORL,
    BMZ_0_8V_VOUT_OOR,
    BMZ_TXDRV_VOUT_OOR,
    BMZ_H_TXDRV_VOUT_OOR,
    BMZ_AVS1_TMP_OORH,
    BMZ_AVS2_TMP_OORH,
    BMZ_AVS3_TMP_OORH,
    BMZ_AVS1_OC,
    BMZ_AVS2_OC,
    BMZ_AVS3_OC,
    BMZ_AVS1_ACCESS_FAIL,
    BMZ_AVS2_ACCESS_FAIL,
    BMZ_AVS3_ACCESS_FAIL,
    BMZ_AVS1_VIN_OORL,
    BMZ_AVS2_VIN_OORL,
    BMZ_AVS3_VIN_OORL,
    BMZ_AVS1_VOUT_OOR,
    BMZ_AVS2_VOUT_OOR,
    BMZ_AVS3_VOUT_OOR,
    BMZ_RETIMER1_FW_FAIL,
    BMZ_RETIMER2_FW_FAIL,
    BMZ_RETIMER3_FW_FAIL,
    BMZ_RETIMER1_AVS_FAIL,
    BMZ_RETIMER2_AVS_FAIL,
    BMZ_RETIMER3_AVS_FAIL,
    BMZ_RETIMER1_INGRESS_DCO_LOS_LOL,
    BMZ_RETIMER2_INGRESS_DCO_LOS_LOL,
    BMZ_RETIMER3_INGRESS_DCO_LOS_LOL,
    BMZ_RETIMER1_INGRESS_CLIENT_LOS_LOL,
    BMZ_RETIMER2_INGRESS_CLIENT_LOS_LOL,
    BMZ_RETIMER3_INGRESS_CLIENT_LOS_LOL,
    BMZ_RETIMER1_INGRESS_FAULT,
    BMZ_RETIMER2_INGRESS_FAULT,
    BMZ_RETIMER3_INGRESS_FAULT,
    BMZ_RETIMER1_LINK_DOWN,
    BMZ_RETIMER2_LINK_DOWN,
    BMZ_RETIMER3_LINK_DOWN,
    BMZ_RETIMER1_EGRESS_LOS_LOL,
    BMZ_RETIMER2_EGRESS_LOS_LOL,
    BMZ_RETIMER3_EGRESS_LOS_LOL,
    BMZ_RETIMER1_EGRESS_FAULT,
    BMZ_RETIMER2_EGRESS_FAULT,
    BMZ_RETIMER3_EGRESS_FAULT,
    BMZ_RETIMER1_EGRESS_LINK_DOWN,
    BMZ_RETIMER2_EGRESS_LINK_DOWN,
    BMZ_RETIMER3_EGRESS_LINK_DOWN,
    BMZ_RETIMER1_ACCESS_FAIL,
    BMZ_RETIMER2_ACCESS_FAIL,
    BMZ_RETIMER3_ACCESS_FAIL,
    BMZ_IOEXP_ACCESS_FAIL,
    BMZ_FPC1_ACCESS_FAIL,
    BMZ_FPC2_ACCESS_FAIL,
    BMZ_PWR_SEQ_ACCESS_FAIL,

    MAX_BOARD_FAULT_ID_NUM
} BoardFaultId;

// Board Access Fault Enums
enum eBoardAF
{
    AF_INVALID = -1
    , AF_HOST_PWR_SEQ_ACCESS_FAIL = 0
    , AF_HOST_HS_ACCESS_FAIL
    , AF_INLET_TEMP_ACCESS_FAIL
    , AF_BOARD_TEMP_ACCESS_FAIL
    , AF_MFG_EEPROM_ACCESS_FAIL
    , AF_CAL_EEPROM_ACCESS_FAIL
    // Add more here...
    , AF_LAST
};

// Board Init Mezzanine Access Fault Enums - Init BOTTOM
enum eBoardMezAF_b
{
    AF_MEZ_BMZ_INVALID = 1
    , AF_MEZ_BMZ_I2C_CLKGEN_I2C_FAIL
    , AF_MEZ_BMZ_IOEXP_ACCESS_FAIL
    , AF_MEZ_BMZ_FPC1_ACCESS_FAIL
    , AF_MEZ_BMZ_FPC2_ACCESS_FAIL
    , AF_MEZ_BMZ_PWR_SEQ_ACCESS_FAIL
    , AF_MEZ_BMZ_TMP_I2C_FAIL
    // Add more here...
    , AF_MEZ_BMZ_LAST
};

// Board Init Mezzanine Access Fault Enums - Init TOP
enum eBoardMezAF_t
{
    AF_MEZ_TMZ_INVALID = 1
    , AF_MEZ_TMZ_I2C_CLKGEN_I2C_FAIL
    , AF_MEZ_TMZ_IOEXP_ACCESS_FAIL
    , AF_MEZ_TMZ_FPC1_ACCESS_FAIL
    , AF_MEZ_TMZ_FPC2_ACCESS_FAIL
    , AF_MEZ_TMZ_PWR_SEQ_ACCESS_FAIL
    , AF_MEZ_TMZ_TMP_I2C_FAIL
    // Add more here...
    , AF_MEZ_TMZ_LAST
};

// Board Init Mezzanine Access Fault Enums - RT BOTTOM
enum eBoardMezAF_RT_b
{
    AF_MEZ_RT_BMZ_INVALID = 1
    , AF_MEZ_RT_BMZ_PWR_SEQ_ACCESS_FAIL
    , AF_MEZ_RT_BMZ_TMP_I2C_FAIL
    // Add more here...
    , AF_MEZ_RT_BMZ_LAST
};

// Board Init Mezzanine Access Fault Enums - RT TOP
enum eBoardMezAF_RT_t
{
    AF_MEZ_RT_TMZ_INVALID = 1
    , AF_MEZ_RT_TMZ_PWR_SEQ_ACCESS_FAIL
    , AF_MEZ_RT_TMZ_TMP_I2C_FAIL
    // Add more here...
    , AF_MEZ_RT_TMZ_LAST
};


// The invalid device address for access tests.
//
const uint8 BAD_DEV_ADDR = 0x7F;

// The Stringizing Macro
//
#define TO_STR(x) #x

// mapAF Macros
#define MAP_AF(NAME, PTR, ADDR)  \
mapAF_[AF_##NAME] = AFV(NAME, TO_STR(NAME), PTR, ADDR);

#define MAP_AF_SIM(NAME) \
mapAF_[AF_##NAME] = AFV(NAME, TO_STR(NAME), 0, 0);

// mapMezAF Macros
#define MAP_MEZ_AF_b(NAME, PTR, ADDR)  \
mapMezAF_b_[AF_MEZ_##NAME] = AFV(NAME, TO_STR(NAME), PTR, ADDR);

#define MAP_MEZ_AF_t(NAME, PTR, ADDR)                             \
mapMezAF_t_[AF_MEZ_##NAME] = AFV(NAME, TO_STR(NAME), PTR, ADDR);

#define MAP_MEZ_AF_SIM_b(NAME)  \
mapMezAF_b_[AF_MEZ_##NAME] = AFV(NAME, TO_STR(NAME), 0, 0);

#define MAP_MEZ_AF_SIM_t(NAME)  \
mapMezAF_t_[AF_MEZ_##NAME] = AFV(NAME, TO_STR(NAME), 0, 0);

// mapMezAF_RT Macros
#define MAP_MEZ_AF_RT_b(NAME, PTR, ADDR)  \
mapMezAF_b_[AF_MEZ_RT_##NAME] = AFV(NAME, TO_STR(NAME), PTR, ADDR);

#define MAP_MEZ_AF_RT_t(NAME, PTR, ADDR)                              \
mapMezAF_t_[AF_MEZ_RT_##NAME] = AFV(NAME, TO_STR(NAME), PTR, ADDR);

#define MAP_MEZ_AF_RT_SIM_b(NAME)                                \
mapMezAF_b_[AF_MEZ_RT_##NAME] = AFV(NAME, TO_STR(NAME), 0, 0);

#define MAP_MEZ_AF_RT_SIM_t(NAME)  \
mapMezAF_t_[AF_MEZ_RT_##NAME] = AFV(NAME, TO_STR(NAME), 0, 0);


// CATCH Macro
//
#define INFN_CATCH() \
catch( regIf::RegIfException &e ) { \
    std::string s("*** regIf::RegIfException caught in " \
		  + std::string(__func__)); \
    INFN_LOG(SeverityLevel::error) << s <<  endl;; \
} \
catch( std::exception &e ) { \
     std::string s("*** std::exception caught in " \
		   + std::string(__func__)); \
     INFN_LOG(SeverityLevel::error) << s <<  endl;; \
} \
catch(...) { \
     std::string s("*** Unknown Exception caught in " \
		   + std::string(__func__)); \
     INFN_LOG(SeverityLevel::error) << s <<  endl;; \
} \


/*
 * Fault Classes
 */

class Chm6BoardFault
{
public:

    Chm6BoardFault(BoardFaultId id,
                   bool isSimEn,
                   faultConditionType condition = FAULT_CLEAR);
    virtual ~Chm6BoardFault(){}

    void SetCondition(faultConditionType condition)
    {
        mCondition = condition;
    }

    virtual void CheckFaultCondition();

    static std::string BoardFaultIdToName(BoardFaultId id);

    static std::string BoardFltCondiToStr(faultConditionType type);

    void SetSimEnable(bool isEn) { mSimEn = isEn; }
    bool GetSimEnable()          { return mSimEn; }

    BoardFaultId GetFaultId() { return mId; }

    void SetSimCondition(faultConditionType cond)
        { mSimCondition = cond; }
    faultConditionType GetSimCondition()
        { return mSimCondition; }

    std::string        GetName()      { return mName; }
    faultConditionType GetCondition()
    {
        if (mSimEn) return mSimCondition;

        return mCondition;
    }

    void Dump(std::ostream &os);
    virtual void DumpSpecific(std::ostream &os);
    virtual void DumpHeaderSpecific(std::ostream &os);

    static void DumpHeader(std::ostream &os);

protected:

    virtual faultConditionType CheckFault();

    BoardFaultId        mId;
    std::string         mName;
    faultConditionType  mCondition;
    std::string         mDescriptiveText;
    std::string         mAlarmType;

    bool                mSimEn;
    faultConditionType  mSimCondition;
};

typedef struct DigitalInputFaultData
{
    BoardFaultId           mFaultId;
    uint32                 mInputMask;
    uint32                 mInputPolarity;
    bool                   mIsInitFault;
} DigitalInputFaultData;

class BoardFaultFpga : public Chm6BoardFault
{
public:

    BoardFaultFpga(const DigitalInputFaultData& inputFltData,
                   std::shared_ptr<BoardCommonDriver> brdDriver,
                   bool isSimEn,
                   faultConditionType condition = FAULT_CLEAR);

    virtual ~BoardFaultFpga() {}

protected:

    virtual faultConditionType CheckFault();
    virtual faultConditionType GetFaultState();

    virtual int GetRegValue(const uint32 &regVal);

    uint32 mInputMsk;
    uint32 mInputPol;

    std::shared_ptr<BoardCommonDriver> mspBrdDriver;
};

class BoardFaultMezzIoExp : public BoardFaultFpga
{
public:

    BoardFaultMezzIoExp(const DigitalInputFaultData& inputFltData,
                        std::shared_ptr<BoardCommonDriver> brdDriver,
                        mezzBoardIdType mezzBrdId,
                        bool isSimEn,
                        faultConditionType condition = FAULT_CLEAR);

    virtual ~BoardFaultMezzIoExp() {}

protected:

    virtual int GetRegValue(const uint32 &regVal);

    mezzBoardIdType mMezzBrdId;
};


struct AFV
{
    BoardFaultId boardFaultId;
    shared_ptr<DevI2cIf> devicePtr;
    uint32 registerAddr;
    bool bSim;
    faultConditionType fCondition;
    std::string fidName;

  AFV()
    : boardFaultId(BOARD_FAULT_INVALID)
    , devicePtr(0)
    , registerAddr(0)
    , bSim(false)
    , fCondition(FAULT_UNKNOWN)
    , fidName("Unknown")
    {}

  AFV(BoardFaultId bfid
      , std::string faultName
      , shared_ptr<DevI2cIf> devPtr
      , uint8 regaddr = 0
      , bool bsim = false
      , faultConditionType condition = FAULT_UNKNOWN )
    : boardFaultId(bfid)
    , devicePtr(devPtr)
    , registerAddr(regaddr)
    , bSim(bsim)
    , fCondition(condition)
    , fidName(faultName)
    {}
};
//
// END struct AFV


class AccessFaultsMap
{
public:

    static AccessFaultsMap& getInstance();

    inline bool IsValidAccessFault(const uint32 faultId) const
        { return IsValidFault( mapAF_, faultId ); }

    inline bool IsValidMezAccessFault_b(const uint32 faultId) const
        { return IsValidFault( mapMezAF_b_, faultId ); }

    inline bool IsValidMezAccessFault_t(const uint32 faultId) const
        { return IsValidFault( mapMezAF_t_, faultId ); }

    inline const std::map<eBoardAF,AFV>& af() const
        { return mapAF_; }

    inline const std::map<eBoardMezAF_t,AFV>& mezaf_t() const
        { return mapMezAF_t_; }

    inline const std::map<eBoardMezAF_b,AFV>& mezaf_b() const
        { return mapMezAF_b_; }

private:
    // Private Constructor
    AccessFaultsMap();

    // Map Collections
    std::map<eBoardAF,AFV> mapAF_;
    std::map<eBoardMezAF_b,AFV> mapMezAF_b_;
    std::map<eBoardMezAF_t,AFV> mapMezAF_t_;

    const bool IsSimEnv();
    void InitBoardAccessFaultsMap();
    void SIM_InitBoardAccessFaultsMap();
    void InitBoardMezAccessFaultsMap_b();
    void InitBoardMezAccessFaultsMap_t();
    void SIM_InitBoardMezAccessFaultsMap_b();
    void SIM_InitBoardMezAccessFaultsMap_t();

    template<typename T>
    bool IsValidFault(const T& mapT, const uint32 faultId) const
    {
        bool bRet = false;

        // Iterate the access faults map.
        for( const auto& kv : mapT )
        {
            // The faultId is an access fault
            // if the faultId is present in the
            // access faults map.
            auto& val = kv.second.boardFaultId;
            if( faultId == static_cast<uint32>(val) )
            {
                bRet = true;
                break;
            }
        }

        return bRet;
    }
};
//
// END class AccessFaultsMap


class RT_AccessFaultsMap
{
public:

    static RT_AccessFaultsMap& getInstance();

    inline bool IsValidAccessFault(const uint32 faultId) const
        { return IsValidFault( mapAF_, faultId ); }

    inline bool IsValidMezAccessFault_b(const uint32 faultId) const
        { return IsValidFault( mapMezAF_b_, faultId ); }

    inline bool IsValidMezAccessFault_t(const uint32 faultId) const
        { return IsValidFault( mapMezAF_t_, faultId ); }

    inline const std::map<eBoardAF,AFV>& af() const
        { return mapAF_; }

    inline const std::map<eBoardMezAF_RT_b,AFV>& mezaf_b() const
        { return mapMezAF_b_; }

    inline const std::map<eBoardMezAF_RT_t,AFV>& mezaf_t() const
        { return mapMezAF_t_; }

private:
    // Private Constructor
    RT_AccessFaultsMap();

    // Map Collections
    std::map<eBoardAF,AFV> mapAF_;
    std::map<eBoardMezAF_RT_b,AFV> mapMezAF_b_;
    std::map<eBoardMezAF_RT_t,AFV> mapMezAF_t_;

    const bool IsSimEnv();
    void InitBoardAccessFaultsMap();
    void SIM_InitBoardAccessFaultsMap();
    void InitBoardMezAccessFaultsMap_b();
    void InitBoardMezAccessFaultsMap_t();
    void SIM_InitBoardMezAccessFaultsMap_b();
    void SIM_InitBoardMezAccessFaultsMap_t();

    template<typename T>
    bool IsValidFault(const T& mapT, const uint32 faultId) const
    {
        bool bRet = false;

        // Iterate the access faults map.
        for( const auto& kv : mapT )
        {
            // The faultId is an access fault
            // if the faultId is present in the
            // access faults map.
            auto& val = kv.second.boardFaultId;
            if( faultId == static_cast<uint32>(val) )
            {
                bRet = true;
                break;
            }
        }

        return bRet;
    }
};
//
// END class RT_AccessFaultsMap


class BoardAccessFault : public Chm6BoardFault
{
public:
    explicit BoardAccessFault(const AFV& afv);
    virtual ~BoardAccessFault() {}

    virtual void DumpHeaderSpecific(std::ostream &os);
    virtual void DumpSpecific(std::ostream &os);

    void SetDevAddr(const uint32& devaddr);
    void RestoreDevAddr();
    inline const bool IsEnabled() { return mEnabled; }

protected:
    virtual faultConditionType CheckFault();
    virtual void TestAccess();

    // Register address recognized by the device.
    const uint32 regAddr_;

    // The correct bus address for the device.
    const uint32 deviceAddress_;

    // The currently used bus address for the device.
    uint32 curDevAddr_;

    // Pointer to the device interface.
    std::shared_ptr<DevI2cIf> mspRegIfDvr;

    // Sim enabled flag.
    bool mEnabled;

    // Fault Name
    std::string fidName_;
};
//
// END class BoardAccessFault


typedef boost::ptr_vector<boardMs::Chm6BoardFault> board_fault_ptr_vec;
typedef board_fault_ptr_vec::iterator board_fault_vec_itr;

extern const DigitalInputFaultData cFpgaHostBoardFaults[];
extern const uint32 cNumFpgaHostBoardFaults;
extern const DigitalInputFaultData cMezzIoExpFaults[];
extern const uint32 cNumMezzIoExpFaults;
extern const uint32 cMezzFaultIdOffset;

} // namespace boardMs

#endif // CHM6_BOARD_MS_SRC_COMMON_BOARD_FAULT_DEFS_H_
