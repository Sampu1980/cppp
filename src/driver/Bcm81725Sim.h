#ifndef     BCM81725SIM_H
#define    BCM81725SIM_H

#include <iostream>

using namespace std;

namespace gearboxsim {

class Bcm81725Sim
{
public:
	Bcm81725Sim();
	~Bcm81725Sim();
    
    int init(const string &mEnvStr);
private:

};

}
#endif // BCM81725SIM_H

