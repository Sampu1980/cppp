#include "epdm.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <cassert>
#include "Bcm81725.h"


namespace gearbox {

const uint32_t FPGA_BASE_ADDR = 0xA0000000;
const uint32_t FPGA_SIZE        = 0xFFFF;
const uint32_t FPGA_MDIO_OFFSET    = 0x0300;
const uint32_t MDIO_FREQ_RATIO  = 0x12;
const uint32_t REG_SIZE = 0xFFFF;

const std::string MDIO_RA_NAME = "Bcm81725MdioRA";
const unsigned int cTopMz    = 1;
const unsigned int cBottomMz = 0;

const unsigned int cPhyIdx0[] = {0x04, 0x08, 0x0c};
const unsigned int cPhyIdx1[] = {0x014, 0x018, 0x01c};
const unsigned int cNumPhyIdx0 = (sizeof(cPhyIdx0) / sizeof(cPhyIdx0[0]));
const unsigned int cNumPhyIdx1 = (sizeof(cPhyIdx1) / sizeof(cPhyIdx1[0]));
const unsigned int cTopMzMdioAddrMask = 0x10;
const unsigned int cNumBcmsPerMz = 3;
const unsigned int cNumMz = 2;

const uint32 cAdr1 = 0x5200c7f4; // ACR 1
const uint32 cAdr2 = 0x5200c7f8; // ACR 2
const uint16 cDat1 = 0x817a;     // ACR 1 data
const uint16 cDat2 = 0x7e85;     // ACR 2 data


static FpgaMdioIf* mdioIfPtr;



int mdio_write(void *user, unsigned int mdioAddr, unsigned int regAddr, unsigned int data) 
{
//    cout << "mdio_write: " << std::hex << regAddr << "  " << data << endl;
    if (!user) 
    {
        std::cout << " mdio_write user empty!!!" << endl;
        return -1; 
    }

    uint8_t busSel = (uint8_t)(*((unsigned int*)user));

    if ( (busSel == cTopMz) || (mdioAddr >> 4 == 1) )
    {
        // Top mz, get lower nibble
        mdioAddr &= 0xF;
    }

    try 
    {
        mdioIfPtr->Write16(busSel, mdioAddr, regAddr, data);
    }
    catch ( ... ) 
    {
        std::cout << " mdio_write failed!!!" << endl;
        return -1;
    }

    return 0;
}

int mdio_read(void *user, unsigned int mdioAddr, unsigned int regAddr, unsigned int *data) 
{
    if (!user || !data) 
    {
        std::cout << " mdio_read invalid parameter!!!" << endl;
        return -1; 
    }

    uint8_t busSel = (uint8_t)(*((unsigned int*)user));

    if ( (busSel == cTopMz) || (mdioAddr >> 4 == 1) )
    {
        // Top mz, get lower nibble
        mdioAddr &= 0xF;
    }

    try {
        *data = static_cast<unsigned int>(mdioIfPtr->Read16(busSel, mdioAddr, regAddr));
    }    
    catch ( ... ) {
        std::cout << " mdio_read failed!!!" << endl;
        return -1;
    }
//    cout << "mdio_read: " << std::hex << regAddr << "  " << *data << endl;
    return 0;
}

Bcm81725::Bcm81725() : myLogPtr(new SimpleLog::Log(2000)), myMilleniObPtr("milleniob") 
{
    std::ostringstream  log;
    log << " Bcm81725::Bcm81725 ";

    // todo: switch to factory for creation
	myFpgaRAPtr = new FpgaRegIf(FPGA_BASE_ADDR, FPGA_SIZE);

    if (!myFpgaRAPtr) 
    {
        log << " m_fpgaRA is null, exit ";
        addLog(__func__, __LINE__, log.str());
        return;
    }

    // todo: switch to factory for creation
	myMdioRAPtr = new FpgaMdioIf(MDIO_RA_NAME,
	                             myBcmMtx,
	                             FPGA_MDIO_OFFSET,
                                 MDIO_FREQ_RATIO,
		                         static_cast<RegIf*>(myFpgaRAPtr),
		                         REG_SIZE);
    if (!myMdioRAPtr) {
        log << " m_mdioRA is null, exit ";
        addLog(__func__, __LINE__, log.str());
        return;
    }
	mdioIfPtr = myMdioRAPtr;

}

Bcm81725::~Bcm81725() 
{
    delete myFpgaRAPtr;
    delete myMdioRAPtr;
}

int Bcm81725::warmInit(const Bcm81725Lane& param)
{
    int rv = 0;

    bcm_plp_access_t phyInfo;
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Initialize chip and verify Firmware
      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    bcm_plp_firmware_load_type_t fwLoadType;
    memset(&fwLoadType, 0, sizeof(bcm_plp_firmware_load_type_t));
    fwLoadType.firmware_load_method = bcmpmFirmwareLoadMethodInternal;

    // skip installs read/write accessors and checks version
    fwLoadType.force_load_method = bcmpmFirmwareLoadSkip;

#ifdef USE_SOCKDA
    /* CoreReset is needed for warm reset to zero info and install drivers */
    rv |= bcm_plp_init_fw_bcast(milleniob, phyInfo, mdio_read, mdio_write,
            &fwLoadType, bcmpmFirmwareBroadcastCoreReset);
    if (rv != 0) {
        cout << "USE_SOCKDA bcm_plp_init_fw_bcast failed." << endl;
        return rv;
    }

    usleep(100000); /* After Soft reset sleep */
#else
    rv |= bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write,
            &fwLoadType, bcmpmFirmwareBroadcastCoreReset);
    if (rv != 0) {
        cout << "step 1: bcm_plp_init_fw_bcast failed." << endl;
        return rv;
    }

