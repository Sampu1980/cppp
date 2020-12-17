#ifndef __GEARBOXIF_H__
#define __GEARBOXIF_H__
#include <string>

namespace gearbox {

struct Bcm81725Lane
{
	unsigned int bus;
	unsigned int mdioAddr;
	unsigned int side;
	unsigned int laneNum;
};

class GearBoxIf
{
public:
	virtual int init(const std::string & mEnvStr) = 0;
	virtual int loadFirmware(const unsigned int &bus) = 0;
};

}
#endif // __GEARBOXIF_H_
