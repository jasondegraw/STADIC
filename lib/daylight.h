/******************************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

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
    explicit Daylight(Control *model);                         //Constructor that takes a Control object as an argument
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

};

}
#endif // DAYLIGHT_H