    sleep(1); /* After Hard reset sleep */
    rv |= bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write,
            &fwLoadType, bcmpmFirmwareBroadcastEnable);
    if (rv != 0) {
        cout << "step 2: bcm_plp_init_fw_bcast failed." << endl;
        return rv;
    }

    rv |= bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write,
            &fwLoadType, bcmpmFirmwareBroadcastFirmwareExecute);
    if (rv != 0) {
        cout << "step 3: bcm_plp_init_fw_bcast failed." << endl;
        return rv;
    }

    rv |= bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write,
            &fwLoadType, bcmpmFirmwareBroadcastFirmwareVerify);
    if (rv != 0) {
        cout << "step 4: bcm_plp_init_fw_bcast failed." << endl;
        return rv;
    }

    rv |= bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write,
            &fwLoadType, bcmpmFirmwareBroadcastEnd);
    if (rv != 0) {
        cout << "step 5: bcm_plp_init_fw_bcast failed." << endl;
        return rv;
    }
#endif
    return rv;
}

string Bcm81725::PrintErrorCode(int errorCode)
{
    string errorString;

    switch(errorCode)
    {
    case -1:
        errorString = " Internal error returned by PHYMOD layer";
        break;
    case -2:
        errorString = " Memory Allocation error returned by PHYMOD layer";
        break;
    case -3:
        errorString = " I/O Error returned by PHYMOD layer";
        break;
    case -4:
        errorString = " Invalid Param error returned by PHYMOD layer";
        break;
    case -5:
        errorString = " Core error returned by PHYMOD layer";
        break;
    case -6:
        errorString = " Phy error returned by PHYMOD layer";
        break;
    case -7:
        errorString = " Busy error returned by PHYMOD layer";
        break;
    case -8:
        errorString = " Failure error returned by PHYMOD layer";
        break;
    case -9:
        errorString = " Timeout returned by PHYMOD layer";
        break;
    case -10:
        errorString = " Resource error returned by PHYMOD layer";
        break;
    case -11:
        errorString = " Configuration error returned by PHYMOD layer";
        break;
    case -12:
        errorString = " Unavailable error returned by PHYMOD layer";
        break;
    case -13:
        errorString = " Initialization error returned by PHYMOD layer";
        break;
    case -14:
        errorString = " Out of bound error returned by PHYMOD layer";
        break;
    case -20:
        errorString = " Chip not found returned by BCM API layer";
        break;
    case -21:
        errorString = " Memory allocation failed returned by BCM API layer";
        break;
    case -22:
        errorString = " Particular core not found returned by BCM API layer";
        break;
    case -23:
        errorString = " Internal code error returned by BCM API layer";
        break;
    case -24:
        errorString = " Trying to initialize existing/initialized PHY returned by BCM API layer";
        break;
    case -25:
        errorString = " Method not applicable returned by BCM API layer";
        break;
    case -26:
        errorString = " Invalid PHY (Mismatched IDs) returned by BCM API layer";
        break;
    case -27:
        errorString = " Functionality not supported or unavailable returned by BCM API layer";
        break;
    case -28:
        errorString = " Invalid Parameter returned by BCM API layer";
        break;
    default:
        errorString = " Unknown error code!";
        break;

    }


    return errorString;
}



