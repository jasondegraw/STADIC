#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include "stadiccontrol.h"
#include <vector>
#include <string>
#include "radfiledata.h"

#include "stadicapi.h"

namespace stadic {
class STADIC_API Daylight
{
public:
    explicit Daylight(Control *model);
    bool simDaylight();

private:
    bool simBSDF(int blindGroupNum, int setting, int bsdfNum,std::string bsdfRad,std::string remainingRad,std::vector<double> normal,std::string thickness,
        std::string bsdfXML, std::string bsdfLayer, Control *model);
    bool simStandard(int blindGroupNum, int setting, Control *model);
    bool simCase1(int blindGroupNum, Control *model);
    bool simCase2(int blindGroupNum, Control *model);
    bool simCase3(int blindGroupNum, Control *model);
    bool simCase4(int blindGroupNum, Control *model);
    bool simCase5(int blindGroupNum, Control *model);
    bool simCase6(int blindGroupNum, Control *model);
    bool uniqueGlazingMaterials(Control *model);
    bool testSimCase(Control *model);
    bool setSimCase(int setting, int simCase);
    bool writeSky(Control *model);
    bool createBaseRadFiles(Control *model);
    bool createOctree(std::vector<std::string> files, std::string octreeName);
    bool sumIlluminanceFiles(Control *model);

    std::vector<int> m_SimCase;
    Control *m_Model;
    std::vector<RadFileData> m_RadFiles;

};

}
#endif // DAYLIGHT_H
