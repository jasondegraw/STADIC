/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
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

#include "processshade.h"
#include "logging.h"
#include "dayill.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "functions.h"

namespace stadic {
ProcessShade::ProcessShade(BuildingControl *model) :
    m_Model(model)
{
}

bool ProcessShade::processShades()
{
    std::vector<std::shared_ptr<Control>> spaces=m_Model->spaces();
    for (int i=0;i<spaces.size();i++){
        //populate the time interval vector
        m_TimeIntervals.clear();
        DaylightIlluminanceData timeData(spaces[i]->illumUnits());
        if (!timeData.parseTimeBased(spaces[i].get()->spaceDirectory()+spaces[i].get()->resultsDirectory()+spaces[i].get()->spaceName()+"_"+spaces[i].get()->windowGroups()[0].name()+"_base.ill")){
            return false;
        }
        for (int j=0;j<timeData.illuminance().size();j++){
            std::vector<std::string> tempVec;
            tempVec.push_back(toString(timeData.illuminance()[j].month()));
            tempVec.push_back(toString(timeData.illuminance()[j].day()));
            tempVec.push_back(toString(timeData.illuminance()[j].hour()));
            m_TimeIntervals.push_back(tempVec);
        }

        //start processing shade algorithms
        if (!makeShadeSched(spaces[i].get())){
            STADIC_LOG(Severity::Error, "The creation of the shade schedule for "+spaces[i].get()->spaceName()+" has failed.");
            return false;
        }
    }

    return true;
}
bool ProcessShade::writeSched(std::vector<std::vector<int>> shadeSched, std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the file "+ file + " has failed.");
        return false;
    }
    //Write out the shade schedule here.  M D H WG1Set WG2Set...
    for (int i=0;i<m_TimeIntervals.size();i++){
        oFile<<m_TimeIntervals[i][0]<<" "<<m_TimeIntervals[i][1]<<" "<<m_TimeIntervals[i][2];
        for (int j=0;j<shadeSched[i].size();j++){
            oFile<<" "<<shadeSched[i][j];
        }
        oFile<<std::endl;
    }

    oFile.close();
    return true;
}

bool ProcessShade::makeShadeSched(Control *model){
    std::vector<std::vector<int>> shadeSchedule;
    //determine schedule for each window group
    for (int j=0;j<model->windowGroups().size();j++){
        if (model->windowGroups()[j].shadeControl()->controlMethod()=="automated_signal"){
            shadeSchedule.push_back(automatedSignal(model, j));
        }else if (model->windowGroups()[j].shadeControl()->controlMethod()=="automated_profile_angle"){
            shadeSchedule.push_back(automatedProfileAngle(model, j));
        }else if (model->windowGroups()[j].shadeControl()->controlMethod()=="automated_profile_angle_signal"){
            shadeSchedule.push_back(automatedProfileAngleSignal(model, j));
        }
    }
    //Write out completed schedule
    if (!writeSched(shadeSchedule, model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_shade.sch")){
        return false;
    }
    //determine final illuminance values using completed schedule


    //Write Illuminance File


    return true;
}
//Shade Control Algorithms
std::vector<int> ProcessShade::automatedSignal(Control *model, int windowGroup){
    std::vector<int> shadeSchedule;


    return shadeSchedule;
}

std::vector<int> ProcessShade::automatedProfileAngle(Control *model, int windowGroup){
    std::vector<int> shadeSchedule;


    return shadeSchedule;
}
std::vector<int> ProcessShade::automatedProfileAngleSignal(Control *model, int windowGroup){
    std::vector<int> shadeSchedule;


    return shadeSchedule;
}



}