int Bcm81725::loadFirmware(const unsigned int & bus) {
    std::ostringstream  log;
    log << " Bcm81725::loadFirmware ";

    int rv = 0;
    unsigned int *phyIdx = (unsigned int *)cPhyIdx0;
    unsigned int numPhyIdx = cNumPhyIdx0;

    unsigned int phyId = 0;
    unsigned int fwVer = 0, fwCrc = 0;

    bcm_plp_access_t  phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    phyInfo.platform_ctxt = (void*)(&bus);
    phyInfo.flags = BCM_PLP_COLD_BOOT;

    if (bus == cTopMz)
    {
        phyIdx = (unsigned int *)cPhyIdx1;
        numPhyIdx = cNumPhyIdx1;
    }

#if 0
    for (phyId = 0; phyId < sizeof(phyIdx)/sizeof(phyIdx[0]); ++phyId)
    {
        phyInfo.phy_addr = phyIdx[phyId];
        rv  = bcm_plp_cleanup(myMilleniObPtr, phyInfo);
        if (rv != 0)
        {
            std::cout << "bcm_plp_cleanup failed with rv=" << rv << endl;
        }
    }
#endif


    /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Initialize chip and download Firmware
      +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    bcm_plp_firmware_load_type_t fwLoadType;
    memset(&fwLoadType, 0, sizeof(bcm_plp_firmware_load_type_t));
    fwLoadType.firmware_load_method = bcmpmFirmwareLoadMethodInternal;
    /* In force firmware download method, downloads the firmware always */
    fwLoadType.force_load_method = bcmpmFirmwareLoadForce;

    /* If user wants to select auto firmware download method, it checks firmware version on chip if it is not latest firmware,
     * it downloads otherwise it won't downloads the firmware. User need to select force_load_method as bcmpmFirmwareLoadAuto.
     * if auto download needed, uncomment below code.
     */
    /* fwLoadType.force_load_method = bcmpmFirmwareLoadAuto; */


    /** Step-1: Reset the core for all phy id in mdio bus *   */
    for (phyId = 0; phyId < numPhyIdx; ++phyId) {
        phyInfo.phy_addr = phyIdx[phyId];
        if((rv = bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write, 
                                       &fwLoadType, bcmpmFirmwareBroadcastCoreReset)) != 0) {
            std::cout << " step 1: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastFirmwareCoreReset API failed rv: " << rv <<
                    PrintErrorCode(rv) << endl;
            
            log << " step 1: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastFirmwareCoreReset API failed rv: " << rv
                    << PrintErrorCode(rv) << endl;
            addLog(__func__, __LINE__, log.str());
            return rv;
        } 

        std::cout << " step 1: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
             << " bcmpmFirmwareBroadcastFirmwareCoreReset API success" << endl;

        log << " step 1: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
             << " bcmpmFirmwareBroadcastFirmwareCoreReset API success" << endl;
        addLog(__func__, __LINE__, log.str());
    }
    sleep(1); /* After Hard reset sleep */

    /** Step-2: Enable the broadcast for all phy id in mdio bus * */
    for (phyId = 0; phyId < numPhyIdx; ++phyId) {
        phyInfo.phy_addr = phyIdx[phyId];
        if((rv = bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write, 
                                       &fwLoadType, bcmpmFirmwareBroadcastEnable)) != 0) {
            log << " step 2: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastFirmwareEnable API failed rv: " << rv <<
                    PrintErrorCode(rv) << endl;
            std::cout << " step 2: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastFirmwareEnable API failed rv: " << rv <<
                                PrintErrorCode(rv) << endl;
            addLog(__func__, __LINE__, log.str());
            return rv;
        }

        std::cout << " step 2: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
             << " bcmpmFirmwareBroadcastFirmwareEnable API success" << endl;

        log << " step 2: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
             << " bcmpmFirmwareBroadcastFirmwareEnable API success" << endl;
        addLog(__func__, __LINE__, log.str());
    }

    /** Step-3: Load the FW for only one phyId, internally it will
     *  broadcast firmware of similar type of phys on same mdio bus
     *  * */
    phyId = 0;
    phyInfo.phy_addr = phyIdx[phyId];  /* Load Firmware for any one Phy/Core in BCAST_LIST */
    if((rv = bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write, 
                                   &fwLoadType, bcmpmFirmwareBroadcastFirmwareExecute)) != 0) {
        log << " step 3: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastFirmwareExecute API failed" <<
                PrintErrorCode(rv) << endl;
        std::cout << " step 3: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastFirmwareExecute API failed" <<
                        PrintErrorCode(rv) << endl;

        addLog(__func__, __LINE__, log.str());
        return rv;
    }

    std::cout << " step 3: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
         << " bcmpmFirmwareBroadcastFirmwareExecute API success" << endl;  

    log << " step 3: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
         << " bcmpmFirmwareBroadcastFirmwareExecute API success" << endl;             
    addLog(__func__, __LINE__, log.str());


    /** Step-4: Disable the broadcast and FW load verify for all phy
     *  id in mdio bus  * */
    for (phyId = 0; phyId < numPhyIdx; ++phyId) {
        phyInfo.phy_addr = phyIdx[phyId];
        if((rv = bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write, 
                                       &fwLoadType, bcmpmFirmwareBroadcastFirmwareVerify)) != 0) {
            cout << __func__ << " step 4: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastFirmwareVerify API failed rv=" <<
                    rv << PrintErrorCode(rv) << endl;
            return rv;
        } 

        std::cout << " step 4: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
                 << " bcmpmFirmwareBroadcastFirmwareVerify API success" << endl;

        log << " step 4: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
                 << " bcmpmFirmwareBroadcastFirmwareVerify API success" << endl;
        addLog(__func__, __LINE__, log.str());        
    }

    /** Step-5: End of the broadcast for all phy id in mdio bus
                 */
    for (phyId = 0; phyId < numPhyIdx; ++phyId) {
        phyInfo.phy_addr = phyIdx[phyId];
        if((rv = bcm_plp_init_fw_bcast(myMilleniObPtr, phyInfo, mdio_read, mdio_write, 
                                       &fwLoadType, bcmpmFirmwareBroadcastEnd)) != 0) {
            log << " step 5: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastEnd API failed rv" << rv <<
                    PrintErrorCode(rv) << endl;
            std::cout << " step 5: bcm_plp_init_fw_bcast for bcmpmFirmwareBroadcastEnd API failed rv" << rv <<
                                PrintErrorCode(rv) << endl;

            addLog(__func__, __LINE__, log.str());
            return rv;
        }


        std::cout << " step 5: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
             << " bcmpmFirmwareBroadcastEnd API success" << endl;

        log << " step 5: bcm_plp_init_fw_bcast for " << phyIdx[phyId] 
             << " bcmpmFirmwareBroadcastEnd API success" << endl;
        addLog(__func__, __LINE__, log.str());
    
    }

    /* FW Version get */
    for (phyId = 0; phyId < numPhyIdx; ++phyId) {
        phyInfo.phy_addr = phyIdx[phyId];
        rv  = bcm_plp_firmware_info_get(myMilleniObPtr, phyInfo, &fwVer, &fwCrc);
        if (rv != 0) {
            log << " step 6: bcm_plp_firmware_info_get failed with rv:" << rv << PrintErrorCode(rv) << endl;
            std::cout << " step 6"
                    ": bcm_plp_firmware_info_get failed with rv:" << rv << PrintErrorCode(rv) << endl;
            addLog(__func__, __LINE__, log.str());
            return rv;
        }

        std::cout << " step 6: bcm_plp_firmware_info_get " << std::hex << phyIdx[phyId] 
            << " fwVer = " << fwVer
            << " fwCrc = " << fwCrc << std::dec << endl;

        log << " step 6: bcm_plp_firmware_info_get " << std::hex << phyIdx[phyId] 
            << " fwVer = " << fwVer
            << " fwCrc = " << fwCrc << std::dec << endl;
        addLog(__func__, __LINE__, log.str());
    }

    return rv;
}

