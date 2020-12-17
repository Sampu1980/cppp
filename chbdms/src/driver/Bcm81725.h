#ifndef __GEARBOXDRIVER_H__
#define __GEARBOXDRIVER_H__

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "GearBoxIf.h"
#include "FpgaMdioIf.h"
#include "FpgaRegIf.h"

#include "SimpleLog.h"
#include "bcm_pm_if/bcm_pm_if_api.h"

using namespace std;

namespace gearbox {

class Bcm81725 :  public GearBoxIf
{
public:

    // Bcm81725Lane -> <bus sel: top-1 bottom-0> <Mdio Address(4, 8, c)>
    //                 <host-1, line-0>          <lane number>
    typedef struct
    {
        Bcm81725Lane bcmLanConfig;
        unsigned int txPolarity;
        unsigned int rxPolarity;
    } mzPolarity;

    static const unsigned int cNumPolarities = 6;

    const mzPolarity bottomMzPol[cNumPolarities] = {
                                           { {0, 4  , 0, 0xFFFF}, 519, 25088}, { {0, 4,   1, 0xFF},   0, 0},
                                           { {0, 8  , 0, 0xFFFF},   5, 24576}, { {0, 8,   1, 0xFF},   0, 0},
                                           { {0, 0xC, 0, 0xFFFF},   4, 16388}, { {0, 0xC, 1, 0xFF}, 127, 0}
                                         };

    const mzPolarity topMzPol[cNumPolarities] = {
                                        { {1, 4  , 0, 0xFFFF}, 519, 25088}, { {1, 4,   1, 0xFF},   0, 0},
                                        { {1, 8  , 0, 0xFFFF},   5, 24576}, { {1, 8,   1, 0xFF},   0, 0},
                                        { {1, 0xC, 0, 0xFFFF},   4, 16388}, { {1, 0xC, 1, 0xFF}, 127, 0}
                                      };




	Bcm81725();
	~Bcm81725();
    
	string PrintErrorCode(int errorCode);
	int LoadMz(const unsigned int &loadFwMz, const mzPolarity * setPolMz);
    int init(const string & mEnvStr);
	int loadFirmware(const unsigned int &bus);
	int warmInit(const Bcm81725Lane& param);

    void dumpLog(std::ostream &os);
    void dumpStatus(std::ostream &os, std::string cmd);
    void resetLog( std::ostream &os );
    void addLog(const std::string &func, uint32_t line, const std::string &text);

    int getPolarity(const Bcm81725Lane& param, unsigned int& tx, unsigned int& rx);
	int setPolarity(const Bcm81725Lane& param, unsigned int tx, unsigned int rx);
	int iWrite(const Bcm81725Lane& param, uint32 regAddr, uint16 data);
	int setAvsConfig(const Bcm81725Lane& param, unsigned int enable, unsigned int margin);
	int getAvsConfig(const Bcm81725Lane& param, unsigned int& enable, unsigned int& disableType, 
		             unsigned int& ctrl, unsigned int& regulator, unsigned int& pkgShare, unsigned int& margin);
	int getAvsStatus(const Bcm81725Lane& param, unsigned int& status);
	int getMode(const Bcm81725Lane& param, int& speed, int& ifType, int& refClk, int& interfaceMode,
                unsigned int& dataRate, unsigned int& modulation, unsigned int& fecType);
	int setMode(const Bcm81725Lane& param, int speed, int ifType, int refClk, int interfaceMode,
                unsigned int dataRate, unsigned int modulation, unsigned int fecType);
	int prbsLinkStatusCheck(const Bcm81725Lane& param, unsigned int& status);
	int prbsStatusCheck(const Bcm81725Lane& param, unsigned int timeVal, unsigned int& status);
	int setPrbsGen(const Bcm81725Lane& param, unsigned int tx, unsigned int poly, 
                   unsigned int inv, unsigned int lb, unsigned int enaDis);
	int setPrbsCheck(const Bcm81725Lane& param, unsigned int rx, unsigned int poly, 
                   unsigned int inv, unsigned int lb, unsigned int enaDis);
	int setLoopback(const Bcm81725Lane& param, unsigned int mode, unsigned int enable);

private:
	std::recursive_mutex myBcmMtx;
	const char*  myMilleniObPtr;
	FpgaMdioIf* myMdioRAPtr;
	FpgaRegIf* myFpgaRAPtr;
    // std::recursive_mutex myMdioMtx;
	std::mutex myLogMtx;
	SimpleLog::Log*  myLogPtr;


};

int mdio_read(void *user, unsigned int mdioAddr, unsigned int regAddr, unsigned int *data);
int mdio_write(void *user, unsigned int mdioAddr, unsigned int regAddr, unsigned int data);

}
#endif // __GEARBOXDRIVER_H_
