#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include <QObject>
#include "stadiccontrol.h"
#include <vector>
#include <string>
#include <QStringList>
#include "radfiledata.h"

#include "stadicapi.h"

namespace stadic {
class STADIC_API Daylight : public QObject
{
    Q_OBJECT
public:
    explicit Daylight(Control *model, QObject *parent = 0);                         //Constructor that takes a Control object as an argument
    bool simDaylight();                                                             //Function to simulate the daylight

private:
    bool simBSDF(int blindGroupNum, int setting, int bsdfNum,std::string bsdfRad,std::string remainingRad,std::vector<double> normal,std::string thickness,std::string bsdfXML, std::string bsdfLayer, Control *model);         //Function for simulating a BSDF case
    bool simStandard(int blindGroupNum, int setting, Control *model);               //Function to simulate the standard radiance material cases
    bool simCase1(int blindGroupNum, Control *model);                               //Function for simulating simCase1 : window groups that do not contain BSDFs
    bool simCase2(int blindGroupNum, Control *model);                               //Function for simulating simCase2 : window groups that contain BSDFs, but not in the base case
    bool simCase3(int blindGroupNum, Control *model);                               //Function for simulating simCase3 : window groups that contain BSDFs even in the base case, but the glazing layers are not BSDFs
    bool simCase4(int blindGroupNum, Control *model);                               //Function for simulating simCase4 : window groups that have shade materials in addition to the glazing layer which is a BSDF
    bool simCase5(int blindGroupNum, Control *model);                               //Function for simulating simCase5 : window groups that have added geometry, but it is a proxy geometry
    bool simCase6(int blindGroupNum, Control *model);                               //Function for simulating simCase6 : window groups that only have the glazing layer as a BSDF
    bool uniqueGlazingMaterials(Control *model);                                    //Function to ensure that the each window group contains unique glazing materials
    bool testSimCase(Control *model);                                               //Function to determine the simulation case for each window group
    bool setSimCase(int setting, int simCase);                                      //Function to set the simulation case for a window group
    bool writeSky(Control *model);                                                  //Function to write the sky rad file
    bool createBaseRadFiles(Control *model);                                        //Function to create the base rad files
    bool createOctree(std::vector<std::string> files, std::string octreeName);      //Function to create an octree given a vector of files
    bool sumIlluminanceFiles(Control *model);                                       //Function to sum the illuminance files for each window group setting

    std::vector<int> m_SimCase;                                                     //Vector holding the simulation case for each window group
    Control *m_Model;                                                               //Control object
    std::vector<RadFileData* > m_RadFiles;                                          //Vector of RadFileData objects

signals:

public slots:

};

}
#endif // DAYLIGHT_H