int Bcm81725::LoadMz(const unsigned int &loadFwMz, const mzPolarity * setPolMz)
{
    std::ostringstream  log;
    int rc = 0;


    if (loadFirmware(loadFwMz))
    {
        std::cout << " Loaded mz=" << loadFwMz << " failed!" << endl;
         log << " load firmware on " << loadFwMz << " MZ failed ";
         addLog(__func__, __LINE__, log.str());
         rc = -1;
     }
     log << " load firmware successfully ";
     std::cout << " Loaded FW on mz=" << loadFwMz << " success!" << endl;

     std::cout << " step 7: Set polarity." << endl;

     for (unsigned int i = 0; i < cNumPolarities; i++)
     {
         std::cout << " Set polarity " << endl;
         rc += setPolarity(setPolMz[i].bcmLanConfig, setPolMz[i].txPolarity, setPolMz[i].rxPolarity);
     }

     Bcm81725Lane *param;
     Bcm81725Lane paramTblTop[cNumBcmsPerMz] = { {1, 0x4, 0, 0xf}, {1, 0x8, 0, 0xf}, {1, 0xc, 0, 0xf} };
     Bcm81725Lane paramTblBot[cNumBcmsPerMz] = { {0, 0x4, 0, 0xf}, {0, 0x8, 0, 0xf}, {0, 0xc, 0, 0xf} };

     if (loadFwMz == cTopMz)
     {
         param = paramTblTop;
     }
     else
     {
        param = paramTblBot;
     }

     /*
     * Apparently these writes are needed to fix a pll config problem.
     * This should be fixed in the next release.
     */

     for (unsigned int i = 0; i < cNumBcmsPerMz; i++)
     {
         rc += iWrite(param[i], cAdr1, cDat1);
         rc += iWrite(param[i], cAdr2, cDat2);
     }


     addLog(__func__, __LINE__, log.str());

     if (rc != 0)
     {
         return -1;
     }
     else
     {
         return 0;
     }
}

