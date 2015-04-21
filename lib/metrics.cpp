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

#include "metrics.h"
#include "logging.h"
#include "dayill.h"
#include <fstream>
#include "functions.h"

namespace stadic {
Metrics::Metrics(BuildingControl *model) :
    m_Model(model)
{
}

bool Metrics::processMetrics()
{
    std::vector<std::shared_ptr<Control>> spaces=m_Model->spaces();
    for (int i=0;i<spaces.size();i++){
        DaylightIlluminanceData daylightIll;
        daylightIll.parseTimeBased(spaces[i].get()->spaceDirectory()+spaces[i].get()->resultsDirectory()+spaces[i].get()->spaceName()+".ill");
        //Test whether Daylight Autonomy needs to be calculated
        if (spaces[i].get()->runDA()){
            if(calculateDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcDA(false);      //Set calculate to false for DA in control file if returned true
            }
        }

        if (spaces[i].get()->runcDA()){
            if (calculatecDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalccDA(false);
            }
        }

        if (spaces[i].get()->runDF()){
            if (calculateDF(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setDF(false);
            }
        }

        if (spaces[i].get()->runUDI()){
            if (calculateUDI(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcUDI(false);
            }
        }

        if (spaces[i].get()->runsDA()){
            if (calculatesDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcsDA(false);
            }
        }

        if (spaces[i].get()->runOccsDA()){
            if (calculateOccsDA(spaces[i].get(), &daylightIll)){
                spaces[i].get()->setCalcOccsDA(false);
            }
        }
    }

    return true;
}

bool Metrics::calculateDA(Control *model, DaylightIlluminanceData *dayIll)
{
    std::vector<int> pointCount;
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        pointCount.push_back(0);
    }
    int hourCount=0;
    for (int i=0;i<dayIll->illuminance().size();i++){
        if(m_Occupancy[i]){
            hourCount++;
            if (model->illumUnits()=="lux"){
                for (int j=0;j<dayIll->illuminance()[i].lux().size();j++){
                    if (dayIll->illuminance()[i].lux()[j]>model->DAIllum()){
                        pointCount[j]++;
                    }
                }
            }else{
                for (int j=0;j<dayIll->illuminance()[i].fc().size();j++){
                    if (dayIll->illuminance()[i].fc()[j]>model->DAIllum()){
                        pointCount[j]++;
                    }
                }
            }
        }
    }

    std::ofstream outDA;
    std::string tmpFileName;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_DA.res";
    outDA.open(tmpFileName);
    if (!outDA.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the Daylight Autonomy results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outDA<<double(pointCount[i])/hourCount<<std::endl;
    }
    outDA.close();
    return true;
}
bool Metrics::calculatecDA(Control *model, DaylightIlluminanceData *dayIll)
{
    std::vector<double> pointCount;
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        pointCount.push_back(0);
    }
    int hourCount=0;
    for (int i=0;i<dayIll->illuminance().size();i++){
        if(m_Occupancy[i]){
            hourCount++;
            if (model->illumUnits()=="lux"){
                for (int j=0;j<dayIll->illuminance()[i].lux().size();j++){
                    pointCount[j]=pointCount[j]+dayIll->illuminance()[i].lux()[j]/model->cDAIllum();
                }
            }else{
                for (int j=0;j<dayIll->illuminance()[i].fc().size();j++){
                    pointCount[j]=pointCount[j]+dayIll->illuminance()[i].fc()[j]/model->cDAIllum();
                }
            }
        }
    }

    std::ofstream outcDA;
    std::string tmpFileName;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_cDA.res";
    outcDA.open(tmpFileName);
    if (!outcDA.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the Continuous Daylight Autonomy results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outcDA<<double(pointCount[i])/hourCount<<std::endl;
    }
    outcDA.close();

    return true;
}
bool Metrics::calculateDF(Control *model, DaylightIlluminanceData *dayIll)
{

    return true;
}
bool Metrics::calculateUDI(Control *model, DaylightIlluminanceData *dayIll)
{
    std::vector<double> countWithin;
    std::vector<double> countBelow;
    std::vector<double> countAbove;
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        countWithin.push_back(0);
        countBelow.push_back(0);
        countAbove.push_back(0);
    }
    int hourCount=0;
    for (int i=0;i<dayIll->illuminance().size();i++){
        if(m_Occupancy[i]){
            hourCount++;
            if (model->illumUnits()=="lux"){
                for (int j=0;j<dayIll->illuminance()[i].lux().size();j++){
                    if (dayIll->illuminance()[i].lux()[j]<model->UDIMin()){
                        countBelow[j]++;
                    }else if(dayIll->illuminance()[i].lux()[j]<=model->UDIMax()){
                        countWithin[j]++;
                    }else{
                        countAbove[j]++;
                    }
                }
            }else{
                for (int j=0;j<dayIll->illuminance()[i].fc().size();j++){
                    if (dayIll->illuminance()[i].fc()[j]<model->UDIMin()){
                        countBelow[j]++;
                    }else if(dayIll->illuminance()[i].fc()[j]<=model->UDIMax()){
                        countWithin[j]++;
                    }else{
                        countAbove[j]++;
                    }
                }
            }
        }
    }

    std::ofstream outUDIbelow;
    std::string tmpFileName;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_below_UDI.res";
    outUDIbelow.open(tmpFileName);
    if (!outUDIbelow.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the below UDI results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outUDIbelow<<double(countBelow[i])/hourCount<<std::endl;
    }
    outUDIbelow.close();

    std::ofstream outUDI;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_UDI.res";
    outUDI.open(tmpFileName);
    if (!outUDI.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the UDI results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outUDI<<double(countWithin[i])/hourCount<<std::endl;
    }
    outUDI.close();

    std::ofstream outUDIabove;
    tmpFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_above_UDI.res";
    outUDIabove.open(tmpFileName);
    if (!outUDIabove.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the above UDI results file "+tmpFileName +" has failed.");
        return false;
    }
    for (int i=0;i<dayIll->illuminance()[0].lux().size();i++){
        outUDIabove<<double(countAbove[i])/hourCount<<std::endl;
    }
    outUDIabove.close();

    return true;
}
bool Metrics::calculatesDA(Control *model, DaylightIlluminanceData *dayIll)
{

    return true;
}
bool Metrics::calculateOccsDA(Control *model, DaylightIlluminanceData *dayIll)
{

    return true;
}
bool Metrics::parseOccupancy(std::string file, double threshold){
    std::ifstream occFile;
    occFile.open(file);
    if (!occFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the occupancy file "+file+" has failed.");
        return false;
    }
    std::string line;
    while (std::getline(occFile, line)){
        std::vector<std::string> vals;
        vals=split(line, ' ');
        if (toDouble(vals[3])<threshold){
            m_Occupancy.push_back(false);
        }else{
            m_Occupancy.push_back(true);
        }
    }
    occFile.close();
    return true;
}

}
