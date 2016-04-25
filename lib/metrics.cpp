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
#include "elecill.h"
#include <fstream>
#include "functions.h"
#include "gridmaker.h"

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
    for (auto v: dayIll->illuminance()[0].lux()){
        pointCount.push_back(0);
    }
    int hourCount=0;
    for (auto v: dayIll->illuminance()){
        if (m_Occupancy[hourCount]){
            if (model->illumUnits()=="lux"){
                int pointIndex=0;
                for (auto p: v.lux()){
                    if (p>model->DAIllum()){
                        pointCount[pointIndex]++;
                    }
                    pointIndex++;
                }
            }else{
                int pointIndex=0;
                for (auto p: v.fc()){
                    if (p>model->DAIllum()){
                        pointCount[pointIndex]++;
                    }
                    pointIndex++;
                }
            }
        }
        hourCount++;
    }

    /*
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
    */
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
    std::vector<int> pointCount;
    for (auto v: dayIll->illuminance()[0].lux()){
        pointCount.push_back(0);
    }
    int hourCount=0;
    for (auto v: dayIll->illuminance()){
        if (m_Occupancy[hourCount]){
            if (model->illumUnits()=="lux"){
                int pointIndex=0;
                for (auto p: v.lux()){
                    pointCount[pointIndex]=pointCount[pointIndex]+p/model->cDAIllum();
                    pointIndex++;
                }
            }else{
                int pointIndex=0;
                for (auto p: v.fc()){
                    pointCount[pointIndex]=pointCount[pointIndex]+p/model->cDAIllum();
                    pointIndex++;
                }
            }
        }
        hourCount++;
    }
    /*
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
    */
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
    for (auto v: dayIll->illuminance()){
        if (m_Occupancy[hourCount]){
            if (model->illumUnits()=="lux"){
                int pointIndex=0;
                for (auto p: v.lux()){
                    if (p<model->UDIMin()){
                        countBelow[pointIndex]++;
                    }else if (p<=model->UDIMax()){
                        countWithin[pointIndex]++;
                    }else{
                        countAbove[pointIndex]++;
                    }
                    pointIndex++;
                }
            }else{
                int pointIndex=0;
                for (auto p: v.fc()){
                    if (p<model->UDIMin()){
                        countBelow[pointIndex]++;
                    }else if (p<=model->UDIMax()){
                        countWithin[pointIndex]++;
                    }else{
                        countAbove[pointIndex]++;
                    }
                    pointIndex++;
                }
            }
        }
        hourCount++;
    }
    /*
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
    */
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
    //Calculate the area of the floor polygons
    GridMaker gridSize(model->spaceDirectory()+model->geoDirectory()+model->geoFile());
    if (model->identifiers()){
        gridSize.setIdentifiers(model->identifiers().get());
    }else if (model->modifiers()){
        gridSize.setLayerNames(model->modifiers().get());
    }else{
        STADIC_LOG(Severity::Warning, "The calculation of sDA for "+model->spaceName()+" cannot be completed without a list of floor geometry.");
        return false;
    }
    if (!gridSize.calcArea()){
        STADIC_LOG(Severity::Error, "The calculation of the floor area for "+model->spaceName()+" has failed.");
        return false;
    }
    double area;
    area=gridSize.area();
    //Correct all values to be in feet.
    if (model->importUnits()=="mm"){
        area=area*0.00328084*0.00328084;
    }else if (model->importUnits()=="in"){
        area=area/144;
    }else if (model->importUnits()=="m"){
        area=area*3.28084*3.28084;
    }

    //Calculate the target sDA Percentage to stay under with direct sun.
    double sDAPercent=0.02;
    if (area<200){
        sDAPercent=2.0/dayIll->illuminance()[0].lux().size();
    }else if (area<500){
        sDAPercent=4.0/dayIll->illuminance()[0].lux().size();
    }else if (area<1000){
        sDAPercent=6.0/dayIll->illuminance()[0].lux().size();
    }
    if (sDAPercent<0.02){
        sDAPercent=0.2;
    }

    //Create possible shade combinations array
    std::vector<std::vector<bool>> combinations;
    combinations.resize(int(std::pow(2, model->windowGroups().size())));        //Resizes the main vector to the number of combinations
    for (int i=0;i<combinations.size();i++){
        combinations[i].resize(model->windowGroups().size());                   //Resizes the inner vectors to the number of window groups
    }
    for (int i=0;i<model->windowGroups().size();i++){                           //This section fills the first window group with [on, off, on, off,...]
        int count=0;                                                            //The second window group with [on, on, off, off, ...]
        bool shadesEmployed=true;
        for (int j=0;j<combinations.size();j++){
            count++;
            combinations[j][i]=shadesEmployed;
            if (count==int(std::pow(2,i))){
                count=0;
                if (shadesEmployed==true){
                    shadesEmployed=false;
                }else{
                    shadesEmployed=true;
                }
            }
        }
    }

    //Parse the direct illuminance files and store objects in vectors for both base and setting cases.
    std::vector<DaylightIlluminanceData> baseDirectIlls;
    for (int i=0;i<model->windowGroups().size();i++){
        DaylightIlluminanceData illBase;
        illBase.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_direct.ill");
        baseDirectIlls.push_back(illBase);
    }
    //Calculate ASE
    std::vector<int> countASE;
    countASE.resize(baseDirectIlls[0].illuminance()[0].lux().size());
    for (int i=0;i<countASE.size();i++){
        countASE[i]=0;
    }
    std::vector<std::vector<double>> tempIll;
    tempIll.resize(baseDirectIlls[0].illuminance().size());
    int j=0;
    for (auto v: baseDirectIlls[0].illuminance()){
        tempIll[j].resize(countASE.size());
        j++;
    }

    for (int i=0;i<baseDirectIlls.size();i++){                  //Loop over the illuminance files
        int hours=0;
        for (auto v: baseDirectIlls[i].illuminance()){          //Loop over the hours in the year for each illuminance file
            int points=0;
            for (auto p: v.lux()){
                tempIll[hours][points]=p;
                points++;
            }
            hours++;
        }
    }
    for (int i=0;i<tempIll.size();i++){
        for (int j=0;j<tempIll[i].size();j++){
            if (tempIll[i][j]>1000){
                countASE[j]++;
            }
        }
    }
    int totalPoints=0;
    for (int i=0;i<countASE.size();i++){
        if (countASE[i]>250){
            totalPoints++;
        }
    }

    /*
    std::vector<int> countASE;
    for (int i=0;i<baseDirectIlls[0].illuminance()[0].lux().size();i++){            //Loop over points
        countASE.push_back(0);
        for (int j=0;j<baseDirectIlls[0].illuminance().size();j++){                 //Loop over hours in the year
            if (baseDirectIlls[0].illuminance()[j].hour()>=model->sDAStart()&&baseDirectIlls[0].illuminance()[j].hour()<=model->sDAEnd()){
                double tempIll=0;
                for (int k=0;k<baseDirectIlls.size();k++){                          //Loop over window groups
                    tempIll=tempIll+baseDirectIlls[k].illuminance()[j].lux()[i];    //Sum the direct illuminance from window groups
                }
                if (tempIll>1000){                                                  //Test whether it is over the threshold of 1000 lux and add 1 to the count if it is
                    countASE[i]++;
                }
            }
        }
    }
    int totalPoints=0;
    for (int i=0;i<countASE.size();i++){
        if (countASE[i]>250){
            totalPoints++;
        }
    }
    */
    //Write out ASE
    std::ofstream outASE;
    outASE.open(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_ASE.res");
    if (!outASE.is_open()){
        STADIC_LOG(Severity::Warning, "The results file for the ASE calculation failed to open for "+model->spaceName()+".");
    }else{
        outASE<<"area= "<<area<<std::endl;
        outASE<<"ASE= "<<double(totalPoints)/countASE.size()<<std::endl;
        outASE.close();
    }

    //Find the combination closest to the sDAPercent without going over and produce shade schedule
    std::vector<std::vector<int>> shadeSchedule;
    shadeSchedule.resize(baseDirectIlls[0].illuminance().size());
    for (int i=0;i<baseDirectIlls[0].illuminance().size();i++){
        for (int j=0;j<model->windowGroups().size();j++){
            shadeSchedule[i].push_back(0);
        }
    }
    for (int i=0;i<baseDirectIlls[0].illuminance().size();i++){         //Loop through the number of hours in the year
        bool allZeros=true;
        for (int p=0;p<model->windowGroups().size();p++){
            if (!baseDirectIlls[p].illuminance()[i].allZeros()){
                allZeros=false;
            }
        }
        if (!allZeros){
            std::vector<double> fracDirectSun;                              //Fraction of direct sun per window group
            for (int j=0;j<model->windowGroups().size();j++){
                fracDirectSun.push_back(baseDirectIlls[j].illuminance()[i].fractionAboveTarget(300));
            }
            int closestCombo;                                               //Variable to hold the index of the best combination
            double tempFracTotal=0;
            double min=1;
            for (int j=0;j<combinations.size();j++){                        //Loop through the possible combinations
                for (int k=0;k<fracDirectSun.size();k++){
                    if (!combinations[j][k]){                                  //True if the shades are employed
                        tempFracTotal=tempFracTotal+fracDirectSun[k];
                    }
                }
                if (sDAPercent>tempFracTotal && (sDAPercent-tempFracTotal)<=min){
                    closestCombo=j;
                    min=sDAPercent-tempFracTotal;
                }
            }
            for (int j=0;j<model->windowGroups().size();j++){
                if (combinations[closestCombo][j]){
                    shadeSchedule[i][j]=model->sDAwgSettings()[j];
                }
            }
        }
    }

    //Write out the sDA shade option file
    std::ofstream sDAShades;
    std::string tempFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_sDA_ShadeSchedule.res";
    sDAShades.open(tempFileName);
    if (!sDAShades.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the file "+tempFileName+" has failed.");
        return false;
    }
    for (int i=0;i<shadeSchedule.size();i++){
        sDAShades<<toString(baseDirectIlls[0].illuminance()[i].month())<<" ";
        sDAShades<<toString(baseDirectIlls[0].illuminance()[i].day())<<" ";
        sDAShades<<toString(baseDirectIlls[0].illuminance()[i].hour());
        for (int j=0;j<shadeSchedule[i].size();j++){
            sDAShades<<" "<<toString(shadeSchedule[i][j])<<std::endl;
        }
    }

    //Combine Illuminance files based on shade option schedule
    std::vector<DaylightIlluminanceData> baseIlls;
    std::vector<DaylightIlluminanceData> settingIlls;
    for (int i=0;i<model->windowGroups().size();i++){
        DaylightIlluminanceData illBase;
        illBase.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base.ill");
        baseIlls.push_back(illBase);
        DaylightIlluminanceData illSetting;
        illSetting.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+toString(model->sDAwgSettings()[i])+".ill");
        settingIlls.push_back(illSetting);
    }
    DaylightIlluminanceData finalIlluminance;
    std::vector<double> finalTemporalIll;
    for (int i=0;i<shadeSchedule.size();i++){               //Loop over the entire year
        finalTemporalIll.clear();
        for (int j=0;j<baseIlls[0].illuminance()[0].lux().size();j++){      //Set the illuminance vector to zero for all points
            finalTemporalIll.push_back(0);
        }
        for (int j=0;j<shadeSchedule[i].size();j++){        //Loop over window groups
            if (shadeSchedule[i][j]){           //Shades Employed
                for (int k=0;k<settingIlls[j].illuminance()[i].lux().size();k++){
                    finalTemporalIll[k]=finalTemporalIll[k] + settingIlls[j].illuminance()[i].lux()[k];
                }
            }else{                              //Use base condition
                for (int k=0;k<baseIlls[j].illuminance()[i].lux().size();k++){
                    finalTemporalIll[k]=finalTemporalIll[k] + baseIlls[j].illuminance()[i].lux()[k];
                }
            }
        }
        TemporalIlluminance dataPoint(baseDirectIlls[0].illuminance()[i].month(), baseDirectIlls[0].illuminance()[i].day(), baseDirectIlls[0].illuminance()[i].hour(), finalTemporalIll);
        finalIlluminance.addDataPoint(dataPoint);
    }
    //Write out sDA by point (DA with sDA shade control)
    int countHours=0;
    std::vector<int> sDACount;
    sDACount.resize(finalIlluminance.illuminance()[0].lux().size());
    if (model->illumUnits()=="lux"){
        for (auto v: finalIlluminance.illuminance()){
            if (v.hour()>=model->sDAStart() && v.hour()<=model->sDAEnd()){
                countHours++;
                int j=0;
                for (auto p: v.lux()){
                    if (p>model->sDAIllum()){
                        sDACount[j]++;
                    }
                    j++;
                }
            }
        }
    }else{
        for (auto v: finalIlluminance.illuminance()){
            if (v.hour()>=model->sDAStart()&& v.hour()<=model->sDAEnd()){
                countHours++;
                int j=0;
                for (auto p: v.fc()){
                    if (p>model->sDAIllum()){
                        sDACount[j]++;
                    }
                    j++;
                }
            }
        }
    }
    /*
    if (model->illumUnits()=="lux"){
        for (int i=0;i<finalIlluminance.illuminance().size();i++){          //Loop over the whole year
            if (finalIlluminance.illuminance()[i].hour()>=model->sDAStart()&&finalIlluminance.illuminance()[i].hour()<=model->sDAEnd()){
                countHours++;
                for (int j=0;j<finalIlluminance.illuminance()[0].lux().size();j++){
                    if (finalIlluminance.illuminance()[i].lux()[j]>model->sDAIllum()){
                        sDACount[j]++;
                    }
                }
            }
        }
    }else{
        for (int i=0;i<finalIlluminance.illuminance().size();i++){          //Loop over the whole year
            if (finalIlluminance.illuminance()[i].hour()>=model->sDAStart()&&finalIlluminance.illuminance()[i].hour()<=model->sDAEnd()){
                countHours++;
                for (int j=0;j<finalIlluminance.illuminance()[0].fc().size();j++){
                    if (finalIlluminance.illuminance()[i].fc()[j]>model->sDAIllum()){
                        sDACount[j]++;
                    }
                }
            }
        }
    }
    */
    finalIlluminance.writeIllFileLux(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_sDA.ill");
    int finalsDA=0;
    for (int i=0;i<sDACount.size();i++){
        if (sDACount[i]/double(countHours)>model->sDAFrac()){
            finalsDA++;
        }
    }
    std::ofstream sDAPoint;
    tempFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_sDA_Points.res";
    sDAPoint.open(tempFileName);
    if (!sDAPoint.is_open()){
        STADIC_LOG(Severity::Warning, "The opening of the sDA points result file "+tempFileName+" has failed.");
        return false;
    }
    sDAPoint<<"area= "<<area<<std::endl;
    sDAPoint<<"points= "<<sDACount.size()<<std::endl;
    sDAPoint<<"sDA= "<<finalsDA/double(sDACount.size())<<std::endl;
    for (int i=0;i<sDACount.size();i++){
        sDAPoint<<toString(sDACount[i]/double(countHours))<<std::endl;
    }
    sDAPoint.close();


    return true;
}
bool Metrics::calculateOccsDA(Control *model, DaylightIlluminanceData *dayIll)
{
    //Calculate the area of the floor polygons
    GridMaker gridSize(model->spaceDirectory()+model->geoDirectory()+model->geoFile());
    if (model->identifiers()){
        gridSize.setIdentifiers(model->identifiers().get());
    }else if (model->modifiers()){
        gridSize.setLayerNames(model->modifiers().get());
    }else{
        STADIC_LOG(Severity::Warning, "The calculation of sDA for "+model->spaceName()+" cannot be completed without a list of floor geometry.");
        return false;
    }
    if (!gridSize.calcArea()){
        STADIC_LOG(Severity::Error, "The calculation of the floor area for "+model->spaceName()+" has failed.");
        return false;
    }
    double area;
    area=gridSize.area();
    //Correct all values to be in feet.
    if (model->importUnits()=="mm"){
        area=area*0.00328084*0.00328084;
    }else if (model->importUnits()=="in"){
        area=area/144;
    }else if (model->importUnits()=="m"){
        area=area*3.28084*3.28084;
    }
    //Parse the shade option file
    std::vector<std::vector<int>> shadeSchedule;
    shadeSchedule.resize(dayIll->illuminance().size());
    for (int i=0;i<shadeSchedule.size();i++){
        for (int j=0;j<model->windowGroups().size();j++){
            shadeSchedule[i].push_back(0);
        }
    }
    std::ifstream shadeFile;
    shadeFile.open(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_shades.sch");
    if (!shadeFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the shade schedule file "+model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_shades.sch has failed.");
        return false;
    }
    std::string line;
    int lineCounter=0;
    while (std::getline(shadeFile, line)){
        std::vector<std::string> vals;
        vals=trimmedSplit(line, ',');
        for (int i=3;i<vals.size();i++){
            shadeSchedule[lineCounter][i-3]=toInteger(vals[i]);
        }
        lineCounter++;
    }


    //Combine Illuminance files based on shade option schedule
    std::vector<DaylightIlluminanceData> baseIlls;
    std::vector<std::vector<DaylightIlluminanceData>> settingIlls;
    settingIlls.resize(model->windowGroups().size());
    for (int i=0;i<model->windowGroups().size();i++){
        DaylightIlluminanceData illBase;
        illBase.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base.ill");
        baseIlls.push_back(illBase);
        for (int j=0;j<model->windowGroups()[i].shadeSettingGeometry().size();j++){
            DaylightIlluminanceData illSetting;
            illSetting.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+toString(j+1)+".ill");
            settingIlls[i].push_back(illSetting);
        }

    }
    DaylightIlluminanceData finalIlluminance;
    std::vector<double> finalTemporalIll;
    for (int i=0;i<shadeSchedule.size();i++){               //Loop over the entire year
        finalTemporalIll.clear();
        for (int j=0;j<baseIlls[0].illuminance()[0].lux().size();j++){      //Set the illuminance vector to zero for all points
            finalTemporalIll.push_back(0);
        }
        for (int j=0;j<shadeSchedule[i].size();j++){        //Loop over window groups
            if (shadeSchedule[i][j]){           //Shades Employed
                for (int k=0;k<settingIlls[j][shadeSchedule[i][j]-1].illuminance()[i].lux().size();k++){
                    finalTemporalIll[k]=finalTemporalIll[k] + settingIlls[j][shadeSchedule[i][j]-1].illuminance()[i].lux()[k];
                }
            }else{                              //Use base condition
                for (int k=0;k<baseIlls[j].illuminance()[i].lux().size();k++){
                    finalTemporalIll[k]=finalTemporalIll[k] + baseIlls[j].illuminance()[i].lux()[k];
                }
            }
        }
        TemporalIlluminance dataPoint(baseIlls[0].illuminance()[i].month(), baseIlls[0].illuminance()[i].day(), baseIlls[0].illuminance()[i].hour(), finalTemporalIll);
        finalIlluminance.addDataPoint(dataPoint);
    }
    //Write out sDA by point (DA with sDA shade control)
    int countHours=0;
    std::vector<int> sDACount;
    sDACount.resize(finalIlluminance.illuminance()[0].lux().size());
    if (model->illumUnits()=="lux"){
        for (int i=0;i<finalIlluminance.illuminance().size();i++){          //Loop over the whole year
            if (m_Occupancy[i]>(0.1)){                                      //This threshold should probably come from the user.
                countHours++;
                for (int j=0;j<finalIlluminance.illuminance()[0].lux().size();j++){
                    if (finalIlluminance.illuminance()[i].lux()[j]>model->occsDAIllum()){
                        sDACount[j]++;
                    }
                }
            }
        }
    }else{
        for (int i=0;i<finalIlluminance.illuminance().size();i++){          //Loop over the whole year
            if (m_Occupancy[i]>(0.1)){
                countHours++;
                for (int j=0;j<finalIlluminance.illuminance()[0].fc().size();j++){
                    if (finalIlluminance.illuminance()[i].fc()[j]>model->occsDAIllum()){
                        sDACount[j]++;
                    }
                }
            }
        }
    }

    finalIlluminance.writeIllFileLux(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_occupancy_sDA.ill");
    int finalsDA=0;
    for (int i=0;i<sDACount.size();i++){
        if (sDACount[i]/double(countHours)>model->occsDAFrac()){
            finalsDA++;
        }
    }
    std::ofstream sDAPoint;
    std::string tempFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_occupancy_sDA_Points.res";
    sDAPoint.open(tempFileName);
    if (!sDAPoint.is_open()){
        STADIC_LOG(Severity::Warning, "The opening of the sDA points result file "+tempFileName+" has failed.");
        return false;
    }
    sDAPoint<<"area= "<<area<<std::endl;
    sDAPoint<<"points= "<<sDACount.size()<<std::endl;
    sDAPoint<<"occupancy_sDA= "<<finalsDA/double(sDACount.size())<<std::endl;
    for (int i=0;i<sDACount.size();i++){
        sDAPoint<<toString(sDACount[i]/double(countHours))<<std::endl;
    }
    sDAPoint.close();

    return true;
}
bool Metrics::parseOccupancy(std::string file, double threshold){
    std::ifstream occFile;
    occFile.open(file);
    if (!occFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the occupancy csv file "+file+" has failed.");
        return false;
    }
    std::string line;
    while (std::getline(occFile, line)){
        std::vector<std::string> vals;
        vals=split(line, ',');
        if (toDouble(vals[3])<threshold){
            m_Occupancy.push_back(false);
        }else{
            m_Occupancy.push_back(true);
        }
    }
    occFile.close();
    return true;
}

bool Metrics::calculateCP(Control *model, DaylightIlluminanceData *dayIll){
    //Check to see how many dimmed zones there are.  Currently the critical point analysis can only handle one dimmed zone
    int numDimZones;
    numDimZones=0;
    for (int i=0;i<model->controlZones().size();i++){
        if (model->controlZones()[i].algorithm()!="null"){
            numDimZones++;
        }
    }
    if (numDimZones>1){
        STADIC_LOG(Severity::Error, "The Critical Point analysis cannot be ran for the space named "+model->spaceName()+" because there is more than one dimmed zone.");
        return false;
    }
    ElectricIlluminanceData dimmedZone;
    std::vector<ElectricIlluminanceData> nonDimmedZones;

    for (int i=0;model->controlZones().size();i++){
        if (model->controlZones()[i].algorithm()!="null"){
            dimmedZone.parseIlluminance(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->controlZones()[i].name()+".ill");
        }else{
            ElectricIlluminanceData tempZone;
            tempZone.parseIlluminance(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->controlZones()[i].name()+".ill");
            nonDimmedZones.push_back(tempZone);
        }
    }






}


}