int Bcm81725::init(const string &mEnvStr)
{
    std::ostringstream  log;
    log << " Bcm81725::init ";
    unsigned int loadFwMz = 0;

    int rc = 0;

    // If chm6EnvStr == HB, skip all
    // if bottom mz, load this
    if (mEnvStr != "hb")
    {

        if (mEnvStr == "" || mEnvStr == "bmz")
        {
            rc += LoadMz(cBottomMz, bottomMzPol);
            std::cout << " Loaded bottom mz" << endl;
        }


        // if top mz load this
        if (mEnvStr == "" || mEnvStr == "tmz")
        {
            rc += LoadMz(cTopMz, topMzPol);
            std::cout << " Loaded top mz" << endl;
        }

    }
    addLog(__func__, __LINE__, log.str());

    if (rc != 0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


void Bcm81725::dumpLog(std::ostream &os) 
{
    unique_lock<mutex> lck(myLogMtx);
    os << "<<<<<<<<<<<<<<<<<<< Bcm81725.dump_log >>>>>>>>>>>>>>>>>>>>>>" << endl << endl;

    if (myLogPtr) {
        myLogPtr->DisplayLog(os);
    }
    else {
        os << "TomDriver Log is not created!" << endl;
    }
}

void Bcm81725::dumpStatus(std::ostream &os, std::string cmd) 
{
}

void Bcm81725::resetLog( std::ostream &os )
{
   unique_lock<mutex> lck(myLogMtx);

    if (myLogPtr) {
        myLogPtr->ResetLog();
        os << "Bcm81725 Log has been reset!" << endl;
    }
}

void Bcm81725::addLog(const std::string &func, uint32_t line, const std::string &text) 
{
    unique_lock<mutex> lck(myLogMtx);

    ostringstream  os;
    os << "Bcm81725::" << func << ":" << line << ": " << text;

    if (myLogPtr) {
        myLogPtr->AddRecord(os.str().c_str());
    }    
}

int Bcm81725::getPolarity(const Bcm81725Lane& param, unsigned int& tx, unsigned int& rx)
{
    std::ostringstream  log;
    log << " Bcm81725::getPolarity ";

    bcm_plp_access_t phyInfo;
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    // if (phyInfo.lane_map == 0) return -1;

    int rc = bcm_plp_polarity_get(myMilleniObPtr, phyInfo, &tx, &rx);
    if(!rc){
        log << "bcm_plp_polarity_get tx=0x" << tx << "rx=0x" << rx << endl;
        std::cout << "bcm_plp_polarity_get tx=0x" << tx << "rx=0x" << rx << endl;
    }
    else {
        log << "bcm_plp_polarity_get failed, rc = " << rc << endl;
        std::cout << "bcm_plp_polarity_get failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}

int Bcm81725::setPolarity(const Bcm81725Lane& param, unsigned int tx, unsigned int rx)
{
    std::ostringstream  log;
    log << " Bcm81725::setPolarity ";
    std::cout << " Bcm81725::setPolarity " << endl;

    bcm_plp_access_t phyInfo;
    //phyInfo.platform_ctxt = (param.bus)? (void*)(cBottomMzBoard.c_str()) : (void*)(cTopMzBoard.c_str());
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    //if (phyInfo.lane_map == 0) return -1;
    std::cout << "bus=" << param.bus << " mdioAddr=" << param.mdioAddr << " side="
            << param.side << " laneNum=" << param.laneNum << endl;

    int rc = bcm_plp_polarity_set(myMilleniObPtr, phyInfo, tx, rx);
    if(!rc){
        log << "bcm_plp_polarity_set successfully tx=0x" << tx << "rx=0x" << rx << endl;
        std::cout << "bcm_plp_polarity_set successfully tx=0x" << tx << "rx=0x" << rx << endl;
    }
    else {
        log << "bcm_plp_polarity_get failed, rc = " << rc << endl;
        std::cout << "bcm_plp_polarity_get failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}


int Bcm81725::iWrite(const Bcm81725Lane& param, uint32 regAddr, uint16 data)
{
    int rc = 0;
    bcm_plp_access_t phyInfo;
    phyInfo.platform_ctxt = (void*)(&param.bus);


    if (param.bus == cTopMz)
    {
        phyInfo.phy_addr = param.mdioAddr | cTopMzMdioAddrMask;
    }
    else
    {
        phyInfo.phy_addr = param.mdioAddr;
    }


    phyInfo.if_side = 0;
    phyInfo.lane_map = 0xf;
    unsigned int dev = 1; // 1 - PMA/PMD, 3 - PCS, 7 - AN ... what are these?


    rc = bcm_plp_reg_value_set(myMilleniObPtr, phyInfo, dev, regAddr, (unsigned int)data);

    if (rc == 0)
    {
        cout << "Wrote Dev=" << dev << " regAddr=" << regAddr << " data=" << data << " successful." << endl;
    }
    else
    {
        cout << "Wrote Dev=" << dev << " regAddr=" << regAddr << " data=" << data << " unsuccessful!" << endl;
    }


    return rc;
}

int Bcm81725::setAvsConfig(const Bcm81725Lane& param, unsigned int enable, unsigned int margin)
{
    std::ostringstream  log;
    log << " Bcm81725::setAvsConfig ";
    std::cout << " Bcm81725::setAvsConfig " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    //phyInfo.platform_ctxt = (param.bus)? (void*)(cBottomMzBoard.c_str()) : (void*)(cTopMzBoard.c_str());
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    bcm_plp_avs_config_t avsConfig;
    memset(&avsConfig, 0, sizeof(bcm_plp_avs_config_t));

    avsConfig.enable = enable;
    avsConfig.avs_disable_type = bcmplpAvsDisableTypeNoFirmwareControl;
    avsConfig.avs_ctrl = bcmplpAvsControlInternal;
    avsConfig.avs_regulator = bcmplpAvsRegulatorTPS546;      /**< TPS546 regulator */
    avsConfig.regulator_i2c_address = 0x1E;
    avsConfig.avs_pkg_share = bcmplpAvsPackageShare1;
    avsConfig.avs_dc_margin = (bcm_plp_avs_board_dc_margin_e) margin; // bcmplpAvsBoardDcMargin0mV;
    avsConfig.ref_clk = bcm_pm_RefClk156Mhz;

    int rc = bcm_plp_avs_config_set(myMilleniObPtr, phyInfo, avsConfig);
    if(!rc){
        log << "bcm_plp_avs_config_set successfully" << endl;
        std::cout << "bcm_plp_avs_config_set successfully" << endl;
    }
    else {
        log << "bcm_plp_avs_config_set failed, rc = " << rc << endl;
        std::cout << "bcm_plp_avs_config_set failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}

int Bcm81725::getAvsConfig(const Bcm81725Lane& param, unsigned int& enable, unsigned int& disableType, 
                     unsigned int& ctrl, unsigned int& regulator, unsigned int& pkgShare, unsigned int& margin)
{
    std::ostringstream  log;
    log << " Bcm81725::getAvsConfig ";
    std::cout << " Bcm81725::getAvsConfig " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    bcm_plp_avs_config_t avsConfig;
    memset(&avsConfig, 0, sizeof(bcm_plp_avs_config_t));

    int rc = bcm_plp_avs_config_get(myMilleniObPtr, phyInfo, &avsConfig);
    if(!rc){        
        enable = avsConfig.enable;
        disableType = avsConfig.avs_disable_type;
        ctrl = avsConfig.avs_ctrl;
        regulator = avsConfig.avs_regulator;
        pkgShare = avsConfig.avs_pkg_share;
        margin = avsConfig.avs_dc_margin;

        log << "bcm_plp_avs_config_get successfully" << endl;
        std::cout << "bcm_plp_avs_config_get successfully" << endl;
    }
    else {
        log << "bcm_plp_avs_config_get failed, rc = " << rc << endl;
        std::cout << "bcm_plp_avs_config_get failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
 }

int Bcm81725::getAvsStatus(const Bcm81725Lane& param, unsigned int& status)
{
    std::ostringstream  log;
    log << " Bcm81725::getAvsConfig ";
    std::cout << " Bcm81725::getAvsConfig " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    bcm_plp_avs_config_status_t avsStatus;
    memset(&avsStatus, 0, sizeof(bcm_plp_avs_config_status_t));

    int rc = bcm_plp_avs_status_get(myMilleniObPtr, phyInfo, &avsStatus);
    if(!rc){        
        status = avsStatus.enable;
        log << "bcm_plp_avs_status_get successfully" << endl;
        std::cout << "bcm_plp_avs_status_get successfully" << endl;
    }
    else {
        log << "bcm_plp_avs_status_get failed, rc = " << rc << endl;
        std::cout << "bcm_plp_avs_status_get failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}

int Bcm81725::getMode(const Bcm81725Lane& param, int& speed, int& ifType, int& refClk, int& interfaceMode,
                unsigned int& dataRate, unsigned int& modulation, unsigned int& fecType)
{
    std::ostringstream  log;
    log << " Bcm81725::getMode ";
    std::cout << " Bcm81725::getMode " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    bcm_plp_milleniob_device_aux_modes_t auxMode;
    memset(&auxMode, 0, sizeof(bcm_plp_milleniob_device_aux_modes_t));

    int rc = bcm_plp_mode_config_get(myMilleniObPtr, phyInfo,  &speed, &ifType, &refClk, &interfaceMode, (void*)&auxMode);
    if(!rc){        
        dataRate = auxMode.lane_data_rate;
        modulation = auxMode.modulation_mode;
        fecType = auxMode.fec_mode_sel;
        log << "bcm_plp_mode_config_get successfully" << endl;
        std::cout << "bcm_plp_mode_config_get successfully" << endl;
    }
    else {
        log << "bcm_plp_mode_config_get failed, rc = " << rc << endl;
        std::cout << "bcm_plp_mode_config_get failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}

int Bcm81725::setMode(const Bcm81725Lane& param, int speed, int ifType, int refClk, int interfaceMode,
                unsigned int dataRate, unsigned int modulation, unsigned int fecType)
{
    std::ostringstream  log;
    log << " Bcm81725::setMode ";
    std::cout << " Bcm81725::setMode " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    bcm_plp_milleniob_device_aux_modes_t auxMode;
    memset(&auxMode, 0, sizeof(bcm_plp_milleniob_device_aux_modes_t));

    auxMode.lane_data_rate   = (bcm_plp_lane_data_rate_t)dataRate;
    auxMode.modulation_mode  = (bcm_plp_modulation_mode_t)modulation;
    auxMode.fec_mode_sel     = (bcm_plp_fec_mode_sel_t)fecType;

    int rc = bcm_plp_mode_config_set(myMilleniObPtr, phyInfo, speed, ifType, refClk, interfaceMode, (void*)&auxMode);
    if(!rc){        
        log << "bcm_plp_mode_config_set successfully" << endl;
        std::cout << "bcm_plp_mode_config_set successfully" << endl;
    }
    else {
        log << "bcm_plp_mode_config_set failed, rc = " << rc << endl;
        std::cout << "bcm_plp_mode_config_set failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}

int Bcm81725::prbsLinkStatusCheck(const Bcm81725Lane& param, unsigned int& status)
{
    std::ostringstream  log;
    log << " Bcm81725::setPrbsGen ";
    std::cout << " Bcm81725::setPrbsGen " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    // if (mask != 0) phy_info.lane_map &= mask;
    // if (phy_info.lane_map == 0) continue;

    status = 0;
    int rc = bcm_plp_link_status_get(myMilleniObPtr, phyInfo, &status);
    if(!rc){        
        log << "bcm_plp_link_status_get successfully" << endl;
        std::cout << "bcm_plp_link_status_get successfully" << endl;
    }
    else {
        log << "bcm_plp_link_status_get failed, rc = " << status << endl;
        std::cout << "bcm_plp_link_status_get failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}

int Bcm81725::setLoopback(const Bcm81725Lane& param, unsigned int mode, unsigned int enable)
{
    std::ostringstream  log;
    log << " Bcm81725::setLoopback ";
    std::cout << " Bcm81725::setLoopback " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    int status = bcm_plp_loopback_set(myMilleniObPtr, phyInfo, mode, enable);
    if(!status){        
        log << "bcm_plp_loopback_set successfully, status is OK" << endl;
        std::cout << "bcm_plp_loopback_set successfully, status is OK" << endl;
    }
    else {
        log << "bcm_plp_loopback_set failed, rc = " << status << endl;
        std::cout << "bcm_plp_loopback_set failed, rc = " << status << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return status;   
}

int Bcm81725::prbsStatusCheck(const Bcm81725Lane& param, unsigned int timeVal, unsigned int& status)
{
    std::ostringstream  log;
    log << " Bcm81725::setPrbsGen ";
    std::cout << " Bcm81725::setPrbsGen " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    status = bcm_plp_prbs_rx_stat(myMilleniObPtr, phyInfo, timeVal);
    if(!status){        
        log << "bcm_plp_prbs_rx_stat successfully, status is OK" << endl;
        std::cout << "bcm_plp_prbs_set successfully, status is OK" << endl;
    }
    else {
        log << "bcm_plp_prbs_set failed, rc = " << status << endl;
        std::cout << "bcm_plp_prbs_set failed, rc = " << status << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return status;   
}
    
int Bcm81725::setPrbsGen(const Bcm81725Lane& param, unsigned int tx, unsigned int poly, 
                   unsigned int inv, unsigned int lb, unsigned int enaDis)
{
    std::ostringstream  log;
    log << " Bcm81725::setPrbsGen ";
    std::cout << " Bcm81725::setPrbsGen " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    // if (mask != 0) phy_info.lane_map &= mask;
    // if (phy_info.lane_map == 0) continue;

    int rc = bcm_plp_prbs_set(myMilleniObPtr, phyInfo, tx, poly, inv, lb, enaDis);
    if(!rc){        
        log << "bcm_plp_prbs_set successfully" << endl;
        std::cout << "bcm_plp_prbs_set successfully" << endl;
    }
    else {
        log << "bcm_plp_prbs_set failed, rc = " << rc << endl;
        std::cout << "bcm_plp_prbs_set failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}

int Bcm81725::setPrbsCheck(const Bcm81725Lane& param, unsigned int rx, unsigned int poly, 
                   unsigned int inv, unsigned int lb, unsigned int enaDis)
{
    std::ostringstream  log;
    log << " Bcm81725::setPrbsGen ";
    std::cout << " Bcm81725::setPrbsGen " << endl;

    bcm_plp_access_t phyInfo;
    memset(&phyInfo, 0, sizeof(bcm_plp_access_t));
    
    phyInfo.platform_ctxt = (void*)(&param.bus);
    phyInfo.phy_addr = param.mdioAddr;
    phyInfo.if_side = param.side;
    phyInfo.lane_map = param.laneNum;

    // if (mask != 0) phy_info.lane_map &= mask;
    // if (phy_info.lane_map == 0) continue;
    
    int rc = bcm_plp_prbs_set(myMilleniObPtr, phyInfo, rx, poly, inv, lb, enaDis);
    if(!rc){        
        log << "bcm_plp_prbs_set successfully" << endl;
        std::cout << "bcm_plp_prbs_set successfully" << endl;
    }
    else {
        log << "bcm_plp_prbs_set failed, rc = " << rc << endl;
        std::cout << "bcm_plp_prbs_set failed, rc = " << rc << endl;
    }

    addLog(__func__, __LINE__, log.str());
    return rc;
}




}
